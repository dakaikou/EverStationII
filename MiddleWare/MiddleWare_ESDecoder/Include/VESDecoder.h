#ifndef __API_VES_DECODER_H__
#define __API_VES_DECODER_H__

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

#include <windows.h>
#include <mmsystem.h>

#include "thirdparty_AL\TALForDirectX\Include\TALForDirectDraw.h"

#include "./common/video_common.h"

#include "ESDecoder.h"

class MW_ES_LIB CVESDecoder : public CESDecoder
{
public:
	CVESDecoder(void);

	int		OpenVideo(HWND hWnd);
	int		CloseVideo(void);
	void		Reset(void);

protected:

	HWND					m_hVidWnd;
	Video_decode_info_t		m_VidDecodeInfo;

private:		//direct audio output

	//direct draw variables
	CTALForDirectDraw*		m_pDirectDraw;

	int						m_nDebugFrameCount;
	uint32_t				m_dwTimeCount;


protected:
public:
	
	int DirectDraw_Render_yuv(void);
	int DirectDraw_Paint(void);

public:
	~CVESDecoder();
};

#endif

