#ifndef __MIDDLEWARE_TRANSPORT_STREAM_H__
#define __MIDDLEWARE_TRANSPORT_STREAM_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_TS_EXPORT
#       define MW_TS_LIB __declspec(dllexport)
#   elif defined(_MW_TS_IMPORT)
#       define MW_TS_LIB __declspec(dllimport)
#   else
#       define MW_TS_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_TS_LIB __attribute__((visibility("default")))
#else
#   define MW_TS_LIB
#endif

#include <stdint.h>

#include "../Template/FIFO.h"
#include "translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

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

#define USE_FIFO_ACCESS_MUTEX		1	

typedef struct
{
	int	mean;
	int	rms;			//均方根值
	int	min;
	int	max;

} BITRATE_ATTRIBUTE_t;

class MW_TS_LIB CTransportStream
{
public:
	CTransportStream(void);

public:
	int			m_bEnableTSRateDebug;

	int			m_nPacketLength;
	int			m_bSynced;
	int			m_TsError;

	int			m_nRunning;				//主动设置的状态
	int			m_nStopRunning;			//线程反馈回来的状态

	int			m_nReceiving;			//主动设置的状态
	int			m_nStopReceiving;		//线程反馈的状态

	int			m_nMode;				//用以识别TS流的打开方式，0 - 表示离线分析， 1 - 表示实时分析

	int			m_nEOF;

	char			m_pszProtocolHead[16];
	char			m_pszProtocolExt[256];

	double		mf_actual_1spulse_frequency;
	double		mf_actual_scan_frequency;
	int			mn_actual_timeslice;

	int64_t			m_llCurReadPos;
	int64_t			m_llTotalFileLength;


	CFIFO<uint8_t, TSCHUNK_MAX_SIZE>   m_ts_fifo;

#if USE_FIFO_ACCESS_MUTEX
	HANDLE			m_hFifoAccess;
#endif

protected:
	int				m_bitrate_available;
	int				m_bitrate_sample_index;
	int				m_bitrate_sample_count;
	int				m_bitrate_sample_array[TS_BITRATE_FIFO_LENGTH];

	int				m_bitrate_cur_value;
	int				m_bitrate_min_value;
	int				m_bitrate_max_value;
	int				m_bitrate_mean_value;
	int				m_bitrate_rms_value;

	//数据库指针，暂时采用写文件的方式，将来（只要我还活着）可以改成数据库	chendelin  2018.11.19
	FILE*		fp_tsrate_dbase;

public:
	~CTransportStream();

	int GetMeasuredBitrateAttribute(BITRATE_ATTRIBUTE_t* pattr);
	int AddBitrateSample(int bitrate);

	int Open(char* tsin_option, char* tsin_description, int mode);
	int Close(void);
	int Reset(void);

	int StartGetData(int64_t offset = -1);
	int StopGetData(void);

	//int	StartGetBitrate(void);
	//int	GetBitrateMap(int pbitrate_map[], int count);
	int GetBitrate(void);
	//int	StopGetBitrate(void);

	void SeekToBegin(void);

	int  Synchronize(int* plength);

	int  SyncReadOnePacket(uint8_t* buf, int* plength);

	//int  PrefetchOnePacket(uint8_t* buf, int* plength);
	//int  SkipOnePacket(void);

//	int  GetOnePacket(uint8_t* buf, int* plength);
	int	 GetLastError(void);
	int64_t Tell(void);

protected:
};


#define UNCREDITABLE_MAX_VALUE				-123456789
#define UNCREDITABLE_MIN_VALUE				123456789

#endif