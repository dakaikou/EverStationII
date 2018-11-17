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

class TAL_DIRECTX_LIB CTALForDirectDraw
{
public:
	CTALForDirectDraw(void);

protected:
	HWND		m_hVidWnd;
	float		m_fViewRatio;

public:
	virtual int OpenVideo(HWND hWnd, int luma_width, int luma_height, char* pszFourCC, float frame_rate=25);
	virtual int CloseVideo(void);
	virtual int RenderYUV(LPBYTE lpY, int y_size, LPBYTE lpU, int u_size, LPBYTE lpV, int v_size);
	virtual int RePaint(void);

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

