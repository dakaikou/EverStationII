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
#include <mmsystem.h>

#include "translate_layer/Mpeg_PESPacket/Include/MPEG_PES_packet.h"
#include "translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

#include "./common/audio_common.h"
#include "./common/file_common.h"

#include "thirdparty_libs/directx/include/dsound.h"

#define DS_NOTIFY_COUNT			2

typedef enum
{
	ES_TRIGGER_NONE	= 0,
	ES_TRIGGER_ONESHOT,
	ES_TRIGGER_CONTINUE
} ES_TRIGGER_STYLE_e;

typedef struct
{
	uint32_t		syncword;
	uint32_t		start_code;

	uint8_t*		rbsp_byte;
	int		NumBytesInRBSP;

} AV_nal_unit_t, *pAV_nal_unit_t;

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

	uint16_t			m_usES_PID;
	uint16_t			m_usPCR_PID;

	uint8_t			m_nStreamType;

	int			(*av_es_parse)(PVOID* pvoid);

	//pull mode functions
	char			m_pszFileName[256];

	int			m_hFile;
	int			m_nFileTotalSize;
	int			m_nFileStartPos;

	//int			m_nWriteCount;

public:

	HWND		m_hMsgWnd;

	int			m_nTriggerType;
	int			m_bTriggering;

public:

	//FIFO_t		m_fifo;

public:

	virtual	int	 Open(HWND hMsgWnd, int nStreamType, char* pszFileName);
	virtual int  Close(void);
	
	int			WriteTSPacket(transport_packet_t* pts_packet, int64_t pos);
//	int			WriteTSPacket(U8* buf, int length, int64_t pos);
	int			WriteESData(uint8_t* buf, int length);
	int			FillData(void);
	int			IsOpened(void);
	int			SetParams(uint16_t ES_PID, uint16_t  PCR_PID);

	int	Preview_EOF(void);
	int	Preview_GetFilePos(void);

	int	Preview_SetFileRatio(int nPercent);
	int Preview_GetFileRatio(void);

public:
	void		Reset(void);
	int			SetTrigger(int nTriggerType);

public:
	~CESDecoder();
};

#endif

