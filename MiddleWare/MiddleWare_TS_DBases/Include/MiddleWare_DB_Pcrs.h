#ifndef _TSMAGIC_DBASE_PCR_H_
#define _TSMAGIC_DBASE_PCR_H_

#include <stdio.h>

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "libs_Mpeg&DVB/Mpeg_TSPacket/Include/Mpeg2_TS_packet.h"

#define PCR_FIFO_LENGTH						1024
#define MAX_SUPPORTED_PCR_PIDS				10

typedef struct
{
	S32	mean;
	S32	rms;			//������ֵ
	S32	min;
	S32	max;

} PCR_JITTER_ATTRIBUTE_t;

typedef struct
{
	S32	mean;
	S32	rms;			//������ֵ
	S32	min;
	S32	max;

} PCR_INTERVAL_ATTRIBUTE_t;

typedef struct
{
	U16			PCR_PID;

	//��¼PCR�ı���
	PCR_code_t	PCR_code;
	S32			pcr_code_positive_discontinuity_count;
	S32			pcr_code_negative_discontinuity_count;

	//��¼PCR�����г��ֵ�λ�û���ʱ��
	S64			PCR_pos;
	S64			TickCount;

	//��¼PCR��������ֵ
	S32			jitter_available;
	S32			jitter_cur_value;
	S32			jitter_mean_value;
	S32			jitter_rms_value;			//������ֵ
	S32			jitter_min_value;
	S32			jitter_max_value;

	S32			jitter_sample_index;
	S32			jitter_sample_count;
	S32			jitter_sample_array[PCR_FIFO_LENGTH];

	//��¼PCR���ͼ������ֵ
	S32			interval_available;
	S32			interval_cur_value;
	S32			interval_mean_value;
	S32			interval_rms_value;
	S32			interval_min_value;
	S32			interval_max_value;

	S32			interval_sample_index;
	S32			interval_sample_count;
	S32			interval_sample_array[PCR_FIFO_LENGTH];

	S32			pcr_ac_error_count;			//+/- 500ns
	S32			pcr_interval_error_count;		//>100ms

	//��¼����˵�27MHzʱ�ӡ����ʲ���ֵ��Ҳ�ǲ²��
	S32			encoder_bitrate_available;
	S32			encoder_bitrate_cur_value;
	S32			encoder_bitrate_mean_value;
	S32			encoder_bitrate_rms_value;
	S32			encoder_bitrate_sample_array[PCR_FIFO_LENGTH];
	S32			encoder_bitrate_sample_count;
	S32			encoder_bitrate_sample_index;

	//���ݿ�ָ�룬��ʱ����д�ļ��ķ�ʽ��������ֻҪ�һ����ţ����Ըĳ����ݿ�
	FILE*		fp_dbase;

} RECORD_PCR_t;

class _CDL_EXPORT CDB_Pcrs
{
public:
	CDB_Pcrs(void);
	~CDB_Pcrs(void);

protected:

	S32		m_bEnablePcrDebug;

	RECORD_PCR_t*	m_pPCR_info[MAX_SUPPORTED_PCR_PIDS];
	S32			m_pcr_pid_count;

	S32		m_jitter_min_value;
	S32		m_jitter_max_value;
	S32		m_jitter_mean_value;			//��ʱ��仯���������ȶ�
	S32		m_jitter_rms_value;				//��ʱ��仯���������ȶ�

	S32		m_interval_mean_value;
	S32		m_interval_min_value;
	S32		m_interval_max_value;
	S32		m_interval_rms_value;

protected:

	S32 IsValidBitrate(S32 newValue, S32 refValue, S32 var, S32 timeLine);
	S32 VlookupPCRRecord(U16 usPCRPID);

public:
	S32 GetTotalRecordCount(void);
	S32 GetRecordByPID(U16 usPCRPID, RECORD_PCR_t* pPcrInfo = NULL);
	S32 GetRecordByIndex(int index, RECORD_PCR_t* pPcrInfo = NULL);
	S32 AddPCRSample(U16 usPcrPID, S64 pos, PCR_code_t* pPCRCode, S32 ref_bitrate_mean, S32 ref_bitrate_var);

	S32 GetMeasuredJitterAttribute(PCR_JITTER_ATTRIBUTE_t* pattr);
	S32 GetMeasuredIntervalAttribute(PCR_INTERVAL_ATTRIBUTE_t* pattr);

	void  Reset(void);
private:
};

#endif	//_TSMAGIC_DBASE_PCR_H_
