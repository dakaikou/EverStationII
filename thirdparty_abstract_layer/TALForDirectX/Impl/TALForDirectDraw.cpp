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

	//m_dFrameRate = 30.0;				//default value 30/s
	m_nSourceWidth = -1;
	m_nSourceHeight = -1;
	//m_nCanvasWidth = -1;
	//m_nCanvasHeight = -1;
	m_ptOrigin.x = 0;
	m_ptOrigin.y = 0;
	m_nViewWidth = -1;
	m_nViewHeight = -1;
	m_dwFourCC = 0x00000000;

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

int CTALForDirectDraw::OpenVideo(HWND hWnd, int source_width, int source_height, DWORD dwFourCC)
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
	m_nViewWidth = m_nSourceWidth;
	m_nViewHeight = m_nSourceHeight;

	//int nCanvasWidth = (int)round(m_nSourceWidth * m_dCanvasEnlargeCoeff);
	//int nCanvasHeight = (int)round(m_nSourceHeight * m_dCanvasEnlargeCoeff);

	m_dwFourCC = dwFourCC;

	//m_dFrameRate = framerate;

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
	m_ptOrigin.x = 0;
	m_ptOrigin.y = 0;
	m_nViewWidth = -1;
	m_nViewHeight = -1;
	//m_dCanvasEnlargeCoeff = 1.0;
	m_dViewEnlargeCoeff = 1.0;
	m_dwFourCC = 0x00000000;

	return rtcode;
}

int CTALForDirectDraw::SetClientRect(RECT rcClient)
{
	int					rtcode = -1;

	m_rcClient = rcClient;

	int nClientWidth = m_rcClient.right - m_rcClient.left;
	int nClientHeight = m_rcClient.bottom - m_rcClient.top;

	int m_nViewWidth = (int)(m_nSourceWidth * m_dViewEnlargeCoeff);
	int m_nViewHeight = (int)(m_nSourceHeight * m_dViewEnlargeCoeff);
	//int dstWidth = m_nViewWidth;
	//int dstHeight = m_nViewHeight;
	//if (m_dViewEnlargeCoeff == 1.0)		//Solve the 4K video display bug
	//{
	//	if ((m_nViewWidth > nClientWidth) || (m_nViewHeight > nClientHeight))
	//	{
	//		double srcAspectRatio = (double)m_nSourceWidth / m_nSourceHeight;
	//		double clientAspectRatio = (double)nClientWidth / nClientHeight;
	//		double aspectRatio = max(srcAspectRatio, clientAspectRatio);

	//		m_nViewWidth = nClientWidth;
	//		m_nViewHeight = nClientHeight;
	//	}

	//	//if (dstHeight > nClientHeight) dstHeight = nClientHeight;
	//}

	m_ptOrigin.x = m_rcClient.left + (nClientWidth - m_nViewWidth) / 2;
	m_ptOrigin.y = (nClientHeight - m_nViewHeight) / 2;

	return rtcode;
}

int CTALForDirectDraw::AllocateDirectDrawResource(HWND hWnd, int canvas_width, int canvas_height, DWORD dwFourCC)
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
#if RENDER_IN_RGB_MODE
				ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_RGB;
				ddsd.ddpfPixelFormat.dwFourCC = 0x32424752;
				ddsd.ddpfPixelFormat.dwRGBBitCount = 24;			//default value
#else
#if RENDER_IN_YUV420_MODE
				ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;
				ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('I', '4', '2', '0');				//0x30323449	Y-U-V
				ddsd.ddpfPixelFormat.dwYUVBitCount = 8;			//default value
				//ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'V', '1', '2');				//0x32315659,  Y-V-U
				//ddsd.ddpfPixelFormat.dwYUVBitCount = 8;			//default value
#else
				ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;
				//ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'U', 'Y', '2');				//0x32595559
				//ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('U', 'Y', 'V', 'Y');					//0x59565955
				//ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', '4', '2', '2');				//0x32323459
				ddsd.ddpfPixelFormat.dwFourCC = dwFourCC;					//0x59565955
				ddsd.ddpfPixelFormat.dwYUVBitCount = 8;			//default value
#endif
#endif

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

int CTALForDirectDraw::FeedToOffScreenSurface(const LPBYTE lpYBuf, const LPBYTE lpUBuf, const LPBYTE lpVBuf, int planeSize)
{
	LPBYTE  lpFrame = NULL;
	LPBYTE  lpSurf = NULL;
	HRESULT	ddRval = -1;

	DDSURFACEDESC2		ddsd;    // DirectDraw 表面描述

	if ((m_lpDDSOffscreen != NULL) &&
		(lpYBuf != NULL) &&
		(lpUBuf != NULL) &&
		(lpVBuf != NULL))
	{
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		do
		{
			ddRval = m_lpDDSOffscreen->Lock(NULL, &ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
			if (ddRval == DD_OK)
			{
				// 填充离屏表面
				lpSurf = (LPBYTE)ddsd.lpSurface;
				if (lpSurf != NULL)
				{
#if RENDER_IN_RGB_MODE
					lpFrame = lpRBuf;
					for (DWORD i = 0; i < ddsd.dwHeight; i++)
					{
						lpSurf[i] = 0x00;
						//memcpy(lpSurf, lpFrame, ddsd.dwWidth);
						lpFrame += ddsd.dwWidth;
						lpSurf += ddsd.lPitch;
					}

					//lpFrame = lpGBuf;
					//for (DWORD i = 0; i < ddsd.dwHeight/2; i++)
					//{
					//	memcpy(lpSurf, lpFrame, ddsd.dwWidth/2);
					//	lpFrame += ddsd.dwWidth/2;
					//	lpSurf += ddsd.lPitch/2;
					//}

					//lpFrame = lpBBuf;
					//for (DWORD i = 0; i < ddsd.dwHeight / 2; i++)
					//{
					//	memcpy(lpSurf, lpFrame, ddsd.dwWidth / 2);
					//	lpFrame += ddsd.dwWidth / 2;
					//	lpSurf += ddsd.lPitch / 2;
					//}
#else
#if RENDER_IN_YUV420_MODE
					uint8_t* ptrY = lpYBuf;
					uint8_t* ptrU = lpUBuf;
					uint8_t* ptrV = lpVBuf;
					for (DWORD i = 0; i < ddsd.dwHeight; i++)
					{
						//for (DWORD j = 0; j < ddsd.dwWidth; j++)
						//{
						//	lpSurf[j] = ptrY[j];
						//}
						memcpy(lpSurf, ptrY, ddsd.dwWidth);
						lpSurf += ddsd.lPitch;
						ptrY += ddsd.dwWidth;
					}

					for (DWORD i = 0; i < ddsd.dwHeight / 2; i++)
					{
						//for (DWORD j = 0; j < ddsd.dwWidth/2; j++)
						//{
						//	lpSurf[j] = ptrU[j];
						//}
						memcpy(lpSurf, ptrU, ddsd.dwWidth / 2);
						ptrU += ddsd.dwWidth / 2;
						lpSurf += ddsd.lPitch / 2;
				}

					for (DWORD i = 0; i < ddsd.dwHeight / 2; i++)
					{
						for (DWORD j = 0; j < ddsd.dwWidth / 2; j++)
						{
							lpSurf[j] = ptrV[j];
						}
						//memcpy(lpSurf, lpFrame, ddsd.dwWidth / 2);
						ptrV += ddsd.dwWidth / 2;
						lpSurf += ddsd.lPitch / 2;
					}
#endif

					int luma_width = ddsd.dwWidth;
					int luma_height = ddsd.dwHeight;
					int chroma_width = ddsd.dwWidth;
					int chroma_height = ddsd.dwHeight;

					uint8_t* ptrY = lpYBuf;
					uint8_t* ptrU = lpUBuf;
					uint8_t* ptrV = lpVBuf;

					if (ddsd.ddpfPixelFormat.dwFourCC == 0x56555949)				//IYUV   4:2:0
					{
						chroma_width = luma_width / 2;
						chroma_height = luma_height / 2;
						for (DWORD i = 0; i < luma_height; i++)
						{
							for (DWORD j = 0; j < luma_width; j++)
							{
								lpSurf[j] = ptrY[j];
							}
							//memcpy(lpSurf, ptrY, ddsd.dwWidth);
							lpSurf += ddsd.lPitch;
							ptrY += luma_width;
						}

						for (DWORD i = 0; i < chroma_height; i++)
						{
							for (DWORD j = 0; j < chroma_width; j++)
							{
								lpSurf[j] = ptrU[j];
							}
							//memcpy(lpSurf, ptrU, ddsd.dwWidth / 2);
							lpSurf += ddsd.lPitch / 2;
							ptrU += chroma_width;
						}

						for (DWORD i = 0; i < chroma_height; i++)
						{
							for (DWORD j = 0; j < chroma_width; j++)
							{
								lpSurf[j] = ptrV[j];
							}
							//memcpy(lpSurf, lpFrame, ddsd.dwWidth / 2);
							lpSurf += ddsd.lPitch / 2;
							ptrV += chroma_width;
						}
					}
					else if (ddsd.ddpfPixelFormat.dwFourCC == 0x32595559)			//YUY2		4:2:2
					{
						chroma_width = luma_width / 2;
						for (DWORD i = 0; i < luma_height; i++)
						{
							int rowIndex = 0;
							for (DWORD j = 0; j < luma_width; j++)
							{
								lpSurf[rowIndex] = ptrY[j];
								rowIndex++;
								if (j % 2 == 0)
								{
									lpSurf[rowIndex] = ptrV[j / 2];
								}
								else
								{
									lpSurf[rowIndex] = ptrU[j / 2];
								}

								rowIndex++;
							}
							//memcpy(lpSurf, ptrY, ddsd.dwWidth);
							lpSurf += ddsd.lPitch;
							ptrY += luma_width;
							ptrU += chroma_width;
							ptrV += chroma_width;
						}
					}
#endif
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

//int CTALForDirectDraw::ToggleView(void)
//{
//	int rtcode = -1;
//
//	RECT  rcClient;
//	::GetClientRect(m_hVidWnd, &rcClient);
//
//	int nClientWidth = rcClient.right - rcClient.left;
//	int nClientHeight = rcClient.bottom - rcClient.top;
//
//	int dstWidth = (int)(m_nSourceWidth * m_dViewEnlargeCoeff);
//	int dstHeight = (int)(m_nSourceHeight * m_dViewEnlargeCoeff);
//	if ((dstWidth >= nClientWidth) || (dstHeight >= nClientHeight))
//	{
//		m_dViewEnlargeCoeff = 0.25;
//	}
//	else
//	{
//		m_dViewEnlargeCoeff *= 2.0;
//
//		if (m_dViewEnlargeCoeff > 4.0)
//		{
//			m_dViewEnlargeCoeff = 0.25;
//		}
//	}
//
//	dstWidth = (int)(m_nSourceWidth * m_dViewEnlargeCoeff);
//	dstHeight = (int)(m_nSourceHeight * m_dViewEnlargeCoeff);
//
//	if (dstHeight > nClientHeight)
//	{
//		m_dViewEnlargeCoeff = (double)nClientHeight / m_nSourceHeight;
//	}
//	else if (dstWidth > nClientWidth)
//	{
//		m_dViewEnlargeCoeff = (double)nClientWidth / m_nSourceWidth;
//	}
//
//	rtcode = RenderOnPrimarySurface();
//
//	return rtcode;
//}

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

		POINT ptScreen;
		RECT  rectSrc, rectDst;

		ptScreen = m_ptOrigin;
		::ClientToScreen(m_hVidWnd, &ptScreen);

		rectDst.left = ptScreen.x;
		rectDst.top = ptScreen.y;
		rectDst.right = rectDst.left + m_nViewWidth;
		rectDst.bottom = rectDst.top + m_nViewHeight;

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
				::MoveToEx(hDC, m_ptOrigin.x, m_ptOrigin.y + delty, NULL);
				::LineTo(hDC, m_ptOrigin.x + m_nViewWidth, m_ptOrigin.y + delty);
			}
			for (int col = m_nGrid; col < m_nSourceWidth; col += m_nGrid)
			{
				int deltx = (int)(col * m_dViewEnlargeCoeff);
				::MoveToEx(hDC, m_ptOrigin.x + deltx, m_ptOrigin.y, NULL);
				::LineTo(hDC, m_ptOrigin.x + deltx, m_ptOrigin.y + m_nViewHeight);
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

