#include <assert.h>
#include <math.h>

#include "../Include/TALForDirectDraw.h"

CTALForDirectDraw::CTALForDirectDraw(void)
{
	m_hVidWnd = NULL;

	//direct draw settings
	m_lpDD = NULL;
	m_lpDDSOverlay = NULL;
	m_lpDDSPrimary = NULL;
	m_pcClipper = NULL;

	m_nGrid = 0;
	m_bFullScreen = 0;

	m_ptOrigin.x = -1;
	m_ptOrigin.y = -1;
}

CTALForDirectDraw::~CTALForDirectDraw()
{
	assert(m_lpDD == NULL);
	assert(m_pcClipper == NULL);
	assert(m_lpDDSOverlay == NULL);
	assert(m_lpDDSPrimary == NULL);
}

int CTALForDirectDraw::OpenVideo(HWND hWnd, int canvas_width, int canvas_height, unsigned int dwFourCC)
{
	int rtcode = -1;

	// 创建DirectCraw对象
	//use the system default display hardware
	rtcode = DirectDrawCreateEx(NULL, (VOID**)&m_lpDD, IID_IDirectDraw7, NULL);
	if (rtcode == DD_OK)
	{
		// 设置协作层
		rtcode = m_lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL | DDSCL_NOWINDOWCHANGES);
		if (rtcode == DD_OK)
		{
			// 创建主表面
			ZeroMemory(&m_ddsd, sizeof(m_ddsd));
			m_ddsd.dwSize = sizeof(m_ddsd);
			m_ddsd.dwFlags = DDSD_CAPS;
			m_ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			rtcode = m_lpDD->CreateSurface(&m_ddsd, &m_lpDDSPrimary, NULL);
			if (rtcode == DD_OK)
			{
				// 创建YUV表面 
				ZeroMemory(&m_ddsd, sizeof(m_ddsd));

				m_ddsd.dwSize = sizeof(m_ddsd);
				m_ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			//	m_ddsd.ddsCaps.dwCaps = DDSCAPS_OVERLAY;
				m_ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
				m_ddsd.dwWidth = canvas_width;
				m_ddsd.dwHeight = canvas_height;
				m_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
				m_ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;

				//DWORD dwFourCC = MAKEFOURCC(pszFourCC[0], pszFourCC[1], pszFourCC[2], pszFourCC[3]);
				m_ddsd.ddpfPixelFormat.dwFourCC = dwFourCC;
				m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;			//default value

				if (dwFourCC == 0x56555949)			//IYUV: Planar YUV, YUV420 Y-U-V, the same as I420
				{
					m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
				}
				else if (dwFourCC == 0x30323449)			//I420: Planar YUV, YUV420 Y-U-V, the same as IYUV
				{
					m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
				}
				else if (dwFourCC == 0x32315659)			//YV12: Planar YUV, YUV420 Y-V-U
				{
					m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
				}
			//	else if (dwFourCC == 0x36315659)			//YV16: Planar YUV422 Y-U-V
			//	{
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
			//	}
				else										//YUV420 Y-V-U
				{
					m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
				}

				rtcode = m_lpDD->CreateSurface(&m_ddsd, &m_lpDDSOverlay, NULL);
				if (rtcode == DD_OK)
				{
					rtcode = m_lpDD->CreateClipper(0, &m_pcClipper, NULL);
					if (rtcode == DD_OK)
					{
						rtcode = m_pcClipper->SetHWnd(0, hWnd);
						if (rtcode == DD_OK)
						{
							//m_fViewRatio = 1.0f;
							m_hVidWnd = hWnd;

							m_nDebugFrameCount = 0;
							m_dwDebugTimeTick = 0;
						}
						else
						{
							m_pcClipper->Release();
							m_pcClipper = NULL;

							m_lpDDSPrimary->Release();
							m_lpDDSPrimary = NULL;

							m_lpDDSOverlay->Release();
							m_lpDDSOverlay = NULL;

							m_lpDD->Release();
							m_lpDD = NULL;
						}
					}
					else
					{
						m_lpDDSPrimary->Release();
						m_lpDDSPrimary = NULL;

						m_lpDDSOverlay->Release();
						m_lpDDSOverlay = NULL;

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

int CTALForDirectDraw::CloseVideo(void)
{
	if (m_lpDDSOverlay)
	{
		m_lpDDSOverlay->Release();
		m_lpDDSOverlay = NULL;
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

	m_hVidWnd = NULL;

	return NO_ERROR;
}

int CTALForDirectDraw::RenderYUV(const LPBYTE lpFrameBuf, int frameSize, const FRAME_PARAMS_t* pstFrameParams)
{
	LPBYTE  lpFrame = NULL;
	LPBYTE  lpSurf = NULL;
	int		i;
	HRESULT	ddRval = -1;

	//int		widthY, widthU, widthV;
	//int		heightY, heightU, heightV;
	int		pitchLuma, pitchChroma;

	if ((m_lpDDSOverlay != NULL) &&
		(lpFrameBuf != NULL) && (pstFrameParams != NULL))
	{
		do
		{
			ddRval = m_lpDDSOverlay->Lock(NULL, &m_ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
			if (ddRval == DD_OK)
			{
				lpFrame = lpFrameBuf;

				DWORD dwFourCC = m_ddsd.ddpfPixelFormat.dwFourCC;
				switch (dwFourCC)
				{
				case 0x32315659: //MAKEFOURCC('Y', 'V', '1', '2'):
				case 0x30323449: //MAKEFOURCC('I', '4', '2', '0'):
				case 0x56555949: //MAKEFOURCC('I', 'Y', 'U', 'V'):
					//widthY = m_ddsd.dwWidth;
					//heightY = m_ddsd.dwHeight;
					pitchLuma = m_ddsd.lPitch;

					//widthV = (m_ddsd.dwWidth >> 1);
					//heightV = (m_ddsd.dwHeight >> 1);
					pitchChroma = (m_ddsd.lPitch >> 1);

					//widthU = (m_ddsd.dwWidth >> 1);
					//heightU = (m_ddsd.dwHeight >> 1);
					//pitchU = (m_ddsd.lPitch >> 1);

					//assert(y_size == (widthY * heightY));
					//assert(u_size == (widthU * heightU));
					//assert(v_size == (widthV * heightV));

					// 填充离屏表面
					lpSurf = (LPBYTE)m_ddsd.lpSurface;
					if ((lpSurf != NULL) && (lpFrame != NULL))
					{
						for (i = 0; i < pstFrameParams->luma_height; i++)
						{
							memcpy(lpSurf, lpFrame, pstFrameParams->luma_width);
							lpFrame += pstFrameParams->luma_width;
							lpSurf += pitchLuma;
						}

						for (i = 0; i < pstFrameParams->chroma_height; i++)
						{
							memcpy(lpSurf, lpFrame, pstFrameParams->chroma_width);
							lpFrame += pstFrameParams->chroma_width;
							lpSurf += pitchChroma;
						}

						for (i = 0; i < pstFrameParams->chroma_height; i++)
						{
							memcpy(lpSurf, lpFrame, pstFrameParams->chroma_width);
							lpFrame += pstFrameParams->chroma_width;
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

						::SendMessage(m_hVidWnd, WM_REPORT_VIDEO_DECODE_FPS, (WPARAM)fps, NULL);		//chendelin

						m_nDebugFrameCount = 0;
						m_dwDebugTimeTick = curTick;
					}
				}

				m_lpDDSOverlay->Unlock(NULL);

				RePaint();
				
				break;
			}

		} while (ddRval == DDERR_WASSTILLDRAWING);
	}

	return ddRval;
}

//int CTALForDirectDraw::RePaint(void)
//{
//	HRESULT	ddRval = -1;
//	HWND	hWnd = m_hVidWnd;
//
//	if ((m_lpDDSPrimary != NULL) &&
//		(m_lpDDSOverlay != NULL) &&
//		(m_pcClipper != NULL))
//	{
//		m_lpDDSPrimary->SetClipper(m_pcClipper);
//
//		RECT  rcClient;
//		::GetClientRect(hWnd, &rcClient);
//
//		int nWidth = rcClient.right - rcClient.left;
//		int nHeight = rcClient.bottom - rcClient.top;
//		if (nWidth > m_nOldScreenWidth)
//		{
//			m_nOldScreenWidth = nWidth;
//		}
//		if (nHeight > m_nOldScreenHeight)
//		{
//			m_nOldScreenHeight = nHeight;
//		}
//
//		//int g_iCurScreenWidth = GetSystemMetrics(SM_CXSCREEN);
//		//int g_iCurScreenHeight = GetSystemMetrics(SM_CYSCREEN);
//
//		//WINDOWPLACEMENT			m_stOldWndPlacement;		//结构中包含了有关窗口在屏幕上位置的信息
//		//GetWindowPlacement(hWnd, &m_stOldWndPlacement);
//		//int iCurScreenWidth = (m_stOldWndPlacement.rcNormalPosition.right - m_stOldWndPlacement.rcNormalPosition.left) -
//		//	((rcWindow.right - rcWindow.left) - (rcClient.right - rcClient.left));
//
//		//int iCurScreenHeight = (m_stOldWndPlacement.rcNormalPosition.bottom - m_stOldWndPlacement.rcNormalPosition.top) -
//		//	((rcWindow.bottom - rcWindow.top) - (rcClient.bottom - rcClient.top));
//		//int iCurScreenWidth = (m_stOldWndPlacement.rcNormalPosition.right - m_stOldWndPlacement.rcNormalPosition.left)/100 * 100;
//		//int iCurScreenHeight = (m_stOldWndPlacement.rcNormalPosition.bottom - m_stOldWndPlacement.rcNormalPosition.top)/100 * 100;
//
//		int iCurScreenWidth = m_nOldScreenWidth;
//		int iCurScreenHeight = m_nOldScreenHeight;
//
//		int dstwidth = 0;
//		int dstheight = 0;
//
//		dstwidth = (int)(m_ddsd.dwWidth * m_fViewRatio);
//		dstheight = (int)(m_ddsd.dwHeight * m_fViewRatio);
//
//		POINT pt;
//
//		//pt.x = (rcClient.right - rcClient.left - dstwidth) / 2;
//		//pt.y = (rcClient.bottom - rcClient.top - dstheight - 80) / 2;
//		pt.x = (iCurScreenWidth - dstwidth) / 2;
//		pt.y = (iCurScreenHeight - dstheight) / 2;
//
//		::ClientToScreen(hWnd, &pt);
//
//		RECT  rectSrc, rectDst;
//
//		rectDst.left = pt.x;
//		rectDst.top = pt.y;
//		rectDst.right = rectDst.left + dstwidth;
//		rectDst.bottom = rectDst.top + dstheight;
//
//		rectSrc.left = 0;
//		rectSrc.top = 0;
//		rectSrc.right = m_ddsd.dwWidth;
//		rectSrc.bottom = m_ddsd.dwHeight;
//
//		ddRval = m_lpDDSPrimary->Blt(&rectDst, m_lpDDSOverlay, &rectSrc, DDBLT_WAIT, NULL);
//
//		m_lpDDSPrimary->SetClipper(NULL);
//	}
//
//	return ddRval;
//}

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

	return 0;
}

int CTALForDirectDraw::ToggleScreen(void)
{
	if (m_bFullScreen)
	{
		m_bFullScreen = false;
	}
	else
	{
		m_bFullScreen = true;
	}

	return 0;
}


int CTALForDirectDraw::RePaint(void)
{
	HRESULT	ddRval = -1;
	//HWND	hWnd = m_hVidWnd;

	if ((m_lpDDSPrimary != NULL) && 
		(m_lpDDSOverlay != NULL) &&
		(m_pcClipper != NULL))
	{
		m_lpDDSPrimary->SetClipper(m_pcClipper);

		//Get the current window client area
		RECT  rcClient;
		::GetClientRect(m_hVidWnd, &rcClient);

		if ((m_ptOrigin.x < 0) || (m_ptOrigin.y < 0))
		{
			int nClientWidth = rcClient.right - rcClient.left;
			int nClientHeight = rcClient.bottom - rcClient.top;

			//int dstWidth = (int)(m_ddsd.dwWidth * m_fViewRatio);
			//int dstHeight = (int)(m_ddsd.dwHeight * m_fViewRatio);

			m_ptOrigin.x = (nClientWidth - m_ddsd.dwWidth) / 2;
			m_ptOrigin.y = (nClientHeight - m_ddsd.dwHeight) / 2;
		}

		POINT ptScreen;
		RECT  rectSrc, rectDst;

		if (m_bFullScreen == false)
		{
			ptScreen = m_ptOrigin;
			::ClientToScreen(m_hVidWnd, &ptScreen);

			rectDst.left = ptScreen.x;
			rectDst.top = ptScreen.y;
			rectDst.right = rectDst.left + m_ddsd.dwWidth;
			rectDst.bottom = rectDst.top + m_ddsd.dwHeight;
		}
		else
		{
			ptScreen.x = 0;
			ptScreen.y = 0;
			::ClientToScreen(m_hVidWnd, &ptScreen);

			rectDst.left = ptScreen.x;
			rectDst.top = ptScreen.y;
			rectDst.right = rectDst.left + (rcClient.right - rcClient.left);
			rectDst.bottom = rectDst.top + (rcClient.bottom - rcClient.top);
		}

		rectSrc.left = 0;
		rectSrc.top = 0;
		rectSrc.right = m_ddsd.dwWidth;
		rectSrc.bottom = m_ddsd.dwHeight;

		ddRval = m_lpDDSPrimary->Blt(&rectDst, m_lpDDSOverlay, &rectSrc, DDBLT_WAIT, NULL);

		if (m_bFullScreen == false)
		{
			if (m_nGrid > 0)
			{
				HDC hDC = ::GetDC(m_hVidWnd);
				//::TextOutA(hDC, 10, 10, "fuck", 4);
				//::Rectangle(hDC, ptClient.x, ptClient.y, ptClient.x + dstWidth, ptClient.y + dstHeight);

				for (DWORD row = m_nGrid; row < m_ddsd.dwHeight; row += m_nGrid)
				{
					::MoveToEx(hDC, m_ptOrigin.x, m_ptOrigin.y + row, NULL);
					::LineTo(hDC, m_ptOrigin.x + m_ddsd.dwWidth, m_ptOrigin.y + row);
				}
				for (DWORD col = m_nGrid; col < m_ddsd.dwWidth; col += m_nGrid)
				{
					::MoveToEx(hDC, m_ptOrigin.x + col, m_ptOrigin.y, NULL);
					::LineTo(hDC, m_ptOrigin.x + col, m_ptOrigin.y + m_ddsd.dwHeight);
				}

				::ReleaseDC(m_hVidWnd, hDC);
			}
		}

		m_lpDDSPrimary->SetClipper(NULL);
	}

	return ddRval;
}


