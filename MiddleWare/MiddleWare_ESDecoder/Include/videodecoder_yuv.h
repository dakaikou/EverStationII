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

class MW_ES_LIB CYUV_VideoDecoder : public CVESDecoder
{
public:
	CYUV_VideoDecoder(void);
	~CYUV_VideoDecoder(void);

public:
/*------------------syntax part---------------------------*/
public:
	int		Open(uint32_t dwStreamType, const char* pszFileName, const YUV_SERIAL_PARAM_t* psource_info = NULL);
	int		Preview_FirstPicture(void);
	int		Preview_LastPicture(void);

	int		Preview_Forward1Picture(void);
	int		Preview_Backward1Picture(void);
	
	int		Preview_ForwardNPicture(int n);
	int		Preview_BackwardNPicture(int n);

	int		Preview_CurPicture(void);
	int		Preview_SeekAtPercent(int nPercent);
	int		Preview_beEOF(void);

	int		Preview_Picture(int nFrameNum);
	int		GetFrameCount(void);

protected:

	int			m_nTotalFrameCount;
	int64_t		m_nFrameEndPos;

private:
};

//MW_ES_LIB void DecodeFourCC2Text(char* pszFourCC, char* pszText, int strSize);


#endif

