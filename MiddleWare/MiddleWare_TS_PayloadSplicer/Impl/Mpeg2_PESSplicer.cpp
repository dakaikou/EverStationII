#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>

#include "../Include/MiddleWare_PesSplicer_ErrorCode.h"
#include "../Include/Mpeg2_PESSplicer.h"

CPESSplicer::CPESSplicer(void)
{
	m_pucPESBuf = NULL;
	m_nBufSize = 0;
	m_nPESLength = 0;

	m_usPID = 0xffff;
	m_nPkIndex = -1;
	m_nWriteSize = 0;

	m_bSplicingStarted = 0;
}

CPESSplicer::~CPESSplicer()
{
	Reset();
}

void CPESSplicer::Init(void)
{
	m_pucPESBuf = NULL;
	m_nBufSize = 0;
	m_nPESLength = 0;
}

void CPESSplicer::Reset(void)
{
	if (m_pucPESBuf != NULL)
	{
		free(m_pucPESBuf);
		m_pucPESBuf = NULL;
	}
	m_nBufSize = 0;
	m_nPESLength = 0;

	m_usPID = 0xffff;
	m_nPkIndex = -1;
	m_nWriteSize = 0;

	m_bSplicingStarted = 0;
}

int CPESSplicer::SetPID(uint16_t usPID)
{
	m_usPID = usPID;

	return 0;
}

//ͨ����ƴ��������д��TS�����ݣ��ɱ�������PES����ʼ�ͽ�����Ȼ��ƴ�ӳ�PES��
//���룺
//-- TS��
//���أ�
//--״̬��
int CPESSplicer::WriteTSPacket(transport_packet_t* ptransport_packet)
{
	int		rtcode = PESES_SPLICE_UNKNOWN_ERROR;

	uint8_t*		payload_buf;
	int				payload_length;

	//control part
	int		bAligned = 0;

	if (ptransport_packet != NULL)
	{
		if (m_usPID == 0xFFFF)		//��ʾ��ǰƴ������δ��ռ�ã�����
		{
			m_usPID = ptransport_packet->PID;
		}
		
		if (ptransport_packet->PID == m_usPID)    //��ǰTS����PID�뵱ǰƴ������PIDһ��
		{
			if (ptransport_packet->transport_error_indicator == 0)				//packet with no error
			{
				if (ptransport_packet->transport_scrambling_control == 0)		//not scrambled
				{
					//��ȡTS������ָ��;��ɳ���
					payload_buf = ptransport_packet->payload_buf;
					payload_length = ptransport_packet->payload_length;

					assert(payload_length <= 184);

					if (m_bSplicingStarted == 0)	//��ǰƴ������δ��ʼƴ��
					{
						if (ptransport_packet->payload_unit_start_indicator == 1)  //ƴ�ӵ����
						{
							assert(m_pucPESBuf == NULL);
							//if (m_pucPESBuf != NULL)
							//{
							//	free(m_pucPESBuf);
							//	m_pucPESBuf = NULL;
							//}
							//m_nPESLength = 0;

							m_nBufSize = 0x10000;
							m_pucPESBuf = (uint8_t*)malloc(m_nBufSize);
							if (m_pucPESBuf != NULL)
							{
								memcpy(m_pucPESBuf, payload_buf, payload_length);
								m_nPESLength = payload_length;
								rtcode = PESES_SPLICE_FIRST_PACKET;
							}
							else
							{
								rtcode = PESES_SPLICE_MEMORY_ERROR;
							}

							//���ص�ǰ����ţ�����һ��TS������ʱ�����Ը��ݰ���ż����м��Ƿ���ڶ���
							m_nPkIndex = ptransport_packet->continuity_counter;

							m_bSplicingStarted = 1;
						}
						else   //��ǰTS��δЯ��PES����ʼ���֣���û���ҵ�PESͷ�����Ե�ǰTS��
						{
							rtcode = PESES_SPLICE_DO_NOT_SYNC;
						}
					}
					else if (m_bSplicingStarted == 1)
					{
						int bAligned = 1;
						//�������
						//if (ptransport_packet->continuity_counter == 0)
						//{
						//	bAligned = (m_nPkIndex == 15) ? 1 : 0;
						//}
						//else
						//{
						//	bAligned = ((m_nPkIndex + 1) == ptransport_packet->continuity_counter) ? 1 : 0;
						//}

						if (bAligned)
						{
							if (ptransport_packet->payload_unit_start_indicator == 1)	//��������һ��PES���Ŀ�ʼ��˵����һ��PES��ƴ�����
							{
								//��ǰ�㷨���ڵ������ǣ�PES��β���жϱ���������һ��PES���Ŀ�ʼ�����ٳ�δ�ȵ���һ�����Ŀ�ʼ�����޷��жϵ�ǰƴ���Ƿ�����
								//��Ҫ�Ľ���   chendelin  2019.5.17
								//������׽PES���ͻ�������
								m_bSplicingStarted = -1;			//��ת״̬

								rtcode = PESES_SPLICE_LAST_PACKET;
							}
							else    //ƴ�ӹ�����
							{
								//ֱ�ӻ�������
								int new_size = m_nPESLength + payload_length;
								if (new_size > m_nBufSize)
								{
									m_nBufSize += 0x8000;
									uint8_t* new_buf = (uint8_t*)realloc(m_pucPESBuf, m_nBufSize);
									m_pucPESBuf = new_buf;
								}

								if (m_pucPESBuf != NULL)
								{
									memcpy(m_pucPESBuf + m_nPESLength, payload_buf, payload_length);
									m_nPESLength += payload_length;
									rtcode = PESES_SPLICE_FOLLOW_PACKET;
								}
								else
								{
									rtcode = PESES_SPLICE_MEMORY_ERROR;
								}
							}
						}
						else
						{
							Reset();
							rtcode = PESES_SPLICE_CONTINUITY_ERROR;
						}
					}
					else  //��һ��PES����δ��Ӧ�ò����ѣ�ƴ������ת�ȴ�
					{
						rtcode = PESES_SPLICE_UNKNOWN_ERROR;
					}
				}
				else    //����TS�����ŵ�����£�����ǰ���Ѿ��ռ��������ݣ���Ҫ�ӵ�
				{
					Reset();
					rtcode = PESES_SPLICE_SCRAMBLED_PACKET;				//TS��������
				}
			}
			else  //����TS���д�����������£�����ǰ���Ѿ��ռ��������ݣ���Ҫ�ӵ�
			{
				Reset();
				rtcode = PESES_SPLICE_TRANSPORT_ERROR;			//TS���������
			}
		}
		else    //��ǰTS����PID�뵱ǰƴ������PID��һ�£�ֱ�Ӷ�����TS��
		{
			rtcode = PESES_SPLICE_PID_ERROR;
		}
	}
	else
	{
		rtcode = PESES_SPLICE_PARAMETER_ERROR;				//�����������
	}

	return rtcode;
}

uint8_t* CPESSplicer::GetPESPacket(int* plength)
{
	if (plength != NULL)
	{
		*plength = m_nPESLength;
	}

	return m_pucPESBuf;
}
/*
int	CSectionSplicer::ReadSection(uint8_t* buf, int* plength)
{
	int		rtcode = NO_ERROR;

	if ((buf != NULL) && (plength != NULL))
	{
		if (*plength >= m_nSectLength)
		{
			memcpy(buf, m_pucSectBuf, m_nSectLength);
			*plength = m_nSectLength;
		}
		else
		{
			rtcode = SECTION_SPLICE_MEMORY_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_SPLICE_PARAMETER_ERROR;
	}

	return rtcode;
}
*/
