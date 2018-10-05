#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>

#include "../Include/MiddleWare_SectionSplicer_ErrorCode.h"
#include "../Include/Mpeg2_SectionSplicer.h"
#include "libs_Mpeg&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_table_id.h"

CSectionSplicer::CSectionSplicer(void)
{
	Init();
	Reset();
}

CSectionSplicer::~CSectionSplicer()
{
	Reset();
}

void CSectionSplicer::Init(void)
{
	m_pucSectionBuf = NULL;
}

void CSectionSplicer::Reset(void)
{
	if (m_pucSectionBuf != NULL)
	{
		free(m_pucSectionBuf);
		m_pucSectionBuf = NULL;
	}

	m_usPID = 0xffff;
	m_ucTableID = 0xff;
	m_nPkIndex = -1;
	m_nSectionLength = 0;
	m_nWriteSize = 0;
}

S32 CSectionSplicer::WriteTSPacket(transport_packet_t* ptransport_packet)
{
	S32		rtcode = SECTION_SPLICE_UNKNOWN_ERROR;

	//section syntax
	U8*		section_buf;
	S32		section_length;
	S32		pointer_field;

	U8*		payload_buf;
	S32		payload_length;

	//control part
	S32		bAligned = 0;
	S32		copy_length;

//	U8		ucOldTableID = 0xFF;

	if (ptransport_packet != NULL)
	{
		//if (ptransport_packet->PID == 0x0600)
		//{
		//	int debug = 1;		//��������
		//}

		if (ptransport_packet->transport_error_indicator == 0)				//packet with no error
		{
			if (ptransport_packet->transport_scrambling_control == 0)		//not scrambled
			{
				payload_buf = ptransport_packet->payload_buf;
				payload_length = ptransport_packet->payload_length;

				assert(payload_length <= 184);

				if (ptransport_packet->payload_unit_start_indicator)
				{
					//payload_unit_start_indicator��ʶ���µ�section��ʼ
					pointer_field = payload_buf[0];

					section_buf = payload_buf + 1 + pointer_field;
					payload_length -= (1 + pointer_field);

					//section ��ʼ����
					if (m_pucSectionBuf != NULL)
					{
						//��ǰTS��Я������һ��Section��β�ͣ���һ��Section�Ŀ�ʼ��ʾ��������SCTV_GSA_AIT_431_13Dec.ts

						memcpy(m_pucSectionBuf + m_nWriteSize, payload_buf + 1, pointer_field);
						m_nWriteSize += pointer_field;

						if (m_nWriteSize >= m_nSectionLength)
						{
							rtcode = SECTION_SPLICE_ONE_MORE_SECTIONS;
						}
						else
						{
							//�ϴβ���������Ҫ����
							Reset();

							rtcode = NO_ERROR;
						}
					}
					else
					{
						rtcode = NO_ERROR;
					}

					if (rtcode == NO_ERROR)
					{
						if (payload_length >= 3)
						{
							S32 maybe_padding = 1;

							//����ֽڼ��
							for (int i = 0; i < payload_length; i++)
							{
								if (section_buf[i] != 0xFF)
								{
									maybe_padding = 0;
								}
							}

							if (maybe_padding == 0)
							{
								//���ɲ���������ݣ���Ҫ���������������

								section_length = (section_buf[1] & 0x0f) << 8;
								section_length |= section_buf[2];

								if ((section_length > 0) && (section_length <= 4093))
								{
									m_nSectionLength = section_length + 3;
									m_usPID = ptransport_packet->PID;
									m_ucTableID = section_buf[0];

									m_nPkIndex = ptransport_packet->continuity_counter;

									m_nWriteSize = 0;
									assert(m_pucSectionBuf == NULL);
									m_pucSectionBuf = (U8*)malloc(m_nSectionLength);
									if (m_pucSectionBuf != NULL)
									{
										copy_length = (payload_length > m_nSectionLength) ? m_nSectionLength : payload_length;

										if (copy_length > 0)
										{
											memcpy(m_pucSectionBuf, section_buf, copy_length);
										}
										m_nWriteSize = copy_length;

										if (m_nWriteSize >= m_nSectionLength)
										{
											rtcode = NO_ERROR;
										}
										else
										{
											rtcode = SECTION_SPLICE_NOT_COMPLETE;
										}
									}
									else
									{
										rtcode = SECTION_SPLICE_MEMORY_ERROR;
									}
								}
								else
								{
									rtcode = SECTION_SPLICE_SYNTAX_ERROR;
								}
							}
							else
							{
								//����ʣ�µĲ���ȫ������ֽڣ���������������
								rtcode = SECTION_SPLICE_PADDING_BYTES;
							}
						}
						else
						{
							//��payload_unit_start_indicatorָʾһ��section����ʼ������TS��payload���������sectionͷ����Ϣ��������ȫ�������sectionֵ�û���

							rtcode = SECTION_SPLICE_SYNTAX_ERROR;
						}
					}
				}
				else
				{
					//����Ҫ�ж�һ���Ƿ��Ѿ���ʼƴ��section
					if (m_usPID != 0xffff)
					{
						if (ptransport_packet->PID == m_usPID)		//PID���뱣��һ��
						{
							//�������
							if (ptransport_packet->continuity_counter == 0)
							{
								bAligned = (m_nPkIndex == 15) ? 1 : 0;
							}
							else
							{
								bAligned = ((m_nPkIndex + 1) == ptransport_packet->continuity_counter) ? 1 : 0;
							}

							if (bAligned)
							{
								if ((m_nWriteSize + payload_length) > m_nSectionLength)
								{
									copy_length = m_nSectionLength - m_nWriteSize;
								}
								else
								{
									copy_length = payload_length;
								}

								memcpy(m_pucSectionBuf + m_nWriteSize, payload_buf, copy_length);
								m_nWriteSize += copy_length;
				
								if (m_nWriteSize >= m_nSectionLength)
								{
									rtcode = NO_ERROR;
								}
								else
								{
									//��������һ��TS��
									m_nPkIndex = ptransport_packet->continuity_counter;

									rtcode = SECTION_SPLICE_NOT_COMPLETE;
								}
							}
							else
							{
								//�������Ų�������˵���ж���������������ʱsection����Ҳ�ǲ����õ�
								Reset();
								rtcode = SECTION_SPLICE_CONTINUITY_ERROR;						//��������
							}
						}
						else
						{
							//��ƴ�ӵĹ����з�����PID�仯����ʱsection����Ҳ�ǲ����õ�
							Reset();
							rtcode = SECTION_SPLICE_PID_ERROR;					//��һ������û��ƴ�꣬������һ����
						}
					}
					else
					{
						//����������TS��
						rtcode = SECTION_SPLICE_NOT_SYNC;
					}
				}
			}
			else
			{
				//����TS�����ŵ�����£�����ǰ���Ѿ��ռ��������ݣ���Ҫ�ӵ�
				Reset();
				rtcode = SECTION_SPLICE_SCRAMBLED_PACKET;				//TS��������
			}
		}
		else
		{
			//����TS�����������£�����ǰ���Ѿ��ռ��������ݣ���Ҫ�ӵ�
			Reset();
			rtcode = SECTION_SPLICE_TRANSPORT_ERROR;			//TS���������
		}
	}
	else
	{
		Reset();
		rtcode = SECTION_SPLICE_PARAMETER_ERROR;				//�����������
	}

	return rtcode;
}

