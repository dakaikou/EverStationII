#ifndef __MIDDLEWARE_TRANSPORT_STREAM_H__
#define __MIDDLEWARE_TRANSPORT_STREAM_H__
#include <Windows.h>

#include "../compile.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "HAL/HAL_Sys/Include/Template/FIFO.h"
#include "libs_Mpeg&DVB/Mpeg_TSPacket/Include/Mpeg2_TS_packet.h"

/*-------------------------------------------------------
	CTransportStream Class definition
	Author:			chendelin
	Date:			20020629
	Historty:		
-------------------------------------------------------*/

#define TSCHUNK_MAX_SIZE			38760 * 20				//(38352 + 408)*20
#define TSCHUNK_RESERVED_SIZE		408						//3 * 204

#define PACKET_BUFFER_SIZE			38352					//188 * 204

#define TS_BITRATE_FIFO_LENGTH		128

typedef struct
{
	S32	mean;
	S32	rms;			//均方根值
	S32	min;
	S32	max;

} BITRATE_ATTRIBUTE_t;

class _CDL_EXPORT CTransportStream
{
public:
	CTransportStream(void);

public:
	S32			m_bEnableTSRateDebug;

	S32			m_nPacketLength;
	S32			m_bSynced;
	S32			m_TsError;

	S32			m_hFile;

	S32			m_nRunning;				//主动设置的状态
	S32			m_nStopRunning;			//线程反馈回来的状态

	S32			m_nReceiving;			//主动设置的状态
	S32			m_nStopReceiving;		//线程反馈的状态

	S32			m_nMode;				//用以识别TS流的打开方式，0 - 表示离线分析， 1 - 表示实时分析

	S32			m_nEOF;

	S8			m_pszProtocolHead[16];
	S8			m_pszProtocolExt[MAX_PATH];

	double		mf_actual_1spulse_frequency;
	double		mf_actual_scan_frequency;
	S32			mn_actual_timeslice;

//	S64			m_llCurFilePos;						//support file size: >= 4G
	S64			m_llCurReadPos;
	S64			m_llTotalFileLength;


	CFIFO<U8, TSCHUNK_MAX_SIZE>   m_ts_fifo;

protected:
	S32				m_bitrate_available;
	S32				m_bitrate_sample_index;
	S32				m_bitrate_sample_count;
	S32				m_bitrate_sample_array[TS_BITRATE_FIFO_LENGTH];

	S32				m_bitrate_cur_value;
	S32				m_bitrate_min_value;
	S32				m_bitrate_max_value;
	S32				m_bitrate_mean_value;
	S32				m_bitrate_rms_value;

	//数据库指针，暂时采用写文件的方式，将来（只要我还活着）可以改成数据库
	FILE*		fp_dbase;

public:
	~CTransportStream();

	S32 GetMeasuredBitrateAttribute(BITRATE_ATTRIBUTE_t* pattr);
	S32 AddBitrateSample(int bitrate);

	S32 Open(char* tsin_option, char* tsin_description, int mode);
	S32 Close(void);
	S32 Reset(void);

	S32 StartGetData(void);
	S32 StopGetData(void);

	S32	StartGetBitrate(void);
	S32	GetBitrateMap(ULONG pbitrate_map[], S32 count);
	S32 GetBitrate(void);
	S32	StopGetBitrate(void);

	void SeekToBegin(void);

	S32  PrefetchOnePacket(U8* buf, S32* plength);
	S32  SkipOnePacket(void);

//	S32  GetOnePacket(U8* buf, S32* plength);
	S32	 GetLastError(void);
	S64 Tell(void);

protected:
};

#endif