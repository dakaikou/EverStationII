#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

#include "..\Common\define.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_Trigger.h"
#include "TSMagic_Trigger_TSPacket.h"

#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TS_ErrorCode.h"
#include "translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_ErrorCode.h"

CTrigger_TSPacket::CTrigger_TSPacket(void)
{
}

CTrigger_TSPacket::~CTrigger_TSPacket(void)
{
}

void ts_packet_trigger_loop(pthread_params_t pThreadParams)
{
	uint8_t	  packet_buf[204];
	int	  packet_length;
	char	  pszDebug[256];

//	int64_t	  read_byte_pos = 0;
	int	  rtcode;
//	int	  getdata_rtcode;

	CTransportStream* ptransport_stream = NULL;
	transport_packet_t			transport_packet;
	CTrigger_TSPacket* pTSPacketTrigger = NULL;

	if (pThreadParams != NULL)
	{
		pThreadParams->ts_trigger_thread_running = 1;

		sprintf_s(pszDebug, sizeof(pszDebug), "TS包捕捉: 开始");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

//		TRIGGER_TS_PACKET.packet_length = ptransport_stream->m_nPacketLength;

		pTSPacketTrigger = pThreadParams->pTrigger_TSPacket;

		ptransport_stream = pThreadParams->pTStream;

		//这里不一定需要重头开始搜
		ptransport_stream->StartGetData();

//		while (pThreadParams->ts_trigger_thread_running == 1)
//		{
//			packet_length = sizeof(packet_buf);
//			rtcode = ptransport_stream->PrefetchOnePacket(packet_buf, &packet_length);
//			if (rtcode == MIDDLEWARE_TS_NO_ERROR)
//			{
//				//成功读出一个TS包
////				read_byte_pos = ptransport_stream->Tell();
//
//				rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);
//				if (rtcode == TSPACKET_PARSE_NO_ERROR)
//				{
//					if (pTSPacketTrigger->IsOpened() && (!pTSPacketTrigger->IsFull()))
//					{
//						if (pTSPacketTrigger->IsMatched(packet_buf, packet_length))
//						{
////							int nOldCatchedCount = pTSPacketTrigger->GetCatchedCount();
//							pTSPacketTrigger->SaveTheWholePacket(packet_buf, packet_length);
////							if (nOldCatchedCount == 0)		//捕捉到第一个匹配section时报告状态
//							{
//								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_TS_TRIGGER_STATE, 0, 0);
//							}
//
//							if (pTSPacketTrigger->IsFull())
//							{
//								break;
//							}
//						}
//					}
//				}
//				else
//				{
//					//解析TS包语法发现错误，为什么发生错误？？？？
//				}
//
//				ptransport_stream->SkipOnePacket();
//			}
//			else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
//			{
//				ptransport_stream->SeekToBegin();
//			}
//			else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
//			{
//				Sleep(10);
//			}
//			else
//			{
//				break;
//			}
//		}

		pThreadParams->ts_trigger_thread_running = 0;
//		pThreadParams->ts_trigger_thread_stopped = 1;

		sprintf_s(pszDebug, sizeof(pszDebug), "TS包捕捉: 结束");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "TS包捕捉: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
}

uint32_t TSMagic_packet_trigger_thread(LPVOID lpParam)
{
	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;
	ts_packet_trigger_loop(pThreadParams);

	return 0;
}




