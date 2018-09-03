#ifndef __API_ES_DECODER_H__
#define __API_ES_DECODER_H__

#include <windows.h>
#include <mmsystem.h>

#include "../Compile.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

#include "libs_Mpeg&DVB/Mpeg_PESPacket/Include/MPEG_PES_packet.h"
#include "libs_Mpeg&DVB/Mpeg_TSPacket/Include/Mpeg2_TS_packet.h"

#include "./common/audio_common.h"
#include "./common/file_common.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
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
	U32		syncword;
	S32		start_code;

	U8*		rbsp_byte;
	S32		NumBytesInRBSP;

} AV_nal_unit_t, *pAV_nal_unit_t;

class _CDL_EXPORT CESDecoder
{
public:
	CESDecoder(void);

public:
	PES_packet_t	m_PES_packet;

protected:

	S64			m_nPrePcrPos;
	S64			m_nPrePtsPos;
	S64			m_nPreDtsPos;

	U16			m_usES_PID;
	U16			m_usPCR_PID;

	U8			m_nStreamType;

	S32			(*av_es_parse)(PVOID* pvoid);

	//pull mode functions
	S8			m_pszFileName[256];

	S32			m_hFile;
	S32			m_nFileTotalSize;
	S32			m_nFileStartPos;

	//S32			m_nWriteCount;

public:

	HWND		m_hMsgWnd;

	S32			m_nTriggerType;
	S32			m_bTriggering;

public:

	//FIFO_t		m_fifo;

public:

	virtual	S32	 Open(HWND hMsgWnd, S32 nStreamType, S8* pszFileName);
	virtual S32  Close(void);
	
	S32			WriteTSPacket(transport_packet_t* pts_packet, S64 pos);
//	S32			WriteTSPacket(U8* buf, S32 length, S64 pos);
	S32			WriteESData(U8* buf, S32 length);
	S32			FillData(void);
	S32			IsOpened(void);
	S32			SetParams(U16 ES_PID, U16  PCR_PID); 

	S32	Preview_EOF(void);
	S32	Preview_GetFilePos(void);

	S32	Preview_SetFileRatio(int nPercent);
	S32 Preview_GetFileRatio(void);

public:
	void		Reset(void);
	S32			SetTrigger(S32 nTriggerType);

public:
	~CESDecoder();
};

#endif

