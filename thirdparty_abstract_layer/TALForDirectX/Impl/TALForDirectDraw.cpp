#include <assert.h>
#include <math.h>

#include "../Include/TALForDirectDraw.h"

CTALForDirectDraw::CTALForDirectDraw(void)
{
	m_hVidWnd = NULL;

	//direct draw settings
	m_lpDD = NULL;
	m_lpDDSOffscreen = NULL;
	m_lpDDSPrimary = NULL;
	m_pcClipper = NULL;

	m_nGrid = 0;
	//m_dCanvasEnlargeCoeff = 1.0;
	m_dViewEnlargeCoeff = 1.0;

	m_dFrameRate = 30.0;				//default value 30/s
	m_nSourceWidth = -1;
	m_nSourceHeight = -1;
	//m_nCanvasWidth = -1;
	//m_nCanvasHeight = -1;
	//m_nViewWidth = -1;
	//m_nViewHeight = -1;

#if USE_SURFACE_ACCESS_MUTEX
	m_hSurfaceAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif
	m_bRenderControllStatus = false;
	m_bRenderResponseStatus = false;

	m_bSurfaceDataAvailable = 0;
}

CTALForDirectDraw::~CTALForDirectDraw()
{
#if USE_SURFACE_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hSurfaceAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		m_bSurfaceDataAvailable = 0;
#if USE_SURFACE_ACCESS_MUTEX
		::ReleaseMutex(m_hSurfaceAccess);
		::CloseHandle(m_hSurfaceAccess);
		m_hSurfaceAccess = NULL;
	}
#endif

	assert(m_lpDD == NULL);
	assert(m_pcClipper == NULL);
	assert(m_lpDDSOffscreen == NULL);
	assert(m_lpDDSPrimary == NULL);
}

#if RENDER_IN_THREAD
void CTALForDirectDraw::StartRenderThread(void)
{
	m_bRenderControllStatus = true;
	m_bRenderResponseStatus = false;
	m_bSurfaceDataAvailable = false;
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_surface_render_process, (LPVOID)this, 0, 0);

	while (m_bRenderResponseStatus == 0)
	{
		Sleep(10);
	}
}

void CTALForDirectDraw::StopRenderThread(void)
{
	//notify the render process to exit
	m_bRenderControllStatus = false;

	while (m_bRenderResponseStatus == 1)
	{
		Sleep(10);
	}
}
#endif

int CTALForDirectDraw::OpenVideo(HWND hWnd, int source_width, int source_height, unsigned int dwFourCC, double framerate)
{
	int					rtcode = -1;

	m_hVidWnd = hWnd;

	m_nSourceWidth = source_width;
	m_nSourceHeight = source_height;

	//initial state: canvas == source
	//m_nCanvasWidth = m_nSourceWidth;
	//m_nCanvasHeight = m_nSourceHeight;

	//m_dCanvasEnlargeCoeff = 1.0;
	m_dViewEnlargeCoeff = 1.0;

	//initial state: view == canvas
	//m_nViewWidth = m_nCanvasWidth;
	//m_nViewHeight = m_nCanvasHeight;

	//int nCanvasWidth = (int)round(m_nSourceWidth * m_dCanvasEnlargeCoeff);
	//int nCanvasHeight = (int)round(m_nSourceHeight * m_dCanvasEnlargeCoeff);

	m_dwFourCC = dwFourCC;

	m_dFrameRate = framerate;

	rtcode = AllocateDirectDrawResource(m_hVidWnd, m_nSourceWidth, m_nSourceHeight, m_dwFourCC);

#if RENDER_IN_THREAD
	StartRenderThread();
#endif

	return rtcode;
}

int CTALForDirectDraw::CloseVideo(void)
{
	int					rtcode = -1;

#if RENDER_IN_THREAD
	StopRenderThread();
#endif

	rtcode = ReleaseDirectDrawResource();

	m_hVidWnd = NULL;
	m_nSourceWidth = -1;
	m_nSourceHeight = -1;
	//m_nCanvasWidth = -1;
	//m_nCanvasHeight = -1;
	//m_nViewWidth = -1;
	//m_nViewHeight = -1;
	//m_dCanvasEnlargeCoeff = 1.0;
	m_dViewEnlargeCoeff = 1.0;
	m_dwFourCC = 0x00000000;

	return rtcode;
}

int CTALForDirectDraw::AllocateDirectDrawResource(HWND hWnd, int canvas_width, int canvas_height, unsigned int dwFourCC)
{
	int					rtcode = -1;
	DDSURFACEDESC2		ddsd;    // DirectDraw 表面描述

	// 创建DirectCraw对象
	//use the system default display hardware
	rtcode = DirectDrawCreateEx(NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL);
	if (rtcode == DD_OK)
	{
		// 设置协作层，即设置与当前Windows窗口之间的协作关系
		//rtcode = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES | DDSCL_FULLSCREEN);
		rtcode = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES);
		if (rtcode == DD_OK)
		{
			// 创建主表面
			ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
			ddsd.dwSize = sizeof(DDSURFACEDESC2);
			ddsd.dwFlags = DDSD_CAPS;
			//ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			//ddsd.dwBackBufferCount = 0;

			rtcode = m_lpDD->CreateSurface(&ddsd, &m_lpDDSPrimary, NULL);
			if (rtcode == DD_OK)
			{
				//HDC hdc;
				//if (m_lpDDSPrimary->GetDC(&hdc) == DD_OK)
				//{
				//	SetBkColor(hdc, RGB(0, 0, 255)); //设置背景颜色
				//	SetTextColor(hdc, RGB(255, 255, 0)); //设置文字颜色
				//	TextOutA(hdc, 220, 200, "caonima", 7); //输出文字
				//	TextOutA(hdc, 280, 240, "fuck ni ma", 10);

				//	m_lpDDSPrimary->ReleaseDC(hdc); //释放资源
				//}

				// 创建YUV表面 
				ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

				ddsd.dwSize = sizeof(DDSURFACEDESC2);
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
				//	ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY;
				ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
				ddsd.dwWidth = canvas_width;
				ddsd.dwHeight = canvas_height;
				ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
				ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;

				//DWORD dwFourCC = MAKEFOURCC(pszFourCC[0], pszFourCC[1], pszFourCC[2], pszFourCC[3]);
				ddsd.ddpfPixelFormat.dwFourCC = dwFourCC;
				ddsd.ddpfPixelFormat.dwYUVBitCount = 8;			//default value

			//	if (dwFourCC == 0x56555949)			//IYUV: Planar YUV, YUV420 Y-U-V, the same as I420
			//	{
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
			//	}
			//	else if (dwFourCC == 0x30323449)			//I420: Planar YUV, YUV420 Y-U-V, the same as IYUV
			//	{
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
			//	}
			//	else if (dwFourCC == 0x32315659)			//YV12: Planar YUV, YUV420 Y-V-U
			//	{
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
			//	}
		////	else if (dwFourCC == 0x36315659)			//YV16: Planar YUV422 Y-U-V
		////	{
		////		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
		////	}
			//	else										//YUV420 Y-V-U
			//	{
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
			//	}

				rtcode = m_lpDD->CreateSurface(&ddsd, &m_lpDDSOffscreen, NULL);
				if (rtcode == DD_OK)
				{
					rtcode = m_lpDD->CreateClipper(0, &m_pcClipper, NULL);
					if (rtcode == DD_OK)
					{
						rtcode = m_pcClipper->SetHWnd(0, hWnd);
						if (rtcode == DD_OK)
						{
							m_nDebugFrameCount = 0;
							m_dwDebugTimeTick = 0;

							//RECT  rcClient;
							//::GetClientRect(m_hVidWnd, &rcClient);

							//int nClientWidth = rcClient.right - rcClient.left;
							//int nClientHeight = rcClient.bottom - rcClient.top;

							//double dWidthEnlargeCoeff = (double)nClientWidth / m_ddsd.dwWidth;
							//double dHeightEnlargeCoeff = (double)nClientHeight / m_ddsd.dwWidth;
							//m_dMaxEnlargeCoeff = min(dWidthEnlargeCoeff, dHeightEnlargeCoeff);

							//int nStep = (int)(m_dMaxEnlargeCoeff / 0.25);
							//m_dEnlargeCoeff = nStep * 0.25;
						}
						else
						{
							m_pcClipper->Release();
							m_pcClipper = NULL;

							m_lpDDSPrimary->Release();
							m_lpDDSPrimary = NULL;

							m_lpDDSOffscreen->Release();
							m_lpDDSOffscreen = NULL;

							m_lpDD->Release();
							m_lpDD = NULL;
						}
					}
					else
					{
						m_lpDDSPrimary->Release();
						m_lpDDSPrimary = NULL;

						m_lpDDSOffscreen->Release();
						m_lpDDSOffscreen = NULL;

						m_lpDD->Release();
						m_lpDD = NULL;
					}
				}
				else
				{
					m_lpDDSPrimary->Release();
					m_lpDDSPrimary = NULL;

					m_lpDD->Release();
					m_lpDD = NULL;
				}
			}
			else
			{
				//MessageBox("Error Create Primary Surface.", s);
				m_lpDD->Release();
				m_lpDD = NULL;
			}
		}
		else
		{
			m_lpDD->Release();
			m_lpDD = NULL;
		}
	}

	return rtcode;
}

int CTALForDirectDraw::ReleaseDirectDrawResource(void)
{
	if (m_lpDDSOffscreen)
	{
		m_lpDDSOffscreen->Release();
		m_lpDDSOffscreen = NULL;
	}

	if (m_lpDDSPrimary)
	{
		m_lpDDSPrimary->Release();
		m_lpDDSPrimary = NULL;
	}

	if (m_lpDD != NULL)
	{
		m_lpDD->Release();
		m_lpDD = NULL;
	}

	if (m_pcClipper != NULL)
	{
		m_pcClipper->Release();
		m_pcClipper = NULL;
	}

	return NO_ERROR;
}

int CTALForDirectDraw::FeedToOffScreenSurface(const LPBYTE lpFrameBuf, int frameSize, const FRAME_PARAMS_t* pstFrameParams)
{
	LPBYTE  lpFrame = NULL;
	LPBYTE  lpSurf = NULL;
	int		i;
	HRESULT	ddRval = -1;

	DDSURFACEDESC2		ddsd;    // DirectDraw 表面描述
	int					pitchLuma, pitchChroma;

	if ((m_lpDDSOffscreen != NULL) &&
		(lpFrameBuf != NULL) && (pstFrameParams != NULL))
	{
		//临时寄存当前帧指针，仅在下一帧图像到来前有效
		//m_pTemporalFrameBuf = lpFrameBuf;
		//m_pTemporalFrameSize = frameSize;
		//memcpy(&m_stTemporalFrameParams, pstFrameParams, sizeof(FRAME_PARAMS_t));

		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		do
		{
			ddRval = m_lpDDSOffscreen->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
			if (ddRval == DD_OK)
			{
				lpFrame = lpFrameBuf;

				DWORD dwFourCC = ddsd.ddpfPixelFormat.dwFourCC;
				switch (dwFourCC)
				{
				case 0x32315659: //MAKEFOURCC('Y', 'V', '1', '2'):
				case 0x30323449: //MAKEFOURCC('I', '4', '2', '0'):
				case 0x56555949: //MAKEFOURCC('I', 'Y', 'U', 'V'):
					//widthY = m_ddsd.dwWidth;
					//heightY = m_ddsd.dwHeight;
					pitchLuma = ddsd.lPitch;

					//widthV = (m_ddsd.dwWidth >> 1);
					//heightV = (m_ddsd.dwHeight >> 1);
					pitchChroma = (ddsd.lPitch >> 1);

					//widthU = (m_ddsd.dwWidth >> 1);
					//heightU = (m_ddsd.dwHeight >> 1);
					//pitchU = (m_ddsd.lPitch >> 1);

					//assert(y_size == (widthY * heightY));
					//assert(u_size == (widthU * heightU));
					//assert(v_size == (widthV * heightV));

					// 填充离屏表面
					lpSurf = (LPBYTE)ddsd.lpSurface;
					if ((lpSurf != NULL) && (lpFrame != NULL))
					{
						for (i = 0; i < pstFrameParams->Y_height; i++)
						{
							memcpy(lpSurf, lpFrame, pstFrameParams->Y_width);
							lpFrame += pstFrameParams->Y_width;
							lpSurf += pitchLuma;
						}

						for (i = 0; i < pstFrameParams->U_height; i++)
						{
							memcpy(lpSurf, lpFrame, pstFrameParams->U_width);
							lpFrame += pstFrameParams->U_width;
							lpSurf += pitchChroma;
						}

						for (i = 0; i < pstFrameParams->V_height; i++)
						{
							memcpy(lpSurf, lpFrame, pstFrameParams->V_width);
							lpFrame += pstFrameParams->V_width;
							lpSurf += pitchChroma;
						}
					}

					break;

				default:
					break;
				}

				if (m_dwDebugTimeTick == 0x00000000)
				{
					m_dwDebugTimeTick = ::GetTickCount();
					m_nDebugFrameCount = 0;
				}
				else
				{
					m_nDebugFrameCount++;

					if (m_nDebugFrameCount >= 100)
					{
						DWORD curTick = ::GetTickCount();
						DWORD dwTickDiff = curTick - m_dwDebugTimeTick;

						//为了精确到小数帧，故意放大1000倍之后报送给上层应用，上层应用需要缩小1000之后得到浮点数
						uint32_t fps = (int)round(1000.0 * m_nDebugFrameCount / (dwTickDiff / 1000.0));

						::PostMessage(m_hVidWnd, WM_REPORT_VIDEO_DECODE_FPS, NULL, (LPARAM)fps);		//chendelin

						m_nDebugFrameCount = 0;
						m_dwDebugTimeTick = curTick;
					}
				}

				m_lpDDSOffscreen->Unlock(NULL);

#if RENDER_IN_THREAD

#if USE_SURFACE_ACCESS_MUTEX
				uint32_t wait_state = ::WaitForSingleObject(m_hSurfaceAccess, INFINITE);
				if (wait_state == WAIT_OBJECT_0)
				{
#endif
					m_bSurfaceDataAvailable = 1;
#if USE_SURFACE_ACCESS_MUTEX
					::ReleaseMutex(m_hSurfaceAccess);
				}
#endif

#else
				RenderOnPrimarySurface();

#endif // RENDER_IN_THREAD

				break;
			}
			else if (ddRval == 0x887601C2)
			{
				//显示器休眠后再次激活后的返回错误码
				ReleaseDirectDrawResource();

				//int nCanvasWidth = (int)round(m_nSourceWidth * m_dCanvasEnlargeCoeff);
				//int nCanvasHeight = (int)round(m_nSourceHeight * m_dCanvasEnlargeCoeff);

				AllocateDirectDrawResource(m_hVidWnd, m_nSourceWidth, m_nSourceHeight, m_dwFourCC);
			}

		} while (ddRval == DDERR_WASSTILLDRAWING);
	}

	return ddRval;
}

int CTALForDirectDraw::ToggleGrid(void)
{
	if (m_nGrid == 0)
	{
		m_nGrid = 8;
	}
	else
	{
		m_nGrid <<= 1;
		if (m_nGrid > 64)
		{
			m_nGrid = 0;
		}
	}

	HRESULT	ddRval = RenderOnPrimarySurface();

	return 0;
}

int CTALForDirectDraw::ToggleView(void)
{
	int rtcode = -1;

	RECT  rcClient;
	::GetClientRect(m_hVidWnd, &rcClient);

	int nClientWidth = rcClient.right - rcClient.left;
	int nClientHeight = rcClient.bottom - rcClient.top;

	int dstWidth = (int)(m_nSourceWidth * m_dViewEnlargeCoeff);
	int dstHeight = (int)(m_nSourceHeight * m_dViewEnlargeCoeff);
	if ((dstWidth >= nClientWidth) || (dstHeight >= nClientHeight))
	{
		m_dViewEnlargeCoeff = 0.25;
	}
	else
	{
		m_dViewEnlargeCoeff *= 2.0;

		if (m_dViewEnlargeCoeff > 4.0)
		{
			m_dViewEnlargeCoeff = 0.25;
		}
	}

	dstWidth = (int)(m_nSourceWidth * m_dViewEnlargeCoeff);
	dstHeight = (int)(m_nSourceHeight * m_dViewEnlargeCoeff);

	if (dstHeight > nClientHeight)
	{
		m_dViewEnlargeCoeff = (double)nClientHeight / m_nSourceHeight;
	}
	else if (dstWidth > nClientWidth)
	{
		m_dViewEnlargeCoeff = (double)nClientWidth / m_nSourceWidth;
	}

	rtcode = RenderOnPrimarySurface();

	return rtcode;
}

int CTALForDirectDraw::RenderOnPrimarySurface(void)
{
	HRESULT	ddRval = -1;

	if ((m_lpDDSPrimary != NULL) && 
		(m_lpDDSOffscreen != NULL) &&
		(m_pcClipper != NULL))
	{
		if (m_lpDDSPrimary != NULL)
		{
			m_lpDDSPrimary->SetClipper(m_pcClipper);
		}

		//Get the current window client area
		RECT  rcClient;
		::GetClientRect(m_hVidWnd, &rcClient);

		POINT ptOrigin;

		int nClientWidth = rcClient.right - rcClient.left;
		int nClientHeight = rcClient.bottom - rcClient.top;

		int dstWidth = (int)(m_nSourceWidth * m_dViewEnlargeCoeff);
		int dstHeight = (int)(m_nSourceHeight * m_dViewEnlargeCoeff);
		//int dstWidth = m_nViewWidth;
		//int dstHeight = m_nViewHeight;
		if (m_dViewEnlargeCoeff == 1.0)		//Solve the 4K video display bug
		{
			if (dstWidth > nClientWidth) dstWidth = nClientWidth;
			if (dstHeight > nClientHeight) dstHeight = nClientHeight;
		}

		ptOrigin.x = (nClientWidth - dstWidth) / 2;
		ptOrigin.y = (nClientHeight - dstHeight) / 2;

		POINT ptScreen;
		RECT  rectSrc, rectDst;

		ptScreen = ptOrigin;
		::ClientToScreen(m_hVidWnd, &ptScreen);

		rectDst.left = ptScreen.x;
		rectDst.top = ptScreen.y;
		rectDst.right = rectDst.left + dstWidth;
		rectDst.bottom = rectDst.top + dstHeight;

		//int nCanvasWidth = (int)round(m_nSourceWidth * m_dCanvasEnlargeCoeff);
		//int nCanvasHeight = (int)round(m_nSourceHeight * m_dCanvasEnlargeCoeff);

		rectSrc.left = 0;
		rectSrc.top = 0;
		rectSrc.right = m_nSourceWidth;
		rectSrc.bottom = m_nSourceHeight;

		if (m_lpDDSPrimary != NULL)
		{
#if USE_SURFACE_ACCESS_MUTEX
			uint32_t wait_state = ::WaitForSingleObject(m_hSurfaceAccess, INFINITE);
			if (wait_state == WAIT_OBJECT_0)
			{
#endif
			//DDSURFACEDESC2		ddsd;    // DirectDraw 表面描述
			//ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
			//ddsd.dwSize = sizeof(DDSURFACEDESC2);

			//ddRval = m_lpDDSOffscreen->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_READONLY, NULL);
			//if (ddRval == DD_OK)
			//{
				ddRval = m_lpDDSPrimary->Blt(&rectDst, m_lpDDSOffscreen, &rectSrc, DDBLT_WAIT, NULL);
				//ddRval = m_lpDDSPrimary->BltFast(rectDst.left, rectDst.top, m_lpDDSOverlay, &rectSrc, DDBLTFAST_WAIT);
			//	m_lpDDSOffscreen->Unlock(NULL);
			//}

				m_bSurfaceDataAvailable = 0;
#if USE_SURFACE_ACCESS_MUTEX
				::ReleaseMutex(m_hSurfaceAccess);
			}
#endif
		}

		if (m_nGrid > 0)
		{
			HDC hDC = ::GetDC(m_hVidWnd);
			//::TextOutA(hDC, 10, 10, "fuck", 4);
			//::Rectangle(hDC, ptClient.x, ptClient.y, ptClient.x + dstWidth, ptClient.y + dstHeight);

			for (int row = m_nGrid; row < m_nSourceHeight; row += m_nGrid)
			{
				int delty = (int)(row * m_dViewEnlargeCoeff);
				::MoveToEx(hDC, ptOrigin.x, ptOrigin.y + delty, NULL);
				::LineTo(hDC, ptOrigin.x + dstWidth, ptOrigin.y + delty);
			}
			for (int col = m_nGrid; col < m_nSourceWidth; col += m_nGrid)
			{
				int deltx = (int)(col * m_dViewEnlargeCoeff);
				::MoveToEx(hDC, ptOrigin.x + deltx, ptOrigin.y, NULL);
				::LineTo(hDC, ptOrigin.x + deltx, ptOrigin.y + dstHeight);
			}

			::ReleaseDC(m_hVidWnd, hDC);
		}

		if (m_lpDDSPrimary != NULL)
		{
			m_lpDDSPrimary->SetClipper(NULL);
		}
	}

	return ddRval;
}

#if RENDER_IN_THREAD
//why should we run a thread to render surface? I don't know.  chendelin 2019.2.21
uint32_t thread_surface_render_process(LPVOID lpParam)
{
	int				rtcode = -1;
	DWORD			dwStartTick = 0;
	int				frameCount = 0;

	CTALForDirectDraw* pFather = (CTALForDirectDraw*)lpParam;

	if (pFather != NULL)
	{
		double frame_rate = pFather->m_dFrameRate;
		double frame_interval = 1000.0 / frame_rate;

		pFather->m_bRenderResponseStatus = 1;
		while (pFather->m_bRenderControllStatus)
		{
			if (pFather->m_bSurfaceDataAvailable)
			{
				pFather->RenderOnPrimarySurface();
				//frameCount++;
				//if (frameCount == 1)
				//{
				//	dwStartTick = ::GetTickCount();
				//}

				////停下来等
				//int timeThreadHold = (int)round(frameCount * frame_interval);
				//while (pFather->m_bRenderControllStatus)
				//{
				//	DWORD dwNowTick = ::GetTickCount();
				//	int timeElapse = dwNowTick - dwStartTick;
				//	if (timeElapse >= timeThreadHold)
				//	{
				//		if (frameCount == 1000000)
				//		{
				//			frameCount = 0;
				//			dwStartTick = dwNowTick;
				//		}
				//		break;
				//	}
				//}
			}
		}
		pFather->m_bRenderResponseStatus = 0;

		rtcode = 0;
	}

	return rtcode;
}

#endif

