#include "stdafx.h"

#include "..\Common\define.h"

#include "TSMagic_Callbacks_From_Lib.h"
#include "TSMagic_Callbacks_To_Gui.h"
#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_Trigger.h"
#include "TSMagic_Trigger_Section.h"
#include "TSMagic_Dsmcc_Download.h"
#include "TSMagic_ErrorCode.h"

#include "..\Magic_TS\TSMagicView.h"

#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_PESSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/MiddleWare_SectionSplicer_ErrorCode.h"
#include "MiddleWare\MiddleWare_TS_PayloadSplicer\Include\MiddleWare_PesSplicer_ErrorCode.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
#include "syntax_translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_Utilities.h"

#include "thirdparty_libs\glog\glog\logging.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

#define AUDIO_ES_READ_SIZE				4096						//16 * 1024
#define VIDEO_ES_READ_SIZE				4096						//32 * 1024

#define MAX_SECTION_FILTERS				16

void realtime_ts_analyzer(ts_thread_params_t* pThreadParams)
{
	int	  rtcode;

	uint8_t	  record_buf[188 * 204];
	int		  write_size;

	uint8_t*  section_buf;
	int		  section_length;
	uint8_t	  packet_buf[204];
	int		  packet_length;

	char	  pszDebug[MAX_TXT_CHARS];
	char	  pszTsFile[MAX_PATH];
	char	  pszTemp[128];
	int64_t	  read_byte_pos = 0;

	int   recording_state = 0;
	int	  recording_size = 0;

	COleDateTime	timeCurrent;
	CString			strTime;
	time_t			syncread_start_time;

	uint32_t	  thread_start_tickcount;
	uint32_t	  thread_end_tickcount;

	//int			  pes_payload_unit_start_indicator = 0;
	//uint32_t	  pes_trigger_start_tickcount;
	//uint32_t	  pes_trigger_end_tickcount;
	//int			  pes_catched_count = 0;
	//uint32_t	  pes_trigger_id;

	//uint32_t	  section_trigger_start_tickcount;
	//uint32_t	  section_trigger_end_tickcount;
	//int			  section_catched_count = 0;
	//uint32_t	  section_trigger_id;

	//uint32_t	  tspacket_trigger_start_tickcount;
	//uint32_t	  tspacket_trigger_end_tickcount;
	//int			  tspacket_catched_count = 0;
	//uint32_t	  tspacket_trigger_id;


	int			  diff_tickcount;

	int	  stream_synced = 0;

	int					filter_index;
	CSectionSplicer		SectionSplicer[MAX_SECTION_FILTERS];
	CSectionSplicer*	pSectionSplicer;
	CPESSplicer			PESSplicer;

	CTransportStream*	ptransport_stream = NULL;
	transport_packet_t	transport_packet;

	RECORD_TSPacket_t		TSPacketInfo;
	CDB_TSPackets*			pDB_TSPackets = NULL;
	CDB_Pcrs*				pDB_Pcrs = NULL;
	CDB_PsiSiObjs*			pDB_PsiSiObjs = NULL;

	CTrigger_PsiSiSection*  pSectionTrigger = NULL;
	CTrigger_TSPacket*		pTSPacketTrigger = NULL;
	CTrigger_PESPacket*		pPESPacketTrigger = NULL;

	FILE*	fp_record = NULL;

	if (pThreadParams != NULL)
	{
		assert(pThreadParams->main_thread_running == 1);
		pThreadParams->main_thread_stopped = 0;

		thread_start_tickcount = ::GetTickCount();
	
		srand(thread_start_tickcount);

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REALTIME_THREAD, 1, NULL);

		timeCurrent = COleDateTime::GetCurrentTime();
		strTime = timeCurrent.Format("%Y年%m月%d日 %H:%M:%S");

		sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 开始(%s)", strTime.GetBuffer(strTime.GetLength()));
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		LOG(INFO) << pszDebug;

		sprintf_s(pszDebug, sizeof(pszDebug), "URL: %s://%s", pThreadParams->pszPathHeader, pThreadParams->pszPathName);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
		LOG(INFO) << pszDebug;

		ptransport_stream = pThreadParams->pTStream;
		ptransport_stream->Reset();

		pTSPacketTrigger = pThreadParams->pTrigger_TSPacket;
		pTSPacketTrigger->Reset();

		pPESPacketTrigger = pThreadParams->pTrigger_PESPacket;
		pPESPacketTrigger->Reset();

		pSectionTrigger = pThreadParams->pTrigger_Section;
		pSectionTrigger->Reset();

		pDB_TSPackets = pThreadParams->pDB_TSPackets;
		pDB_TSPackets->Reset();

		pDB_Pcrs = pThreadParams->pDB_Pcrs;
		pDB_Pcrs->Reset();

		pDB_PsiSiObjs = pThreadParams->pDB_PsiSiObjs;
		pDB_PsiSiObjs->Reset();

		ptransport_stream->StartGetData(0);			//启动接收线程

		while (pThreadParams->main_thread_running == 1)
		{
			if (stream_synced == 0)
			{
				do
				{
					rtcode = ptransport_stream->Synchronize(&packet_length);
					if (rtcode == MIDDLEWARE_TS_NO_ERROR)
					{
						stream_synced = 1;

						//read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动
						//sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 找到同步, TS包长 %d字节（文件位置：0x%llx）\n", packet_length, read_byte_pos);

						timeCurrent = COleDateTime::GetCurrentTime();
						strTime = timeCurrent.Format("%Y年%m月%d日 %H:%M:%S");

						sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 找到同步(%s)\n", strTime.GetBuffer(strTime.GetLength()));
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
						LOG(INFO) << pszDebug;

						syncread_start_time = time(NULL);
						break;
					}
				} while (pThreadParams->main_thread_running == 1);
			}
			else if (stream_synced == 1)
			{
				packet_length = sizeof(packet_buf);
				rtcode = ptransport_stream->SyncReadOnePacket(packet_buf, &packet_length);
				read_byte_pos = ptransport_stream->Tell();
				uint32_t read_tickcount = ::GetTickCount();
				time_t read_time = time(NULL);

				int second_change = ((read_time - syncread_start_time) >= 2) ? 1 : 0;
				if (second_change)
				{
					syncread_start_time = read_time;
				}

				if (rtcode == MIDDLEWARE_TS_NO_ERROR)
				{
					//汇报TS包长度
					if (packet_length != pDB_TSPackets->m_packet_length)
					{
						pDB_TSPackets->m_packet_length = packet_length;
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_PACKET_LENGTH, (WPARAM)packet_length, NULL);

						sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: TS包长 = %d\n", packet_length);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
						LOG(INFO) << pszDebug;
					}

					//对该TS包进行语法分析
					rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);
					if (rtcode == NO_ERROR)
					{
#if OPEN_TS_PACKET_TRIGGER					
						//TS packet trigger
						if (pTSPacketTrigger->IsOpened() && (!pTSPacketTrigger->IsFull()))
						{
							if (pTSPacketTrigger->IsMatched(packet_buf, packet_length))
							{
								//							int nOldCatchedCount = pTSPacketTrigger->GetCatchedCount();
								pTSPacketTrigger->SaveTheWholePacket(packet_buf, packet_length);
								//							if (nOldCatchedCount == 0)		//捕捉到第一个匹配TS包时报告状态
								{
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_TS_TRIGGER_STATE, 0, 0);
								}
							}
						}
#endif

#if OPEN_PES_PACKET_TRIGGER					
						//PES packet trigger
						if (pPESPacketTrigger->IsOpened() && (!pPESPacketTrigger->IsFull()))
						{
							if (transport_packet.PID == pPESPacketTrigger->GetPID())
							{
								//TS_PID与待捕捉的PES包一致
								rtcode = PESSplicer.WriteTSPacket(&transport_packet);
								if (rtcode == PESES_SPLICE_FIRST_PACKET)
								{
									//pes_trigger_start_tickcount = ::GetTickCount();
									//pes_catched_count = 1;

									//pes_trigger_id = rand();
								}
								else if (rtcode == PESES_SPLICE_FOLLOW_PACKET)
								{
									//pes_catched_count++;
								}
								else if (rtcode == PESES_SPLICE_LAST_PACKET)
								{
									//pes_trigger_end_tickcount = ::GetTickCount();
									//diff_tickcount = pes_trigger_end_tickcount - pes_trigger_start_tickcount;

									//sprintf_s(pszDebug, sizeof(pszDebug), "PES捕捉<ID:0x%08X>: 找到匹配pes包, 共抓到%d个TS包，耗时%dms\n", pes_trigger_id, pes_catched_count, diff_tickcount);
									//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

									//LOG(INFO) << pszDebug;

									int pes_length;
									uint8_t* pes_buf = PESSplicer.GetPESPacket(&pes_length);
									pPESPacketTrigger->SaveTheWholePacket(pes_buf, pes_length);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 2, 0);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 0, 0);

									PESSplicer.Reset();
									//pes_catched_count = 0;
								}
							}
						}
#endif

#if OPEN_PACKET_STATISTIC
						rtcode = pDB_TSPackets->AddPacket(&transport_packet);

						if (second_change)
						{
							if (pDB_TSPackets->callback_gui_update != NULL)
							{
								//if ((pDB_TSPackets->m_total_packet_count % 20000) == 0)
								//{
									pDB_TSPackets->callback_gui_update((int)ptransport_stream->GetBitrate(), NULL);
								//}
								//if ((pDB_TSPackets->m_total_packet_count % 20000000000) == 0)
								//{
									pDB_TSPackets->PartialReset();
								//}
							}
						}
#endif					

						//pTS_packet_info = pDB_TSPackets->GetRecord(transport_packet.PID);
						if (pDB_TSPackets->GetRecord(transport_packet.PID, &TSPacketInfo) == NO_ERROR)
						{
							//TS包录制
							if (TSPacketInfo.bDecimate == 1)
							{
								if (recording_state == 0)
								{
									timeCurrent = COleDateTime::GetCurrentTime();
									strTime = timeCurrent.Format("%Y%m%d_%H%M%S");

									sprintf_s(pszTsFile, sizeof(pszTsFile), "%s\\%s.ts", pThreadParams->pszDecimatePath, strTime.GetBuffer(strTime.GetLength()));

									sprintf_s(pszDebug, sizeof(pszDebug), "TS包录制: 开始, 文件存储路径=%s", pszTsFile);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_RECORD_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

									recording_state = 1;
									recording_size = 0;
									pThreadParams->packet_decimate_thread_running = 1;

									memset(record_buf, 0x00, sizeof(record_buf));
									write_size = 0;

									fopen_s(&fp_record, pszTsFile, "wb");

									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_RECORD_THREAD, 1, NULL);
								}

								memcpy(record_buf + write_size, packet_buf, packet_length);
								write_size += packet_length;

								if (write_size >= (188 * 204))
								{
									if (fp_record != NULL)
									{
										fwrite(record_buf, 1, write_size, fp_record);
										recording_size += write_size;

										::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_RECORD_RATIO, (WPARAM)NULL, (LPARAM)recording_size);
									}

									write_size = 0;
									memset(record_buf, 0x00, sizeof(record_buf));
								}
							}

							if (TSPacketInfo.ucClass == TSPAYLOAD_CLASS_SECTION)
							{
#if OPEN_SECTION_SPLICER
								//首先寻找是否有已经建立的过滤器
								pSectionSplicer = NULL;
								for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
								{
									if (SectionSplicer[filter_index].m_usPID == transport_packet.PID)
									{
										pSectionSplicer = SectionSplicer + filter_index;
										break;
									}
								}

								if (filter_index == MAX_SECTION_FILTERS)
								{
									//说明没有找到

									//寻找第一个可用的FILTER
									for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
									{
										if (SectionSplicer[filter_index].m_usPID == 0xFFFF)
										{
											pSectionSplicer = SectionSplicer + filter_index;
											break;
										}
									}

									if (filter_index == MAX_SECTION_FILTERS)
									{
										sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 超出section_filter的最大数量!");
										::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
									}
								}

								if (pSectionSplicer != NULL)
								{
									do
									{
										rtcode = pSectionSplicer->WriteTSPacket(&transport_packet);

										if (rtcode == SECTION_SPLICE_FIRST_PACKET)
										{
											//正常现象，不需要报错
										}
										else if (rtcode == SECTION_SPLICE_FOLLOW_PACKET)
										{
											//正常现象，不需要报错
										}
										else if ((rtcode == SECTION_SPLICE_LAST_PACKET) || (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START))
										{
											//如果SECTION拼接成功，则进行section分析

											section_buf = pSectionSplicer->m_pucSectionBuf;
											section_length = pSectionSplicer->m_nSectionLength;

											int report_rtcode = REPORT_PSISI_section(pDB_PsiSiObjs, pThreadParams->hMainWnd, pSectionSplicer->m_usPID, section_buf, section_length);
											if (report_rtcode != TSMAGIC_NO_ERROR)
											{
												if (report_rtcode == TSMAGIC_NOT_SUPPORTED_TABLE)
												{
												}
												else
												{
													TSMagic_ErrorCodeLookup(report_rtcode, pszTemp, sizeof(pszTemp));
													sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: section报告——%s\n", pszTemp);

													::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
												}
											}

#if OPEN_SECTION_TRIGGER
											//PSISI section trigger
											if (pSectionTrigger->IsOpened() && (!pSectionTrigger->IsFull()))
											{
												if (pSectionTrigger->IsMatched(section_buf, section_length))
												{
													int nOldCatchedCount = pSectionTrigger->GetCatchedCount();
													pSectionTrigger->SaveTheWholePacket(section_buf, section_length);
													if (nOldCatchedCount == 0)		//捕捉到第一个匹配section时报告状态
													{
														::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_SECTION_TRIGGER_STATE, 0, 0);
													}
												}
											}
#endif
											//完成使命，释放当前section拼接器资源
											pSectionSplicer->Reset();
										}
										else if (rtcode == SECTION_SPLICE_DO_NOT_SYNC)
										{
											//正常现象，不需要报错
										}
										else
										{
											TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
											sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: %s\n", pszTemp);
											::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
										}

									} while (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START);
								}
#endif
							}
							//else if (pTS_packet_info->nMainType == PID_MAINTYPE_VIDEO)
							//{
							//	if (pThreadParams->pVidDecoder != NULL)
							//	{
							//		pThreadParams->pVidDecoder->WriteTSPacket(&transport_packet, read_byte_pos);
							//	}
							//}
							//else if (pTS_packet_info->nMainType == PID_MAINTYPE_AUDIO)
							//{
							//	if (pThreadParams->pAudDecoder != NULL)
							//	{
							//		pThreadParams->pAudDecoder->WriteTSPacket(&transport_packet, read_byte_pos);
							//	}
							//}
							else
							{
							}
						}

#if OPEN_PCR_ANALYZER
						//实时分析情况下的PCR检测，根据码流的PCR计算码率
						if (transport_packet.adaptation_field.PCR_flag)
						{
							//int		pcr_index;

							PCR_code_t pcr_code;
							pcr_code.base_32_30 = transport_packet.adaptation_field.program_clock_reference_base_32_30;
							pcr_code.base_29_15 = transport_packet.adaptation_field.program_clock_reference_base_29_15;
							pcr_code.base_14_0 = transport_packet.adaptation_field.program_clock_reference_base_14_0;
							pcr_code.extension = transport_packet.adaptation_field.program_clock_reference_extension;

							//BITRATE_ATTRIBUTE_t old_bitrate_attr;
							//ptransport_stream->GetMeasuredBitrateAttribute(&old_bitrate_attr);
							int referenceBitrate = ptransport_stream->GetBitrate();

							rtcode = pDB_Pcrs->AddPCRSample(transport_packet.PID, read_byte_pos - packet_length + 12, &pcr_code, referenceBitrate, CALLBACK_REPORT_PCR_Observation);
							if (rtcode == NO_ERROR)
							{
								RECORD_PCR_t PCRRecord;
								//采用获取副本的方式，防止子程序对原始数据进行修改
								pDB_Pcrs->GetRecordByPID(transport_packet.PID, &PCRRecord);
								CALLBACK_REPORT_PCR_Diagnosis(&PCRRecord);

								if (PCRRecord.encoder_bitrate_available)
								{
									ptransport_stream->AddBitrateSample(PCRRecord.encoder_bitrate_cur_value, CALLBACK_REPORT_bitrate);

									//BITRATE_ATTRIBUTE_t new_bitrate_attr;
									//ptransport_stream->GetMeasuredBitrateAttribute(&new_bitrate_attr);
									//CALLBACK_REPORT_bitrates(&new_bitrate_attr);
								}

								//if (PCRRecord.interval_available && PCRRecord.jitter_available)
								//{
								//	PCR_INTERVAL_ATTRIBUTE_t interval_attr;
								//	PCR_JITTER_ATTRIBUTE_t jitter_attr;
								//	pDB_Pcrs->GetMeasuredIntervalAttribute(&interval_attr);
								//	pDB_Pcrs->GetMeasuredJitterAttribute(&jitter_attr);

								//	CALLBACK_REPORT_PCR_Observation(PCRRecord.PCR_PID, PCRRecord.interval_cur_value, PCRRecord.jitter_cur_value, &interval_attr, &jitter_attr);
								//}

								//这里仅是一种码流速率的估算方法,实际上是不可以自己证明自己的，应该通过其他方式计算码率
								//{
								//	int64_t sum = 0;
								//	int count = 0;
								//	int pcr_count = pDB_Pcrs->GetTotalRecordCount();

								//	for (int i = 0; i < pcr_count; i++)
								//	{
								//		if (pDB_Pcrs->GetRecordByIndex(i, &PCRRecord) == NO_ERROR)
								//		{
								//			if (PCRRecord.encoder_bitrate_available)
								//			{
								//				sum += PCRRecord.encoder_bitrate_mean_value;
								//				count++;
								//			}
								//		}
								//	}
								//	if (count > 0)
								//	{
								//		int bitrate_cur = (int)(sum / count);

								//		ptransport_stream->AddBitrateSample(bitrate_cur);

								//		ptransport_stream->GetMeasuredBitrateAttribute(&bitrate_attr);
								//		CALLBACK_REPORT_bitrates(bitrate_cur, &bitrate_attr);

								//		pDB_TSPackets->m_total_bitrate_from_software = bitrate_cur;
								//	}
								//}
							}
						}
#endif
					}
					else
					{
						//解析TS包语法发现错误，为什么发生错误？？？？
						TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
						sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: [PID=0x%04X] %s\n", transport_packet.PID, pszTemp);

						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					}
				}
				else if (rtcode == ETR290_TS_SYNC_LOSS)
				{
					//读取TS包发生错误——同步丢失
					stream_synced = 0;

					timeCurrent = COleDateTime::GetCurrentTime();
					strTime = timeCurrent.Format("%Y年%m月%d日_%H:%M:%S");

					sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 丢失同步(%s)\n", strTime.GetBuffer(strTime.GetLength()));
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(ERROR) << pszDebug;
				}
				else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
				{
					Sleep(1);
				}
				else
				{
					//读取TS包发生错误
					timeCurrent = COleDateTime::GetCurrentTime();
					strTime = timeCurrent.Format("%Y年%m月%d日_%H:%M:%S");

					TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
					sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 发生错误-%s(%s)\n", pszTemp, strTime.GetBuffer(strTime.GetLength()));
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
				}

				//如果已经进行过TS流录制
				if (recording_state == 1)
				{
					//如果用户停止了录制
					if (pThreadParams->packet_decimate_thread_running == 0)
					{
						if (write_size > 0)
						{
							if (fp_record != NULL)
							{
								fwrite(record_buf, 1, write_size, fp_record);
								recording_size += write_size;

								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_RECORD_RATIO, (WPARAM)NULL, (LPARAM)recording_size);
							}

							write_size = 0;
							memset(record_buf, 0x00, sizeof(record_buf));
						}

						recording_state = 0;
						recording_size = 0;

						if (fp_record != NULL)
						{
							fclose(fp_record);
							fp_record = NULL;
						}

						pThreadParams->packet_decimate_thread_stopped = 1;

						sprintf_s(pszDebug, sizeof(pszDebug), "TS包录制: 结束（共录制%.3fM字节）", recording_size / 1000000.0);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_RECORD_THREAD, 0, NULL);
					}
				}
			}
		}

		ptransport_stream->StopGetData();

		//RSISI_REPORT_check_memory(pThreadParams->hMainWnd);

		//释放sectionsplicer的资源
		for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
		{
			SectionSplicer[filter_index].Reset();
		}

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REALTIME_THREAD, 0, NULL);

		pThreadParams->main_thread_stopped = 1;			//通知应用程序，主线程顺利退出

		thread_end_tickcount = ::GetTickCount();
		diff_tickcount = thread_end_tickcount - thread_start_tickcount;

		timeCurrent = COleDateTime::GetCurrentTime();
		strTime = timeCurrent.Format("%Y年%m月%d日 %H:%M:%S");

		sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 结束(%s)！(耗时%.3f s)", strTime.GetBuffer(strTime.GetLength()), diff_tickcount/1000.0);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		LOG(INFO) << pszDebug;
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "实时分析: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}

	if (fp_record != NULL)
	{
		fclose(fp_record);

//		if (pThreadParams->downloading == 1)
//		{
//			pThreadParams->downloading = 0;

//			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_THREAD, 0, NULL);
//		}
	}

//	assert(pThreadParams->packet_decimate_thread_stopped == 1);
}

uint32_t TSMagic_realtime_analyzer(LPVOID lpParam)
{
	ts_thread_params_t*	pThreadParams = (ts_thread_params_t*)lpParam;

	realtime_ts_analyzer(pThreadParams);

	return 0;
}

void realtime_ts_monitor(ts_thread_params_t* pThreadParams)
{
	char				pszDebug[MAX_TXT_CHARS];
	int					bitrate;
	int					get_count = 0;
	int					find_signal = 0;

	CTransportStream* ptransport_stream = NULL;
	CDB_TSPackets* pDB_TSPackets = NULL;
	RECORD_TSPacket_t*	pTS_packet_info = NULL;

	CDB_Pcrs* pDB_Pcrs = NULL;

	if (pThreadParams != NULL)
	{
		assert(pThreadParams->monitor_thread_running == 1);
		pThreadParams->monitor_thread_stopped = 0;

		sprintf_s(pszDebug, sizeof(pszDebug), "实时监测: 开始");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		sprintf_s(pszDebug, sizeof(pszDebug), "实时监测: 等待信号......");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_SIGNAL_STATUS, NULL, 1);

		pDB_TSPackets = pThreadParams->pDB_TSPackets;
		pDB_TSPackets->Reset();

		pDB_Pcrs = pThreadParams->pDB_Pcrs;
		pDB_Pcrs->Reset();

		ptransport_stream = pThreadParams->pTStream;
		ptransport_stream->StartGetBitrate();

		while (pThreadParams->monitor_thread_running == 1)
		{
			bitrate = (int)ptransport_stream->GetBitrate();

			if (bitrate > 0)
			{
				if (find_signal == 0)
				{
					sprintf_s(pszDebug, sizeof(pszDebug), "实时监测: 收到数据");
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

					find_signal = 1;
					pThreadParams->find_signal = 1;
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_SIGNAL_STATUS, NULL, 0);

					get_count = 0;
				}

				get_count ++;

				if (get_count > 1)
				{
					if (bitrate >= 1000000000)			
					{
						//虚假的速录，实际是达不到的
					}
					else
					{
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_BITRATE, (WPARAM)bitrate, NULL);
						pDB_TSPackets->m_total_bitrate_from_hardware = bitrate;
						//pDB_Pcrs->m_estimated_datarate_mean = bitrate;
					}
				}
			}
			else if (bitrate == 0)
			{
				if (find_signal == 1)
				{
					sprintf_s(pszDebug, sizeof(pszDebug), "实时监测: 数据丢失xxx");
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);

					find_signal = 0;
					pThreadParams->find_signal = 0;

					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_SIGNAL_STATUS, NULL, 1);
				}
			}
			else
			{
				Sleep(200);
			}
		}

		ptransport_stream->StopGetBitrate();

		pThreadParams->monitor_thread_stopped = 1;

		sprintf_s(pszDebug, sizeof(pszDebug), "实时监测: 结束");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "实时监测: 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
}

uint32_t TSMagic_realtime_monitor(LPVOID lpParam)
{
	ts_thread_params_t*	pThreadParams = (ts_thread_params_t*)lpParam;
	realtime_ts_monitor(pThreadParams);

	return 0;
}

void TSMagic_threadparams_init(ts_thread_params_t* pthread_params)
{
	if (pthread_params != NULL)
	{
		pthread_params->offline = OFFLINE_ANALYZE;
		
		pthread_params->main_thread_running = 0;
		pthread_params->main_thread_stopped = 1;

		pthread_params->packet_decimate_thread_running = 0;
		pthread_params->packet_decimate_thread_stopped = 1;

		pthread_params->dsmcc_download_thread_running = 0;
		pthread_params->dsmcc_download_thread_stopped = 1;

		pthread_params->monitor_thread_running = 0;
		pthread_params->monitor_thread_stopped = 1;

		pthread_params->ts_trigger_thread_running = 0;
//		pthread_params->ts_trigger_thread_stopped = 1;

		pthread_params->es_trigger_thread_running = 0;
		//pthread_params->es_trigger_thread_stopped = 1;

		pthread_params->section_trigger_thread_running = 0;
//		pthread_params->section_trigger_thread_stopped = 1;

		pthread_params->stream_option = STREAM_TS;
//		pthread_params->standard_option = 1;						//按照DVB标准分析

//		pthread_params->callback_report_streaminfo = REPORT_StreamInformation;
//		pthread_params->callback_report_progress = NULL;

		//pthread_params->pVidDecoder = NULL;
		//pthread_params->pAudDecoder = NULL;

//		pthread_params->file_read_offset = 0;
//		pthread_params->file_read_size = 0;
//		pthread_params->file_total_size = 0;

		pthread_params->nDecimateStyle = DECIMATE_NONE;

		memset(pthread_params->pszPathHeader, 0x00, sizeof(pthread_params->pszPathHeader));
		memset(pthread_params->pszPathName, 0x00, sizeof(pthread_params->pszPathName));
		memset(pthread_params->pszDecimatePath, 0x00, sizeof(pthread_params->pszDecimatePath));
		memset(pthread_params->pszVesFileName, 0x00, sizeof(pthread_params->pszVesFileName));
		memset(pthread_params->pszAesFileName, 0x00, sizeof(pthread_params->pszAesFileName));
	}
}

