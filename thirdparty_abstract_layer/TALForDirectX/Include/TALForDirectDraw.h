#ifndef __API_DIRECT_DRAW_WRAPPER_H__
#define __API_DIRECT_DRAW_WRAPPER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _TAL_DIRECTX_EXPORT
#       define TAL_DIRECTX_LIB __declspec(dllexport)
#   elif defined(_TAL_DIRECTX_IMPORT)
#       define TAL_DIRECTX_LIB __declspec(dllimport)
#   else
#       define TAL_DIRECTX_LIB
#   endif
#elif __GNUC__ >= 4
#   define TAL_DIRECTX_LIB __attribute__((visibility("default")))
#else
#   define TAL_DIRECTX_LIB
#endif

#include <stdint.h>

#include <windows.h>
#include <mmsystem.h>

#include "thirdparty_libs/directx/include/ddraw.h"

#define WM_REPORT_VIDEO_DECODE_FPS		WM_USER	+ 0x46ED

typedef struct
{
	int luma_width;
	int luma_height;
	int chroma_width;
	int chroma_height;

} FRAME_PARAMS_t;

class TAL_DIRECTX_LIB CTALForDirectDraw
{
public:
	CTALForDirectDraw(void);

protected:
	HWND		m_hVidWnd;
	int			m_nGrid;
	double		m_dEnlargeCoeff;
	//double		m_dMaxEnlargeCoeff;
	int			m_nDebugFrameCount;
	uint32_t	m_dwDebugTimeTick;

	//POINT		m_ptOrigin;

public:
	virtual int OpenVideo(HWND hWnd, int canvas_width, int canvas_height, unsigned int dwFourCC);
	virtual int CloseVideo(void);
	virtual int RenderYUV(const LPBYTE lpFrameBuf, int frameSize, const FRAME_PARAMS_t* pstFrameParams);
	virtual int RePaint(void);
	virtual int ToggleGrid(void);
	virtual int ToggleScreen(void);

private:		//direct audio output

	//direct draw variables
	LPDIRECTDRAW7           m_lpDD;    // DirectDraw 对象指针
	LPDIRECTDRAWSURFACE7    m_lpDDSPrimary;  // DirectDraw 主表面指针
	LPDIRECTDRAWSURFACE7    m_lpDDSOverlay;  // DirectDraw 离屏表面指针
	LPDIRECTDRAWCLIPPER		m_pcClipper;
	DDSURFACEDESC2		    m_ddsd;    // DirectDraw 表面描述

public:
	~CTALForDirectDraw();
};

#endif	//__API_DIRECT_DRAW_WRAPPER_H__

