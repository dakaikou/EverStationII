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

#include "thirdparty_abstract_layer\TALForDirectX\Include\TALForDirectDraw.h"

#include "./common/video_common.h"

#include "ESDecoder.h"

class MW_ES_LIB CVESDecoder : public CESDecoder
{
public:
	CVESDecoder(void);

	int		Open(uint32_t dwStreamType, char* pszFileName, Video_decode_info_t* pdecode_info);
	int		Close(void);

	int		OpenDirectxWnd(HWND hWnd);
	int		CloseDirectxWnd(void);

	//virtual int	Preview_FirstPicture(void);
	//virtual int	Preview_LastPicture(void);

	//virtual int	Preview_NextPicture(void);
	//virtual int	Preview_PrePicture(void);

	//virtual int	Preview_ForwardPicture(void);
	//virtual int	Preview_BackwardPicture(void);

	//virtual int	Preview_AtPercent(int nPercent);
	//virtual int	Preview_EOF(void);

protected:

	HWND					m_hVidWnd;
	Video_decode_info_t		m_VidDecodeInfo;

	//int						m_nFileType;
	uint8_t*				m_pucOutputFrameBuf[3];

private:		//direct audio output

	//direct draw variables
	CTALForDirectDraw*		m_pDirectDraw;

	int						m_nDebugFrameCount;
	uint32_t				m_dwDebugTimeTick;


protected:
public:
	
	int DirectDraw_Render_yuv(void);
	int DirectDraw_Paint(void);
	void GetDecodeInfo(Video_decode_info_t* pparam);
	HWND GetWndHandle(void);

public:
	~CVESDecoder();
};

#endif

