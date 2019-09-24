#include "stdafx.h"

#include "..\Common\define.h"

#include "TSMagic_Callbacks_From_Lib.h"
#include "TSMagic_Callbacks_To_Gui.h"

#include "TSMagic_AnalyseThread.h"
#include "TSMagic_AnalyseThread_MSG.h"
#include "TSMagic_ErrorCode.h"

#include "TSMagic_Trigger.h"
#include "TSMagic_Trigger_Section.h"

#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_SectionSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/Mpeg2_PESSplicer.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/MiddleWare_PesSplicer_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_PayloadSplicer/Include/MiddleWare_SectionSplicer_ErrorCode.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TS_ErrorCode.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_Utilities.h"

#include "thirdparty_libs\glog\glog\logging.h"

#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <assert.h>


uint32_t TSMagic_AnalyseThread_offline_main(LPVOID lpParam)
{
	uint8_t*  section_buf;
	int		  section_length;

	uint8_t	  packet_buf[204];
	int		  packet_length;

	char	  pszDebug[256];
	char	  pszTemp[128];

#if TS_FILE_READ_INTEGRITY_DIAGNOSIS
	char  pszCheckFile[128];
	FILE* fpCheckFile = NULL;
#endif

	double file_size_div_100;
	int64_t	  read_byte_pos = 0;			//ֻ�ܶ�С��2G���ļ�

	int			  old_ratio = 0;
	int			  analyse_ratio = 0;
	uint32_t	  thread_start_tickcount;
	uint32_t	  thread_finish_tickcount;
	time_t		  syncread_start_time;
	int			  diff_tickcount;

	int	  stream_synced = 0;

	COleDateTime	timeCurrent;
	CString			strTime;

	int					rtcode;
	int					filter_index;
	CSectionSplicer		SectionSplicer[MAX_SECTION_FILTERS];
	CSectionSplicer* pSectionSplicer;
	CPESSplicer			PESSplicer;

	CTransportStream* ptransport_stream = NULL;
	transport_packet_t	transport_packet;

	RECORD_TSPacket_t		TSPacketInfo;
	CDB_TSPackets* pDB_TSPackets = NULL;
	CDB_Pcrs* pDB_Pcrs = NULL;
	CDB_PsiSiObjs* pDB_PsiSiObjs = NULL;

	CTrigger_PsiSiSection* pSectionTrigger = NULL;
	CTrigger_TSPacket* pTSPacketTrigger = NULL;
	CTrigger_PESPacket* pPESPacketTrigger = NULL;

	ts_thread_params_t* pThreadParams = (ts_thread_params_t*)lpParam;

	if (pThreadParams != NULL)
	{
		assert(pThreadParams->main_thread_running == 1);		//ֻ���� ȷ���ǲ����ϲ�Ӧ��֪ͨ�߳����е�
		pThreadParams->main_thread_stopped = 0;

		thread_start_tickcount = ::GetTickCount();

		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_OFFLINE_THREAD, 1, NULL);

		timeCurrent = COleDateTime::GetCurrentTime();
		strTime = timeCurrent.Format("%Y��%m��%d�� %H:%M:%S");

		sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: ��ʼ(%s)", strTime.GetBuffer(strTime.GetLength()));
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

		ptransport_stream->StartGetData(0);			//������������

		file_size_div_100 = ptransport_stream->m_llTotalFileLength / 100.0;

#if TS_FILE_READ_INTEGRITY_DIAGNOSIS
		sprintf_s(pszCheckFile, sizeof(pszCheckFile), "e:\\temp\\check.ts");
		fopen_s(&fpCheckFile, pszCheckFile, "wb");
#endif
		while (pThreadParams->main_thread_running == 1)
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
						sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: �ҵ�ͬ��, TS���� %d�ֽڣ��ļ�λ�ã�0x%llx��\n", packet_length, read_byte_pos);

						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
						LOG(INFO) << pszDebug;

						syncread_start_time = time(NULL);
						break;
					}
					else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
					{
						break;
					}
				} while (pThreadParams->main_thread_running == 1);
			}
			else if (stream_synced == 1)
			{
				packet_length = sizeof(packet_buf);
				rtcode = ptransport_stream->SyncReadOnePacket(packet_buf, &packet_length);
				read_byte_pos = ptransport_stream->Tell();			//��Ϊ��Ԥ��һ������ʵ��FIFO��ָ�벢δ�����ƶ�
				uint32_t read_tickcount = ::GetTickCount();
				time_t read_time = time(NULL);

				int second_change = ((read_time - syncread_start_time) >= 2) ? 1 : 0;
				if (second_change)
				{
					syncread_start_time = read_time;
				}

				if (rtcode == MIDDLEWARE_TS_NO_ERROR)
				{
#if TS_FILE_READ_INTEGRITY_DIAGNOSIS
					if (fpCheckFile != NULL)
					{
						fwrite(packet_buf, sizeof(char), packet_length, fpCheckFile);
					}
#endif

#if GUI_REPORT_FILE_ANALYSE_RATIO
					//�����㱨�ļ���������
					if (ptransport_stream->m_llTotalFileLength > 0)
					{
						analyse_ratio = (uint32_t)(read_byte_pos / file_size_div_100);
						if (analyse_ratio > old_ratio)
						{
							old_ratio = analyse_ratio;
							::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_RATIO, (WPARAM)NULL, (LPARAM)analyse_ratio);
						}
					}
#endif

#if OPEN_TS_PACKET_ANALYZER
					//�Ը�TS�������﷨����
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
								//							if (nOldCatchedCount == 0)		//��׽����һ��ƥ��TS��ʱ����״̬
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
								//TS_PID�����׽��PES��һ��
								rtcode = PESSplicer.WriteTSPacket(&transport_packet);
								if (rtcode == PESES_SPLICE_LAST_PACKET)
								{
									int pes_length;
									uint8_t* pes_buf = PESSplicer.GetPESPacket(&pes_length);
									pPESPacketTrigger->SaveTheWholePacket(pes_buf, pes_length);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 2, 0);
									::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_PES_TRIGGER_STATE, 0, 0);

									PESSplicer.Reset();
								}
							}
						}
#endif

#if OPEN_PCR_ANALYZER
						//���߷�������µ�PCR���
						if (transport_packet.adaptation_field.PCR_flag)
						{
							PCR_code_t pcr_code;
							pcr_code.base_32_30 = transport_packet.adaptation_field.program_clock_reference_base_32_30;
							pcr_code.base_29_15 = transport_packet.adaptation_field.program_clock_reference_base_29_15;
							pcr_code.base_14_0 = transport_packet.adaptation_field.program_clock_reference_base_14_0;
							pcr_code.extension = transport_packet.adaptation_field.program_clock_reference_extension;

							//BITRATE_ATTRIBUTE_t old_bitrate_attr;
							//ptransport_stream->GetMeasuredBitrateAttribute(&old_bitrate_attr);
							int referenceBitrate = ptransport_stream->GetBitrate();

							rtcode = pDB_Pcrs->AddPCRSample(transport_packet.PID, read_byte_pos - packet_length + 12, &pcr_code, referenceBitrate, CALLBACK_REPORT_PCR_Observation);
							if (rtcode == MIDDLEWARE_DB_NO_ERROR)
							{
								RECORD_PCR_t PCRRecord;
								//���û�ȡ�����ķ�ʽ����ֹ�ӳ����ԭʼ���ݽ����޸�
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
							}
						}
#endif

#if OPEN_PACKET_STATISTIC
						rtcode = pDB_TSPackets->AddPacket(&transport_packet);
						if (rtcode != NO_ERROR)
						{
							//��������д��󣬻㱨����״̬
							if (rtcode == ETR290_CONTINUITY_COUNT_ERROR)
							{
								sprintf_s(pszDebug, sizeof(pszDebug), "���߷���:TS��ͳ�ơ����������������ļ�λ�ã�0x%llx, PID = 0x%04x��\n", read_byte_pos, transport_packet.PID);
								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
								LOG(ERROR) << pszDebug;
							}
							else
							{
								TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
								sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: TS��ͳ�ơ���%s���ļ�λ�ã�0x%llx��\n", pszTemp, read_byte_pos);

								::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
								LOG(INFO) << pszDebug;
							}
						}

#if GUI_REPORT_TS_STATISTIC
						if (second_change)
						{
							//ÿ��1s�㱨һ�α�����
							if (pDB_TSPackets->callback_gui_update != NULL)
							{
								pDB_TSPackets->callback_gui_update((int)ptransport_stream->GetBitrate(), NULL);
							}
						}
#endif

#endif					

#if OPEN_SECTION_SPLICER
						//pTS_packet_info = pDB_TSPackets->GetRecord(transport_packet.PID);
						if (pDB_TSPackets->GetRecord(transport_packet.PID, &TSPacketInfo) == NO_ERROR)
						{
							if (TSPacketInfo.ucClass == TSPAYLOAD_CLASS_SECTION)
							{
								//����Ѱ���Ƿ��п��õĹ�����
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
									//˵��û���ҵ�

									//Ѱ�ҵ�һ�����õ�FILTER
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
										sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: sectionƴ�ӡ�������section_filter���������!");
										::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
										LOG(ERROR) << pszDebug;
									}
								}

								if (pSectionSplicer != NULL)
								{
									do
									{
										rtcode = pSectionSplicer->WriteTSPacket(&transport_packet);

										if (rtcode == SECTION_SPLICE_FIRST_PACKET)
										{
											//�������󣬲���Ҫ����
										}
										else if (rtcode == SECTION_SPLICE_FOLLOW_PACKET)
										{
											//�������󣬲���Ҫ����
										}
										else if ((rtcode == SECTION_SPLICE_LAST_PACKET) || (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START))
										{
											//if (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START)
											//{
											//	rtcode = SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START;
											//}

											//���SECTIONƴ�ӳɹ��������section����

											section_buf = pSectionSplicer->m_pucSectionBuf;
											section_length = pSectionSplicer->m_nSectionLength;

											int report_rtcode = REPORT_PSISI_section(pDB_PsiSiObjs, pThreadParams->hMainWnd, pSectionSplicer->m_usPID, section_buf, section_length);
											if (report_rtcode != TSMAGIC_NO_ERROR)
											{
												if (report_rtcode == TSMAGIC_NOT_SUPPORTED_TABLE)
												{
													sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: section���桪����TS_PID=0x%04X�Ϸ���δʶ���PSI/SI��(table_id=0x%02x)���ļ�λ�ã�0x%llx��\n", pSectionSplicer->m_usPID, pSectionSplicer->m_ucTableID, read_byte_pos);

													::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
													LOG(ERROR) << pszDebug;
												}
												else if (report_rtcode == MIDDLEWARE_PSISI_VERSION_CHANGE)
												{
													sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: section���桪��PSI/SI��(TS_PID=0x%04X, table_id=0x%02x)�汾����������ļ�λ�ã�0x%llx��\n", pSectionSplicer->m_usPID, section_buf[0], read_byte_pos);

													::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
													LOG(INFO) << pszDebug;
												}
												else if (report_rtcode == MIDDLEWARE_PSISI_DUPLICATED_SECTION)
												{
													//											sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: section���桪��PSI/SI���ظ��Ķ�(TS_PID=0x%04X, table_id=0x%02X)�汾����������ļ�λ�ã�0x%llx��\n", pSectionSplicer->m_usPID, section_buf[0], read_byte_pos);
													//
													//											::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
												}
												else
												{
													TSMagic_ErrorCodeLookup(report_rtcode, pszTemp, sizeof(pszTemp));
													sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: section���桪��PSI/SI��(TS_PID=0x%04X, table_id=0x%02X) %s���ļ�λ�ã�0x%llx��\n", pSectionSplicer->m_usPID, section_buf[0], pszTemp, read_byte_pos);

													::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
													LOG(INFO) << pszDebug;
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

													if (nOldCatchedCount == 0)		//��׽����һ��ƥ��sectionʱ����״̬
													{
														::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_SECTION_TRIGGER_STATE, 0, 0);
													}
												}
											}
#endif
											//���ʹ�����ͷŵ�ǰsectionƴ������Դ
											pSectionSplicer->Reset();
										}
										else if (rtcode == SECTION_SPLICE_DO_NOT_SYNC)
										{
											//�������󣬲���Ҫ����
										}
										else if (rtcode == SECTION_SPLICE_PADDING_BYTES)
										{
											//�������󣬲���Ҫ����
										}
										else if (rtcode == SECTION_SPLICE_TRANSPORT_ERROR)
										{
											sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: sectionƴ�ӡ���TS��(TS_PID=0x%04X)��ͷָʾ��������ļ�λ�ã�0x%llx��\n", transport_packet.PID, read_byte_pos);
											::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_WARNING);
											LOG(INFO) << pszDebug;
										}
										else if (rtcode == SECTION_SPLICE_CONTINUITY_ERROR)
										{
											sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: sectionƴ�ӡ���TS��(TS_PID=0x%04X)��Ų��������ļ�λ�ã�0x%llx��\n", transport_packet.PID, read_byte_pos);
											::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
											LOG(INFO) << pszDebug;
										}
										else
										{
											TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
											sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: sectionƴ�ӡ���%s��TS_PID=0x%04X, �ļ�λ�ã�0x%llx��\n", pszTemp, transport_packet.PID, read_byte_pos);
											::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
											LOG(INFO) << pszDebug;
										}

									} while (rtcode == SECTION_SPLICE_LAST_PACKET_WITH_ANOTHER_START);
								}
							}
						}
#endif
					}
					else
					{
						//����TS���﷨���ִ���Ϊʲô�������󣿣�����
						TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
						sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: [PID=0x%04X] %s���ļ�λ�ã���ʼ��0x%llx - ������0x%llx��\n", transport_packet.PID, pszTemp, read_byte_pos - packet_length, read_byte_pos);

						::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
						LOG(INFO) << pszDebug;
					}
#endif
				}
				else if (rtcode == ETR290_TS_SYNC_LOSS)
				{
					//��ȡTS���������󡪡�ͬ����ʧ
					stream_synced = 0;

					sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: ��ʧͬ�����ļ�λ�ã�0x%llx��\n", read_byte_pos);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;
				}
				else if (rtcode == MIDDLEWARE_TS_FILE_EOF_ERROR)
				{
					break;
				}
				else if (rtcode == MIDDLEWARE_TS_FIFO_EMPTY_ERROR)
				{
					//Sleep(1);
				}
				else
				{
					assert(0);

					//��ȡTS����������
					TSMagic_ErrorCodeLookup(rtcode, pszTemp, sizeof(pszTemp));
					sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: %s���ļ�λ�ã�0x%llx��\n", pszTemp, read_byte_pos);

					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(INFO) << pszDebug;
				}
			}
		}

		ptransport_stream->StopGetData();			//ֹͣ��������

#if TS_FILE_READ_INTEGRITY_DIAGNOSIS
		if (fpCheckFile != NULL)
		{
			fclose(fpCheckFile);
			fpCheckFile = NULL;
		}
#endif
		//����ڴ�ռ�����
		//RSISI_REPORT_check_memory(pThreadParams->hMainWnd);

		//�ͷ�sectionsplicer����Դ
		for (filter_index = 0; filter_index < MAX_SECTION_FILTERS; filter_index++)
		{
			SectionSplicer[filter_index].Reset();
		}

#if OPEN_PACKET_STATISTIC
		int		error_count = 0;
		int		i;
		//RECORD_TSPacket_t* pRecord;

		for (i = 0; i < MAX_PID_COUNT; i++)
		{
			//pRecord = pDB_TSPackets->GetRecord(i);
			if (pDB_TSPackets->GetRecord(i, &TSPacketInfo) == NO_ERROR)
			{
				error_count = TSPacketInfo.continuity_counter_error;
				if (error_count > 0)
				{
					sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: ������������PID = 0x%04x, error_count = %d��\n", i, error_count);
					//::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_ETR290_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
					LOG(ERROR) << pszDebug;
				}
			}
		}

		if (pDB_TSPackets->callback_gui_update != NULL)
		{
			pDB_TSPackets->callback_gui_update((int)ptransport_stream->GetBitrate(), NULL);
		}
#endif	

		//�����ļ���λ
//		ptransport_stream->SeekToBegin();

		thread_finish_tickcount = ::GetTickCount();
		diff_tickcount = thread_finish_tickcount - thread_start_tickcount;

		timeCurrent = COleDateTime::GetCurrentTime();
		strTime = timeCurrent.Format("%Y��%m��%d�� %H:%M:%S");

		sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: ����(%s)��(��ʱ%.3f s)", strTime.GetBuffer(strTime.GetLength()), diff_tickcount / 1000.0);
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_CLOSE);
		LOG(INFO) << pszDebug;

		pThreadParams->main_thread_stopped = 1;			//֪ͨӦ�ó������߳�˳���˳�

		if (pThreadParams->main_thread_running == 0)
		{
			//˵�����û�ǿ��Ҫ���˳����̣߳��������������
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_OFFLINE_THREAD, 0, NULL);			//ǿ�ƽ���
		}
		else
		{
			//˵���û���û��ֹͣ�������ǵ����ļ�β�߳��Զ��˳�
			analyse_ratio = 100;
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_REPORT_RATIO, (WPARAM)NULL, (LPARAM)analyse_ratio);

			//˵�������߷��������������˳����̣߳���������������
			::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_OFFLINE_THREAD, 2, NULL);			//���߷�������
		}
		}
	else
	{
		sprintf_s(pszDebug, sizeof(pszDebug), "���߷���: �����������");
		::SendMessage(pThreadParams->hMainWnd, WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_ERROR);
		LOG(INFO) << pszDebug;
	}

	return 0;
}

