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
#include "syntax_translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

/*-------------------------------------------------------
	CTransportStream Class definition
	Author:			chendelin
	Date:			20020629
	Historty:		
-------------------------------------------------------*/

#define TSCHUNK_MAX_SIZE			38760 * 20				//(38352 + 408)*20
#define TSCHUNK_RESERVED_SIZE		408						//3 * 204

#define PACKET_BUFFER_SIZE			38352					//188 * 204

#define TS_BITRATE_FIFO_LENGTH		5						//40ms x 25 = 1s

#define USE_FIFO_ACCESS_MUTEX		1	

//typedef struct
//{
//	int	mean;
//	int	rms;
//	int	min;
//	int	max;
//
//} BITRATE_ATTRIBUTE_t;

class MW_TS_LIB CTransportStream
{
public:
	CTransportStream(void);

public:
	int			m_bEnableTSRateDebug;

	int			m_nPacketLength;
	int			m_bSynced;
	int			m_TsError;

	int			m_nRunning;
	int			m_nStopRunning;

	int			m_nReceiving;
	int			m_nStopReceiving;

	int			m_nMode;				//0-offline, 1-realtime
	int			m_nEOF;

	char		m_pszProtocolHead[16];
	char		m_pszProtocolExt[256];

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
	//int				m_bitrate_available;

	int				m_bitrate_original_sample_index;
	int				m_bitrate_original_sample_count;
	int				m_bitrate_original_sample_array[TS_BITRATE_FIFO_LENGTH];

	//int				m_bitrate_original_mean_value;
	//int				m_bitrate_dixon_mean_value;
	//int				m_bitrate_deletion_mean_value;

	int				m_bitrate_estimate_cur_value;
	int				m_bitrate_estimate_min_value;
	int				m_bitrate_estimate_max_value;

	FILE*			fp_tsrate_dbase;

	//int(*callback_report_bitrate)(int bitrate);

public:
	~CTransportStream();

	//int GetMeasuredBitrateAttribute(BITRATE_ATTRIBUTE_t* pattr);
	int AddBitrateSample(int bitrate, int (*callback)(int, int));

	int Open(char* tsin_option, char* tsin_description, int mode);
	int Close(void);
	int Reset(void);

	int StartGetData(int64_t offset = -1);
	int StopGetData(void);

	int	StartGetBitrate(void);
	int GetBitrate(void);
	int	StopGetBitrate(void);

	void SeekToBegin(void);
	void Seek(int64_t offset);
	int64_t Tell(void);

	int  Synchronize(int* plength);

	int  SyncReadOnePacket(uint8_t* buf, int* plength);

	//int  PrefetchOnePacket(uint8_t* buf, int* plength);
	//int  SkipOnePacket(void);

//	int  GetOnePacket(uint8_t* buf, int* plength);
	int	 GetLastError(void);

protected:

};


#define UNCREDITABLE_MAX_VALUE				-123456789
#define UNCREDITABLE_MIN_VALUE				123456789

#endif