#include <string.h>
#include <math.h>
#include <Windows.h>
#include <assert.h>


#include "../Include/MiddleWare_DB_Pcrs.h"
#include "../Include/MiddleWare_DB_ErrorCode.h"

#include "toolbox_libs\TOOL_Directory\Include\TOOL_Directory.h"

#define UNCREDITABLE_MAX_VALUE				-123456789
#define UNCREDITABLE_MIN_VALUE				123456789

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

CDB_Pcrs::CDB_Pcrs()
{
	int	i;

	for (i = 0; i < MAX_SUPPORTED_PCR_PIDS; i++)
	{
		m_pPCR_info[i] = NULL;
		//fp_debug[i] = NULL;
	}
	m_pcr_pid_count = 0;

	Reset();
}

CDB_Pcrs::~CDB_Pcrs()
{
	//for (int i = 0; i < MAX_SUPPORTED_PCR_PIDS; i++)
	//{
	//	if (fp_debug[i] != NULL)
	//	{
	//		fclose(fp_debug[i]);
	//		fp_debug[i] = NULL;
	//	}
	//}

	Reset();
}

void CDB_Pcrs::Reset(void)
{
	int	i;

	//m_jitter_mean_value = UNCREDITABLE_MAX_VALUE;
	//m_jitter_rms_value = UNCREDITABLE_MAX_VALUE;
	//m_jitter_min_value = UNCREDITABLE_MIN_VALUE;
	//m_jitter_max_value = UNCREDITABLE_MAX_VALUE;				//100ms

	//m_interval_min_value = UNCREDITABLE_MIN_VALUE;
	//m_interval_mean_value = UNCREDITABLE_MAX_VALUE;
	//m_interval_max_value = UNCREDITABLE_MAX_VALUE;				//100ms
	//m_interval_rms_value = UNCREDITABLE_MAX_VALUE;

	for (i = 0; i < m_pcr_pid_count; i++)
	{
		if (m_pPCR_info[i] != NULL)
		{
			if (m_pPCR_info[i]->fp_dbase != NULL)
			{
				fclose(m_pPCR_info[i]->fp_dbase);
			}
			delete m_pPCR_info[i];
			m_pPCR_info[i] = NULL;
		}
		//if (fp_debug[i] != NULL)
		//{
		//	fclose(fp_debug[i]);
		//	fp_debug[i] = NULL;
		//}
	}
	m_pcr_pid_count = 0;

	char	pszExeFile[256];
	char	pszIniFile[256];
	int		len;

	GetModuleFileNameA(NULL, pszExeFile, sizeof(pszExeFile));
	len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile);

	m_bEnablePcrDebug = GetPrivateProfileIntA("编程者诊断", "PCR", 0, pszIniFile);
}

int CDB_Pcrs::VlookupPCRRecord(uint16_t usPCRPID)
{
	int rtcode;
	int	pcr_index;
	int found_pcr = 0;

	for (pcr_index = 0; pcr_index < m_pcr_pid_count; pcr_index++)
	{
		if (m_pPCR_info[pcr_index]->PCR_PID == usPCRPID)
		{
			found_pcr = 1;
			break;
		}
	}

	if (found_pcr)
	{
		rtcode = pcr_index;		// >= 0
	}
	else
	{
		if (m_pcr_pid_count >= MAX_SUPPORTED_PCR_PIDS)
		{
			rtcode = -2;				//full
		}
		else
		{
			rtcode = -1;				//has free slot
		}
	}

	return rtcode;

}

int CDB_Pcrs::GetTotalRecordCount(void)
{
	return m_pcr_pid_count;
}

//采用副本拷贝的方式，以保护原始数据不被破坏
int CDB_Pcrs::GetRecordByIndex(int index, RECORD_PCR_t* pPcrInfo)
{
	int	rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;

	if ((index >= 0) && (index < m_pcr_pid_count))
	{
		if (pPcrInfo != NULL)
		{
			memcpy(pPcrInfo, m_pPCR_info[index], sizeof(RECORD_PCR_t));
		}

		rtcode = MIDDLEWARE_DB_NO_ERROR;
	}

	return rtcode;
}

//采用副本拷贝的方式，以保护原始数据不被破坏
int CDB_Pcrs::GetRecordByPID(uint16_t usPCRPID, RECORD_PCR_t* pPcrInfo)
{
	int	pcr_index;
	
	pcr_index = VlookupPCRRecord(usPCRPID);
	if (pcr_index >= 0)
	{
		if (pPcrInfo != NULL)
		{
			memcpy(pPcrInfo, m_pPCR_info[pcr_index], sizeof(RECORD_PCR_t));
		}
	}

	return pcr_index;
}

// pos	--	ts packet start byte offset
int CDB_Pcrs::AddPCRSample(uint16_t usPcrPID, int64_t pos, PCR_code_t* pPCRCode, int ext_reference_bitrate, int(*callback)(int, int, int))
{
	int			rtcode = MIDDLEWARE_DB_UNKNOWN_ERROR;

	PCR_code_t  old_PCR_code;

	int64_t		old_pcr_pos;
	int			pcr_code_diff;
	int			pos_diff;

	int64_t		code_time_diff_value;
	int64_t		ts_time_diff_value;

	int			data_rate_new;
	//int64_t		data_rate_sum;
	int			jitter_cur_value;
	int			interval_cur_value;
	int			sample_index;
	//int64_t		pcr_pos = pos + 12;				//必须要确保pcr_pos 要大于pos。程序需要考虑计数溢出的问题
	int64_t		pcr_pos = pos;
	uint32_t	old_tickcount;

	int64_t sum = 0;
	int	pcr_index;
	int	count = 0;

	RECORD_PCR_t*	pInfo;

	assert(pPCRCode != NULL);

	if (pPCRCode != NULL)
	{
		pcr_index = VlookupPCRRecord(usPcrPID);
		if (pcr_index == -1)		//新的PCR，建立参考
		{
			pInfo = new RECORD_PCR_t;
			memset(pInfo, 0x00, sizeof(RECORD_PCR_t));

			pInfo->encoder_bitrate_sample_count = 0;
			pInfo->encoder_bitrate_sample_index = 0;

			pInfo->jitter_sample_count = 0;
			pInfo->jitter_sample_index = 0;
			pInfo->interval_sample_count = 0;
			pInfo->interval_sample_index = 0;

			pInfo->pcr_code_positive_discontinuity_count = 0;
			pInfo->pcr_code_negative_discontinuity_count = 0;
			pInfo->pcr_ac_error_count = 0;
			pInfo->pcr_interval_error_count = 0;

			pInfo->PCR_PID = usPcrPID;
			pInfo->PCR_pos = pcr_pos;
			memcpy(&(pInfo->PCR_code), pPCRCode, sizeof(PCR_code_t));

			pInfo->encoder_bitrate_mean_value = 0;
			pInfo->encoder_bitrate_rms_value = 0;
			pInfo->encoder_bitrate_available = 0;

			pInfo->jitter_mean_value = UNCREDITABLE_MAX_VALUE;
			pInfo->jitter_rms_value = UNCREDITABLE_MAX_VALUE;
			//pInfo->jitter_available = 0;
			pInfo->jitter_min_value = UNCREDITABLE_MIN_VALUE;
			pInfo->jitter_max_value = UNCREDITABLE_MAX_VALUE;

			pInfo->interval_max_value = UNCREDITABLE_MAX_VALUE;		//ms
			pInfo->interval_min_value = UNCREDITABLE_MIN_VALUE;
			pInfo->interval_mean_value = UNCREDITABLE_MAX_VALUE;
			pInfo->interval_rms_value = UNCREDITABLE_MAX_VALUE;
			//pInfo->interval_available = 0;

			pcr_index = m_pcr_pid_count;
			m_pPCR_info[pcr_index] = pInfo;
			m_pcr_pid_count++;

			old_tickcount = ::GetTickCount();

			char	pszExeFile[MAX_PATH];
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
			sprintf_s(file_name, sizeof(file_name), "%s\\tick_%08x_pcr_debug_0x%04X.txt", pszPcrPath, old_tickcount, pInfo->PCR_PID);

			if (m_bEnablePcrDebug)
			{
				fopen_s(&(pInfo->fp_dbase), file_name, "wt");
			}

			if (pInfo->fp_dbase != NULL)
			{
				fprintf(pInfo->fp_dbase, "pos, bits:32-30, bits:29-15, bits:14-0, extension, pos_diff, pcr_code_diff, deltT_PCR(ns), 计算比特率(bps), 外部码率(bps), deltT_TS(ns), PCR_AC(ns), PCR间隔(us), 间隔均值(us), 间隔方差(us), AC均值(ns), AC rms(ns)\n");
				fprintf(pInfo->fp_dbase, "%lld, %d, %d, %d, %d\n",
					pInfo->PCR_pos, 
					pInfo->PCR_code.base_32_30,
					pInfo->PCR_code.base_29_15,
					pInfo->PCR_code.base_14_0,
					pInfo->PCR_code.extension);
			}
			rtcode = PCR_PARSE_FIRST_SAMPLE;
		}
		else if (pcr_index >= 0)
		{
			pInfo = m_pPCR_info[pcr_index];
			assert(pInfo != NULL);

			//读出旧值
			memcpy(&old_PCR_code, &(pInfo->PCR_code), sizeof(PCR_code_t));
			old_pcr_pos = pInfo->PCR_pos;

			//保存当前TS包中的PCR值
			memcpy(&(pInfo->PCR_code), pPCRCode, sizeof(PCR_code_t));
			pInfo->PCR_pos = pcr_pos;

			if (pInfo->fp_dbase != NULL)
			{
				fprintf(pInfo->fp_dbase, "%I64d, %d, %d, %d, %d",
					pInfo->PCR_pos,
					pInfo->PCR_code.base_32_30,
					pInfo->PCR_code.base_29_15,
					pInfo->PCR_code.base_14_0,
					pInfo->PCR_code.extension);
			}

			pos_diff = (int)(pInfo->PCR_pos - old_pcr_pos);
			if (pos_diff > 0)
			{
				if (pInfo->fp_dbase != NULL)
				{
					fprintf(pInfo->fp_dbase, ", %d", pos_diff);
				}

				pcr_code_diff = MPEG_PCR_minus(&(pInfo->PCR_code), &old_PCR_code);
				if (pcr_code_diff >= 0)
				{
					//正常情况下PCR计数器应该是增长的，但是码流播放器播放文件时会循环播，所以会出现跳变点
					//或者实时PCR计数器由于计数位数不足，也会溢出产生跳变点

					if (pInfo->fp_dbase != NULL)
					{
						fprintf(pInfo->fp_dbase, ", %d", pcr_code_diff);
					}

					code_time_diff_value = (int64_t)(pcr_code_diff * 37.037);					//ns，会溢出， 37.037不是常数，是根据系统27MHz估算出来的，系统27MHz时钟是不稳定的
					if (pInfo->fp_dbase != NULL)
					{
						fprintf(pInfo->fp_dbase, ", %I64d", code_time_diff_value);
					}

					//计算新码率
					data_rate_new = (int)(pos_diff * 8000.0 / code_time_diff_value * 1000000);
					if (pInfo->fp_dbase != NULL)
					{
						fprintf(pInfo->fp_dbase, ", %d", data_rate_new);
					}
					pInfo->encoder_bitrate_cur_value = data_rate_new;
					pInfo->encoder_bitrate_available = 1;

					//int stable_stream = 1;
					if (ext_reference_bitrate > 0)
					{
						if (pInfo->fp_dbase != NULL)
						{
							fprintf(pInfo->fp_dbase, ", %d", ext_reference_bitrate);
						}

						//PCR间隔以实际检测到的字节接收时间间隔为准
						ts_time_diff_value = (int64_t)((8000000000.0 / ext_reference_bitrate) * pos_diff);			//ns
						if (pInfo->fp_dbase != NULL)
						{
							fprintf(pInfo->fp_dbase, ", %I64d", ts_time_diff_value);
						}

						//计算PCR_AC，等效于在复用器输出端测量
						jitter_cur_value = (int)(code_time_diff_value - ts_time_diff_value);		//参考TR101 290计算定义
						pInfo->jitter_cur_value = jitter_cur_value;

						if (pInfo->fp_dbase != NULL)
						{
							fprintf(pInfo->fp_dbase, ", %d", pInfo->jitter_cur_value);
						}

						if (jitter_cur_value > 500)		//500ns
						{
							pInfo->pcr_ac_error_count++;
						}
						if (jitter_cur_value < -500)
						{
							pInfo->pcr_ac_error_count++;
						}

						if (jitter_cur_value > pInfo->jitter_max_value)
						{
							pInfo->jitter_max_value = jitter_cur_value;
						}
						if (jitter_cur_value < pInfo->jitter_min_value)
						{
							pInfo->jitter_min_value = jitter_cur_value;
						}

						//if (pInfo->jitter_max_value > m_jitter_max_value)
						//{
						//	m_jitter_max_value = pInfo->jitter_max_value;
						//}
						//if (pInfo->jitter_min_value < m_jitter_min_value)
						//{
						//	m_jitter_min_value = pInfo->jitter_min_value;
						//}

						interval_cur_value = (int)(round(ts_time_diff_value / 1000.0));			//us
						//interval_cur_value = ts_time_diff_value;			//ns

						pInfo->interval_cur_value = interval_cur_value;
						if (interval_cur_value > pInfo->interval_max_value)
						{
							pInfo->interval_max_value = interval_cur_value;
						}
						if (interval_cur_value < pInfo->interval_min_value)
						{
							pInfo->interval_min_value = interval_cur_value;
						}

						//if (pInfo->interval_max_value > m_interval_max_value)
						//{
						//	m_interval_max_value = pInfo->interval_max_value;
						//}
						//if (pInfo->interval_min_value < m_interval_min_value)
						//{
						//	m_interval_min_value = pInfo->interval_min_value;
						//}

						if (pInfo->interval_cur_value > 100000)
						{
							pInfo->pcr_interval_error_count++;
							if (pInfo->fp_dbase != NULL)
							{
								fprintf(pInfo->fp_dbase, " *%d, *%d, *%d", pInfo->interval_cur_value, pInfo->interval_mean_value, pInfo->interval_rms_value);
								//fprintf(pInfo->fp_dbase, ", *%d, *%d, *%d", pInfo->interval_cur_value, pInfo->interval_mean_value, pInfo->interval_rms_value);
								//fprintf(pInfo->fp_dbase, ", *%d, *%d, *%d", pInfo->interval_cur_value, pInfo->interval_mean_value, pInfo->interval_rms_value);
							}
						}
						else
						{
							//pInfo->interval_available = 1;
							if (pInfo->fp_dbase != NULL)
							{
								fprintf(pInfo->fp_dbase, ", %d", pInfo->interval_cur_value);
							}

							if (callback != NULL)
							{
								callback(usPcrPID, pInfo->interval_cur_value, pInfo->jitter_cur_value);
							}

							//PCR间隔太长的样本是否要保留，是否要参与均值和方差计算？
							pInfo->interval_sample_array[pInfo->interval_sample_index] = pInfo->interval_cur_value;
							pInfo->interval_sample_index++;
							pInfo->interval_sample_index %= PCR_FIFO_LENGTH;
							if (pInfo->interval_sample_count < PCR_FIFO_LENGTH)
							{
								pInfo->interval_sample_count++;
							}

							if (pInfo->interval_sample_count >= 2)
							{
								double interval_sum = 0;
								for (sample_index = 0; sample_index < pInfo->interval_sample_count; sample_index++)
								{
									interval_sum += pInfo->interval_sample_array[sample_index];
								}
								pInfo->interval_mean_value = (int)(interval_sum / pInfo->interval_sample_count);

								////均值滤波，可以改成alfa-bete滤波，或者其他算法
								//sum = 0;
								//count = 0;
								//for (int i = 0; i < m_pcr_pid_count; i++)
								//{
								//	if (m_pPCR_info[i]->interval_available)
								//	{
								//		sum += m_pPCR_info[i]->interval_mean_value;
								//		count++;
								//	}
								//}

								//if (count > 0)
								//{
								//	m_interval_mean_value = (int)(sum / count);
								//}

								interval_sum = 0;
								for (sample_index = 0; sample_index < pInfo->interval_sample_count; sample_index++)
								{
									interval_sum += pow(pInfo->interval_sample_array[sample_index] - pInfo->interval_mean_value, 2);
								}
								interval_sum /= (pInfo->interval_sample_count - 1);
								pInfo->interval_rms_value = (int)sqrt(interval_sum);

								//int interval_max_rms_value = 0;
								//for (int i = 0; i < m_pcr_pid_count; i++)
								//{
								//	if (m_pPCR_info[i]->interval_available)
								//	{
								//		if (m_pPCR_info[i]->interval_rms_value > interval_max_rms_value)
								//		{
								//			interval_max_rms_value = m_pPCR_info[i]->interval_rms_value;
								//		}
								//	}
								//}

								//m_interval_rms_value = interval_max_rms_value;

								if (pInfo->fp_dbase != NULL)
								{
									fprintf(pInfo->fp_dbase, ", %d, %d", pInfo->interval_mean_value, pInfo->interval_rms_value);
								}
							}
							else
							{
								if (pInfo->fp_dbase != NULL)
								{
									fprintf(pInfo->fp_dbase, ", , ");
								}
							}

							//						int jitter_filter_thread = max(50000, 5 * m_jitter_rms_value);
							int jitter_filter_thread = 50000;
							if (abs(jitter_cur_value) < jitter_filter_thread)			//为什么要放大5倍？？？
							{
								//对抖动不超过2000ns秒的PCR值视为有效，参与均值和方差计算
								pInfo->jitter_sample_array[pInfo->jitter_sample_index] = pInfo->jitter_cur_value;
								pInfo->jitter_sample_index++;
								pInfo->jitter_sample_index %= PCR_FIFO_LENGTH;
								if (pInfo->jitter_sample_count < PCR_FIFO_LENGTH)
								{
									pInfo->jitter_sample_count++;
								}

								if (pInfo->jitter_sample_count >= 2)
								{
									double jitter_sum = 0.0;
									for (sample_index = 0; sample_index < pInfo->jitter_sample_count; sample_index++)
									{
										jitter_sum += pInfo->jitter_sample_array[sample_index];
									}
									pInfo->jitter_mean_value = (int)(jitter_sum / pInfo->jitter_sample_count);
									if (pInfo->fp_dbase != NULL)
									{
										fprintf(pInfo->fp_dbase, ", %d", pInfo->jitter_mean_value);
									}

									//pInfo->jitter_available = 1;

									//sum = 0;
									//count = 0;
									//for (int i = 0; i < m_pcr_pid_count; i++)
									//{
									//	if (m_pPCR_info[i]->jitter_available)
									//	{
									//		sum += m_pPCR_info[i]->jitter_mean_value;
									//		count++;
									//	}
									//}
									//if (count > 0)
									//{
									//	m_jitter_mean_value = (int)(sum / count);
									//}

									jitter_sum = 0.0;
									for (sample_index = 0; sample_index < pInfo->jitter_sample_count; sample_index++)
									{
										jitter_sum += pow(pInfo->jitter_sample_array[sample_index] - pInfo->jitter_mean_value, 2);
									}
									jitter_sum /= (pInfo->jitter_sample_count - 1);
									int jitter_rms_value = (int)sqrt(jitter_sum);
									if (jitter_rms_value > pInfo->jitter_rms_value)
									{
										pInfo->jitter_rms_value = jitter_rms_value;
									}
									if (pInfo->fp_dbase != NULL)
									{
										fprintf(pInfo->fp_dbase, ", %d", pInfo->jitter_rms_value);
									}

									//jitter_sum = 0.0;
									//for (sample_index = 0; sample_index < pInfo->jitter_sample_count; sample_index++)
									//{
									//	jitter_sum += pow(pInfo->jitter_sample_array[sample_index] - m_jitter_mean_value, 2);
									//}
									//jitter_sum /= pInfo->jitter_sample_count;
									//jitter_rms_value = (int)sqrt(jitter_sum);
									//if (jitter_rms_value > m_jitter_rms_value)
									//{
									//	m_jitter_rms_value = jitter_rms_value;
									//}
								}
							}
							else
							{
								if (pInfo->fp_dbase != NULL)
								{
									if (pInfo->jitter_sample_count >= 2)
									{
										fprintf(pInfo->fp_dbase, ", *%d, *%d", pInfo->jitter_mean_value, pInfo->jitter_rms_value);
									}
								}
							}
						}
					}

					rtcode = MIDDLEWARE_DB_NO_ERROR;
				}
				else
				{
					if (pInfo->fp_dbase != NULL)
					{
						fprintf(pInfo->fp_dbase, ", *%d", pcr_code_diff);
					}

					//assert(0);
					//正常情况下PCR计数器应该是增长的，但是码流播放器播放文件时会循环播，所以会出现跳变点
					//或者实时PCR计数器由于计数位数不足，也会溢出产生跳变点
					//pInfo->jitter_available = 0;
					pInfo->pcr_code_negative_discontinuity_count++;

					rtcode = PCR_PARSE_NEGATIVE_DISCONTINUITY;
				}
			}
			else
			{
				if (pInfo->fp_dbase != NULL)
				{
					fprintf(pInfo->fp_dbase, ", *%d",	pos_diff);
				}
				//位置计数器发生溢出
				//assert(0);
			}

			if (pInfo->fp_dbase != NULL)
			{
				fprintf(pInfo->fp_dbase, "\n");
			}
		}
	}
	else
	{
		rtcode = PCR_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//int CDB_Pcrs::IsValidBitrate(int newValue, int refValue, int var, int timeLine)
//{
//	int valid_sample = 0;
//
//	if (timeLine < 16)		////为什么是16个样本？
//	{
//		valid_sample = 1;
//	}
//	else if (timeLine < 32)		////为什么是64个样本？
//	{
//		if (fabs(refValue - newValue) < fabs(0.2*refValue))		//门限大于200K bps
//		{
//			valid_sample = 1;
//		}
//	}
//	else if (timeLine < 64)		////为什么是64个样本？
//	{
//		if (fabs(refValue - newValue) < fabs(0.1*refValue))		//门限大于100K bps
//		{
//			valid_sample = 1;
//		}
//	}
//	else if (timeLine < 128)		////为什么是64个样本？
//	{
//		double threshold_128;
//
//		if (var == 0)
//		{
//			threshold_128 = fabs(0.1 * refValue);
//		}
//		else
//		{
//			threshold_128 = min(20 * var, fabs(0.1*refValue));
//		}
//
//		if (fabs(refValue - newValue) < threshold_128)
//		{
//			valid_sample = 1;
//		}
//	}
//	else
//	{
//		double threshold_other;
//
//		if (var == 0)
//		{
//			threshold_other = fabs(0.1 * refValue);
//		}
//		else
//		{
//			threshold_other = min(15 * var, fabs(0.1*refValue));
//		}
//
//		if (fabs(refValue - newValue) < threshold_other)//误差范围不超过100Kbps，为什么要过滤？
//		{
//			valid_sample = 1;
//		}
//	}
//
//	return valid_sample;
//}


//int CDB_Pcrs::GetMeasuredJitterAttribute(PCR_JITTER_ATTRIBUTE_t* pattr)
//{
//	int rtcode = MIDDLEWARE_DB_NO_ERROR;
//	if (pattr != NULL)
//	{
//		pattr->min = m_jitter_min_value;
//		pattr->mean = m_jitter_mean_value;
//		pattr->max = m_jitter_max_value;
//		pattr->rms = m_jitter_rms_value;
//	}
//	else
//	{
//		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
//	}
//
//	return rtcode;
//}

//int CDB_Pcrs::GetMeasuredIntervalAttribute(PCR_INTERVAL_ATTRIBUTE_t* pattr)
//{
//	int rtcode = MIDDLEWARE_DB_NO_ERROR;
//	if (pattr != NULL)
//	{
//		pattr->min = m_interval_min_value;
//		pattr->mean = m_interval_mean_value;
//		pattr->max = m_interval_max_value;
//		pattr->rms = m_interval_rms_value;
//	}
//	else
//	{
//		rtcode = MIDDLEWARE_DB_PARAMETER_ERROR;
//	}
//
//	return rtcode;
//}

int MPEG_PCR_minus(PCR_code_t* pcr_code_minuend, PCR_code_t* pcr_code_substractor)
{
	int64_t pcr_value_minuend, pcr_value_substractor;
	int pcr_diff = -1;

	if ((pcr_code_minuend != NULL) && (pcr_code_substractor != NULL))
	{
		pcr_value_minuend = pcr_code_minuend->base_32_30;
		pcr_value_minuend <<= 15;
		pcr_value_minuend |= pcr_code_minuend->base_29_15;
		pcr_value_minuend <<= 15;
		pcr_value_minuend |= pcr_code_minuend->base_14_0;
		pcr_value_minuend *= 300;
		pcr_value_minuend += pcr_code_minuend->extension;

		pcr_value_substractor = pcr_code_substractor->base_32_30;
		pcr_value_substractor <<= 15;
		pcr_value_substractor |= pcr_code_substractor->base_29_15;
		pcr_value_substractor <<= 15;
		pcr_value_substractor |= pcr_code_substractor->base_14_0;
		pcr_value_substractor *= 300;
		pcr_value_substractor += pcr_code_substractor->extension;

		pcr_diff = (int)(pcr_value_minuend - pcr_value_substractor);
	}

	return pcr_diff;
}
