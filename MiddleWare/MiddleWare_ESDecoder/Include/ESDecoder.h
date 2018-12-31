#ifndef __API_ES_DECODER_H__
#define __API_ES_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_ES_EXPORT
#       define MW_ES_LIB __declspec(dllexport)
#   elif defined(_MW_ES_IMPORT)
#       define MW_ES_LIB __declspec(dllimport)
#   else
#       define MW_ES_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_ES_LIB __attribute__((visibility("default")))
#else
#   define MW_ES_LIB
#endif

#include <stdint.h>

#include <windows.h>
//#include <mmsystem.h>

#include "translate_layer/Mpeg_PESPacket/Include/MPEG_PES_packet.h"
#include "translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

//#include "./common/audio_common.h"
//#include "./common/file_common.h"

//#include "thirdparty_libs/directx/include/dsound.h"

#define WM_REPORT_VIDEO_DECODE_FPS		WM_USER	+ 2500
#define DS_NOTIFY_COUNT					2

//typedef enum
//{
//	ES_TRIGGER_NONE	= 0,
//	ES_TRIGGER_ONESHOT,
//	ES_TRIGGER_CONTINUE
//} ES_TRIGGER_STYLE_e;

//typedef struct
//{
//	uint32_t		syncword;
//	uint32_t		start_code;
//
//	uint8_t*		rbsp_byte;
//	int				NumBytesInRBSP;
//
//} AV_nal_unit_t, *pAV_nal_unit_t;

class MW_ES_LIB CESDecoder
{
public:
	CESDecoder(void);

public:
	PES_packet_t	m_PES_packet;

protected:

	int64_t			m_nPrePcrPos;
	int64_t			m_nPrePtsPos;
	int64_t			m_nPreDtsPos;

	uint16_t		m_usES_PID;
	uint16_t		m_usPCR_PID;

	uint32_t		m_dwStreamType;

	int				(*m_callback_av_es_parse)(PVOID* pvoid);

	//pull mode functions
	char			m_pszFileName[256];

	int				m_hFile;
	int64_t			m_nFileTotalSize;
	int64_t			m_nFileStartPos;
	int64_t			m_nCurReadPos;

public:

	int			Open(uint32_t dwStreamType, char* pszFileName);
	int			Close(void);
	int			GetTitle(char* pszTitle, int strLen);
	
	int			WriteTSPacket(transport_packet_t* pts_packet, int64_t pos);
//	int			WriteTSPacket(U8* buf, int length, int64_t pos);
	int			WriteESData(uint8_t* buf, int length);
	int			FillData(void);
	int			SetParams(uint16_t ES_PID, uint16_t  PCR_PID);

public:
	virtual int	Preview_FirstPicture(void);
	virtual int	Preview_LastPicture(void);

	virtual int	Preview_Forward1Picture(void);
	virtual int	Preview_Backward1Picture(void);

	virtual int	Preview_ForwardNPicture(int n);
	virtual int	Preview_BackwardNPicture(int n);

	virtual int	Preview_SeekAtPercent(int nPercent);
	virtual int	Preview_beEOF(void);

public:
	~CESDecoder();
};

#endif

