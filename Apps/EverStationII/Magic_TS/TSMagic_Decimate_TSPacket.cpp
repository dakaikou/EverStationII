#include "stdafx.h"
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>

#include "..\Common\define.h"

#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_ErrorCode.h"
#include "TSMagic_Decimate_TSPacket.h"

#include "syntax_translate_layer/Mpeg_PESPacket/Include/MPEG_stream_id.h"
#include "syntax_translate_layer/Mpeg2_TSPacket\Include\Mpeg2_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TS_ErrorCode.h"
#include "thirdparty_libs\glog\glog\logging.h"

void ts2es_decimate_loop(ts_thread_params_t* pThreadParams)
{
	uint8_t	  ts_packet_buf[204];
	int		  ts_packet_length;
	char	  pszDebug[MAX_TXT_CHARS];
	char	  pszEsFile[MAX_PATH];

	float file_size_div_100;
	int64_t	  read_byte_pos = 0;
	int	  decimate_size = 0;

	int	  old_ratio = 0;
	int	  analyse_ratio = 0;
	int	  rtcode;
	COleDateTime	timeCurrent;
	CString			strTime;

	int		  stream_synced = 0;

	CTransportStream* ptransport_stream = NULL;
	transport_packet_t			transport_packet;

	CDB_TSPackets* pDB_TSPackets = NULL;
	RECORD_TSPacket_t			TSPacketInfo;

	uint16_t							usPID;
	FILE*						fp = NULL;

	uint8_t		adaptation_field_length;
	uint8_t		payload_length;
	uint8_t*		ts_payload_buf;
	uint8_t*		pes_buf = NULL;
	uint8_t		pes_header_length;
	uint8_t*		es_buf = NULL;
	bool	es_payload_start = 0;

	uint32_t		start_code;
	uint8_t		stream_id;

	if (pThreadParams != NULL)
	{
		pThreadParams->packet_decimate_thread_running = 1;
		pDB_TSPackets = pThreadParams->pDB_TSPackets;

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_THREAD, 1, NULL);

//		curTime = CTime::GetCurrentTime();
		timeCurrent = COleDateTime::GetCurrentTime();
		strTime = timeCurrent.Format("%Y%m%d_%H%M%S");

		sprintf_s(pszEsFile, sizeof(pszEsFile), "%s\\%s.es", pThreadParams->pszDecimatePath, strTime.GetBuffer(strTime.GetLength()));
		
		sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->ES): 开始, 文件存储路径=%s", pszEsFile);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		fopen_s(&fp, pszEsFile, "wb");
		if (fp != NULL)
		{
//			TRIGGER_TS_PACKET.packet_length = ptransport_stream->m_nPacketLength;

			ptransport_stream = pThreadParams->pTStream;
			file_size_div_100 = ptransport_stream->m_llTotalFileLength / 100.0f;
			
			ptransport_stream->StartGetData(0);

			while (pThreadParams->packet_decimate_thread_running == 1)
			{
				//first synchronize the stream
				if (stream_synced == 0)
				{
					do
					{
						rtcode = ptransport_stream->Synchronize(&ts_packet_length);
						if (rtcode == MIDDLEWARE_TS_NO_ERROR)
						{
							stream_synced = 1;

							read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动
							sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->ES): 找到同步, TS包长 %d字节（文件位置：0x%llx）\n", ts_packet_length, read_byte_pos);

							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
							LOG(INFO) << pszDebug;
							break;
						}
						else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
						{
							break;
						}
					} while (pThreadParams->packet_decimate_thread_running == 1);
				}
				else if (stream_synced == 1)
				{
					ts_packet_length = sizeof(ts_packet_buf);
					rtcode = ptransport_stream->SyncReadOnePacket(ts_packet_buf, &ts_packet_length);
					read_byte_pos = ptransport_stream->Tell();
					if (rtcode == MIDDLEWARE_TS_NO_ERROR)
					{
						analyse_ratio = (uint32_t)(read_byte_pos / file_size_div_100);
						if (analyse_ratio > old_ratio)
						{
							old_ratio = analyse_ratio;
							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_RATIO, (WPARAM)NULL, (LPARAM)analyse_ratio);
						}

						//成功读出一个TS包
						rtcode = MPEG_decode_TS_packet(ts_packet_buf, ts_packet_length, &transport_packet);

						if (rtcode == TSPACKET_PARSE_NO_ERROR)
						{
							usPID = transport_packet.PID;

							rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);
							if (rtcode == NO_ERROR)
							{
								if (TSPacketInfo.bDecimate == 1)
								{
									if (transport_packet.transport_error_indicator == 0)
									{
										if (transport_packet.transport_scrambling_control == 0)		//not scrambled
										{
											if ((transport_packet.adaptation_field_control & 0x02) != 0x00)					//has adaptation field
											{
												adaptation_field_length = ts_packet_buf[4];
												ts_payload_buf = ts_packet_buf + 5 + adaptation_field_length;
											}
											else
											{
												adaptation_field_length = 0;
												ts_payload_buf = ts_packet_buf + 4;
											}

											payload_length = 188 - (unsigned char)(ts_payload_buf - ts_packet_buf);

											//get pes stream
											if (transport_packet.payload_unit_start_indicator)
											{
												//discard the pes header
												start_code = ts_payload_buf[0];
												start_code <<= 8;
												start_code |= ts_payload_buf[1];
												start_code <<= 8;
												start_code |= ts_payload_buf[2];

												stream_id = ts_payload_buf[3];

												if (start_code == 0x000001)					//payload maybe ES
												{
													if ((stream_id != PROGRAM_STREAM_MAP) &&
														(stream_id != PADDING_STREAM) &&
														(stream_id != PRIVATE_STREAM_2) &&
														(stream_id != ECM_STREAM) &&
														(stream_id != EMM_STREAM) &&
														(stream_id != PROGRAM_STREAM_DIRECTORY) &&
														(stream_id != DSMCC_STREAM) &&
														(stream_id != TYPE_E_STREAM))
													{
														//识别为PES流
														pes_buf = ts_payload_buf;

														uint8_t mark_bits = (pes_buf[6] & 0xc0) >> 6;
														uint8_t PES_scrambling_control = (pes_buf[6] & 0x30) >> 4;
														uint8_t PES_priority = (pes_buf[6] & 0x08) >> 3;
														uint8_t data_alignment_indicator = (pes_buf[6] & 0x04) >> 2;
														uint8_t copyright = (pes_buf[6] & 0x02) >> 1;
														uint8_t original_or_copy = (pes_buf[6] & 0x01) >> 0;

														if ((mark_bits == 0x02) && (PES_scrambling_control == 0x00))
														{
															//														if (data_alignment_indicator == 1)
															//														{
															//														}
															//														else
															//														{
															//														}

															pes_header_length = pes_buf[8] + 9;
															es_buf = pes_buf + pes_header_length;
															payload_length -= pes_header_length;

															//如果ES流还没有对齐，则先对齐
															if (es_payload_start == 0)
															{
																if ((stream_id >= AUDIO_STREAM_MIN) && (stream_id <= AUDIO_STREAM_MAX))      //0xC0~0xDF
																{
																	if (data_alignment_indicator == 1)
																	{
																		uint16_t audio_sync_word = (es_buf[0] << 8) | es_buf[1];			//获取音频同步字

																		if ((audio_sync_word & 0xFFF0) == 0xFFF0)
																		{
																			es_payload_start = 1;
																		}
																	}
																	else
																	{
																		es_payload_start = 1;
																	}
																}
																else if ((stream_id >= VIDEO_STREAM_MIN) && (stream_id <= VIDEO_STREAM_MAX))		//0xE0~EF
																{
																	//																uint32_t es_start_code = (es_buf[0] << 24) | (es_buf[1] << 16) | (es_buf[2] << 8) | es_buf[3];
																	uint32_t es_start_code = es_buf[0];
																	es_start_code <<= 8;
																	es_start_code |= es_buf[1];
																	es_start_code <<= 8;
																	es_start_code |= es_buf[2];
																	es_start_code <<= 8;
																	es_start_code |= es_buf[3];

																	if ((es_start_code & 0xFFFFFF00) == 0x00000100)			//mpeg video start_code
	//																if (es_start_code == 0x000001B3)						//sequence_start_code
																	{
																		es_payload_start = 1;
																	}
																	else if (es_start_code == 0x00000000)				//h.264 video start_code
																	{
																		es_payload_start = 1;
																	}
																}
																else if (stream_id == PRIVATE_STREAM_1)
																{
																	uint16_t es_start_code = (es_buf[0] << 8) | es_buf[1];

																	if (es_start_code == 0x0B77)			//AC-3 stream
																	{
																		es_payload_start = 1;
																	}
																	else
																	{
																	}
																}
															}
														}
														else
														{
															payload_length = 0;					//有错误，丢弃这个包
														}
													}
													else if ((stream_id == PROGRAM_STREAM_MAP) ||
														(stream_id == PRIVATE_STREAM_2) ||
														(stream_id == ECM_STREAM) ||
														(stream_id == EMM_STREAM) ||
														(stream_id == PROGRAM_STREAM_DIRECTORY) ||
														(stream_id == DSMCC_STREAM) ||
														(stream_id == TYPE_E_STREAM))
													{
														es_buf = pes_buf + 6;
														payload_length -= 6;

														if (es_payload_start == 0)
														{
															es_payload_start = 1;
														}
													}
													else if (stream_id == PADDING_STREAM)
													{
														payload_length = 0;
													}
													else
													{
														payload_length = 0;
													}
												}
												else
												{
													payload_length = 0;
												}
											}
											else
											{
												es_buf = ts_payload_buf;
											}

											if (es_payload_start == 1)
											{
												if (payload_length > 0)
												{
													fwrite(es_buf, 1, payload_length, fp);
													decimate_size += payload_length;
												}
											}
										}
									}
								}
							}
						}
						else
						{
							//解析TS包语法发现错误，为什么发生错误？？？？
							sprintf_s(pszDebug, sizeof(pszDebug), "TS分解(TS->ES): 文件位置0x%llx 发生错误", read_byte_pos);
							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
						}
					}
					else if (rtcode == ETR290_TS_SYNC_LOSS)
					{
						//读取TS包发生错误——同步丢失
						stream_synced = 0;

						sprintf_s(pszDebug, sizeof(pszDebug), "TS分解(TS->ES): 丢失同步（文件位置：0x%llx）\n", read_byte_pos);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
						LOG(INFO) << pszDebug;
					}
					else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
					{
						break;
					}
					else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
					{
						Sleep(10);
					}
				}
			}

			pThreadParams->packet_decimate_thread_running = 0;

			ptransport_stream->StopGetData();

			fclose(fp);
		}
		else
		{
			sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->ES): 打开文件 %s 错误", pszEsFile);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
		}

		for (usPID = 0x0000; usPID <= 0x1FFF; usPID ++)
		{
			pDB_TSPackets->SetDecimateFlag(usPID, 0);
			//rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);
			//if (pPacketInfo != NULL)
			//{
			//	pPacketInfo->bDecimate = 0;
			//}
		}

		sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->ES): 结束(共录制%.3fM字节)", decimate_size / 1000000.0f);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_THREAD, 0, NULL);

		pThreadParams->packet_decimate_thread_stopped = 1;    //通知应用程序，抽选线程已经退出
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->ES): 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
}

uint32_t TSMagic_ts2es_decimate_thread(LPVOID lpParam)
{
	ts_thread_params_t*	pThreadParams = (ts_thread_params_t*)lpParam;
	ts2es_decimate_loop(pThreadParams);

	return 0;
}

//仅用于离线分析时的TS->TS抽选
void ts2ts_decimate_loop(ts_thread_params_t* pThreadParams)
{
	uint8_t	  packet_buf[204];
	int		  packet_length;
	char	  pszDebug[MAX_TXT_CHARS];
	char	  pszTsFile[MAX_PATH];

	float file_size_div_100;
	int64_t	  read_byte_pos = 0;
	int	  decimate_size = 0;

	int	  old_ratio = 0;
	int	  analyse_ratio = 0;
	int	  rtcode;
//	int   getdata_rtcode;
	COleDateTime	timeCurrent;
	CString			strTime;

	int		  stream_synced = 0;

	CTransportStream* ptransport_stream = NULL;
	transport_packet_t			transport_packet;

	CDB_TSPackets* pDB_TSPackets = NULL;
	RECORD_TSPacket_t			TSPacketInfo;

	uint16_t					usPID;
	FILE*						fp = NULL;

	if (pThreadParams != NULL)
	{
		pThreadParams->packet_decimate_thread_running = 1;		//通知应用进入抽选线程

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_THREAD, 1, NULL);

		timeCurrent = COleDateTime::GetCurrentTime();
		strTime = timeCurrent.Format("%Y%m%d_%H%M%S");

		sprintf_s(pszTsFile, sizeof(pszTsFile), "%s\\%s.ts", pThreadParams->pszDecimatePath, strTime.GetBuffer(strTime.GetLength()));
		sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->TS): 开始, 文件存储路径=%s", pszTsFile);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);

		fopen_s(&fp, pszTsFile, "wb");
		if (fp != NULL)
		{
//			TRIGGER_TS_PACKET.packet_length = ptransport_stream->m_nPacketLength;
			pDB_TSPackets = pThreadParams->pDB_TSPackets;

			ptransport_stream = pThreadParams->pTStream;
			ptransport_stream->StartGetData(0);

			file_size_div_100 = ptransport_stream->m_llTotalFileLength / 100.0f;

			while (pThreadParams->packet_decimate_thread_running == 1)
			{
				if (stream_synced == 0)
				{
					do
					{
						rtcode = ptransport_stream->Synchronize(&packet_length);
						if (rtcode == MIDDLEWARE_TS_NO_ERROR)
						{
							stream_synced = 1;

							read_byte_pos = ptransport_stream->Tell();			//因为是预读一个包，实际FIFO读指针并未发生移动
							sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->TS): 找到同步, TS包长 %d字节（文件位置：0x%llx）\n", packet_length, read_byte_pos);

							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
							LOG(INFO) << pszDebug;
							break;
						}
						else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
						{
							break;
						}
					} while (pThreadParams->packet_decimate_thread_running == 1);
				}
				else if (stream_synced == 1)
				{
					packet_length = sizeof(packet_buf);
					rtcode = ptransport_stream->SyncReadOnePacket(packet_buf, &packet_length);
					read_byte_pos = ptransport_stream->Tell();
					if (rtcode == NO_ERROR)
					{
						analyse_ratio = (uint32_t)(read_byte_pos / file_size_div_100);
						if (analyse_ratio > old_ratio)
						{
							old_ratio = analyse_ratio;
							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_RATIO, (WPARAM)NULL, (LPARAM)analyse_ratio);
						}

						//成功读出一个TS包
						rtcode = MPEG_decode_TS_packet(packet_buf, packet_length, &transport_packet);

						if (rtcode == TSPACKET_PARSE_NO_ERROR)
						{
							usPID = transport_packet.PID;

							rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);
							if (rtcode == NO_ERROR)
							{
								if (TSPacketInfo.bDecimate == 1)
								{
									fwrite(packet_buf, 1, packet_length, fp);
									decimate_size += packet_length;
								}
							}
						}
						else
						{
							//解析TS包语法发现错误，为什么发生错误？？？？
							sprintf_s(pszDebug, sizeof(pszDebug), "TS分解(TS->TS): 文件位置0x%llx 发生错误", read_byte_pos);
							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_OPEN);
						}
					}
					else if (rtcode == ETR290_TS_SYNC_LOSS)
					{
						//读取TS包发生错误——同步丢失
						stream_synced = 0;

						sprintf_s(pszDebug, sizeof(pszDebug), "TS分解(TS->TS): 丢失同步（文件位置：0x%llx）\n", read_byte_pos);
						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
						LOG(INFO) << pszDebug;
					}
					else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
					{
						break;
					}
					else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
					{
						//Sleep(10);
					}
				}
			}

			pThreadParams->packet_decimate_thread_running = 0;

			ptransport_stream->StopGetData();

			fclose(fp);
		}
		else
		{
			sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->TS): 打开文件 %s 错误", pszTsFile);
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
		}

		for (usPID = 0x0000; usPID <= 0x1FFF; usPID ++)
		{
			pDB_TSPackets->SetDecimateFlag(usPID, 0);
			//pPacketInfo = pDB_TSPackets->GetRecord(usPID);
			//if (pPacketInfo != NULL)
			//{
			//	pPacketInfo->bDecimate = 0;
			//}
		}

		sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->TS): 结束（共录制%.3fM字节）", decimate_size / 1000000.0f);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_DECIMATE_THREAD, 0, NULL);

		pThreadParams->packet_decimate_thread_stopped = 1;    //通知应用程序，抽选线程已经退出
	}
	//else
	//{
	//	sprintf_s(pszDebug, sizeof(pszDebug), "TS包分解(TS->TS): 输入参数错误！");
	//	::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
	//}
}

uint32_t TSMagic_ts2ts_decimate_thread(LPVOID lpParam)
{
	ts_thread_params_t*	pThreadParams = (ts_thread_params_t*)lpParam;
	ts2ts_decimate_loop(pThreadParams);

	return 0;
}



