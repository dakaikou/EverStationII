#ifndef __API_DIRECT_DRAW_WRAPPER_H__
#define __API_DIRECT_DRAW_WRAPPER_H__

#include <windows.h>
#include <mmsystem.h>

#include "../Compile.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

#include "thirdparty_libs/directx/include/ddraw.h"

class _CDL_EXPORT CHALForDirectDraw
{
public:
	CHALForDirectDraw(void);

protected:
	HWND		m_hVidWnd;
	float		m_fViewRatio;

public:
	virtual S32 OpenVideo(HWND hWnd, S32 luma_width, S32 luma_height, char* pszFourCC, float frame_rate=25);
	virtual S32 CloseVideo(void);
	virtual S32 RenderYUV(LPBYTE lpY, S32 y_size, LPBYTE lpU, S32 u_size, LPBYTE lpV, S32 v_size);
	virtual S32 RePaint(void);

private:		//direct audio output

	//direct draw variables
	LPDIRECTDRAW7           m_lpDD;    // DirectDraw 对象指针
	LPDIRECTDRAWSURFACE7    m_lpDDSPrimary;  // DirectDraw 主表面指针
	LPDIRECTDRAWSURFACE7    m_lpDDSOverlay;  // DirectDraw 离屏表面指针
	LPDIRECTDRAWCLIPPER		m_pcClipper;
	DDSURFACEDESC2		    m_ddsd;    // DirectDraw 表面描述

public:
	~CHALForDirectDraw();
};

#endif	//__API_DIRECT_DRAW_WRAPPER_H__

