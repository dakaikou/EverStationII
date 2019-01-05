#ifndef _TSMAGIC_DBASE_PCR_H_
#define _TSMAGIC_DBASE_PCR_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_DB_EXPORT
#       define MW_DB_LIB __declspec(dllexport)
#   elif defined(_MW_DB_IMPORT)
#       define MW_DB_LIB __declspec(dllimport)
#   else
#       define MW_DB_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_DB_LIB __attribute__((visibility("default")))
#else
#   define MW_DB_LIB
#endif

#include <stdint.h>
#include <stdio.h>
#include "syntax_translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

#define PCR_FIFO_LENGTH						1024
#define MAX_SUPPORTED_PCR_PIDS				10

typedef struct
{
	int	mean;
	int	rms;			//均方根值
	int	min;
	int	max;

} PCR_JITTER_ATTRIBUTE_t;

typedef struct
{
	int	mean;
	int	rms;			//均方根值
	int	min;
	int	max;

} PCR_INTERVAL_ATTRIBUTE_t;

typedef struct
{
	uint16_t			PCR_PID;

	//记录PCR的编码
	PCR_code_t	PCR_code;
	int			pcr_code_positive_discontinuity_count;
	int			pcr_code_negative_discontinuity_count;

	//记录PCR在流中出现的位置或者时间
	int64_t			PCR_pos;
	int64_t			TickCount;

	//记录PCR抖动测量值
	//int			jitter_available;
	int			jitter_cur_value;
	int			jitter_mean_value;
	int			jitter_rms_value;			//均方根值
	int			jitter_min_value;
	int			jitter_max_value;

	int			jitter_sample_index;
	int			jitter_sample_count;
	int			jitter_sample_array[PCR_FIFO_LENGTH];

	//记录PCR发送间隔测量值
	//int			interval_available;
	int			interval_cur_value;
	int			interval_mean_value;
	int			interval_rms_value;
	int			interval_min_value;
	int			interval_max_value;

	int			interval_sample_index;
	int			interval_sample_count;
	int			interval_sample_array[PCR_FIFO_LENGTH];

	int			pcr_ac_error_count;			//+/- 500ns
	int			pcr_interval_error_count;		//>100ms

	//记录编码端的27MHz时钟、码率测量值，也是猜测的
	int			encoder_bitrate_available;
	int			encoder_bitrate_cur_value;
	int			encoder_bitrate_mean_value;
	int			encoder_bitrate_rms_value;
	int			encoder_bitrate_sample_array[PCR_FIFO_LENGTH];
	int			encoder_bitrate_sample_count;
	int			encoder_bitrate_sample_index;

	//数据库指针，暂时采用写文件的方式，将来（只要我还活着）可以改成数据库
	FILE*		fp_dbase;

} RECORD_PCR_t;

class MW_DB_LIB CDB_Pcrs
{
public:
	CDB_Pcrs(void);
	~CDB_Pcrs(void);

protected:

	int		m_bEnablePcrDebug;

	RECORD_PCR_t*	m_pPCR_info[MAX_SUPPORTED_PCR_PIDS];
	int			m_pcr_pid_count;

	//int		m_jitter_min_value;
	//int		m_jitter_max_value;
	//int		m_jitter_mean_value;			//随时间变化，逐步收敛稳定
	//int		m_jitter_rms_value;				//随时间变化，逐步收敛稳定

	//int		m_interval_mean_value;
	//int		m_interval_min_value;
	//int		m_interval_max_value;
	//int		m_interval_rms_value;

protected:

	//int IsValidBitrate(int newValue, int refValue, int var, int timeLine);
	int VlookupPCRRecord(uint16_t usPCRPID);

public:
	int GetTotalRecordCount(void);
	int GetRecordByPID(uint16_t usPCRPID, RECORD_PCR_t* pPcrInfo = NULL);
	int GetRecordByIndex(int index, RECORD_PCR_t* pPcrInfo = NULL);
	int AddPCRSample(uint16_t usPcrPID, int64_t pos, PCR_code_t* pPCRCode, int ref_bitrate, int(*callback)(int,int,int));

	//int GetMeasuredJitterAttribute(PCR_JITTER_ATTRIBUTE_t* pattr);
	//int GetMeasuredIntervalAttribute(PCR_INTERVAL_ATTRIBUTE_t* pattr);

	void  Reset(void);
private:
};

int MPEG_PCR_minus(PCR_code_t* pcr_code_minuend, PCR_code_t* pcr_code_substractor);

#endif	//_TSMAGIC_DBASE_PCR_H_
