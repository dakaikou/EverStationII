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
#define USE_SURFACE_ACCESS_MUTEX		1	

#define RENDER_IN_THREAD				0	

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

#if USE_SURFACE_ACCESS_MUTEX
	HANDLE		m_hSurfaceAccess;
#endif
	int			m_bSurfaceDataAvailable;
	int			m_bRenderControllStatus;
	int			m_bRenderResponseStatus;

	double		m_dFrameRate;

protected:
	int			m_nGrid;
	double		m_dViewEnlargeCoeff;
	int			m_nDebugFrameCount;
	uint32_t	m_dwDebugTimeTick;

public:
	virtual int OpenVideo(HWND hWnd, int source_width, int source_height, unsigned int dwFourCC, double dFrameRate);
	virtual int CloseVideo(void);
	virtual int FeedToOffScreenSurface(const LPBYTE lpFrameBuf, int frameSize, const FRAME_PARAMS_t* pstFrameParams);
	virtual int RenderOnPrimarySurface(void);
	virtual int ToggleGrid(void);
	virtual int ToggleView(void);

private:		//direct audio output

	//direct draw variables
	LPDIRECTDRAW7           m_lpDD;    // DirectDraw 对象指针
	LPDIRECTDRAWSURFACE7    m_lpDDSPrimary;  // DirectDraw 主表面指针
	LPDIRECTDRAWSURFACE7    m_lpDDSOffscreen;  // DirectDraw 离屏表面指针
	LPDIRECTDRAWCLIPPER		m_pcClipper;

	int						m_nSourceWidth;
	int						m_nSourceHeight;
	//int						m_nCanvasWidth;
	//int						m_nCanvasHeight;
	//int						m_nViewWidth;
	//int						m_nViewHeight;
	HWND					m_hVidWnd;
	DWORD					m_dwFourCC;

	//FRAME_PARAMS_t			m_stTemporalFrameParams;
	//LPBYTE					m_pTemporalFrameBuf;
	//int						m_pTemporalFrameSize;

	int AllocateDirectDrawResource(HWND hWnd, int canvas_width, int canvas_height, unsigned int dwFourCC);
	int ReleaseDirectDrawResource(void);

#if RENDER_IN_THREAD
	void StartRenderThread(void);
	void StopRenderThread(void);
#endif

public:
	~CTALForDirectDraw();
};

uint32_t thread_surface_render_process(LPVOID lpParam);


#endif	//__API_DIRECT_DRAW_WRAPPER_H__

