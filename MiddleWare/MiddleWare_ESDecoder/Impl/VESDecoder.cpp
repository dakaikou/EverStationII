#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "thirdparty_abstract_layer\TALForDirectX\Include\TALForDirectDraw.h"

#include "../Include/common/video_common.h"
#include "../Include/VESDecoder.h"
#include "../Include/ESDecoder_ErrorCode.h"

CVESDecoder::CVESDecoder(void)
{
	memset(&m_stYUVSequenceParam, 0x00, sizeof(YUV_SEQUENCE_PARAM_t));
	m_pucYUVFrameBuf = NULL;
	m_nYUVFrameSize = 0;

	memset(&m_stOutputPlaneParam, 0x00, sizeof(YUV_SEQUENCE_PARAM_t));
	m_pucOutputPlaneBuf = NULL;
	m_nOutputPlaneSize = 0;

	m_pDirectDraw = NULL;

	//direct draw settings

	m_callback_report_yuv_luma_stats = NULL;
	m_hwnd_for_caller = NULL;

#if USE_FRAMEBUF_ACCESS_MUTEX
	m_hSourceFrameBufAccess = ::CreateMutex(NULL, FALSE, NULL);
	m_hOutputFrameBufAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif

	m_hSourceFrameBufEmptyEvent = ::CreateEvent(NULL, TRUE, TRUE, NULL);
	m_hSourceFrameBufFullEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);

	m_bFrameProcessControllStatus = 0;
	m_bFrameProcessResponseStatus = 0;
}

CVESDecoder::~CVESDecoder()
{
	assert(m_pDirectDraw == NULL);

#if USE_FRAMEBUF_ACCESS_MUTEX
	uint32_t wait_state0 = ::WaitForSingleObject(m_hSourceFrameBufAccess, INFINITE);
	if (wait_state0 == WAIT_OBJECT_0)
	{
		::ReleaseMutex(m_hSourceFrameBufAccess);
		::CloseHandle(m_hSourceFrameBufAccess);
		m_hSourceFrameBufAccess = NULL;
	}
#endif

#if USE_FRAMEBUF_ACCESS_MUTEX
	uint32_t wait_state1 = ::WaitForSingleObject(m_hOutputFrameBufAccess, INFINITE);
	if (wait_state1 == WAIT_OBJECT_0)
	{
		::ReleaseMutex(m_hOutputFrameBufAccess);
		::CloseHandle(m_hOutputFrameBufAccess);
		m_hOutputFrameBufAccess = NULL;
	}
#endif

	::SetEvent(m_hSourceFrameBufEmptyEvent);
	::CloseHandle(m_hSourceFrameBufEmptyEvent);

	::SetEvent(m_hSourceFrameBufFullEvent);
	::CloseHandle(m_hSourceFrameBufFullEvent);
}

//void CVESDecoder::Reset(void)
//{
//	m_VidDecodeInfo.getparams = 0;
//
//	CESDecoder::Reset();
//}

int CVESDecoder::Open(uint32_t dwStreamType, const char* pszFileName)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	rtcode = CESDecoder::Open(dwStreamType, pszFileName);

	return rtcode;
}

int CVESDecoder::Close(void)
{
	int rtcode = ESDECODER_PARAMETER_ERROR;

	//memset(&m_stYUVSequenceParam, 0x00, sizeof(YUV_SEQUENCE_PARAM_t));

	if (m_pucYUVFrameBuf != NULL)
	{
		free(m_pucYUVFrameBuf);
		m_pucYUVFrameBuf = NULL;
	}
	m_nYUVFrameSize = 0;
	//m_bSourceDataAvailable = 0;

	//assert(m_bFrameProcessResponseStatus == 0);

	//memset(&m_stRGBSequenceParam, 0x00, sizeof(RGB_SEQUENCE_PARAM_t));

	if (m_pucOutputPlaneBuf != NULL)
	{
		free(m_pucOutputPlaneBuf);
		m_pucOutputPlaneBuf = NULL;
	}
	m_nOutputPlaneSize = 0;

	rtcode = CESDecoder::Close();

	return rtcode;
}

void CVESDecoder::StartFrameProcessThread(void)
{
	m_bFrameProcessControllStatus = true;
	m_bFrameProcessResponseStatus = false;
	//m_bSourceDataAvailable = false;

	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_frame_process, (LPVOID)this, 0, 0);

	while (m_bFrameProcessResponseStatus == 0)
	{
		Sleep(10);
	}
}

void CVESDecoder::StopFrameProcessThread(void)
{
	m_bFrameProcessControllStatus = false;
	::SetEvent(m_hSourceFrameBufFullEvent);
	//::SetEvent(m_hSourceFrameBufEmptyEvent);

	while (m_bFrameProcessResponseStatus == 1)
	{
		Sleep(10);
	}
}

int CVESDecoder::AttachWnd(HWND hWnd, int(*callback_luma)(HWND, WPARAM, LPARAM), int(*callback_chroma)(HWND, WPARAM, LPARAM))
{
	HRESULT ddRval = -1;

	m_hwnd_for_caller = hWnd;

	assert(m_pDirectDraw == NULL);
	m_pDirectDraw = new CTALForDirectDraw;
	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_stOutputPlaneParam.luma_width, m_stOutputPlaneParam.luma_height, m_stOutputPlaneParam.dwFourCC);

	if (callback_luma != NULL)
	{
		m_callback_report_yuv_luma_stats = callback_luma;
	}
	if (callback_chroma != NULL)
	{
		m_callback_report_yuv_chroma_stats = callback_chroma;
	}

	StartFrameProcessThread();

	return ddRval;
}

int CVESDecoder::SetClientRect(RECT rcWnd)
{
	HRESULT ddRval = -1;

	//m_rcWnd = rcWnd;
	m_pDirectDraw->SetClientRect(rcWnd);

	return ddRval;
}

int CVESDecoder::DetachWnd(HWND hWnd)
{
	StopFrameProcessThread();

	HRESULT ddRval = -1;

	if (m_pDirectDraw != NULL)
	{
		ddRval = m_pDirectDraw->CloseVideo();
		delete m_pDirectDraw;
		m_pDirectDraw = NULL;
	}

	m_callback_report_yuv_luma_stats = NULL;
	m_callback_report_yuv_chroma_stats = NULL;

	return ddRval;
}

int CVESDecoder::FrameProcessAndFeedToDirectDraw(void)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;
	assert(m_pDirectDraw != NULL);

#if USE_FRAMEBUF_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hOutputFrameBufAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		uint8_t* pucFrameY = m_pucYUVFrameBuf;
		uint8_t* pucFrameU = pucFrameY + m_stYUVSequenceParam.luma_plane_size;
		uint8_t* pucFrameV = pucFrameU + m_stYUVSequenceParam.chroma_plane_size;

#if USE_FRAMEBUF_ACCESS_MUTEX
		uint32_t wait_state = ::WaitForSingleObject(m_hSourceFrameBufAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
#endif
			uint8_t R, G, B;
			uint8_t Y, U, V;

#if RENDER_IN_AUTO_YUV_MODE

			uint8_t* pSrcY = pucFrameY;
			uint8_t* pSrcU = pucFrameU;
			uint8_t* pSrcV = pucFrameV;

			if (m_stYUVSequenceParam.dwFourCC == 0x56555941)			//ARGB	4:4:4
			{
				assert(m_stYUVSequenceParam.luma_plane_size == m_stYUVSequenceParam.chroma_plane_size);
				assert(m_stOutputPlaneParam.luma_plane_size == m_stOutputPlaneParam.chroma_plane_size);

				uint8_t* pucPlaneR = m_pucOutputPlaneBuf;
				uint8_t* pucPlaneG = pucPlaneR + m_stOutputPlaneParam.luma_plane_size;
				uint8_t* pucPlaneB = pucPlaneG + m_stOutputPlaneParam.luma_plane_size;

				uint8_t* pDstR = pucPlaneR;
				uint8_t* pDstG = pucPlaneG;
				uint8_t* pDstB = pucPlaneB;

				for (int row = 0; row < m_stOutputPlaneParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputPlaneParam.luma_width; col++)
					{
						int luma_col = col;
						int chroma_col = col;

						if (m_decimate_coeff > 0)
						{
							luma_col /= m_decimate_coeff;
							chroma_col /= m_decimate_coeff;
						}
						else if (m_decimate_coeff < 0)
						{
							luma_col *= -m_decimate_coeff;
							chroma_col *= -m_decimate_coeff;
						}

						PICTURE_yuv2rgb(m_stOutputPlaneParam.nColorSpace, pSrcY[luma_col], pSrcU[chroma_col], pSrcV[chroma_col], &R, &G, &B);

						pDstR[col] = R;
						pDstG[col] = G;
						pDstB[col] = B;
					}
					pDstR += m_stOutputPlaneParam.luma_width;
					pDstG += m_stOutputPlaneParam.luma_width;
					pDstB += m_stOutputPlaneParam.luma_width;

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stYUVSequenceParam.luma_width;
						pSrcU += m_stYUVSequenceParam.chroma_width;
						pSrcV += m_stYUVSequenceParam.chroma_width;
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stYUVSequenceParam.luma_width;
							pSrcU += m_stYUVSequenceParam.chroma_width;
							pSrcV += m_stYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stYUVSequenceParam.luma_width * m_decimate_coeff);
						pSrcU += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
						pSrcV += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
					}
				}
			}
			else
			{
				uint8_t* pucPlaneY = m_pucOutputPlaneBuf;
				uint8_t* pucPlaneU = pucPlaneY + m_stOutputPlaneParam.luma_plane_size;
				uint8_t* pucPlaneV = pucPlaneU + m_stOutputPlaneParam.chroma_plane_size;

				if (m_decimate_coeff == 0)
				{
					memcpy(pucPlaneY, pucFrameY, m_stYUVSequenceParam.luma_plane_size);
					memcpy(pucPlaneU, pucFrameU, m_stYUVSequenceParam.chroma_plane_size);
					memcpy(pucPlaneV, pucFrameV, m_stYUVSequenceParam.chroma_plane_size);
				}
				else if (m_decimate_coeff > 0)
				{
					PICTURE_Enlarge(pucFrameY, m_stYUVSequenceParam.luma_width, m_stYUVSequenceParam.luma_height,
						pucPlaneY, m_stOutputPlaneParam.luma_width, m_stOutputPlaneParam.luma_height, m_decimate_coeff);

					PICTURE_Enlarge(pucFrameU, m_stYUVSequenceParam.chroma_width, m_stYUVSequenceParam.chroma_height,
						pucPlaneU, m_stOutputPlaneParam.chroma_width, m_stOutputPlaneParam.chroma_height, m_decimate_coeff);

					PICTURE_Enlarge(pucFrameV, m_stYUVSequenceParam.chroma_width, m_stYUVSequenceParam.chroma_height,
						pucPlaneV, m_stOutputPlaneParam.chroma_width, m_stOutputPlaneParam.chroma_height, m_decimate_coeff);
				}
				else if (m_decimate_coeff < 0)
				{
					PICTURE_Reduce(pucFrameY, m_stYUVSequenceParam.luma_width, m_stYUVSequenceParam.luma_height,
						pucPlaneY, m_stOutputPlaneParam.luma_width, m_stOutputPlaneParam.luma_height, -m_decimate_coeff);

					PICTURE_Reduce(pucFrameU, m_stYUVSequenceParam.chroma_width, m_stYUVSequenceParam.chroma_height,
						pucPlaneU, m_stOutputPlaneParam.chroma_width, m_stOutputPlaneParam.chroma_height, -m_decimate_coeff);

					PICTURE_Reduce(pucFrameV, m_stYUVSequenceParam.chroma_width, m_stYUVSequenceParam.chroma_height,
						pucPlaneV, m_stOutputPlaneParam.chroma_width, m_stOutputPlaneParam.chroma_height, -m_decimate_coeff);
				}
			}
#else
			uint8_t* pucPlaneR = m_pucOutputPlaneBuf;
			uint8_t* pucPlaneG = pucPlaneR + m_stOutputPlaneParam.luma_plane_size;
			uint8_t* pucPlaneB = pucPlaneG + m_stOutputPlaneParam.luma_plane_size;

			uint8_t* pSrcY = pucFrameY;
			uint8_t* pSrcU = pucFrameU;
			uint8_t* pSrcV = pucFrameV;
			uint8_t* pDstR = pucPlaneR;
			uint8_t* pDstG = pucPlaneG;
			uint8_t* pDstB = pucPlaneB;

			if (m_stYUVSequenceParam.dwFourCC == 0x56555941)			//ARGB	4:4:4
			{
				assert(m_stYUVSequenceParam.luma_plane_size == m_stYUVSequenceParam.chroma_plane_size);
				assert(m_stOutputPlaneParam.luma_plane_size == m_stOutputPlaneParam.chroma_plane_size);

				for (int row = 0; row < m_stOutputPlaneParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputPlaneParam.luma_width; col++)
					{
						int luma_col = col;
						int chroma_col = col;

						if (m_decimate_coeff > 0)
						{
							luma_col /= m_decimate_coeff;
							chroma_col /= m_decimate_coeff;
						}
						else if (m_decimate_coeff < 0)
						{
							luma_col *= -m_decimate_coeff;
							chroma_col *= -m_decimate_coeff;
						}

						Y = pSrcY[luma_col];
						U = pSrcU[chroma_col];
						V = pSrcV[chroma_col];
						PICTURE_yuv2rgb(m_stOutputPlaneParam.nColorSpace, Y, U, V, &R, &G, &B);

						pDstR[col] = R;
						pDstG[col] = G;
						pDstB[col] = B;
					}
					pDstR += m_stOutputPlaneParam.luma_width;
					pDstG += m_stOutputPlaneParam.luma_width;
					pDstB += m_stOutputPlaneParam.luma_width;

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stYUVSequenceParam.luma_width;
						pSrcU += m_stYUVSequenceParam.chroma_width;
						pSrcV += m_stYUVSequenceParam.chroma_width;
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stYUVSequenceParam.luma_width;
							pSrcU += m_stYUVSequenceParam.chroma_width;
							pSrcV += m_stYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stYUVSequenceParam.luma_width * m_decimate_coeff);
						pSrcU += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
						pSrcV += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
					}
				}
			}
			else if (m_stYUVSequenceParam.dwFourCC == 0x32595559)		//YUY2  4:2:2
			{
				for (int row = 0; row < m_stOutputPlaneParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputPlaneParam.luma_width; col++)
					{
						int luma_col = col;
						int chroma_col = col / 2;

						if (m_decimate_coeff > 0)
						{
							luma_col /= m_decimate_coeff;
							chroma_col /= m_decimate_coeff;
						}
						else if (m_decimate_coeff < 0)
						{
							luma_col *= -m_decimate_coeff;
							chroma_col *= -m_decimate_coeff;
						}

						Y = pSrcY[luma_col];
						U = pSrcU[chroma_col];
						V = pSrcV[chroma_col];
						PICTURE_yuv2rgb(m_stOutputPlaneParam.nColorSpace, Y, U, V, &R, &G, &B);

						pDstR[col] = R;
						pDstG[col] = G;
						pDstB[col] = B;
					}
					pDstR += m_stOutputPlaneParam.luma_width;
					pDstG += m_stOutputPlaneParam.luma_width;
					pDstB += m_stOutputPlaneParam.luma_width;

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stYUVSequenceParam.luma_width;
						pSrcU += m_stYUVSequenceParam.chroma_width;
						pSrcV += m_stYUVSequenceParam.chroma_width;
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stYUVSequenceParam.luma_width;
							pSrcU += m_stYUVSequenceParam.chroma_width;
							pSrcV += m_stYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stYUVSequenceParam.luma_width * m_decimate_coeff);
						pSrcU += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
						pSrcV += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
					}
				}
			}
			else if (m_stYUVSequenceParam.dwFourCC == 0x56555949)		//IYUV   4:2:0
			{
				for (int row = 0; row < m_stOutputPlaneParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputPlaneParam.luma_width; col++)
					{
						int luma_col = col;
						int chroma_col = col / 2;

						if (m_decimate_coeff > 0)
						{
							luma_col /= m_decimate_coeff;
							chroma_col /= m_decimate_coeff;
						}
						else if (m_decimate_coeff < 0)
						{
							luma_col *= -m_decimate_coeff;
							chroma_col *= -m_decimate_coeff;
						}

						Y = pSrcY[luma_col];
						U = pSrcU[chroma_col];
						V = pSrcV[chroma_col];
						PICTURE_yuv2rgb(m_stOutputPlaneParam.nColorSpace, Y, U, V, &R, &G, &B);

						pDstR[col] = R;
						pDstG[col] = G;
						pDstB[col] = B;
					}
					pDstR += m_stOutputPlaneParam.luma_width;
					pDstG += m_stOutputPlaneParam.luma_width;
					pDstB += m_stOutputPlaneParam.luma_width;

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stYUVSequenceParam.luma_width;
						if (row % 2 == 1)
						{
							pSrcU += m_stYUVSequenceParam.chroma_width;
							pSrcV += m_stYUVSequenceParam.chroma_width;
						}
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stYUVSequenceParam.luma_width;
						}
						int chroma_decimate_coeff = m_decimate_coeff * 2;
						if ((row % chroma_decimate_coeff) == (chroma_decimate_coeff - 1))
						{
							pSrcU += m_stYUVSequenceParam.chroma_width;
							pSrcV += m_stYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stYUVSequenceParam.luma_width * m_decimate_coeff);
						if (row % 2 == 1)
						{
							pSrcU += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
							pSrcV += -(m_stYUVSequenceParam.chroma_width * m_decimate_coeff);
						}
					}

				}
			}
#endif

#if USE_FRAMEBUF_ACCESS_MUTEX
			::ReleaseMutex(m_hSourceFrameBufAccess);
		}
#endif
		::SetEvent(m_hSourceFrameBufEmptyEvent);

		if (m_decimate_coeff == 0)
		{
			if (m_callback_report_yuv_luma_stats != NULL)
			{
				m_callback_report_yuv_luma_stats(m_hwnd_for_caller, (WPARAM)m_pucOutputPlaneBuf, (LPARAM)m_nOutputPlaneSize);
			}

			if (m_callback_report_yuv_chroma_stats != NULL)
			{
				m_callback_report_yuv_chroma_stats(m_hwnd_for_caller, (WPARAM)m_pucOutputPlaneBuf, (LPARAM)m_nOutputPlaneSize);
			}
		}

		//FeedToDirectDraw(m_pucOutputFrameBuf, m_nOutputFrameSize, &m_stOutputFrameParams);
		HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(m_pucOutputPlaneBuf, m_nOutputPlaneSize);

#if USE_FRAMEBUF_ACCESS_MUTEX
		::ReleaseMutex(m_hOutputFrameBufAccess);
	}
#endif
	
	//FeedToDirectDraw();

	return rtcode;
}

//int CVESDecoder::FeedToDirectDraw(const LPBYTE lpRGBFrameBuf, int frameSize)
//{
//	int rtcode = ESDECODER_UNKNOWN_ERROR;
//	assert(m_pDirectDraw != NULL);
//
////#if USE_FRAMEBUF_ACCESS_MUTEX
////	uint32_t wait_state = ::WaitForSingleObject(m_hOutputFrameBufAccess, INFINITE);
////	if (wait_state == WAIT_OBJECT_0)
////	{
////#endif
////		//FRAME_PARAMS_t stFrameParams;
////
////		//stFrameParams.luma_width = m_VidDecodeInfo.source_luma_width;
////		//stFrameParams.luma_height = m_VidDecodeInfo.source_luma_height;
////		//stFrameParams.chroma_width = m_VidDecodeInfo.source_chroma_width;
////		//stFrameParams.chroma_height = m_VidDecodeInfo.source_chroma_height;
////
////		//HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(m_pucOutputFrameBuf, m_VidDecodeInfo.frame_buf_size, &stFrameParams);
////#if USE_FRAMEBUF_ACCESS_MUTEX
////		::ReleaseMutex(m_hOutputFrameBufAccess);
////	}
////#endif
//
//	if ((m_pDirectDraw != NULL) && (lpRGBFrameBuf != NULL))
//	{
//		HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(lpRGBFrameBuf, frameSize);
//	}
//
//	return rtcode;
//}

int CVESDecoder::DirectDraw_RePaint(void)
{
	assert(m_pDirectDraw != NULL);

	HRESULT	ddRval = m_pDirectDraw->RenderOnPrimarySurface();
	
	return ddRval;
}

//void CVESDecoder::GetDecodeInfo(Video_decode_info_t* pparam)
//{
//	if (pparam != NULL)
//	{
//		memcpy(pparam, &m_VidDecodeInfo, sizeof(Video_decode_info_t));
//	}
//}

double CVESDecoder::GetDisplayFrameRate(void)
{
	return m_stYUVSequenceParam.framerate;
}


void CVESDecoder::ToggleGrid(void)
{
	assert(m_pDirectDraw != NULL);

	m_pDirectDraw->ToggleGrid();

	return;
}

void CVESDecoder::SaveSnapshot(const char* dstfilename)
{
}

int CVESDecoder::CanvasEnlarge(void)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	int display_decimate_coeff = m_decimate_coeff;

	display_decimate_coeff += 2;
	if (display_decimate_coeff > 4)
	{
		display_decimate_coeff = -4;
	}

	rtcode = CanvasSetup(display_decimate_coeff);

	return rtcode;
}

int CVESDecoder::CanvasReduce(void)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	int display_decimate_coeff = m_decimate_coeff;

	display_decimate_coeff -= 2;
	if (display_decimate_coeff < -4)
	{
		display_decimate_coeff = 4;
	}

	rtcode = CanvasSetup(display_decimate_coeff);

	return rtcode;
}

int CVESDecoder::CanvasSetup(int display_decimate_coeff)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;
	assert(m_pDirectDraw != NULL);

#if USE_FRAMEBUF_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hOutputFrameBufAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		m_decimate_coeff = display_decimate_coeff;

		if (display_decimate_coeff == 0)
		{
			m_stOutputPlaneParam.luma_width = m_stYUVSequenceParam.luma_width;
			m_stOutputPlaneParam.luma_height = m_stYUVSequenceParam.luma_height;
#if RENDER_IN_AUTO_YUV_MODE
			m_stOutputPlaneParam.chroma_width = m_stYUVSequenceParam.chroma_width;
			m_stOutputPlaneParam.chroma_height = m_stYUVSequenceParam.chroma_height;
			m_stOutputPlaneParam.chroma_width = m_stYUVSequenceParam.chroma_width;
			m_stOutputPlaneParam.chroma_height = m_stYUVSequenceParam.chroma_height;
#else
			m_stOutputPlaneParam.chroma_width = m_stOutputPlaneParam.luma_width;
			m_stOutputPlaneParam.chroma_height = m_stOutputPlaneParam.luma_height;
			m_stOutputPlaneParam.chroma_width = m_stOutputPlaneParam.luma_width;
			m_stOutputPlaneParam.chroma_height = m_stOutputPlaneParam.luma_height;
#endif
		}
		else if (display_decimate_coeff > 0)
		{
			m_stOutputPlaneParam.luma_width = m_stYUVSequenceParam.luma_width * display_decimate_coeff;
			m_stOutputPlaneParam.luma_height = m_stYUVSequenceParam.luma_height * display_decimate_coeff;
#if RENDER_IN_AUTO_YUV_MODE
			m_stOutputPlaneParam.chroma_width = m_stYUVSequenceParam.chroma_width * display_decimate_coeff;
			m_stOutputPlaneParam.chroma_height = m_stYUVSequenceParam.chroma_height * display_decimate_coeff;
			m_stOutputPlaneParam.chroma_width = m_stYUVSequenceParam.chroma_width * display_decimate_coeff;
			m_stOutputPlaneParam.chroma_height = m_stYUVSequenceParam.chroma_height * display_decimate_coeff;
#else
			m_stOutputPlaneParam.chroma_width = m_stOutputPlaneParam.luma_width;
			m_stOutputPlaneParam.chroma_height = m_stOutputPlaneParam.luma_height;
			m_stOutputPlaneParam.chroma_width = m_stOutputPlaneParam.luma_width;
			m_stOutputPlaneParam.chroma_height = m_stOutputPlaneParam.luma_height;
#endif
		}
		else if (display_decimate_coeff < 0)
		{
			m_stOutputPlaneParam.luma_width = -m_stYUVSequenceParam.luma_width / display_decimate_coeff;
			m_stOutputPlaneParam.luma_height = -m_stYUVSequenceParam.luma_height / display_decimate_coeff;
#if RENDER_IN_AUTO_YUV_MODE
			m_stOutputPlaneParam.chroma_width = -m_stYUVSequenceParam.chroma_width / display_decimate_coeff;
			m_stOutputPlaneParam.chroma_height = -m_stYUVSequenceParam.chroma_height / display_decimate_coeff;
			m_stOutputPlaneParam.chroma_width = -m_stYUVSequenceParam.chroma_width / display_decimate_coeff;
			m_stOutputPlaneParam.chroma_height = -m_stYUVSequenceParam.chroma_height / display_decimate_coeff;
#else
			m_stOutputPlaneParam.chroma_width = m_stOutputPlaneParam.luma_width;
			m_stOutputPlaneParam.chroma_height = m_stOutputPlaneParam.luma_height;
			m_stOutputPlaneParam.chroma_width = m_stOutputPlaneParam.luma_width;
			m_stOutputPlaneParam.chroma_height = m_stOutputPlaneParam.luma_height;
#endif
		}

		m_pDirectDraw->CloseVideo();
		m_pDirectDraw->OpenVideo(m_hwnd_for_caller, m_stOutputPlaneParam.luma_width, m_stOutputPlaneParam.luma_height, m_stOutputPlaneParam.dwFourCC);

		if (m_pucOutputPlaneBuf != NULL)
		{
			free(m_pucOutputPlaneBuf);

			m_stOutputPlaneParam.luma_plane_size = m_stOutputPlaneParam.luma_width * m_stOutputPlaneParam.luma_height;
			m_stOutputPlaneParam.chroma_plane_size = m_stOutputPlaneParam.chroma_width * m_stOutputPlaneParam.chroma_height;

			m_nOutputPlaneSize = m_stOutputPlaneParam.luma_plane_size + 
				m_stOutputPlaneParam.chroma_plane_size + m_stOutputPlaneParam.chroma_plane_size;			//RGB 3 plane

			m_pucOutputPlaneBuf = (uint8_t*)malloc(m_nOutputPlaneSize);			//RGB 3 plane
			memset(m_pucOutputPlaneBuf, 0x00, m_nOutputPlaneSize);
		}

#if USE_FRAMEBUF_ACCESS_MUTEX
		::ReleaseMutex(m_hOutputFrameBufAccess);
	}
#endif

	rtcode = FrameProcessAndFeedToDirectDraw();

	return rtcode;
}

int CVESDecoder::GetCanvasWH(int* pnwidth, int* pnheight)
{
	int rtcode = ESDECODER_NO_ERROR;

	if ((pnwidth != NULL) && (pnheight != NULL))
	{
		*pnwidth = m_stOutputPlaneParam.luma_width;
		*pnheight = m_stOutputPlaneParam.luma_height;
	}
	else
	{
		rtcode = ESDECODER_PARAMETER_ERROR;
	}

	return rtcode;
}

//void CVESDecoder::ToggleView(void)
//{
//	assert(m_pDirectDraw != NULL);
//
//	m_pDirectDraw->ToggleView();
//}


uint32_t thread_frame_process(LPVOID lpParam)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

#if USE_FRAMERATE_CONTROLL
	DWORD			dwStartTick = 0;
	int				frameCount = 0;
#endif

	CVESDecoder* pDecoder = (CVESDecoder*)lpParam;

	if (pDecoder != NULL)
	{
#if USE_FRAMERATE_CONTROLL
		double frame_rate = pDecoder->GetDisplayFrameRate();
		double frame_interval = 1000.0 / frame_rate;
#endif

		pDecoder->m_bFrameProcessResponseStatus = 1;
		while (pDecoder->m_bFrameProcessControllStatus)
		{
			uint32_t wait_state0 = ::WaitForSingleObject(pDecoder->m_hSourceFrameBufFullEvent, INFINITE);
			if (wait_state0 == WAIT_OBJECT_0)
			{
				::ResetEvent(pDecoder->m_hSourceFrameBufFullEvent);

				if (pDecoder->m_bFrameProcessControllStatus)
				{
#if USE_FRAMERATE_CONTROLL
					if (frameCount == 0)
					{
						dwStartTick = ::GetTickCount();
					}
#endif

					pDecoder->FrameProcessAndFeedToDirectDraw();

#if USE_FRAMERATE_CONTROLL
					frameCount++;

					int timeThreadHold = (int)round(frameCount * frame_interval);

					do
					{
						DWORD dwNowTick = ::GetTickCount();
						int timeElapse = dwNowTick - dwStartTick;
						if (timeElapse >= timeThreadHold)
						{
							if (frameCount == 1000000)
							{
								frameCount = 100;
								dwStartTick = dwNowTick - (int)round(frameCount * frame_interval);
							}
							break;
						}
					} while (pDecoder->m_bFrameProcessControllStatus == 1);
#endif
				}
			}
		}
		pDecoder->m_bFrameProcessResponseStatus = 0;

		rtcode = ESDECODER_NO_ERROR;
	}

	return rtcode;
}

int PICTURE_Enlarge(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int decimate_coeff)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	if ((src != NULL) && (dst != NULL))
	{
		uint8_t* pSrcRowStart = src;
		uint8_t* pDstRowStart = dst;
		for (int src_row = 0; src_row < src_h; src_row++)
		{
			for (int j = 0; j < decimate_coeff; j++)
			{
				int dst_col_base = 0;
				for (int src_col = 0; src_col < src_w; src_col++)
				{
					for (int i = 0; i < decimate_coeff; i++)
					{
						pDstRowStart[dst_col_base + i] = pSrcRowStart[src_col];
					}
					dst_col_base += decimate_coeff;
				}

				pDstRowStart += dst_w;
			}

			pSrcRowStart += src_w;
		}
	}

	return rtcode;
}

int PICTURE_Reduce(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int decimate_coeff)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	if ((src != NULL) && (dst != NULL))
	{
		uint8_t* pSrcRowStart = src;
		uint8_t* pDstRowStart = dst;
		for (int src_row = 0; src_row < src_h; src_row+=decimate_coeff)
		{
			int dst_col = 0;
			for (int src_col = 0; src_col < src_w; src_col+= decimate_coeff)
			{
				pDstRowStart[dst_col] = pSrcRowStart[src_col];
				dst_col ++;
			}

			pDstRowStart += dst_w;
			pSrcRowStart += (src_w * decimate_coeff);
		}
	}

	return rtcode;
}


double PICTURE_psnr(uint8_t* reference, uint8_t* working, int size)
{
	unsigned char*  pOrg = reference;
	unsigned char*  pRec = working;
	double          ssd = 0;
	int             diff;
	double			psnr = 99.99;

	for (int i = 0; i < size; i++)
	{
		diff = pRec[i] - pOrg[i];
		ssd += (double)(diff * diff);
	}

	if (ssd > 0)
	{
		psnr = (10.0 * log10(size * 65025.0 / ssd));			//255 * 255 = 65025
	}

	return psnr;
}

MW_ES_LIB int PICTURE_yuv2rgb(int colorSpace, uint8_t Y, uint8_t U, uint8_t V, uint8_t* R, uint8_t* G, uint8_t* B)
{
	if (colorSpace == 601)
	{
		int C = Y - 16;
		int D = U - 128;
		int E = V - 128;
		int nR = ((298 * C + 409 * E + 128) >> 8);
		int nG = ((298 * C - 100 * D - 208 * E + 128) >> 8);
		int nB = ((298 * C + 516 * D + 128) >> 8);

		*R = (uint8_t)clip3(0, 255, nR);
		*G = (uint8_t)clip3(0, 255, nG);
		*B = (uint8_t)clip3(0, 255, nB);
	}
	else if (colorSpace == 709)
	{
		int C = Y - 16;
		int D = U - 128;
		int E = V - 128;
		int nR = ((298 * C + 409 * E + 128) >> 8);
		int nG = ((298 * C - 100 * D - 208 * E + 128) >> 8);
		int nB = ((298 * C + 516 * D + 128) >> 8);

		*R = (uint8_t)clip3(0, 255, nR);
		*G = (uint8_t)clip3(0, 255, nG);
		*B = (uint8_t)clip3(0, 255, nB);
	}
	else if (colorSpace == 2020)
	{
		int C = Y - 16;
		int D = U - 128;
		int E = V - 128;
		int nR = ((298 * C + 409 * E + 128) >> 8);
		int nG = ((298 * C - 100 * D - 208 * E + 128) >> 8);
		int nB = ((298 * C + 516 * D + 128) >> 8);

		*R = (uint8_t)clip3(0, 255, nR);
		*G = (uint8_t)clip3(0, 255, nG);
		*B = (uint8_t)clip3(0, 255, nB);
	}

	return 0;
}

