#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>

#include "../Include/MiddleWare_PesSplicer_ErrorCode.h"
#include "../Include/Mpeg2_PESSplicer.h"

CPESSplicer::CPESSplicer(void)
{
	Init();
	Reset();
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

	m_pes_payload_unit_started = 0;
}

S32 CPESSplicer::SetPID(U16 usPID)
{
	m_usPID = usPID;

	return 0;
}

S32 CPESSplicer::WriteTSPacket(transport_packet_t* ptransport_packet)
{
	S32		rtcode = PESES_SPLICE_UNKNOWN_ERROR;

	U8*		payload_buf;
	S32		payload_length;

	//control part
	S32		bAligned = 0;

	if (ptransport_packet != NULL)
	{
		if (m_usPID == 0xFFFF)
		{
			m_usPID = ptransport_packet->PID;
		}

		if (ptransport_packet->PID == m_usPID)
		{
			if (ptransport_packet->transport_error_indicator == 0)				//packet with no error
			{
				if (ptransport_packet->transport_scrambling_control == 0)		//not scrambled
				{
					payload_buf = ptransport_packet->payload_buf;
					payload_length = ptransport_packet->payload_length;

					assert(payload_length <= 184);

					if (m_pes_payload_unit_started == 0)
					{
						if (ptransport_packet->payload_unit_start_indicator == 1)  //拼接的起点
						{
							m_pes_payload_unit_started = 1;

							m_nPkIndex = ptransport_packet->continuity_counter;

							assert(m_pucPESBuf == NULL);
							//if (m_pucPESBuf != NULL)
							//{
							//	free(m_pucPESBuf);
							//	m_pucPESBuf = NULL;
							//}
							//m_nPESLength = 0;

							m_nBufSize = 0x10000;
							m_pucPESBuf = (U8*)malloc(m_nBufSize);
							memcpy(m_pucPESBuf, payload_buf, payload_length);
							m_nPESLength = payload_length;
							//hLastRecordHandler = pPESPacketTrigger->SaveAsNewStart(transport_packet.payload_buf, transport_packet.payload_length);

							//if (main_type == PID_MAINTYPE_VIDEO)
							//{
							//	if ((sub_type == VIDEO_MPEG1) ||
							//		(sub_type == VIDEO_MPEG2))
							//	{
							//		pAVDecoder = new CMPEG_VideoDecoder;
							//	}
							//	else if (sub_type == VIDEO_H264)
							//	{
							//		pAVDecoder = new CH264_VideoDecoder;
							//	}
							//	else if (sub_type == VIDEO_AVS)
							//	{
							//		pAVDecoder = new CAVS_VideoDecoder;
							//	}
							//}
							//else if (main_type == PID_MAINTYPE_AUDIO)
							//{
							//	if ((sub_type == AUDIO_MPEG1) ||
							//		(sub_type == AUDIO_MPEG1_L1) ||
							//		(sub_type == AUDIO_MPEG1_L2) ||
							//		(sub_type == AUDIO_MPEG1_L3) ||
							//		(sub_type == AUDIO_MPEG2) ||
							//		(sub_type == AUDIO_MPEG2_L1) ||
							//		(sub_type == AUDIO_MPEG2_L2) ||
							//		(sub_type == AUDIO_MPEG2_L3))
							//	{
							//		pAVDecoder = new CMPEG_AudioDecoder;
							//	}
							//	else if (sub_type == AUDIO_AC3)
							//	{
							//		pAVDecoder = new CAC3_AudioDecoder;
							//	}
							//	else if (sub_type == AUDIO_MPEG2_AAC)
							//	{
							//		pAVDecoder = new CAAC_AudioDecoder;
							//	}
							//}
							//else
							//{
							//	pAVDecoder = new CESDecoder;
							//}

							//if (pAVDecoder != NULL)
							//{
							//	pAVDecoder->Open(pThreadParams->hPesEsMsgWnd, STREAM_TS, NULL);
							//	pAVDecoder->SetParams(transport_packet.PID, 0xFFFF);
							//	pAVDecoder->m_bTriggering = 1;

							//	pAVDecoder->WriteTSPacket(&transport_packet, read_byte_pos);
							//}
						}
						else
						{
							//还没有找到PES头，忽略当前TS包
							rtcode = PESES_SPLICE_NOT_SYNC;
						}
					}
					else if (m_pes_payload_unit_started == 1)
					{
						S32 bAligned = 1;
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
							if (ptransport_packet->payload_unit_start_indicator == 1)	//拼接的终点
							{
								//遇到了下一个PES包的开始

								//if (pAVDecoder != NULL)
								//{
								//	pAVDecoder->m_bTriggering = 0;
								//}

								//S32 nOldCatchedCount = pPESPacketTrigger->GetCatchedCount();

								//pPESPacketTrigger->SaveAndClose(hLastRecordHandler, NULL, 0);

								//if (nOldCatchedCount == 0)		//捕捉到第一个匹配PES包时报告状态
								//{
								//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 2, 0);
								//}

								//if (main_type == PID_MAINTYPE_VIDEO)			//检测是否遇到过sequence_end
								//{
								//}

								m_pes_payload_unit_started = 0;

								rtcode = PESES_SPLICE_NO_ERROR;
							}
							else
							{
								//拼接过程中

								//直接缓存数据
								S32 new_size = m_nPESLength + payload_length;
								if (new_size > m_nBufSize)
								{
									m_nBufSize += 0x8000;
									U8* new_buf = (U8*)realloc(m_pucPESBuf, m_nBufSize);
									m_pucPESBuf = new_buf;
								}

								memcpy(m_pucPESBuf + m_nPESLength, payload_buf, payload_length);
								m_nPESLength += payload_length;

								//assert(hLastRecordHandler >= 0);
								//pPESPacketTrigger->AppendToLast(hLastRecordHandler, transport_packet.payload_buf, transport_packet.payload_length);

								//if (pAVDecoder != NULL)
								//{
								//	pAVDecoder->WriteTSPacket(&transport_packet, read_byte_pos);
								//}

								rtcode = PESES_SPLICE_NOT_COMPLETE;
							}
						}
						else
						{
							Reset();
							rtcode = PESES_SPLICE_CONTINUITY_ERROR;
						}

					}
				}
				else
				{
					//遇到TS包加扰的情况下，不管前面已经收集多少数据，都要扔掉
					Reset();
					rtcode = PESES_SPLICE_SCRAMBLED_PACKET;				//TS包被加扰
				}
			}
			else
			{
				//遇到TS包错误的情况下，不管前面已经收集多少数据，都要扔掉
				Reset();
				rtcode = PESES_SPLICE_TRANSPORT_ERROR;			//TS包传输错误
			}
		}
		else
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

U8* CPESSplicer::GetPESPacket(S32* plength)
{
	if (plength != NULL)
	{
		*plength = m_nPESLength;
	}

	return m_pucPESBuf;
}
/*
S32	CSectionSplicer::ReadSection(U8* buf, S32* plength)
{
	S32		rtcode = NO_ERROR;

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
