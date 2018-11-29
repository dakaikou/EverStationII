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

#include "thirdparty_libs\glog\glog\logging.h"

CTrigger_TSPacket::CTrigger_TSPacket(void)
{
}

CTrigger_TSPacket::~CTrigger_TSPacket(void)
{
}

void ts_packet_trigger_loop(pthread_params_t pThreadParams)
{
	uint8_t	  packet_buf[204];
	int		  packet_length;
	char	  pszDebug[256];
	int		  stream_synced = 0;

	int64_t	  read_byte_pos = 0;
	int		  rtcode;
	int		  catched_count = 0;

	CTransportStream*	ptransport_stream = NULL;
	transport_packet_t	transport_packet;
	CTrigger_TSPacket* pTSPacketTrigger = NULL;

	if (pThreadParams != NULL)
	{
		uint32_t tickCount = ::GetTickCount();
		srand(tickCount);
		uint32_t randNum = rand();

		pThreadParams->ts_trigger_thread_running = 1;

		sprintf_s(pszDebug, sizeof(pszDebug), "TS����׽<ID:0x%08X>: ��ʼ", randNum);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		LOG(INFO) << pszDebug;

		pTSPacketTrigger = pThreadParams->pTrigger_TSPacket;

		ptransport_stream = pThreadParams->pTStream;

		//���ﲻһ����Ҫ��ͷ��ʼ��
		ptransport_stream->StartGetData();

		while (pThreadParams->ts_trigger_thread_running == 1)
		{
			//first synchronize the stream
			if (stream_synced == 0)
			{
				do
				{
					rtcode = ptransport_stream->Synchronize(&packet_length);
					if (rtcode == MIDDLEWARE_TS_NO_ERROR)
					{
						stream_synced = 1;
						read_byte_pos = ptransport_stream->Tell();			//��Ϊ��Ԥ��һ������ʵ��FIFO��ָ�벢δ�����ƶ�

						sprintf_s(pszDebug, sizeof(pszDebug), "TS����׽<ID:0x%08X>: �ҵ�ͬ��, TS���� %d�ֽڣ��ļ�λ�ã�0x%llx��\n", randNum, packet_length, read_byte_pos);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
						LOG(INFO) << pszDebug;
						break;
					}
					else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
					{
						ptransport_stream->SeekToBegin();
						//break;
					}
				} while (pThreadParams->ts_trigger_thread_running == 1);
			}
			else if (stream_synced == 1)
			{
				packet_length = sizeof(packet_buf);
				rtcode = ptransport_stream->SyncReadOnePacket(packet_buf, &packet_length);
				read_byte_pos = ptransport_stream->Tell();			//��Ϊ��Ԥ��һ������ʵ��FIFO��ָ�벢δ�����ƶ�
				if (rtcode == MIDDLEWARE_TS_NO_ERROR)
				{
					rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);
					if (rtcode == TSPACKET_PARSE_NO_ERROR)
					{
						if (pTSPacketTrigger->IsOpened() && (!pTSPacketTrigger->IsFull()))
						{
							if (pTSPacketTrigger->IsMatched(packet_buf, packet_length))
							{
								pTSPacketTrigger->SaveTheWholePacket(packet_buf, packet_length);
								{
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_TS_TRIGGER_STATE, 0, 0);

									sprintf_s(pszDebug, sizeof(pszDebug), "TS����׽<ID:0x%08X>: �ҵ�ƥ��ts��[%d]���ļ�λ�ã���ʼ��0x%llx-������0x%llx��\n", randNum, catched_count, read_byte_pos - packet_length, read_byte_pos);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
									LOG(INFO) << pszDebug;

									catched_count++;
								}

								if (pTSPacketTrigger->IsFull())
								{
									ptransport_stream->Seek(read_byte_pos);
									break;
								}
							}
						}
					}
					else
					{
						//����TS���﷨���ִ���Ϊʲô�������󣿣�����
					}
				}
				else if (rtcode == ETR290_TS_SYNC_LOSS)
				{
					//��ȡTS���������󡪡�ͬ����ʧ
					stream_synced = 0;

					sprintf_s(pszDebug, sizeof(pszDebug), "TS����׽<ID:0x%08X>: ��ʧͬ�����ļ�λ�ã�0x%llx��\n", randNum, read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;
				}
				else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
				{
					ptransport_stream->SeekToBegin();
				}
				else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
				{
				}
				else
				{
					break;
				}
			}
		}

		pThreadParams->ts_trigger_thread_running = 0;
//		pThreadParams->ts_trigger_thread_stopped = 1;

		uint32_t existTickCount = ::GetTickCount();
		uint32_t diff_tickcount = existTickCount - tickCount;

		sprintf_s(pszDebug, sizeof(pszDebug), "TS����׽<ID:0x%08X>: ��������ץ%d��TS������ʱ%dms", randNum, catched_count, diff_tickcount);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		LOG(INFO) << pszDebug;
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "TS����׽: �����������");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
}

uint32_t TSMagic_packet_trigger_thread(LPVOID lpParam)
{
	pthread_params_t	pThreadParams = (pthread_params_t)lpParam;
	ts_packet_trigger_loop(pThreadParams);

	return 0;
}




