#ifndef __API_MPEG_PS_VIDEO_DECODER_H__
#define __API_MPEG_PS_VIDEO_DECODER_H__

#include <stdio.h>
#include "XStream_Include/inttypes.h"
#include "./ESDecoder.h"
#include "./VideoDecoder_Mpeg2.h"
#include "./AudioDecoder_Mpeg2.h"
#include "./AudioDecoder_AC3.h"
#include "./kernel/kernel_mpeg_ps_decoder.h"

#define MAX_VOB				16

#define		PS_OPEN_VIDEO_DECODER		1
#define		PS_OPEN_AUDIO_DECODER		1

#define		PS_DEBUG_PACK_UNIT			0
#define		PS_DEBUG_VIDEO_UNIT			0
#define		PS_DEBUG_AUDIO_UNIT			0

class _CDL_EXPORT CMPEG_PS_Decoder
{
public:
	CMPEG_PS_Decoder();
	~CMPEG_PS_Decoder();

public:

	//pull mode functions
	S8			m_pszFileName[256];
	S32			m_hFile;
	S32			m_nFileSize;

	//FIFO_t		m_fifo;

	S32			m_nPackLength;

//	U8*			m_pucPackBuf;
	S32			m_nStartPos;

	HWND		m_hMsgWnd;

	//CMPEG_VideoDecoder		M2VDecoder;
	//CMPEG_AudioDecoder		M2ADecoder;

	//CAC3_AudioDecoder		AC3Decoder;

	FILE*					m_hDebugFile;
	//call back functions

/*------------------syntax part---------------------------*/
	S32			m_nTriggerType;
	S32			m_bTriggering;

	S32			m_ps_stream_type;
	S32			m_nStreamType;
	S32			m_bSynced;

	U8			m_ucVidID;
	U8			m_ucAudID;
	char		m_pszVFile[MAX_PATH];
	char		m_pszAFile[MAX_PATH];
	HWND		m_hDispatchMsgWnd;

	U8			m_ucVStreamID[MAX_VOB];
	S32			m_nVidMuxRate[MAX_VOB];
	S32			m_nVStreamCount;

	U8			m_ucAStreamID[MAX_VOB];
	S32			m_nAudMuxRate[MAX_VOB];
	S32			m_nAStreamCount;

	MPEG_pack_header_t		m_pack_header;
	MPEG_system_header_t	m_system_header;

public:

	S32		Open(HWND hMsgWnd, S32 nStreamType, S8* pszFileName);
	S32		Close(void);
	void	Reset(void);

	void	OpenVideo(HWND hWnd, S8* pszFourCC, S32 strSize);
	void	CloseVideo(void);
	void	OpenAudio(HWND hWnd);
	void	CloseAudio(void);

	S32		SetTrigger(S32 nTriggerType);
	void	SetGrid(void);

	S32		ExtDebug(void);

	S32		Preview_Init(void);
	S32		Preview_NextPack(void);
	S32		Preview_AtPercent(S32 nPercent);
	S32		Preview_EOF(void);

	S32		PackDecimate(HWND hMsgWnd, char* pszVFile, char* pszAFile);
	//S32		get_pack_unit(FIFO_t* pfifo, AV_nal_unit_t* pnal_unit);
	S32		get_next_start_code(AV_nal_unit_t* pnal_unit);

	S32		FillData(void);
	S32		GetSize(void);

private:

	S32		Seek(S32 filepos);
};

U32 MPEG_PS_dispatch(PVOID* pvoid);

#endif

