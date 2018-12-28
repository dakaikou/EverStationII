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
#include "toolbox_libs\TOOL_Math\Include\Sort.h"

static double DIXON_950_alpha_table[] = { 1.000, 1.000, 0.941, 0.765, 0.642, 0.562, 0.507, 0.554, 0.512, 0.477, 0.575, 0.546, 0.521, 0.546, 0.524, 
                                          0.505, 0.489, 0.475, 0.462, 0.450, 0.440, 0.431, 0.422, 0.413, 0.406, 0.399, 0.393, 0.387, 0.381, 0.376};

static double DIXON_990_alpha_table[] = { 1.000, 1.000, 0.988, 0.889, 0.782, 0.698, 0.637, 0.681, 0.635, 0.597, 0.674, 0.642, 0.617, 0.640, 0.618,
							 			  0.597, 0.580, 0.564, 0.550, 0.538, 0.526, 0.516, 0.507, 0.497, 0.489, 0.482, 0.474, 0.468, 0.462, 0.456};

static double DIXON_995_alpha_table[] = { 1.000, 1.000, 0.994, 0.920, 0.823, 0.744, 0.680, 0.723, 0.676, 0.638, 0.707, 0.675, 0.649, 0.672, 0.649,
										  0.629, 0.611, 0.595, 0.580, 0.568, 0.556, 0.545, 0.536, 0.526, 0.519, 0.510, 0.503, 0.496, 0.489, 0.484};

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
									if (ptransport_stream->m_nMode == 0)		//offline mode
									{
										ptransport_stream->m_nEOF = 1;
										rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
									}
									else     //realtime mode
									{
										file_seek(0);
									}
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

	//m_bitrate_original_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_original_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	//m_bitrate_original_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_original_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	m_bitrate_original_sample_index = 0;
	m_bitrate_original_sample_count = 0;
	memset(m_bitrate_original_sample_array, 0, sizeof(m_bitrate_original_sample_array));

	//m_bitrate_dixon_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_dixon_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	//m_bitrate_dixon_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_dixon_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_deletion_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_deletion_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	//m_bitrate_deletion_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_deletion_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_processed_sample_index = 0;
	//m_bitrate_processed_sample_count = 0;
	//memset(m_bitrate_processed_sample_array, 0, sizeof(m_bitrate_processed_sample_array));

	m_bitrate_estimate_cur_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	m_bitrate_estimate_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	m_bitrate_estimate_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_available = 0;

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
		//fprintf(fp_tsrate_dbase, "观测值, 最大值(原始), 均值(原始), 最小值(原始), 标准差(原始), 堆栈状态, 最大值(处理后), 均值(处理后), 最小值(处理后), 标准差(处理后)\n");
		//fprintf(fp_tsrate_dbase, "观测值, 堆栈状态, 均值(原始), 均值(极值删除), 均值(Dixon), 标准差(原始), 标准差(极值删除), 标准差(Dixon)\n");
		//fprintf(fp_tsrate_dbase, "堆栈状态, 均值(原始), 均值(极值删除), 均值(Dixon), 标准差(原始), 标准差(极值删除), 标准差(Dixon)\n");
		fprintf(fp_tsrate_dbase, "均值(原始), 均值(极值删除), 均值(Dixon), 均值(择优后)\n");
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

	//m_bitrate_cur_value = UNCREDITABLE_MAX_VALUE;
	//m_bitrate_original_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_original_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_original_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_original_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差

	m_bitrate_original_sample_index = 0;
	m_bitrate_original_sample_count = 0;
	memset(m_bitrate_original_sample_array, 0, sizeof(m_bitrate_original_sample_array));

	//m_bitrate_dixon_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_dixon_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率
	//m_bitrate_dixon_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_dixon_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差

	//m_bitrate_deletion_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_deletion_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率
	//m_bitrate_deletion_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_deletion_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差

	m_bitrate_estimate_cur_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	m_bitrate_estimate_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	m_bitrate_estimate_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_processed_sample_index = 0;
	//m_bitrate_processed_sample_count = 0;
	//memset(m_bitrate_processed_sample_array, 0, sizeof(m_bitrate_processed_sample_array));

	//m_bitrate_available = 0;

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

	//m_bitrate_cur_value = UNCREDITABLE_MAX_VALUE;
	//m_bitrate_original_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_original_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_original_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_original_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	m_bitrate_original_sample_index = 0;
	m_bitrate_original_sample_count = 0;
	memset(m_bitrate_original_sample_array, 0, sizeof(m_bitrate_original_sample_array));

	//m_bitrate_dixon_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_dixon_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率
	//m_bitrate_dixon_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_dixon_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差
	//m_bitrate_processed_sample_index = 0;
	//m_bitrate_processed_sample_count = 0;
	//memset(m_bitrate_processed_sample_array, 0, sizeof(m_bitrate_processed_sample_array));

	//m_bitrate_deletion_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	//m_bitrate_deletion_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率
	//m_bitrate_deletion_mean_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	//m_bitrate_deletion_rms_value = UNCREDITABLE_MAX_VALUE;									//比特率抖动方差

	m_bitrate_estimate_cur_value = UNCREDITABLE_MAX_VALUE;									//平滑后的比特率
	m_bitrate_estimate_min_value = UNCREDITABLE_MIN_VALUE;									//平滑后的最小比特率
	m_bitrate_estimate_max_value = UNCREDITABLE_MAX_VALUE;									//平滑后的最大比特率

	//m_bitrate_available = 0;

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

int CTransportStream::StartGetBitrate(void)
{
	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			smartts_start_timer();
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
	//		dektec_start_timer();
		}
#endif
	}

	return rtcode;
}

int CTransportStream::StopGetBitrate(void)
{
	int rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			smartts_stop_timer();
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
	//		dektec_stop_timer();
		}
#endif
	}

	return rtcode;
}

int CTransportStream::GetBitrate(void)
{
	int  rtvalue = -1;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		rtvalue = m_bitrate_estimate_cur_value;
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
				m_bitrate_estimate_cur_value = (int)round(bitrate_value * 8 * mf_actual_1spulse_frequency);
				rtvalue = m_bitrate_estimate_cur_value;
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

//int CTransportStream::GetMeasuredBitrateAttribute(BITRATE_ATTRIBUTE_t* pattr)
//{
//	int rtcode = NO_ERROR;
//	if (pattr != NULL)
//	{
//		pattr->min = m_bitrate_estimate_min_value;
//		pattr->max = m_bitrate_estimate_max_value;
//
//		pattr->mean = m_bitrate_estimate_cur_value;
//		//pattr->rms = m_bitrate_estimate_rms_value;
//	}
//	else
//	{
//		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;
//	}
//
//	return rtcode;
//}

//int CTransportStream::AddBitrateSample(int new_bitrate)
//{
//	if (fp_tsrate_dbase != NULL)
//	{
//		fprintf(fp_tsrate_dbase, "%d", new_bitrate);
//	}
//
//	if (new_bitrate > m_bitrate_original_max_value)
//	{
//		m_bitrate_original_max_value = new_bitrate;
//		//m_bitrate_processed_max_value = bitrate;
//	}
//	if (new_bitrate < m_bitrate_original_min_value)
//	{
//		m_bitrate_original_min_value = new_bitrate;
//		//m_bitrate_processed_min_value = bitrate;
//	}
//
//	m_bitrate_original_sample_array[m_bitrate_original_sample_index] = new_bitrate;
//	m_bitrate_original_sample_index++;
//	m_bitrate_original_sample_index %= TS_BITRATE_FIFO_LENGTH;
//
//	if (m_bitrate_original_sample_count < TS_BITRATE_FIFO_LENGTH)
//	{
//		m_bitrate_original_sample_count++;
//	}
//
//	if (m_bitrate_original_sample_count >= 4)
//	{
//		double original_sample_sum = 0;
//		double original_power_sum = 0;
//		int	   n = m_bitrate_original_sample_count;
//		for (int i = 0; i < m_bitrate_original_sample_count; i++)
//		{
//			double normal_bitrate = (double)m_bitrate_original_sample_array[i] / m_bitrate_original_max_value;
//			original_sample_sum += normal_bitrate;
//			original_power_sum += (normal_bitrate * normal_bitrate);
//		}
//		m_bitrate_original_mean_value = (int)round(m_bitrate_original_max_value * original_sample_sum / n);
//
//		double original_sigma2 = original_power_sum - (original_sample_sum * original_sample_sum) / n;
//
//		if (original_sigma2 < 0)
//		{
//			original_sigma2 = 0.0;
//		}
//
//		double original_sigma = sqrt(original_sigma2 / (n - 1));
//
//		m_bitrate_original_rms_value = (int)round(m_bitrate_original_max_value*original_sigma);
//
//		//int64_t sum_delt2 = 0;
//		//for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
//		//{
//		//	sum_delt2 += (int64_t)pow(m_bitrate_sample_array[sample_index] - m_bitrate_mean_value, 2);
//		//}
//		//sum_delt2 /= (m_bitrate_sample_count - 1);
//		//int bitrate_rms_value = round(sqrt((double)sum_delt2));
//
//		if (fp_tsrate_dbase != NULL)
//		{
//			//fprintf(fp_tsrate_dbase, ", %d, %d, %d", m_bitrate_mean_value, m_bitrate_rms_value, bitrate_rms_value);
//			fprintf(fp_tsrate_dbase, ", %d, %d, %d, %d", m_bitrate_original_max_value, m_bitrate_original_mean_value, m_bitrate_original_min_value, m_bitrate_original_rms_value);
//		}
//
//		//样本检验
//		if (m_bitrate_processed_sample_count == 0)
//		{
//			int* temp_array = (int*)malloc(m_bitrate_original_sample_count * sizeof(int));
//			for (int i = 0; i < m_bitrate_original_sample_count; i++)
//			{
//				temp_array[i] = m_bitrate_original_sample_array[i];
//			}
//			quick_sort_method1(temp_array, 0, m_bitrate_original_sample_count - 1);
//
//			for (int i = 0; i < m_bitrate_original_sample_count - 2; i++)
//			{
//				m_bitrate_processed_sample_array[i] = temp_array[i + 1];
//				if (m_bitrate_processed_sample_array[i] > m_bitrate_processed_max_value)
//				{
//					m_bitrate_processed_max_value = m_bitrate_processed_sample_array[i];
//				}
//				if (m_bitrate_processed_sample_array[i] < m_bitrate_processed_min_value)
//				{
//					m_bitrate_processed_min_value = m_bitrate_processed_sample_array[i];
//				}
//			}
//			m_bitrate_processed_sample_count = m_bitrate_original_sample_count - 2;
//			//m_bitrate_processed_sample_index = m_bitrate_processed_sample_count;
//
//			free(temp_array);
//
//			if (fp_tsrate_dbase != NULL)
//			{
//				fprintf(fp_tsrate_dbase, ", , ,");
//			}
//		}
//		else
//		{
//			int bValidBitrate = 1;
//			int deltBitrate = abs(new_bitrate - m_bitrate_processed_mean_value);
//			if (deltBitrate > 10)	//+/- 10bps
//			{
//				int n = m_bitrate_processed_sample_count + 1;
//				int* temp_array = (int*)malloc(n * sizeof(int));
//				for (int i = 0; i < m_bitrate_processed_sample_count; i++)
//				{
//					temp_array[i] = m_bitrate_processed_sample_array[i];
//				}
//				temp_array[n - 1] = new_bitrate;
//
//				quick_sort_method1(temp_array, 0, n - 1);
//
//				//Dixon deletion
//
//				double rh = 0, rl = 0;
//				if ((n >= 3) && (n <= 7))
//				{
//					if (temp_array[n - 1] > temp_array[0])
//					{
//						rh = (double)(temp_array[n - 1] - temp_array[n - 2]) / (temp_array[n - 1] - temp_array[0]);
//						rl = (double)(temp_array[1] - temp_array[0]) / (temp_array[n - 1] - temp_array[0]);
//					}
//				}
//				else if ((n >= 8) && (n <= 10))
//				{
//					if (temp_array[n - 1] > temp_array[1])
//					{
//						rh = (double)(temp_array[n - 1] - temp_array[n - 2]) / (temp_array[n - 1] - temp_array[1]);
//					}
//					if (temp_array[n - 2] > temp_array[0])
//					{
//						rl = (double)(temp_array[1] - temp_array[0]) / (temp_array[n - 2] - temp_array[0]);
//					}
//				}
//				else if ((n >= 11) && (n <= 13))
//				{
//					if (temp_array[n - 1] > temp_array[1])
//					{
//						rh = (double)(temp_array[n - 1] - temp_array[n - 3]) / (temp_array[n - 1] - temp_array[1]);
//					}
//					if (temp_array[n - 2] > temp_array[0])
//					{
//						rl = (double)(temp_array[2] - temp_array[0]) / (temp_array[n - 2] - temp_array[0]);
//					}
//				}
//				else if ((n >= 14) && (n <= 30))
//				{
//					if (temp_array[n - 1] > temp_array[2])
//					{
//						rh = (double)(temp_array[n - 1] - temp_array[n - 3]) / (temp_array[n - 1] - temp_array[2]);
//					}
//					if (temp_array[n - 3] > temp_array[0])
//					{
//						rl = (double)(temp_array[2] - temp_array[0]) / (temp_array[n - 3] - temp_array[0]);
//					}
//				}
//				free(temp_array);
//
//				if (fp_tsrate_dbase != NULL)
//				{
//					fprintf(fp_tsrate_dbase, ",%d, %.3f,%.3f", n, rh, rl);
//				}
//
//				if (rh > DIXON_995_alpha_table[n - 1])
//				{
//					bValidBitrate = 0;
//				}
//			}
//			else
//			{
//				if (fp_tsrate_dbase != NULL)
//				{
//					fprintf(fp_tsrate_dbase, ",,,");
//				}
//			}
//
//			//rewrite
//			if (bValidBitrate)
//			{
//				if (m_bitrate_processed_sample_count >= TS_BITRATE_FIFO_LENGTH)
//				{
//					for (int i = 1; i < m_bitrate_processed_sample_count; i++)
//					{
//						m_bitrate_processed_sample_array[i - 1] = m_bitrate_processed_sample_array[i];
//					}
//				}
//
//				if (new_bitrate > m_bitrate_processed_max_value)
//				{
//					m_bitrate_processed_max_value = new_bitrate;
//				}
//				if (new_bitrate < m_bitrate_processed_min_value)
//				{
//					m_bitrate_processed_min_value = new_bitrate;
//				}
//
//				if (m_bitrate_processed_sample_count < TS_BITRATE_FIFO_LENGTH)
//				{
//					m_bitrate_processed_sample_count++;
//				}
//
//				m_bitrate_processed_sample_array[m_bitrate_processed_sample_count - 1] = new_bitrate;
//			}
//		}
//
//		if (m_bitrate_processed_sample_count >= 2)
//		{
//			double processed_sample_sum = 0;
//			double processed_power_sum = 0;
//			n = m_bitrate_processed_sample_count;
//			for (int i = 0; i < m_bitrate_processed_sample_count; i++)
//			{
//				double normal_bitrate = (double)m_bitrate_processed_sample_array[i] / m_bitrate_processed_max_value;
//				processed_sample_sum += normal_bitrate;
//				processed_power_sum += normal_bitrate * normal_bitrate;
//			}
//			m_bitrate_processed_mean_value = (int)round(m_bitrate_processed_max_value * processed_sample_sum / n);
//
//			double processed_sigma2 = processed_power_sum - (processed_sample_sum * processed_sample_sum) / n;
//
//			if (processed_sigma2 < 0)
//			{
//				processed_sigma2 = 0.0;
//			}
//
//			double processed_sigma = sqrt(processed_sigma2 / (n - 1));
//
//			m_bitrate_processed_rms_value = (int)round(m_bitrate_processed_max_value*processed_sigma);
//
//			if (fp_tsrate_dbase != NULL)
//			{
//				fprintf(fp_tsrate_dbase, ", %d, %d, %d, %d, ", m_bitrate_processed_max_value, m_bitrate_processed_mean_value, m_bitrate_processed_min_value, m_bitrate_processed_rms_value);
//
//				for (int i = 0; i < m_bitrate_processed_sample_count; i++)
//				{
//					fprintf(fp_tsrate_dbase, "%d-", m_bitrate_processed_sample_array[i]);
//				}
//			}
//
//			if (m_bitrate_available == 0) m_bitrate_available = 1;
//		}
//		else
//		{
//			fprintf(fp_tsrate_dbase, ", , , ,");
//		}
//	}
//	else
//	{
//		if (fp_tsrate_dbase != NULL)
//		{
//			fprintf(fp_tsrate_dbase, ", , , , , , , ,");
//		}
//	}
//
//	if (fp_tsrate_dbase != NULL)
//	{
//		fprintf(fp_tsrate_dbase, "\n");
//	}
//
//	return NO_ERROR;
//}

//int CTransportStream::AddBitrateSample(int new_bitrate)
//{
//	if (fp_tsrate_dbase != NULL)
//	{
//		fprintf(fp_tsrate_dbase, "%d,", new_bitrate);
//	}
//
//	if (new_bitrate > m_bitrate_original_max_value)
//	{
//		m_bitrate_original_max_value = new_bitrate;
//	}
//	if (new_bitrate < m_bitrate_original_min_value)
//	{
//		m_bitrate_original_min_value = new_bitrate;
//	}
//
//	m_bitrate_original_sample_array[m_bitrate_original_sample_index] = new_bitrate;
//	m_bitrate_original_sample_index++;
//	m_bitrate_original_sample_index %= TS_BITRATE_FIFO_LENGTH;
//
//	if (m_bitrate_original_sample_count < TS_BITRATE_FIFO_LENGTH)
//	{
//		m_bitrate_original_sample_count++;
//	}
//
//	if (m_bitrate_original_sample_count >= 4)
//	{
//		double original_sample_sum = 0;
//		double original_power_sum = 0;
//		int	   n = m_bitrate_original_sample_count;
//		for (int i = 0; i < m_bitrate_original_sample_count; i++)
//		{
//			double normal_bitrate = (double)m_bitrate_original_sample_array[i] / m_bitrate_original_max_value;
//			original_sample_sum += normal_bitrate;
//			original_power_sum += (normal_bitrate * normal_bitrate);
//		}
//		m_bitrate_original_mean_value = (int)round(m_bitrate_original_max_value * original_sample_sum / n);
//
//		double original_sigma2 = original_power_sum - (original_sample_sum * original_sample_sum) / n;
//
//		if (original_sigma2 < 0)
//		{
//			original_sigma2 = 0.0;
//		}
//
//		double original_sigma = sqrt(original_sigma2 / (n - 1));
//
//		m_bitrate_original_rms_value = (int)round(m_bitrate_original_max_value*original_sigma);
//
//		if (m_bitrate_original_mean_value > m_bitrate_dixon_max_value)
//		{
//			m_bitrate_dixon_max_value = m_bitrate_original_mean_value;
//		}
//		if (m_bitrate_original_mean_value < m_bitrate_dixon_min_value)
//		{
//			m_bitrate_dixon_min_value = m_bitrate_original_mean_value;
//		}
//
//		if (m_bitrate_original_mean_value > m_bitrate_deletion_max_value)
//		{
//			m_bitrate_deletion_max_value = m_bitrate_original_mean_value;
//		}
//		if (m_bitrate_original_mean_value < m_bitrate_deletion_min_value)
//		{
//			m_bitrate_deletion_min_value = m_bitrate_original_mean_value;
//		}
//
//		//int64_t sum_delt2 = 0;
//		//for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
//		//{
//		//	sum_delt2 += (int64_t)pow(m_bitrate_sample_array[sample_index] - m_bitrate_mean_value, 2);
//		//}
//		//sum_delt2 /= (m_bitrate_sample_count - 1);
//		//int bitrate_rms_value = round(sqrt((double)sum_delt2));
//
//		//if (fp_tsrate_dbase != NULL)
//		//{
//		//	//fprintf(fp_tsrate_dbase, ", %d, %d, %d", m_bitrate_mean_value, m_bitrate_rms_value, bitrate_rms_value);
//		//	//fprintf(fp_tsrate_dbase, ", %d, %d, %d, %d,", m_bitrate_original_max_value, m_bitrate_original_mean_value, m_bitrate_original_min_value, m_bitrate_original_rms_value);
//		//	fprintf(fp_tsrate_dbase, ", %d", m_bitrate_original_mean_value);
//		//}
//
//		//样本检验
//		//int n = m_bitrate_original_sample_count;
//		int* temp_array = (int*)malloc(n * sizeof(int));
//		for (int i = 0; i < n; i++)
//		{
//			temp_array[i] = m_bitrate_original_sample_array[i];
//		}
//
//		quick_sort_method1(temp_array, 0, n - 1);
//
//		//min max deletion
//		double deletion_sample_sum = 0;
//		double deletion_power_sum = 0;
//		int m = n - 2;
//		for (int i = 1; i < n-1; i++)
//		{
//			double normal_bitrate = (double)temp_array[i] / m_bitrate_dixon_max_value;
//			deletion_sample_sum += normal_bitrate;
//			deletion_power_sum += normal_bitrate * normal_bitrate;
//		}
//		m_bitrate_deletion_mean_value = (int)round(m_bitrate_dixon_max_value * deletion_sample_sum / m);
//
//		double deletion_sigma2 = deletion_power_sum - (deletion_sample_sum * deletion_sample_sum) / m;
//
//		if (deletion_sigma2 < 0)
//		{
//			deletion_sigma2 = 0.0;
//		}
//
//		double deletion_sigma = sqrt(deletion_sigma2 / (m - 1));
//
//		m_bitrate_deletion_rms_value = (int)round(m_bitrate_deletion_max_value*deletion_sigma);
//
//		//Dixon deletion
//		do
//		{
//			if (fp_tsrate_dbase != NULL)
//			{
//				fprintf(fp_tsrate_dbase, "[");
//				for (int i = 0; i < n; i++)
//				{
//					fprintf(fp_tsrate_dbase, "%d ", temp_array[i]);
//				}
//				fprintf(fp_tsrate_dbase, "] ");
//			}
//
//			double rh = 0, rl = 0;
//			if ((n >= 3) && (n <= 7))
//			{
//				if (temp_array[n - 1] > temp_array[0])
//				{
//					rh = (double)(temp_array[n - 1] - temp_array[n - 2]) / (temp_array[n - 1] - temp_array[0]);
//					rl = (double)(temp_array[1] - temp_array[0]) / (temp_array[n - 1] - temp_array[0]);
//				}
//			}
//			else if ((n >= 8) && (n <= 10))
//			{
//				if (temp_array[n - 1] > temp_array[1])
//				{
//					rh = (double)(temp_array[n - 1] - temp_array[n - 2]) / (temp_array[n - 1] - temp_array[1]);
//				}
//				if (temp_array[n - 2] > temp_array[0])
//				{
//					rl = (double)(temp_array[1] - temp_array[0]) / (temp_array[n - 2] - temp_array[0]);
//				}
//			}
//			else if ((n >= 11) && (n <= 13))
//			{
//				if (temp_array[n - 1] > temp_array[1])
//				{
//					rh = (double)(temp_array[n - 1] - temp_array[n - 3]) / (temp_array[n - 1] - temp_array[1]);
//				}
//				if (temp_array[n - 2] > temp_array[0])
//				{
//					rl = (double)(temp_array[2] - temp_array[0]) / (temp_array[n - 2] - temp_array[0]);
//				}
//			}
//			else if ((n >= 14) && (n <= 30))
//			{
//				if (temp_array[n - 1] > temp_array[2])
//				{
//					rh = (double)(temp_array[n - 1] - temp_array[n - 3]) / (temp_array[n - 1] - temp_array[2]);
//				}
//				if (temp_array[n - 3] > temp_array[0])
//				{
//					rl = (double)(temp_array[2] - temp_array[0]) / (temp_array[n - 3] - temp_array[0]);
//				}
//			}
//
//			if (fp_tsrate_dbase != NULL)
//			{
//				fprintf(fp_tsrate_dbase, "<n=%d&rh=%.3f&rl=%.3f>", n, rh, rl);
//			}
//
//			if ((rh <= DIXON_950_alpha_table[n - 1]) && (rl <= DIXON_950_alpha_table[n - 1]))
//			{
//				break;
//			}
//			else
//			{
//				if (rh > DIXON_950_alpha_table[n - 1])
//				{
//					n--;
//				}
//				if (rl > DIXON_950_alpha_table[n - 1])
//				{
//					for (int i = 1; i < n; i++)
//					{
//						temp_array[i - 1] = temp_array[i];
//					}
//
//					n--;
//				}
//			}
//
//		} while (n >= 3);
//
//		double processed_sample_sum = 0;
//		double processed_power_sum = 0;
//		for (int i = 0; i < n; i++)
//		{
//			double normal_bitrate = (double)temp_array[i] / m_bitrate_dixon_max_value;
//			processed_sample_sum += normal_bitrate;
//			processed_power_sum += normal_bitrate * normal_bitrate;
//		}
//		m_bitrate_dixon_mean_value = (int)round(m_bitrate_dixon_max_value * processed_sample_sum / n);
//
//		double processed_sigma2 = processed_power_sum - (processed_sample_sum * processed_sample_sum) / n;
//
//		if (processed_sigma2 < 0)
//		{
//			processed_sigma2 = 0.0;
//		}
//
//		double processed_sigma = sqrt(processed_sigma2 / (n - 1));
//
//		m_bitrate_dixon_rms_value = (int)round(m_bitrate_dixon_max_value*processed_sigma);
//
//		if (fp_tsrate_dbase != NULL)
//		{
//			//fprintf(fp_tsrate_dbase, ", %d, %d, %d, %d", m_bitrate_processed_max_value, m_bitrate_processed_mean_value, m_bitrate_processed_min_value, m_bitrate_processed_rms_value);
//			fprintf(fp_tsrate_dbase, ", %d , %d, %d, %d, %d, %d", m_bitrate_original_mean_value, m_bitrate_deletion_mean_value, m_bitrate_dixon_mean_value, m_bitrate_original_rms_value, m_bitrate_deletion_rms_value, m_bitrate_dixon_rms_value);
//		}
//
//		if (m_bitrate_available == 0) m_bitrate_available = 1;
//
//		free(temp_array);
//	}
//	else
//	{
//		//if (fp_tsrate_dbase != NULL)
//		//{
//		//	fprintf(fp_tsrate_dbase, ", , , , , , , ,");
//		//}
//	}
//
//	if (fp_tsrate_dbase != NULL)
//	{
//		fprintf(fp_tsrate_dbase, "\n");
//	}
//
//	return NO_ERROR;
//}

int CTransportStream::AddBitrateSample(int new_bitrate, int(*callback)(int, int))
{
	//if (fp_tsrate_dbase != NULL)
	//{
	//	fprintf(fp_tsrate_dbase, "%d,", new_bitrate);
	//}

	//if (new_bitrate > m_bitrate_original_max_value)
	//{
	//	m_bitrate_original_max_value = new_bitrate;
	//}
	//if (new_bitrate < m_bitrate_original_min_value)
	//{
	//	m_bitrate_original_min_value = new_bitrate;
	//}

	m_bitrate_original_sample_array[m_bitrate_original_sample_index] = new_bitrate;
	m_bitrate_original_sample_index++;
	m_bitrate_original_sample_index %= TS_BITRATE_FIFO_LENGTH;

	//if (m_bitrate_original_sample_count < TS_BITRATE_FIFO_LENGTH)
	//{
		m_bitrate_original_sample_count++;
	//}

	if (m_bitrate_original_sample_index == 0)
	{
		assert(m_bitrate_original_sample_count == TS_BITRATE_FIFO_LENGTH);

		int	   n = m_bitrate_original_sample_count;
		int	   max_sample_value = UNCREDITABLE_MAX_VALUE;
		int    min_sample_value = UNCREDITABLE_MIN_VALUE;
		for (int i = 0; i < m_bitrate_original_sample_count; i++)
		{
			if (m_bitrate_original_sample_array[i] > max_sample_value)
			{
				max_sample_value = m_bitrate_original_sample_array[i];
			}
			if (m_bitrate_original_sample_array[i] < min_sample_value)
			{
				min_sample_value = m_bitrate_original_sample_array[i];
			}
		}

		double original_sample_sum = 0;
		//double original_power_sum = 0;
		for (int i = 0; i < m_bitrate_original_sample_count; i++)
		{
			double normal_bitrate = (double)m_bitrate_original_sample_array[i] / max_sample_value;
			original_sample_sum += normal_bitrate;
			//original_power_sum += (normal_bitrate * normal_bitrate);
		}
		int bitrate_original_mean_value = (int)round(max_sample_value * original_sample_sum / n);

		//double original_sigma2 = original_power_sum - (original_sample_sum * original_sample_sum) / n;

		//if (original_sigma2 < 0)
		//{
		//	original_sigma2 = 0.0;
		//}

		//double original_sigma = sqrt(original_sigma2 / (n - 1));

		//m_bitrate_original_rms_value = (int)round(max_sample_value*original_sigma);

		//int64_t sum_delt2 = 0;
		//for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
		//{
		//	sum_delt2 += (int64_t)pow(m_bitrate_sample_array[sample_index] - m_bitrate_mean_value, 2);
		//}
		//sum_delt2 /= (m_bitrate_sample_count - 1);
		//int bitrate_rms_value = round(sqrt((double)sum_delt2));

		//if (fp_tsrate_dbase != NULL)
		//{
		//	//fprintf(fp_tsrate_dbase, ", %d, %d, %d", m_bitrate_mean_value, m_bitrate_rms_value, bitrate_rms_value);
		//	//fprintf(fp_tsrate_dbase, ", %d, %d, %d, %d,", m_bitrate_original_max_value, m_bitrate_original_mean_value, m_bitrate_original_min_value, m_bitrate_original_rms_value);
		//	fprintf(fp_tsrate_dbase, ", %d", m_bitrate_original_mean_value);
		//}

		//样本检验
		//int n = m_bitrate_original_sample_count;
		int* temp_array = (int*)malloc(n * sizeof(int));
		for (int i = 0; i < n; i++)
		{
			temp_array[i] = m_bitrate_original_sample_array[i];
		}

		quick_sort_method1(temp_array, 0, n - 1);
		assert(temp_array[0] == min_sample_value);
		assert(temp_array[n - 1] == max_sample_value);

		//min max deletion
		double deletion_sample_sum = 0;
		//double deletion_power_sum = 0;
		int m = n - 2;
		for (int i = 1; i < n - 1; i++)
		{
			double normal_bitrate = (double)temp_array[i] / max_sample_value;
			deletion_sample_sum += normal_bitrate;
			//deletion_power_sum += normal_bitrate * normal_bitrate;
		}
		int bitrate_deletion_mean_value = (int)round(max_sample_value * deletion_sample_sum / m);

		//double deletion_sigma2 = deletion_power_sum - (deletion_sample_sum * deletion_sample_sum) / m;

		//if (deletion_sigma2 < 0)
		//{
		//	deletion_sigma2 = 0.0;
		//}

		//double deletion_sigma = sqrt(deletion_sigma2 / (m - 1));

		//m_bitrate_deletion_rms_value = (int)round(max_sample_value*deletion_sigma);

		//if (m_bitrate_deletion_mean_value > m_bitrate_deletion_max_value)
		//{
		//	m_bitrate_deletion_max_value = m_bitrate_deletion_mean_value;
		//}
		//if (m_bitrate_deletion_mean_value < m_bitrate_deletion_min_value)
		//{
		//	m_bitrate_deletion_min_value = m_bitrate_deletion_mean_value;
		//}

		//Dixon deletion
		do
		{
			//if (fp_tsrate_dbase != NULL)
			//{
			//	fprintf(fp_tsrate_dbase, "[");
			//	for (int i = 0; i < n; i++)
			//	{
			//		fprintf(fp_tsrate_dbase, "%d ", temp_array[i]);
			//	}
			//	fprintf(fp_tsrate_dbase, "] ");
			//}

			double rh = 0, rl = 0;
			if ((n >= 3) && (n <= 7))
			{
				if (temp_array[n - 1] > temp_array[0])
				{
					rh = (double)(temp_array[n - 1] - temp_array[n - 2]) / (temp_array[n - 1] - temp_array[0]);
					rl = (double)(temp_array[1] - temp_array[0]) / (temp_array[n - 1] - temp_array[0]);
				}
			}
			else if ((n >= 8) && (n <= 10))
			{
				if (temp_array[n - 1] > temp_array[1])
				{
					rh = (double)(temp_array[n - 1] - temp_array[n - 2]) / (temp_array[n - 1] - temp_array[1]);
				}
				if (temp_array[n - 2] > temp_array[0])
				{
					rl = (double)(temp_array[1] - temp_array[0]) / (temp_array[n - 2] - temp_array[0]);
				}
			}
			else if ((n >= 11) && (n <= 13))
			{
				if (temp_array[n - 1] > temp_array[1])
				{
					rh = (double)(temp_array[n - 1] - temp_array[n - 3]) / (temp_array[n - 1] - temp_array[1]);
				}
				if (temp_array[n - 2] > temp_array[0])
				{
					rl = (double)(temp_array[2] - temp_array[0]) / (temp_array[n - 2] - temp_array[0]);
				}
			}
			else if ((n >= 14) && (n <= 30))
			{
				if (temp_array[n - 1] > temp_array[2])
				{
					rh = (double)(temp_array[n - 1] - temp_array[n - 3]) / (temp_array[n - 1] - temp_array[2]);
				}
				if (temp_array[n - 3] > temp_array[0])
				{
					rl = (double)(temp_array[2] - temp_array[0]) / (temp_array[n - 3] - temp_array[0]);
				}
			}

			//if (fp_tsrate_dbase != NULL)
			//{
			//	fprintf(fp_tsrate_dbase, "<n=%d&rh=%.3f&rl=%.3f>", n, rh, rl);
			//}

			if ((rh <= DIXON_950_alpha_table[n - 1]) && (rl <= DIXON_950_alpha_table[n - 1]))
			{
				break;
			}
			else
			{
				if (rh > DIXON_950_alpha_table[n - 1])
				{
					n--;
				}
				if (rl > DIXON_950_alpha_table[n - 1])
				{
					for (int i = 1; i < n; i++)
					{
						temp_array[i - 1] = temp_array[i];
					}

					n--;
				}
			}

		} while (n >= 3);

		double dixon_sample_sum = 0;
		//double dixon_power_sum = 0;
		for (int i = 0; i < n; i++)
		{
			double normal_bitrate = (double)temp_array[i] / max_sample_value;
			dixon_sample_sum += normal_bitrate;
			//dixon_power_sum += normal_bitrate * normal_bitrate;
		}
		int bitrate_dixon_mean_value = (int)round(max_sample_value * dixon_sample_sum / n);

		//double processed_sigma2 = dixon_power_sum - (dixon_sample_sum * dixon_sample_sum) / n;

		//if (processed_sigma2 < 0)
		//{
		//	processed_sigma2 = 0.0;
		//}

		//double processed_sigma = sqrt(processed_sigma2 / (n - 1));

		//m_bitrate_dixon_rms_value = (int)round(max_sample_value*processed_sigma);

		//if (m_bitrate_dixon_mean_value > m_bitrate_dixon_max_value)
		//{
		//	m_bitrate_dixon_max_value = m_bitrate_dixon_mean_value;
		//}
		//if (m_bitrate_dixon_mean_value < m_bitrate_dixon_min_value)
		//{
		//	m_bitrate_dixon_min_value = m_bitrate_dixon_mean_value;
		//}

		int amplitue1 = abs(bitrate_deletion_mean_value - m_bitrate_estimate_cur_value);
		int amplitue2 = abs(bitrate_dixon_mean_value - m_bitrate_estimate_cur_value);

		//基于码率变化是缓慢的假设，没有突变
		if (amplitue1 < amplitue2)
		{
			m_bitrate_estimate_cur_value = bitrate_deletion_mean_value;
		}
		else
		{
			m_bitrate_estimate_cur_value = bitrate_dixon_mean_value;
		}

		if (m_bitrate_estimate_cur_value > m_bitrate_estimate_max_value)
		{
			m_bitrate_estimate_max_value = m_bitrate_estimate_cur_value;
		}
		if (m_bitrate_estimate_cur_value < m_bitrate_estimate_min_value)
		{
			m_bitrate_estimate_min_value = m_bitrate_estimate_cur_value;
		}

		if (fp_tsrate_dbase != NULL)
		{
			//fprintf(fp_tsrate_dbase, ", %d, %d, %d, %d", m_bitrate_processed_max_value, m_bitrate_processed_mean_value, m_bitrate_processed_min_value, m_bitrate_processed_rms_value);
			fprintf(fp_tsrate_dbase, "%d, %d, %d, %d\n", bitrate_original_mean_value, bitrate_deletion_mean_value, bitrate_dixon_mean_value, m_bitrate_estimate_cur_value);
		}

		//if (m_bitrate_available == 0) m_bitrate_available = 1;
		if (callback != NULL)
		{
			callback(0, m_bitrate_estimate_cur_value);
		}

		free(temp_array);

		m_bitrate_original_sample_count = 0;
	}
	else
	{
		//if (fp_tsrate_dbase != NULL)
		//{
		//	fprintf(fp_tsrate_dbase, ", , , , , , , ,");
		//}
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

void CTransportStream::Seek(int64_t offset)
{
	m_llCurReadPos = offset;
	if (offset < m_llTotalFileLength)
	{
		m_nEOF = 0;
	}
	else
	{
		m_nEOF = 1;
	}

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		file_seek(offset);
	}
}

int64_t CTransportStream::Tell(void)
{
	return m_llCurReadPos;
}

int CTransportStream::GetLastError()
{
	return m_TsError;
}



