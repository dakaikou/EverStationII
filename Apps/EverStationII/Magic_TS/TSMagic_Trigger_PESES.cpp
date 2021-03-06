#include "stdafx.h"

#include "..\Common\define.h"

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"

#include "TSMagic_AnalyseThread.h"
#include "TSMagic_AnalyseThread_MSG.h"
#include "TSMagic_ErrorCode.h"
#include "TSMagic_Trigger_PESES.h"

#include "syntax_translate_layer/Mpeg_PESPacket/Include/MPEG_stream_id.h"
#include "syntax_translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_PESSplicer.h"
#include "MiddleWare\MiddleWare_TS_PayloadSplicer\Include\MiddleWare_PesSplicer_ErrorCode.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
//#include "MiddleWare/MiddleWare_ESDecoder/Include/VideoDecoder_Mpeg2.h"
//#include "MiddleWare/MiddleWare_ESDecoder/Include/VideoDecoder_H264.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_AVS.h"
//#include "MiddleWare_ESDecoder/Include/VideoDecoder_YUV.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_Mpeg2.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AC3.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_AAC.h"
//#include "MiddleWare_ESDecoder/Include/AudioDecoder_WAVE.h"
//#include "MiddleWare_ESDecoder/Include/Decoder_MPEG2_PS.h"
#include "thirdparty_libs\glog\glog\logging.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

CTrigger_PESPacket::CTrigger_PESPacket(void)
{
	m_usPID = 0xFFFF;
}

CTrigger_PESPacket::~CTrigger_PESPacket(void)
{
}

void CTrigger_PESPacket::SetMatchParams(int filter_id, uint16_t PID, uint32_t style, int demand_count)
{
	m_usPID = PID;
	m_dwStyle = style;

	CTrigger::SetMatchParams(NULL, NULL, 0, demand_count);
}

void CTrigger_PESPacket::Reset(void)
{
	m_usPID = 0xFFFF;
	m_dwStyle = 0x00000000;
	CTrigger::Reset();
}

uint16_t CTrigger_PESPacket::GetPID(void)
{
	return m_usPID;
}

uint32_t CTrigger_PESPacket::GetStyle(void)
{
	return m_dwStyle;
}

//int CTrigger_PESPacket::IsMatched(uint8_t* buf, int length)
//{
//	int		 equal = -1;
//	int		 i;
//	uint8_t		 mask;
//	uint8_t		 data;
//	uint8_t		 src;
//	uint8_t		 dst;
//
//	if (buf != NULL)
//	{
//		equal = 1;
//
//		mask = m_ucMatchMasks[0];
//		data = m_ucMatchDatas[0];
//
//		src = buf[0] & mask;
//		dst = data & mask;
//
//		if (src == dst)
//		{
//			for (i = 1; i < m_nMatchLength; i++)
//			{
//				mask = m_ucMatchMasks[i];
//				data = m_ucMatchDatas[i];
//
//				src = buf[i + 2] & mask;					//section length field is not compared
//				dst = data & mask;
//				if (src != dst)
//				{
//					equal = 0;
//					break;
//				}
//			}
//		}
//		else
//		{
//			equal = 0;
//		}
//	}
//
//	return equal;
//}

//void ts_es_trigger_loop(pthread_params_t pThreadParams)
//{
//	uint8_t	  packet_buf[204];
//	int	  packet_length;
//
//	S8	  pszDebug[256];
//
//	int64_t	  read_byte_pos = 0;			//只能读小于2G的文件
//
//	int	  old_ratio = 0;
//	int	  analyse_ratio = 0;
//	int	  rtcode;
//	//	int	  getdata_rtcode;
//
//	CTransportStream*	 ptransport_stream = NULL;
//	transport_packet_t			transport_packet;
//
//	if (pThreadParams != NULL)
//	{
//		pThreadParams->es_trigger_thread_running = 1;
//
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ES_TRIGGER_STATE, 1, NULL);
//
//		sprintf_s(pszDebug, sizeof(pszDebug), "ES捕捉: 开始");
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
//
//		ptransport_stream = pThreadParams->pTStream;
//		//这里不一定需要重头开始搜
//		//		ptransport_stream->SeekToBegin();
//		ptransport_stream->StartGetData();
//
//		while (pThreadParams->es_trigger_thread_running)
//		{
//			packet_length = sizeof(packet_buf);
//			rtcode = ptransport_stream->PrefetchOnePacket(packet_buf, &packet_length);
//			if (rtcode == MIDDLEWARE_TS_NO_ERROR)
//			{
//				read_byte_pos = ptransport_stream->Tell();
//
//				//成功读出一个TS包
//				rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);
//				if (rtcode == TSPACKET_PARSE_NO_ERROR)
//				{
//					if (pThreadParams->pVidDecoder != NULL)
//					{
//						//						pThreadParams->triggering |= (pThreadParams->pVidDecoder->m_nTriggerType != AV_TRIGGER_NONE);
//						pThreadParams->pVidDecoder->WriteTSPacket(&transport_packet, read_byte_pos);
//					}
//
//					if (pThreadParams->pAudDecoder != NULL)
//					{
//						//						pThreadParams->triggering |= (pThreadParams->pAudDecoder->m_nTriggerType != AV_TRIGGER_NONE);
//						pThreadParams->pAudDecoder->WriteTSPacket(&transport_packet, read_byte_pos);
//					}
//				}
//				else
//				{
//					//解析TS包语法发现错误，为什么发生错误？？？？
//				}
//
//				//				read_byte_pos += packet_length;
//				//				if (read_byte_pos < 0)
//				//				{
//				//					read_byte_pos += 0x80000000;
//				//				}
//
//				ptransport_stream->SkipOnePacket();
//			}
//			else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
//			{
//				ptransport_stream->SeekToBegin();
//			}
//			else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
//			{
//				Sleep(1);
//			}
//			else
//			{
//				assert(0);
//				break;
//			}
//		}
//
//		ptransport_stream->StopGetData();
//
//		assert(pThreadParams->es_trigger_thread_running == 0);
//		pThreadParams->es_trigger_thread_stopped = 1;
//
//		sprintf_s(pszDebug, sizeof(pszDebug), "ES捕捉: 结束");
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
//
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ES_TRIGGER_STATE, 0, NULL);
//	}
//	else
//	{
//		sprintf_s(pszDebug, sizeof(pszDebug), "ES捕捉: 输入参数错误！");
//		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
//	}
//}
//
//uint32_t TSMagic_es_trigger_thread(LPVOID lpParam)
//{
//	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;
//
//	ts_es_trigger_loop(pThreadParams);
//
//	return 0;
//}

void ts_pes_trigger_loop(ts_thread_params_t* pThreadParams)
{
	uint8_t	TS_packet_buf[204];
	int		TS_packet_length;
	int		catched_count = 0;
	int		stream_synced = 0;
	int64_t	  read_byte_pos = 0;			//只能读小于2G的文件
	int64_t	  pes_start_pos = -1;
	int64_t	  pes_end_pos = -1;

	char	  pszDebug[256];

	int	  rtcode;

	CTransportStream*			ptransport_stream = NULL;
	transport_packet_t			transport_packet;
	CTrigger_PESPacket*			pPESPacketTrigger = NULL;
	CPESSplicer					PESSplicer;

	int	hLastRecordHandler = -1;

	if (pThreadParams != NULL)
	{
		uint32_t tickCount = ::GetTickCount();
		srand(tickCount);
		uint32_t randNum = rand();

		pThreadParams->pes_trigger_thread_running = 1;

		sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 开始", randNum);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		LOG(INFO) << pszDebug;

		pPESPacketTrigger = pThreadParams->pTrigger_PESPacket;
//		PESSplicer.SetPID(pPESPacketTrigger->GetPID());

		ptransport_stream = pThreadParams->pTStream;
		//这里不一定需要重头开始搜
		ptransport_stream->StartGetData();

		while (pThreadParams->pes_trigger_thread_running == 1)
		{
			if (stream_synced == 0)
			{
				do
				{
					rtcode = ptransport_stream->Synchronize(&TS_packet_length);
					if (rtcode == MIDDLEWARE_TS_NO_ERROR)
					{
						stream_synced = 1;
						read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动

						sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 找到同步, TS包长 %d字节（文件位置：0x%llx）\n", randNum, TS_packet_length, read_byte_pos);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
						LOG(INFO) << pszDebug;
						break;
					}
					else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
					{
						ptransport_stream->SeekToBegin();
						//break;
					}
				} while (pThreadParams->pes_trigger_thread_running == 1);
			}
			else if (stream_synced == 1)
			{
				TS_packet_length = sizeof(TS_packet_buf);
				rtcode = ptransport_stream->SyncReadOnePacket(TS_packet_buf, &TS_packet_length);
				read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动
				if (rtcode == MIDDLEWARE_TS_NO_ERROR)
				{
					rtcode = MPEG_decode_TS_packet(TS_packet_buf, TS_packet_length, &transport_packet);
					if (rtcode == TSPACKET_PARSE_NO_ERROR)
					{
						//该包无语法错误，有效
						//PES packet trigger
						if (pPESPacketTrigger->IsOpened() && (!pPESPacketTrigger->IsFull()))
						{
							if (transport_packet.PID == pPESPacketTrigger->GetPID())
							{
								//TS_PID与待捕捉的PES包一致
								rtcode = PESSplicer.WriteTSPacket(&transport_packet);
								if (rtcode == PESES_SPLICE_FIRST_PACKET)
								{
									catched_count = 1;

									pes_start_pos = read_byte_pos - TS_packet_length;
									//sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 找到匹配pes包起始（文件位置：0x%llx）\n", randNum, read_byte_pos - TS_packet_length);
									//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

									//LOG(INFO) << pszDebug;
								}
								else if (rtcode == PESES_SPLICE_FOLLOW_PACKET)
								{
									catched_count++;
								}
								else if (rtcode == PESES_SPLICE_LAST_PACKET)
								{
									int pes_length;
									uint8_t* pes_buf = PESSplicer.GetPESPacket(&pes_length);
									pPESPacketTrigger->SaveTheWholePacket(pes_buf, pes_length);			//内存块的搬移，数据量还是比较大的，待优化
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 2, 0);

									pes_end_pos = read_byte_pos - TS_packet_length;

									sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 找到匹配pes包且遇到下一个起始（文件位置：开始于0x%llx - 结束于0x%llx）\n", randNum, pes_start_pos, pes_end_pos);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

									LOG(INFO) << pszDebug;

									//sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 共 %d个TS包\n", randNum, catched_count);
									//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

									//LOG(INFO) << pszDebug;

									PESSplicer.Reset();

									ptransport_stream->Seek(read_byte_pos - TS_packet_length);

									break;
								}
							}
						}
					}
					else
					{
						//解析TS包语法发现错误，为什么发生错误？？？？
					}
				}
				else if (rtcode == ETR290_TS_SYNC_LOSS)
				{
					//读取TS包发生错误——同步丢失
					stream_synced = 0;

					sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 丢失同步（文件位置：0x%llx）\n", randNum, read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;
				}
				else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
				{
					pPESPacketTrigger->Reset();
					PESSplicer.Reset();

					stream_synced = 0;

					sprintf_s(pszDebug, sizeof(pszDebug), "PES包捕捉<ID:0x%08X>: 到达文件尾！（文件位置：0x%llx）\n", randNum, read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;

					ptransport_stream->SeekToBegin();
				}
				else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
				{
					//Sleep(1);
				}
				else
				{
					//				assert(0);			//示例码流：tcl_short.ts  henan.ts
					//				break;
				}
			}
		}

		ptransport_stream->StopGetData();
		//if (pAVDecoder != NULL)
		//{
		//	pAVDecoder->Close();
		//	delete pAVDecoder;
		//	pAVDecoder = NULL;
		//}

		pThreadParams->pes_trigger_thread_running = 0;

		uint32_t existTickCount = ::GetTickCount();
		uint32_t diff_tickcount = existTickCount - tickCount;

		sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 结束, 共抓 %d个TS包, 耗时%dms", randNum, catched_count, diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		LOG(INFO) << pszDebug;

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 0, NULL);
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
}

uint32_t TSMagic_pes_trigger_thread(LPVOID lpParam)
{
	ts_thread_params_t*	pThreadParams = (ts_thread_params_t*)lpParam;
	ts_pes_trigger_loop(pThreadParams);

	return 0;
}






