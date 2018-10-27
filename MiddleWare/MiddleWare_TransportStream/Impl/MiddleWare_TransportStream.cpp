#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>

#include "../Include/MiddleWare_TransportStream.h"
#include "libs_Mpeg&DVB/Mpeg_TSPacket\Include\Mpeg2_TS_ErrorCode.h"
#include "../Include/MiddleWare_TS_ErrorCode.h"

#include "HAL/HAL_TSInput/Include/Dektec_TransRecv.h"
#include "HAL/HAL_TSInput/Include/UDP_TransRecv.h"
#include "thirdparty_HW/SmartTS/smartts_drvapi.h"

#include "libs_Utilities\Include\XStream_Utilities.h"

#define MAX_PCR_FIELD_TRIGGER_COUNT			100

U32 receive_transport_stream(LPVOID lpParam)
{
	S32 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	U8	buf[30 * 1024];
//	U8	buf[6144];
	S32 rdsize;

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
						assert(ptransport_stream->m_hFile >= 0);

						if (ptransport_stream->m_nEOF == 0)
						{
							if (ptransport_stream->m_ts_fifo.GetAvailableSpace() >= sizeof(buf))
							{
								rdsize = _read(ptransport_stream->m_hFile, buf, sizeof(buf));
								if (rdsize > 0)
								{
									rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
								}

								S64 llCurFilePos = _telli64(ptransport_stream->m_hFile);

								if (llCurFilePos >= ptransport_stream->m_llTotalFileLength)
								{
									//到达文件尾，停止读文件
									ptransport_stream->m_nEOF = 1;

									//如果文件是以实时的方式打开，模拟实时分析环境
									if (ptransport_stream->m_nMode == 1)
									{
										ptransport_stream->SeekToBegin();
									}
								}
							}
							else
							{
								rdsize = 0;
								//FIFO空间不够，暂停写入。等待100ms
//								Sleep(1);
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
									rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
								}
								else
								{
									rtcode = MIDDLEWARE_TS_STREAM_NODATA_ERROR;				//没有采集到实时流
								}
							}
							else
							{
//								Sleep(1);
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
									rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
									if (rtcode != NO_ERROR)
									{
										assert(0);
									}
								}
								else
								{
									rtcode = MIDDLEWARE_TS_STREAM_NODATA_ERROR;				//没有采集到实时流
								}
							}
							else
							{
//								Sleep(1);
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
								rtcode = ptransport_stream->m_ts_fifo.WriteData(buf, rdsize);
								assert(rtcode == NO_ERROR);
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

	m_hFile			 = -1;
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
}


CTransportStream::~CTransportStream()
{
	//delete mp_ts_fifo;

	if (m_hFile != -1)
	{
		_close(m_hFile);
		m_hFile = -1;
	}
}

S32 CTransportStream::Open(char* tsin_option, char* tsin_description, int mode)
{
	S32	 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;					//默认返回成功
//	S8*	 device_name;
	S8*  pdest;
	S8   idx;
	S8   ip_addr[16];
	S32  port;
//	S32  netcard_index = 1;

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
	len = GetModulePathLength(pszExeFile);
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
		errno_t errcode = _sopen_s(&m_hFile, m_pszProtocolExt, _O_BINARY | _O_RDONLY, _SH_DENYWR, 0);

		if (m_hFile != -1)
		{
			_lseeki64(m_hFile, 0, SEEK_END);
			m_llTotalFileLength = _telli64(m_hFile);

			_lseeki64(m_hFile, 0, SEEK_SET);

//			m_llCurFilePos = 0;
			m_llCurReadPos = 0;

			m_nEOF = 0;

			rtcode = MIDDLEWARE_TS_NO_ERROR;
		}
		else
		{
			rtcode = MIDDLEWARE_TS_STREAM_OPEN_ERROR;						//打不开文件
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
	BuildDirectory(pszPcrPath);

	char file_name[128];
	U32 old_tickcount = ::GetTickCount();
	sprintf_s(file_name, sizeof(file_name), "%s\\tick_%08x_ts_bitrate.txt", pszPcrPath, old_tickcount);

	fp_dbase = NULL;
	if (m_bEnableTSRateDebug)
	{
		fopen_s(&fp_dbase, file_name, "wt");
	}
	if (fp_dbase != NULL)
	{
		fprintf(fp_dbase, "当前值, 均值, 标准差\n");
	}

	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)receive_transport_stream, (LPVOID)this, 0, 0);

	return rtcode;
}

S32 CTransportStream::Close()
{
	S32		rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

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

//	m_llCurFilePos						= 0;
	m_llCurReadPos = 0;
	m_llTotalFileLength					= 0;

	//ByteFIFO_Term(&m_ts_fifo);
	m_ts_fifo.Reset();

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		if (m_hFile != -1)
		{
			if (_close(m_hFile) == 0)
			{
				m_hFile = -1;
				m_TsError = MIDDLEWARE_TS_NO_ERROR;
				rtcode = MIDDLEWARE_TS_NO_ERROR;
			}
			else
			{
				m_hFile = -1;
				m_TsError = MIDDLEWARE_TS_FILE_CLOSE_ERROR;
				rtcode = MIDDLEWARE_TS_FILE_CLOSE_ERROR;								//无法关闭文件
			}
		}

		m_nEOF = 0;

		memset(m_pszProtocolExt, '\0', MAX_PATH);
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

	if (fp_dbase != NULL)
	{
		fclose(fp_dbase);
		fp_dbase = NULL;
	}

	return rtcode;
}

S32 CTransportStream::StartGetData(void)
{
	S32 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		rtcode = MIDDLEWARE_TS_NO_ERROR;
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

S32 CTransportStream::StopGetData(void)
{
	S32 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

	m_nReceiving = 0;

	//等待线程退出
	while (m_nStopReceiving == 0)
	{
		Sleep(100);
	}

	m_bSynced = 0;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
//		SeekToBegin();			//是否定位到文件头，由应用自行决定
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

	//m_bitrate_mean_value = 0;									//平滑后的比特率
	//m_bitrate_rms_value = 0;									//比特率抖动方差
	//m_bitrate_min_value = 123456789;									//平滑后的最小比特率
	//m_bitrate_max_value = -123456789;									//平滑后的最大比特率

	m_ts_fifo.Reset();

	return rtcode;
}

S32 CTransportStream::Reset()
{
	S32		rtcode = MIDDLEWARE_TS_NO_ERROR;

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


/*
S32 CTransportStream::GetData(void)
{
	S32 rtcode = NO_ERROR;

	U8	buf[30 * 1024];
//	U8	buf[6144];
	S32 rdsize;
	
	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		if (m_hFile != -1)
		{
			rdsize = _read(m_hFile, buf, sizeof(buf));
			if (rdsize > 0)
			{
				ByteFIFO_WriteRawData(&m_ts_fifo, buf, rdsize);
			}

#if LOW_LEVEL_IO
			m_dwCurFilePos = _telli64(m_hFile);
#else
			m_dwCurFilePos = _tell(m_hFile);
#endif
			//reach the end of the file
			if (rdsize < sizeof(buf))
			{
	//			SeekToBegin();
				rtcode = FILE_EOF_ERROR;				//到达文件尾
			}
		}
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			rdsize = smartts_receive(buf, 4096);

			if (rdsize > 0)
			{
				rtcode = ByteFIFO_WriteRawData(&m_ts_fifo, buf, rdsize);
			}
			else
			{
				rtcode = STREAM_NODATA_ERROR;				//没有采集到实时流
			}
		}
		else if (strcmp(m_pszProtocolExt, "DEKTEC") == 0)
		{
			rdsize = dektec_receive(buf, sizeof(buf));

			if (rdsize > 0)
			{
				rtcode = ByteFIFO_WriteRawData(&m_ts_fifo, buf, rdsize);
			}
			else
			{
				rtcode = STREAM_NODATA_ERROR;				//没有采集到实时流
			}
		}
	}
	else if (strcmp(m_pszProtocolHead, "UDP") == 0)
	{
		rdsize = udp_receive(buf, sizeof(buf));

		if (rdsize >0)
		{
			rtcode = ByteFIFO_WriteRawData(&m_ts_fifo, buf, rdsize);
		}
		else
		{
			rtcode = STREAM_NODATA_ERROR;				//没有采集到实时流
		}
	}
	else
	{
	}

	return rtcode;
}
*/

S32 CTransportStream::PrefetchOnePacket(U8* buf, S32* plength)
{
	S32						rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
	U32						wait_state = WAIT_FAILED;
//	U8*						rdptr;
//	U8*						old_rdptr;
//	U8*						old_wrptr;
//	S32						tail_length;
//	S32						copy_length;
	U8						temp_buffer[613];

	if ((buf != NULL) && (plength != NULL))
	{
		assert(*plength >= 188);
		*plength = 0;

		if (m_ts_fifo.GetAvailablePayload() > 0)
		{
			if (m_bSynced == 0)
			{
				//找同步
				while (m_ts_fifo.GetAvailablePayload() >= 613)		//3 * 204 + 1
				{
					m_ts_fifo.PrefetchData(temp_buffer, sizeof(temp_buffer));

					if ((temp_buffer[0] == 0x47) &&
						(temp_buffer[188] == 0x47) &&
						(temp_buffer[376] == 0x47) &&
						(temp_buffer[564] == 0x47))
					{
						m_bSynced = 1;
						m_nPacketLength = 188;

						break;
					}
					else if ((temp_buffer[0] == 0x47) &&
						(temp_buffer[204] == 0x47) &&
						(temp_buffer[408] == 0x47) &&
						(temp_buffer[612] == 0x47))
					{
						m_bSynced = 1;
						m_nPacketLength = 204;

						break;
					}
					else
					{
						m_ts_fifo.SkipData(1);
						m_llCurReadPos++;
					}
				}
			}

			if (m_bSynced == 1)
			{
				if (m_ts_fifo.GetAvailablePayload() >= (m_nPacketLength + 1))
				{
					m_ts_fifo.PrefetchData(temp_buffer, (m_nPacketLength + 1));

					if ((temp_buffer[0] == 0x47) && (temp_buffer[m_nPacketLength] == 0x47))
					{
						memcpy(buf, temp_buffer, m_nPacketLength);
						*plength = m_nPacketLength;
						
						rtcode = MIDDLEWARE_TS_NO_ERROR;
					}
					else
					{
						m_bSynced = FALSE;
						m_nPacketLength = 0;

						rtcode = ETR290_TS_SYNC_LOSS;						//同步丢失
					}
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
				if (m_nEOF == 1)
				{
					rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
				}
				else
				{
					rtcode = ETR290_TS_SYNC_LOSS;
				}
			}
		}
		else
		{
			if (m_nEOF == 1)
			{
				rtcode = MIDDLEWARE_TS_FILE_EOF_ERROR;
			}
			else
			{
				rtcode = MIDDLEWARE_TS_FIFO_EMPTY_ERROR;						//FIFO空
			}
		}
	}
	else
	{
		rtcode = MIDDLEWARE_TS_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}

S32 CTransportStream::SkipOnePacket(void)
{
	S32	rtcode = NO_ERROR;

	m_llCurReadPos += m_nPacketLength;
	m_ts_fifo.SkipData(m_nPacketLength);

	return rtcode;
}

S32 CTransportStream::StartGetBitrate(void)
{
	S32 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

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

S32 CTransportStream::StopGetBitrate(void)
{
	S32 rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;

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

int CTransportStream::GetBitrateMap(ULONG pbitrate_map[], int count)
{
	int		rtcode = MIDDLEWARE_TS_UNKNOWN_ERROR;
	int		readcount;
	double	coeff = 8.0;
	
	if (m_hFile == -1)
	{
		coeff *= mf_actual_scan_frequency;

		if (count >= mn_actual_timeslice)
		{
			readcount = mn_actual_timeslice;
		}
		else
		{
			readcount = count;
		}

#ifdef _WIN64
#else
		rtcode = smartts_get_asiin_bitrate_map(pbitrate_map, readcount);

		if (rtcode > 0)
		{
			for (int i = 0; i < readcount; i++)
			{
				temp = pbitrate_map[i];
				temp *= coeff;
				pbitrate_map[i] = (ULONG)temp;
			}
		}
#endif

	}

	return rtcode;
}

S32 CTransportStream::GetBitrate(void)
{
	S32  rtvalue = -1;

	if (strcmp(m_pszProtocolHead, "FILE") == 0)
	{
		//为了调试程序，暂时假定流的速率为1000Mbps，以后更正     chendelin 2012.01.02
		rtvalue = m_bitrate_mean_value;
	}
	else if (strcmp(m_pszProtocolHead, "ASI") == 0)
	{
#ifdef _WIN64
#else
		if (strcmp(m_pszProtocolExt, "SMARTTS") == 0)
		{
			bitrate_value = smartts_get_asiin_bitrate();
			if (bitrate_value >= 0)
			{
				m_bitrate_mean_value = (S32)(bitrate_value * 8 * mf_actual_1spulse_frequency);
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

S32 CTransportStream::GetMeasuredBitrateAttribute(BITRATE_ATTRIBUTE_t* pattr)
{
	S32 rtcode = NO_ERROR;
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

S32 CTransportStream::AddBitrateSample(int bitrate)
{
	m_bitrate_cur_value = bitrate;
	if (fp_dbase != NULL)
	{
		fprintf(fp_dbase, "%d", m_bitrate_cur_value);
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
		S64 data_rate_sum = 0;
		for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
		{
			data_rate_sum += m_bitrate_sample_array[sample_index];
		}
		m_bitrate_mean_value = (S32)(data_rate_sum / m_bitrate_sample_count);

		m_bitrate_available = 1;

		data_rate_sum = 0;
		for (int sample_index = 0; sample_index < m_bitrate_sample_count; sample_index++)
		{
			data_rate_sum += (S64)pow(m_bitrate_sample_array[sample_index] - m_bitrate_mean_value, 2);
		}
		data_rate_sum /= m_bitrate_sample_count;
		m_bitrate_rms_value = (S32)sqrt((double)data_rate_sum);

		if (fp_dbase != NULL)
		{
			fprintf(fp_dbase, ", %d, %d", m_bitrate_mean_value, m_bitrate_rms_value);
		}
	}
	if (fp_dbase != NULL)
	{
		fprintf(fp_dbase, "\n");
	}

	return NO_ERROR;
}


void CTransportStream::SeekToBegin(void)
{
	if (m_hFile != -1)
	{
		_lseeki64(m_hFile, 0, SEEK_SET);
		m_nEOF = 0;
	}
}
/*
void CTransportStream::Seek(S64 fileoffset)
{
	if (m_hFile != -1)
	{
//		m_llCurFilePos = fileoffset;
		_lseeki64(m_hFile, fileoffset, SEEK_SET);

		S64 llCurFilePos = _telli64(m_hFile);
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
S64 CTransportStream::Tell(void)
{
	return m_llCurReadPos;
}

S32 CTransportStream::GetLastError()
{
	return m_TsError;
}



