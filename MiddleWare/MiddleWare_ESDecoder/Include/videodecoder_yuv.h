#ifndef __API_YUV_VIDEO_DECODER_H__
#define __API_YUV_VIDEO_DECODER_H__

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

#include <stdio.h>

#include "./VESDecoder.h"
#include "./common/video_common.h"

typedef enum
{
	YUV_FILE_DY_DU_DV	= 0,
	YUV_FILE_DYUV,
	YUV_FILE_YUV
} YUV_FILE_TYPE_e;

class MW_ES_LIB CYUV_VideoDecoder : public CVESDecoder
{
public:
	CYUV_VideoDecoder(void);
	~CYUV_VideoDecoder(void);

public:
/*------------------syntax part---------------------------*/
public:

	int		Open(int nFileType, char* pszFileName, Video_decode_info_t* pdecode_info);
	int		Close(void);
	void	Reset(void);

	int	OpenVideo(HWND hWnd, char* pszFourCC, int strSize);
	int	CloseVideo(void);

	void	SetGrid(void);

	int		Preview_FirstPicture(void);
	int		Preview_LastPicture(void);

	int		Preview_NextPicture(void);
	int		Preview_PrePicture(void);
	
	int		Preview_ForwardPicture(void);
	int		Preview_BackwardPicture(void);

	int		Preview_AtPercent(int nPercent);
	int		Preview_EOF(void);

	int		Get_decode_info(Video_decode_info_t* pdecode_info);

protected:

//	Video_decode_info_t	m_DecodeInfo;

	int		m_nFileType;
	uint8_t*		m_pucFrameBuf[3];

private:
};

MW_ES_LIB void DecodeFourCC2Text(char* pszFourCC, char* pszText, int strSize);


#endif

