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

//通过往拼接器持续写入TS包数据，由本程序检测PES的起始和结束，然后拼接出PES包
//输入：
//-- TS包
//返回：
//--状态码
int CPESSplicer::WriteTSPacket(transport_packet_t* ptransport_packet)
{
	int		rtcode = PESES_SPLICE_UNKNOWN_ERROR;

	uint8_t*		payload_buf;
	int				payload_length;

	//control part
	int		bAligned = 0;

	if (ptransport_packet != NULL)
	{
		if (m_usPID == 0xFFFF)		//表示当前拼接器尚未被占用，可用
		{
			m_usPID = ptransport_packet->PID;
		}
		
		if (ptransport_packet->PID == m_usPID)    //当前TS包的PID与当前拼接器的PID一致
		{
			if (ptransport_packet->transport_error_indicator == 0)				//packet with no error
			{
				if (ptransport_packet->transport_scrambling_control == 0)		//not scrambled
				{
					//获取TS包净荷指针和净荷长度
					payload_buf = ptransport_packet->payload_buf;
					payload_length = ptransport_packet->payload_length;

					assert(payload_length <= 184);

					if (m_bSplicingStarted == 0)	//当前拼接器尚未开始拼接
					{
						if (ptransport_packet->payload_unit_start_indicator == 1)  //拼接的起点
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

							//记载当前包序号，当下一个TS包到达时，可以根据包序号检验中间是否存在丢包
							m_nPkIndex = ptransport_packet->continuity_counter;

							m_bSplicingStarted = 1;
						}
						else   //当前TS包未携带PES包起始部分，还没有找到PES头，忽略当前TS包
						{
							rtcode = PESES_SPLICE_DO_NOT_SYNC;
						}
					}
					else if (m_bSplicingStarted == 1)
					{
						int bAligned = 1;
						//检查包序号
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
							if (ptransport_packet->payload_unit_start_indicator == 1)	//遇到了下一个PES包的开始，说明上一个PES包拼接完成
							{
								//当前算法存在的问题是，PES包尾的判断必须依赖下一个PES包的开始，若迟迟未等到下一个包的开始，则无法判断当前拼接是否完整
								//需要改进。   chendelin  2019.5.17
								//连续捕捉PES包就会有问题
								m_bSplicingStarted = -1;			//空转状态

								rtcode = PESES_SPLICE_LAST_PACKET;
							}
							else    //拼接过程中
							{
								//直接缓存数据
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
					else  //上一个PES包还未被应用层消费，拼接器空转等待
					{
						rtcode = PESES_SPLICE_UNKNOWN_ERROR;
					}
				}
				else    //遇到TS包加扰的情况下，不管前面已经收集多少数据，都要扔掉
				{
					Reset();
					rtcode = PESES_SPLICE_SCRAMBLED_PACKET;				//TS包被加扰
				}
			}
			else  //遇到TS包有传输错误的情况下，不管前面已经收集多少数据，都要扔掉
			{
				Reset();
				rtcode = PESES_SPLICE_TRANSPORT_ERROR;			//TS包传输错误
			}
		}
		else    //当前TS包的PID与当前拼接器的PID不一致，直接丢弃该TS包
		{
			rtcode = PESES_SPLICE_PID_ERROR;
		}
	}
	else
	{
		rtcode = PESES_SPLICE_PARAMETER_ERROR;				//输入参数错误
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
