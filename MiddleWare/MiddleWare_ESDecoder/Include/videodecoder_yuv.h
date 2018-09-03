#ifndef __API_YUV_VIDEO_DECODER_H__
#define __API_YUV_VIDEO_DECODER_H__

#include <stdio.h>
#include "HAL/HAL_Sys/Include/INTTYPES.H"

#include "./VESDecoder.h"
#include "./common/video_common.h"

typedef enum
{
	YUV_FILE_DY_DU_DV	= 0,
	YUV_FILE_DYUV,
	YUV_FILE_YUV
} YUV_FILE_TYPE_e;

class _CDL_EXPORT CYUV_VideoDecoder : public CVESDecoder
{
public:
	CYUV_VideoDecoder(void);
	~CYUV_VideoDecoder(void);

public:
/*------------------syntax part---------------------------*/
public:

	S32		Open(S32 nFileType, S8* pszFileName, Video_decode_info_t* pdecode_info);
	S32		Close(void);
	void	Reset(void);

	S32	OpenVideo(HWND hWnd, S8* pszFourCC, S32 strSize);
	S32	CloseVideo(void);

	void	SetGrid(void);

	S32		Preview_FirstPicture(void);
	S32		Preview_LastPicture(void);

	S32		Preview_NextPicture(void);
	S32		Preview_PrePicture(void);
	
	S32		Preview_ForwardPicture(void);
	S32		Preview_BackwardPicture(void);

	S32		Preview_AtPercent(int nPercent);
	S32		Preview_EOF(void);

	S32		Get_decode_info(Video_decode_info_t* pdecode_info);

protected:

//	Video_decode_info_t	m_DecodeInfo;

	S32		m_nFileType;
	U8*		m_pucFrameBuf[3];

private:
};

_CDL_EXPORT void DecodeFourCC2Text(S8* pszFourCC, S8* pszText, S32 strSize);


#endif

