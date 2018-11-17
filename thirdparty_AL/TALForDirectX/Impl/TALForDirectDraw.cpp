#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "../Include/TALForDirectDraw.h"

CTALForDirectDraw::CTALForDirectDraw(void)
{
	m_hVidWnd = NULL;

	//direct draw settings
	m_lpDD = NULL;
	m_lpDDSOverlay = NULL;
	m_lpDDSPrimary = NULL;
	m_pcClipper = NULL;
}

CTALForDirectDraw::~CTALForDirectDraw()
{
	assert(m_lpDD == NULL);
	assert(m_pcClipper == NULL);
	assert(m_lpDDSOverlay == NULL);
	assert(m_lpDDSPrimary == NULL);
}

int CTALForDirectDraw::OpenVideo(HWND hWnd, int luma_width, int luma_height, char* pszFourCC, float frame_rate)
{
	int rtcode = -1;

	// 创建DirectCraw对象
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
				m_ddsd.dwWidth = luma_width;
				m_ddsd.dwHeight = luma_height;
				m_ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
				m_ddsd.ddpfPixelFormat.dwFlags = DDPF_FOURCC | DDPF_YUV;

			//	if (strcmp(pszFourCC, "IYUV") == 0)			//YUV420 Y-U-V
			//	{
			//		m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('I','Y', 'U', 'V');
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
			//	}
			//	else if (strcmp(pszFourCC, "YV16") == 0)			//YUV420 Y-V-U
			//	{
			//		m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y','V', '1', '6');
			//		m_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
			//	}

				if (strcmp(pszFourCC, "YV12") == 0)			//YUV420 Y-V-U
				{
					m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'V', '1', '2');
					m_ddsd.ddpfPixelFormat.dwYUVBitCount = 12;
				}
				else if (strcmp(pszFourCC, "YUY2") == 0)			//YUV420 Y-V-U
				{
					m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'U', 'Y', '2');
					m_ddsd.ddpfPixelFormat.dwYUVBitCount = 16;
				}
				else										//YUV420 Y-V-U
				{
					m_ddsd.ddpfPixelFormat.dwFourCC = MAKEFOURCC('Y', 'V', '1', '2');
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
							m_fViewRatio = 1.0f;
							//m_fDisplayCycle = (float)(1000000.0 / frame_rate);

							//m_nDebugFrameCount = 0;
							//m_dwTickCount = 0x00000000;

							m_hVidWnd = hWnd;
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

int CTALForDirectDraw::RenderYUV(LPBYTE lpY, int y_size, LPBYTE lpU, int u_size, LPBYTE lpV, int v_size)
{
	LPBYTE  lpSurf = NULL;
	int		i;
	HRESULT	ddRval = -1;

	int		widthY, widthU, widthV;
	int		heightY, heightU, heightV;
	int		pitchY, pitchU, pitchV;

	if ((m_lpDDSOverlay != NULL) &&
		(lpY != NULL) && (lpU != NULL) && (lpV != NULL))
	{
		do
		{
			ddRval = m_lpDDSOverlay->Lock(NULL, &m_ddsd, DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);

			if (ddRval == DD_OK)
			{
				DWORD dwFourCC = m_ddsd.ddpfPixelFormat.dwFourCC;
				switch (dwFourCC)
				{
				case MAKEFOURCC('Y', 'V', '1', '2'):
					widthY = m_ddsd.dwWidth;
					heightY = m_ddsd.dwHeight;
					pitchY = m_ddsd.lPitch;

					widthU = (m_ddsd.dwWidth >> 1);
					heightU = (m_ddsd.dwHeight >> 1);
					pitchU = (m_ddsd.lPitch >> 1);

					widthV = (m_ddsd.dwWidth >> 1);
					heightV = (m_ddsd.dwHeight >> 1);
					pitchV = (m_ddsd.lPitch >> 1);

					assert(y_size == (widthY * heightY));
					assert(u_size == (widthU * heightU));
					assert(v_size == (widthV * heightV));

					// 填充离屏表面
					lpSurf = (LPBYTE)m_ddsd.lpSurface;
					if (lpSurf)
					{
						for (i = 0; i < heightY; i++)
						{
							memcpy(lpSurf, lpY, widthY);
							lpY += widthY;
							lpSurf += pitchY;
						}

						for (i = 0; i < heightV; i++)
						{
							memcpy(lpSurf, lpV, widthV);
							lpV += widthV;
							lpSurf += pitchV;
						}

						for (i = 0; i < heightU; i++)
						{
							memcpy(lpSurf, lpU, widthU);
							lpU += widthU;
							lpSurf += pitchU;
						}
					}

					break;

				default:
					break;
				}

				m_lpDDSOverlay->Unlock(NULL);

				RePaint();
				
				//if (m_dwTickCount == 0)
				//{
				//	m_dwTickCount = GetTickCount();
				//}
				//else
				//{
				//	ULONGLONG newTickCount;

				//	newTickCount = GetTickCount();
				//	ULONGLONG dwTimeDiff = newTickCount - m_dwTickCount;

				//	if (dwTimeDiff >= 1000)
				//	{
				//		uint32_t fps = (uint32_t)(m_nDebugFrameCount * 1000000 / dwTimeDiff);

				//		//::SendMessage(m_hVidWnd, WM_REPORT_VIDEO_DECODE_FPS, (WPARAM)fps, NULL);

				//		m_nDebugFrameCount = 0;
				//		m_dwTickCount = newTickCount;
				//	}
				//	else
				//	{
				//		m_nDebugFrameCount++;
				//	}
				//}

				break;
			}

		} while (ddRval == DDERR_WASSTILLDRAWING);
	}

	return ddRval;
}

int CTALForDirectDraw::RePaint(void)
{
	HRESULT	ddRval = -1;
	HWND	hWnd = m_hVidWnd;

	if ((m_lpDDSPrimary != NULL) && 
		(m_lpDDSOverlay != NULL) &&
		(m_pcClipper != NULL))
	{
		m_lpDDSPrimary->SetClipper(m_pcClipper);

		RECT  rcClient;
		::GetClientRect(hWnd, &rcClient);

		int dstwidth = 0;
		int dstheight = 0;

		dstwidth = (int)(m_ddsd.dwWidth * m_fViewRatio);
		dstheight = (int)(m_ddsd.dwHeight * m_fViewRatio);
		if (dstwidth > 1000)
		{
			dstwidth = 960;
			dstheight = 540;
		}

		POINT pt;

		pt.x = (rcClient.right - rcClient.left - dstwidth) / 2;
		pt.y = (rcClient.bottom - rcClient.top - dstheight - 80) / 2;

		::ClientToScreen(hWnd, &pt);
		
		RECT  rectSrc, rectDst;

		rectDst.left = pt.x;
		rectDst.top = pt.y;
		rectDst.right = rectDst.left + dstwidth;
		rectDst.bottom = rectDst.top + dstheight;

		rectSrc.left = 0;
		rectSrc.top = 0;
		rectSrc.right = m_ddsd.dwWidth;
		rectSrc.bottom = m_ddsd.dwHeight;

		ddRval = m_lpDDSPrimary->Blt(&rectDst, m_lpDDSOverlay, &rectSrc, DDBLT_WAIT, NULL);

		m_lpDDSPrimary->SetClipper(NULL);
	}

	return ddRval;
}


