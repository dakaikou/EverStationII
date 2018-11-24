#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <assert.h>
#include <math.h>

#include "../Include/MiddleWare_TransportStream.h"
#include "../Include/MiddleWare_TS_ErrorCode.h"

#include "streamio_layer/SIOWithDektec/Include/SIOWithDektec.h"
#include "streamio_layer/SIOWithUDP/Include/SIOWithUDP.h"
#include "streamio_layer/SIOWithFile/Include/SIOWithFile.h"
#include "thirdparty_HW/SmartTS/smartts_drvapi.h"

#include "toolbox_libs\TOOL_Directory\Include\TOOL_Directory.h"


uint32_t thread_receive_transport_stream(LPVOID lpParam)
{
	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	uint8_t	buf[30 * 1024];
	int rdsize;

	CTransportStream*	ptransport_stream = (CTransportStream*)lpParam;

	if (ptransport_stream != NULL)
	{
		while (ptransport_stream->m_nRunning)
		{
			if (ptransport_stream->m_nReceiving)
			{
				while (ptransport_stream->m_nReceiving)
				{
					if (strcmp(ptransport_stream->m_pszProtocolHead, "FILE") == 0)				//离线分析模式，读文件
					{
						if (ptransport_stream->m_nEOF == 0)
						{
							if (ptransport_stream->m_ts_fifo.GetAvailableSpace() >= sizeof(buf))
							{
								rdsize = file_receive(buf, sizeof(buf));
								if (rdsize > 0)
								{
#if USE_FIFO_ACCESS_MUTEX
									uint32_t wait_state = ::WaitForSingleObject(ptransport_stream->m_hFifoAccess, INFINITE);
									if (wait_state == WAIT_OBJECT_0)
									{
#endif
										rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
#if USE_FIFO_ACCESS_MUTEX
										::ReleaseMutex(ptransport_stream->m_hFifoAccess);
									}
#endif
								}
								else if (rdsize == 0)
								{
									ptransport_stream->m_nEOF = 1;
									rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
								}
								else
								{
									assert(0);
									rtcode = MIDDLEWARE_TS_STREAM_NODATA_ERROR;				//没有采集到实时流
								}
							}
						}
						else
						{
							//正在文件尾，空转，等待应用重置文件指针
							Sleep(100);
						}
					}
					else if (strcmp(ptransport_stream->m_pszProtocolHead, "ASI") == 0)
					{
						if (strcmp(ptransport_stream->m_pszProtocolExt, "SMARTTS") == 0)
						{
							if (ptransport_stream->m_ts_fifo.GetAvailableSpace() >= 4096)
							{
#ifdef _WIN64
								rdsize = 0;
#else
								rdsize = smartts_receive(buf, 4096);
#endif
								if (rdsize > 0)
								{
#if USE_FIFO_ACCESS_MUTEX
									uint32_t wait_state = ::WaitForSingleObject(ptransport_stream->m_hFifoAccess, INFINITE);
									if (wait_state == WAIT_OBJECT_0)
									{
#endif
										rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
#if USE_FIFO_ACCESS_MUTEX
										::ReleaseMutex(ptransport_stream->m_hFifoAccess);
									}
#endif
								}
								else
								{
									rtcode = MIDDLEWARE_TS_STREAM_NODATA_ERROR;				//没有采集到实时流
								}
							}

						}
						else if (strcmp(ptransport_stream->m_pszProtocolExt, "DEKTEC") == 0)
						{
							if (ptransport_stream->m_ts_fifo.GetAvailableSpace() >= sizeof(buf))
							{
#ifdef _WIN64
								rdsize = 0;
#else
								rdsize = dektec_receive(buf, sizeof(buf));
#endif
								if (rdsize > 0)
								{
#if USE_FIFO_ACCESS_MUTEX
									uint32_t wait_state = ::WaitForSingleObject(ptransport_stream->m_hFifoAccess, INFINITE);
									if (wait_state == WAIT_OBJECT_0)
									{
#endif
										rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
										if (rtcode != NO_ERROR)
										{
											assert(0);
										}
#if USE_FIFO_ACCESS_MUTEX
										::ReleaseMutex(ptransport_stream->m_hFifoAccess);
									}
#endif
								}
								else
								{
									rtcode = MIDDLEWARE_TS_STREAM_NODATA_ERROR;				//没有采集到实时流
								}
							}
						}
					}
					else if (strcmp(ptransport_stream->m_pszProtocolHead, "UDP") == 0)
					{
						if (ptransport_stream->m_ts_fifo.GetAvailableSpace() >= sizeof(buf))
						{
							rdsize = udp_receive(buf, sizeof(buf));

							if (rdsize > 0)
							{
#if USE_FIFO_ACCESS_MUTEX
								uint32_t wait_state = ::WaitForSingleObject(ptransport_stream->m_hFifoAccess, INFINITE);
								if (wait_state == WAIT_OBJECT_0)
								{
#endif
									rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
									assert(rtcode == NO_ERROR);
#if USE_FIFO_ACCESS_MUTEX
									::ReleaseMutex(ptransport_stream->m_hFifoAccess);
								}
#endif
							}
							else
							{
								rtcode = MIDDLEWARE_TS_STREAM_NODATA_ERROR;				//没有采集到实时流
							}
						}
					}
					else
					{
//						Sleep(100);
					}
				}

				ptransport_stream->m_nStopReceiving = 1; 
			}
			else
			{
				Sleep(100);    //空转，等待下次接收指令
			}
		}

		ptransport_stream->m_nStopReceiving = 1;
		ptransport_stream->m_nStopRunning = 1;

		rtcode = MIDDLEWARE_TS_NO_ERROR;
	}
	else
	{
		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;
	}

	return rtcode;
}

CTransportStream::CTransportStream(void)
{
	memset(m_pszProtocolHead, '\0', 16);
	memset(m_pszProtocolExt, '\0', MAX_PATH);

	m_nPacketLength	 = 0;

	m_TsError	= MIDDLEWARE_TS_NO_ERROR;

//	m_llCurFilePos						= 0;
	m_llCurReadPos = 0;
	m_llTotalFileLength					= 0;

	m_bitrate_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	m_bitrate_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	m_bitrate_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	m_bitrate_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	m_bitrate_sample_index = 0;
	m_bitrate_sample_count = 0;
	memset(m_bitrate_sample_array, 0, sizeof(m_bitrate_sample_array));

	m_bitrate_available = 0;

	m_nReceiving = 0;
	m_nStopReceiving = 1;

	m_nRunning = 0;
	m_nStopRunning = 1;

	m_nEOF = 0;
	m_bSynced = 0;

#if USE_FIFO_ACCESS_MUTEX
	m_hFifoAccess = NULL;
#endif
}


CTransportStream::~CTransportStream()
{
}

int CTransportStream::Open(char* tsin_option, char* tsin_description, int mode)
{
	int	 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;					//默认返回成功
	char*  pdest;
	int   idx;
	char   ip_addr[16];
	int  port;
//	int  netcard_index = 1;

	//初始状态设置
	m_nMode = mode;

	m_nRunning = 1;
	m_nStopRunning = 0;
	m_nReceiving = 0;
	m_nStopReceiving = 1;

	m_bSynced = 0;

	char	pszExeFile[MAX_PATH];
	char	pszIniFile[MAX_PATH];
	int		len;

	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile);

	m_bEnableTSRateDebug = GetPrivateProfileIntA("编程者诊断", "TS码率", 0, pszIniFile);

	//URL解析，从URL获取输入参数
	//pdest = strstr(url_str, "://");
	//if (pdest != NULL)
	//{
	//	idx = (int)(pdest - url_str);

	//	memcpy(m_pszProtocolHead, url_str, idx);
	//	m_pszProtocolHead[idx] = '\0';
	//	errno_t errcode = _strupr_s(m_pszProtocolHead, strlen(m_pszProtocolHead) + 1);
	//}
	strcpy_s(m_pszProtocolHead, sizeof(m_pszProtocolHead), tsin_option);
	errno_t errcode = _strupr_s(m_pszProtocolHead, strlen(m_pszProtocolHead) + 1);
	strcpy_s(m_pszProtocolExt, sizeof(m_pszProtocolExt), tsin_description);
	errcode = _strupr_s(m_pszProtocolExt, strlen(m_pszProtocolExt) + 1);

	memset(ip_addr, 0x00, sizeof(ip_addr));

	if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
#ifdef _WIN64
			rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
#else
			rtcode = smartts_init();
			
			mf_actual_1spulse_frequency = smartts_get_timer(SMARTTS_TIMER0);
			mf_actual_scan_frequency = smartts_get_timer(SMARTTS_TIMER1);
			mn_actual_timeslice = smartts_get_timeslice_count();
#endif
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
#ifdef _WIN64
			rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
#else
			rtcode = dektec_receive_init();
#endif
		}
		else
		{
			rtcode = MIDDLEWARE_TS_STREAM_OPEN_ERROR;
		}
	}
	else if (strcmp(m_pszProtocolHead, "UDP") == 0)
	{
		pdest = strchr(m_pszProtocolExt, ':');
		if (pdest != NULL)
		{
			idx = (int)(pdest - m_pszProtocolExt);

			memcpy(ip_addr, m_pszProtocolExt, idx);
			ip_addr[idx] = '\0';

			port = atoi(m_pszProtocolExt + idx + 1);
		}
		else
		{
			port = -1;
		}

		rtcode = udp_receive_init(ip_addr, port);
	}
	else if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		rtcode = file_receive_init(m_pszProtocolExt);
		if (rtcode == TSFILE_NO_ERROR)
		{
			m_llTotalFileLength = file_attr_get_total_length();
		}
	}
	else
	{
		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;						//协议错误
	}

	//char	pszExeFile[MAX_PATH];
	char	exeDrive[3];
	char	pszAppTempPath[MAX_PATH];
	char	pszPcrPath[MAX_PATH];
	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	exeDrive[0] = pszExeFile[0];
	exeDrive[1] = pszExeFile[1];
	exeDrive[2] = '\0';
	sprintf_s(pszAppTempPath, sizeof(pszAppTempPath), "%s\\~EverStationII", exeDrive);
	sprintf_s(pszPcrPath, sizeof(pszPcrPath), "%s\\pcr", pszAppTempPath);
	DIR_BuildDirectory(pszPcrPath);

	char file_name[128];
	uint32_t old_tickcount = ::GetTickCount();
	sprintf_s(file_name, sizeof(file_name), "%s\\tick_%08x_ts_bitrate.txt", pszPcrPath, old_tickcount);

	fp_tsrate_dbase = NULL;
	if (m_bEnableTSRateDebug)
	{
		fopen_s(&fp_tsrate_dbase, file_name, "wt");
	}
	if (fp_tsrate_dbase != NULL)
	{
		fprintf(fp_tsrate_dbase, "当前值, 均值, 标准差\n");
	}

#if USE_FIFO_ACCESS_MUTEX
	m_hFifoAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_receive_transport_stream, (LPVOID)this, 0, 0);

	return rtcode;
}

int CTransportStream::Close()
{
	int		rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	m_nReceiving = 0;
	m_nRunning = 0;

	Sleep(100);

	while (m_nStopReceiving == 0)
	{
		Sleep(100);
	}

	while (m_nStopRunning == 0)
	{
		Sleep(100);
	}

	m_nPacketLength	 = 0;

	m_llCurReadPos = 0;
	m_llTotalFileLength					= 0;

#if USE_FIFO_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hFifoAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		m_ts_fifo.Reset();
		
#if USE_FIFO_ACCESS_MUTEX
		::CloseHandle(m_hFifoAccess);
		m_hFifoAccess = NULL;
	}
#endif

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		rtcode = file_receive_fini();
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt,"SMARTTS") == 0)
		{
			rtcode = smartts_fini();
		}
		else if (strcmp(m_pszProtocolExt,"DEKTEC") == 0)
		{
			rtcode = dektec_receive_fini();
		}
#endif
	}
	else if (strcmp(m_pszProtocolHead, "UDP") == 0)
	{
		rtcode = udp_receive_fini();
	}

	memset(m_pszProtocolHead, '\0', 16);
	memset(m_pszProtocolExt, '\0', MAX_PATH);

	m_bSynced = 0;

	m_bitrate_cur_value = UNCREDITABLE_MAX_VALUE;
	m_bitrate_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	m_bitrate_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	m_bitrate_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	m_bitrate_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	m_bitrate_sample_index = 0;
	m_bitrate_sample_count = 0;
	memset(m_bitrate_sample_array, 0, sizeof(m_bitrate_sample_array));

	m_bitrate_available = 0;

	if (fp_tsrate_dbase != NULL)
	{
		fclose(fp_tsrate_dbase);
		fp_tsrate_dbase = NULL;
	}

	return rtcode;
}

int CTransportStream::StartGetData(int64_t offset)
{
	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		if (offset >= 0)
		{
			m_llCurReadPos = offset;
			if (offset < m_llTotalFileLength)
			{
				m_nEOF = 0;
			}
		}
		rtcode = file_start_receive(offset);				//read from the beginning of the file
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			rtcode = smartts_start_receive();
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
			rtcode = dektec_start_receive();
		}
#endif
	}
	else if (strcmp(m_pszProtocolHead, "UDP") == 0)
	{
		rtcode = udp_start_receive();
	}

	m_bSynced = 0;
	m_nStopReceiving = 0;
	m_nReceiving = 1;

	return rtcode;
}

int CTransportStream::StopGetData(void)
{
	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		file_stop_receive();
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			rtcode = smartts_stop_receive();
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
			rtcode = dektec_stop_receive();
		}
#endif
	}
	else if (strcmp(m_pszProtocolHead, "UDP") == 0)
	{
		rtcode = udp_stop_receive();
	}

	m_nReceiving = 0;

	//等待线程退出
	while (m_nStopReceiving == 0)
	{
		Sleep(100);
	}

	m_bSynced = 0;

#if USE_FIFO_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hFifoAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		m_ts_fifo.Reset();

#if USE_FIFO_ACCESS_MUTEX
		::ReleaseMutex(m_hFifoAccess);
	}
#endif

	return rtcode;
}

int CTransportStream::Reset()
{
	int		rtcode = MIDDLEWARE_TS_NO_ERROR;

	m_bitrate_cur_value = UNCREDITABLE_MAX_VALUE;
	m_bitrate_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	m_bitrate_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	m_bitrate_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	m_bitrate_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	m_bitrate_sample_index = 0;
	m_bitrate_sample_count = 0;
	memset(m_bitrate_sample_array, 0, sizeof(m_bitrate_sample_array));

	m_bitrate_available = 0;

	return rtcode;
}

//找同步
int CTransportStream::Synchronize(int* plength)
{
	int						rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
	uint32_t				wait_state = WAIT_FAILED;
	uint8_t					temp_buffer[613];
	int						bExist = 0;

	do
	{
		if (m_ts_fifo.GetAvailablePayload() >= 613)		//3 * 204 + 1
		{
#if USE_FIFO_ACCESS_MUTEX
			uint32_t wait_state = ::WaitForSingleObject(m_hFifoAccess, INFINITE);
			if (wait_state == WAIT_OBJECT_0)
			{
#endif
				m_ts_fifo.PrefetchData(temp_buffer, sizeof(temp_buffer));

				//if (((temp_buffer[0] == 0x47) || (temp_buffer[0] == 0xB8)) &&
				//	((temp_buffer[188] == 0x47) || (temp_buffer[188] == 0xB8)) &&
				//	((temp_buffer[376] == 0x47) || (temp_buffer[376] == 0xB8)) &&
				//	((temp_buffer[564] == 0x47) || (temp_buffer[564] == 0xB8)))
				if ((temp_buffer[0] == 0x47) &&
					(temp_buffer[188] == 0x47) &&
					(temp_buffer[376] == 0x47) &&
					(temp_buffer[564] == 0x47))
				{
					//188 packet seems could not occurs syncword 0xB8

					m_bSynced = 1;
					m_nPacketLength = 188;

					if (plength != NULL)
					{
						*plength = m_nPacketLength;
					}

					rtcode = MIDDLEWARE_TS_NO_ERROR;

					bExist = 1;
				}
				else if (((temp_buffer[0] == 0x47) || (temp_buffer[0] == 0xB8)) &&
					((temp_buffer[204] == 0x47) || (temp_buffer[204] == 0xB8)) &&
					((temp_buffer[408] == 0x47) || (temp_buffer[408] == 0xB8)) &&
					((temp_buffer[612] == 0x47) || (temp_buffer[612] == 0xB8)))
				{
					//zongyang-204 sync error.ts is as this case

					m_bSynced = 1;
					m_nPacketLength = 204;

					if (plength != NULL)
					{
						*plength = m_nPacketLength;
					}

					rtcode = MIDDLEWARE_TS_NO_ERROR;

					bExist = 1;
				}
				else
				{
					m_ts_fifo.SkipData(1);
					m_llCurReadPos++;
				}
#if USE_FIFO_ACCESS_MUTEX
				::ReleaseMutex(m_hFifoAccess);
			}
#endif
		}
		else
		{
			//the avalilable payload is not enough
			if (m_nEOF == 1)
			{
				rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
			}
			else
			{
				rtcode = MIDDLEWARE_TS_FIFO_EMPTY_ERROR;				//FIFO空
			}

			bExist = 1;
		}


	} while (bExist == 0);

	return rtcode;
}

int CTransportStream::SyncReadOnePacket(uint8_t* buf, int* plength)
{
	int						rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
	uint32_t				wait_state = WAIT_FAILED;

	if (buf != NULL)
	{
		assert(*plength >= m_nPacketLength);
		assert(m_bSynced == 1);

		//this case the stream is synced
		if (m_ts_fifo.GetAvailablePayload() >= m_nPacketLength)
		{
#if USE_FIFO_ACCESS_MUTEX
			uint32_t wait_state = ::WaitForSingleObject(m_hFifoAccess, INFINITE);
			if (wait_state == WAIT_OBJECT_0)
			{
#endif
				m_ts_fifo.PrefetchData(buf, m_nPacketLength);

				if ((buf[0] == 0x47) || (buf[0] == 0xB8))
				{
					*plength = m_nPacketLength;

					m_llCurReadPos += m_nPacketLength;
					m_ts_fifo.SkipData(m_nPacketLength);

					rtcode = MIDDLEWARE_TS_NO_ERROR;
				}
				else
				{
					m_bSynced = 0;
					m_nPacketLength = 0;
					*plength = 0;

					rtcode = ETR290_TS_SYNC_LOSS;						//同步丢失
				}
#if USE_FIFO_ACCESS_MUTEX
				::ReleaseMutex(m_hFifoAccess);
			}
#endif
		}
		else		//FIFO数据量不够
		{
			if (m_nEOF == 1)
			{
				rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
			}
			else
			{
				rtcode = MIDDLEWARE_TS_FIFO_EMPTY_ERROR;				//FIFO空
			}
		}
	}
	else
	{
		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}

//int CTransportStream::PrefetchOnePacket(uint8_t* buf, int* plength)
//{
//	int						rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
//	uint32_t				wait_state = WAIT_FAILED;
//
//	if (buf != NULL)
//	{
//		assert(*plength >= m_nPacketLength);
//		assert(m_bSynced == 1);
//
//		//this case the stream is synced
//		if (m_ts_fifo.GetAvailablePayload() >= m_nPacketLength)
//		{
//			m_ts_fifo.PrefetchData(buf, m_nPacketLength);
//
//			if ((buf[0] == 0x47) || (buf[0] == 0xB8))
//			{
//				*plength = m_nPacketLength;
//				rtcode = MIDDLEWARE_TS_NO_ERROR;
//			}
//			else
//			{
//				m_bSynced = 0;
//				m_nPacketLength = 0;
//				*plength = 0;
//
//				rtcode = ETR290_TS_SYNC_LOSS;						//同步丢失
//			}
//		}
//		else		//FIFO数据量不够
//		{
//			if (m_nEOF == 1)
//			{
//				rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
//			}
//			else
//			{
//				rtcode = MIDDLEWARE_TS_FIFO_EMPTY_ERROR;				//FIFO空
//			}
//		}
//	}
//	else
//	{
//		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;						//输入参数错误
//	}
//
//	return rtcode;
//}

//int CTransportStream::SkipOnePacket(void)
//{
//	int	rtcode = MIDDLEWARE_TS_NO_ERROR;
//
//	m_llCurReadPos += m_nPacketLength;
//	m_ts_fifo.SkipData(m_nPacketLength);
//
//	return rtcode;
//}

//int CTransportStream::StartGetBitrate(void)
//{
//	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
//
//	if (strcmp(m_pszProtocolHead, "FILE") == 0)
//	{
//	}
//	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
//	{
//#ifdef _WIN64
//#else
//		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
//		{
//			smartts_start_timer();
//		}
//		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
//		{
//	//		dektec_start_timer();
//		}
//#endif
//	}
//
//	return rtcode;
//}

//int CTransportStream::StopGetBitrate(void)
//{
//	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
//
//	if (strcmp(m_pszProtocolHead, "FILE") == 0)
//	{
//	}
//	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
//	{
//#ifdef _WIN64
//#else
//		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
//		{
//			smartts_stop_timer();
//		}
//		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
//		{
//	//		dektec_stop_timer();
//		}
//#endif
//	}
//
//	return rtcode;
//}

//int CTransportStream::GetBitrateMap(int pbitrate_map[], int count)
//{
//	int		rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
////	int		readcount;
////	double	coeff = 8.0;
////	
////	if (m_hFile == -1)
////	{
////		coeff *= mf_actual_scan_frequency;
////
////		if (count >= mn_actual_timeslice)
////		{
////			readcount = mn_actual_timeslice;
////		}
////		else
////		{
////			readcount = count;
////		}
////
////#ifdef _WIN64
////#else
////		//rtcode = smartts_get_asiin_bitrate_map(pbitrate_map, readcount);
////
////		//if (rtcode > 0)
////		//{
////		//	for (int i = 0; i < readcount; i++)
////		//	{
////		//		double temp = pbitrate_map[i];
////		//		temp *= coeff;
////		//		pbitrate_map[i] = (ULONG)temp;
////		//	}
////		//}
////#endif
////
////	}
//
//	return rtcode;
//}

int CTransportStream::GetBitrate(void)
{
	int  rtvalue = -1;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		rtvalue = m_bitrate_mean_value;
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			ULONG bitrate_value = smartts_get_asiin_bitrate();
			if (bitrate_value >= 0)
			{
				m_bitrate_mean_value = (int)(bitrate_value * 8 * mf_actual_1spulse_frequency);
				rtvalue = m_bitrate_mean_value;
			}
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
			rtvalue = dektec_get_asiin_bitrate();
		}
#endif
	}
	else if (strcmp(m_pszProtocolHead, "UDP") == 0)
	{
		rtvalue = udp_get_bitrate();
	}

	return rtvalue;
}

int CTransportStream::GetMeasuredBitrateAttribute(BITRATE_ATTRIBUTE_t* pattr)
{
	int rtcode = NO_ERROR;
	if (pattr != NULL)
	{
		pattr->min = m_bitrate_min_value;
		pattr->mean = m_bitrate_mean_value;
		pattr->max = m_bitrate_max_value;
		pattr->rms = m_bitrate_rms_value;
	}
	else
	{
		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;
	}

	return rtcode;
}

int CTransportStream::AddBitrateSample(int bitrate)
{
	m_bitrate_cur_value = bitrate;
	if (fp_tsrate_dbase != NULL)
	{
		fprintf(fp_tsrate_dbase, "%d", m_bitrate_cur_value);
	}

	if (bitrate > m_bitrate_max_value)
	{
		m_bitrate_max_value = bitrate;
	}
	if (bitrate < m_bitrate_min_value)
	{
		m_bitrate_min_value = bitrate;
	}

	m_bitrate_sample_array[m_bitrate_sample_index] = bitrate;
	m_bitrate_sample_index++;
	m_bitrate_sample_index %= TS_BITRATE_FIFO_LENGTH;

	if (m_bitrate_sample_count < TS_BITRATE_FIFO_LENGTH)
	{
		m_bitrate_sample_count++;
	}

	if (m_bitrate_sample_count >= 2)
	{
		int64_t data_rate_sum = 0;
		for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
		{
			data_rate_sum += m_bitrate_sample_array[sample_index];
		}
		m_bitrate_mean_value = (int)(data_rate_sum / m_bitrate_sample_count);

		m_bitrate_available = 1;

		data_rate_sum = 0;
		for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
		{
			data_rate_sum += (int64_t)pow(m_bitrate_sample_array[sample_index] - m_bitrate_mean_value, 2);
		}
		data_rate_sum /= m_bitrate_sample_count;
		m_bitrate_rms_value = (int)sqrt((double)data_rate_sum);

		if (fp_tsrate_dbase != NULL)
		{
			fprintf(fp_tsrate_dbase, ", %d, %d", m_bitrate_mean_value, m_bitrate_rms_value);
		}
	}
	if (fp_tsrate_dbase != NULL)
	{
		fprintf(fp_tsrate_dbase, "\n");
	}

	return NO_ERROR;
}

void CTransportStream::SeekToBegin(void)
{
	m_llCurReadPos = 0;
	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		file_seek(0);
		m_nEOF = 0;
	}
}
/*
void CTransportStream::Seek(int64_t fileoffset)
{
	if (m_hFile != -1)
	{
//		m_llCurFilePos = fileoffset;
		_lseeki64(m_hFile, fileoffset, SEEK_SET);

		int64_t llCurFilePos = _telli64(m_hFile);
		if (llCurFilePos >= m_llTotalFileLength)
		{
			m_nEOF = 1;
		}
		else
		{
			m_nEOF = 0;
		}
	}
}
*/
int64_t CTransportStream::Tell(void)
{
	return m_llCurReadPos;
}

int CTransportStream::GetLastError()
{
	return m_TsError;
}



