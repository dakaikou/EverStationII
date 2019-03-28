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
#include "Utilities/Graphics/Include/Graphics.h"

#include "../Include/common/video_common.h"
#include "../Include/VESDecoder.h"
#include "../Include/ESDecoder_ErrorCode.h"

CVESDecoder::CVESDecoder(void)
{
	memset(&m_stInputYUVSequenceParam, 0x00, sizeof(INPUT_YUV_SEQUENCE_PARAM_t));
	m_pucInputYUVFrameBuf = NULL;
	m_nInputYUVFrameSize = 0;

	memset(&m_stOutputYUVSequenceParam, 0x00, sizeof(OUTPUT_YUV_SEQUENCE_PARAM_t));
	m_pucOutputYUVFrameBuf = NULL;
	m_nOutputYUVFrameSize = 0;

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

	if (m_pucInputYUVFrameBuf != NULL)
	{
		free(m_pucInputYUVFrameBuf);
		m_pucInputYUVFrameBuf = NULL;
	}
	m_nInputYUVFrameSize = 0;
	//m_bSourceDataAvailable = 0;

	//assert(m_bFrameProcessResponseStatus == 0);

	//memset(&m_stRGBSequenceParam, 0x00, sizeof(RGB_SEQUENCE_PARAM_t));

	if (m_pucOutputYUVFrameBuf != NULL)
	{
		free(m_pucOutputYUVFrameBuf);
		m_pucOutputYUVFrameBuf = NULL;
	}
	m_nOutputYUVFrameSize = 0;

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
	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_stOutputYUVSequenceParam.luma_width, m_stOutputYUVSequenceParam.luma_height, m_stOutputYUVSequenceParam.dwFourCC);

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

#if USE_FRAMEBUF_ACCESS_MUTEX
		uint32_t wait_state = ::WaitForSingleObject(m_hSourceFrameBufAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
#endif

#if RENDER_IN_AUTO_YUV_MODE

			if (m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('A', 'Y', 'U', 'V'))			//AYUV	4:4:4, Packed Mode, FourCC code = 0x56555941
			{
				if (
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '4', 'B')) ||			//YUV	4:4:4, Planar Mode
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('4', '4', '4', 'P'))				//YVU	4:4:4, Planar Mode
					)
				{
					uint8_t R, G, B;
					uint8_t Y, U, V;
					uint8_t *pSrcY, *pSrcU, *pSrcV;

					if (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '4', 'B'))		//YUV 4:4:4 Planar
					{
						pSrcY = m_pucInputYUVFrameBuf;
						pSrcU = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
						pSrcV = pSrcU + m_stInputYUVSequenceParam.chroma_plane_size;
					}
					else  //YVU 4:4:4 Planar   444P
					{
						pSrcY = m_pucInputYUVFrameBuf;
						pSrcV = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
						pSrcU = pSrcV + m_stInputYUVSequenceParam.chroma_plane_size;
					}

					assert(m_stInputYUVSequenceParam.luma_plane_size == m_stInputYUVSequenceParam.chroma_plane_size);
					assert(m_stOutputYUVSequenceParam.luma_plane_size == m_stOutputYUVSequenceParam.chroma_plane_size);
					assert(m_stOutputYUVSequenceParam.luma_plane_size == m_stOutputYUVSequenceParam.alpha_plane_size);

					uint8_t* pDstLine = m_pucOutputYUVFrameBuf;

					for (int dstRow = 0; dstRow < m_stOutputYUVSequenceParam.luma_height; dstRow++)
					{
						for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.luma_width; dstCol++)
						{
							int srcLumaCol = dstCol;
							int srcChroma = dstCol;

							if (m_decimate_coeff > 0)
							{
								srcLumaCol /= m_decimate_coeff;
								srcChroma /= m_decimate_coeff;
							}
							else if (m_decimate_coeff < 0)
							{
								srcLumaCol *= -m_decimate_coeff;
								srcChroma *= -m_decimate_coeff;
							}

							Y = pSrcY[srcLumaCol];
							U = pSrcU[srcChroma];
							V = pSrcV[srcChroma];
#if 1
							GRAPHICS_yuv2rgb(m_stOutputYUVSequenceParam.nColorSpace, Y, U, V, &R, &G, &B);
#else
							R = Y;
							G = U;
							B = V;
#endif
							int colOffset = (dstCol << 2);
							pDstLine[colOffset + 0] = R;
							pDstLine[colOffset + 1] = G;
							pDstLine[colOffset + 2] = B;
							pDstLine[colOffset + 3] = 0;
						}
						pDstLine += (m_stOutputYUVSequenceParam.luma_width << 2);

						if (m_decimate_coeff == 0)
						{
							pSrcY += m_stInputYUVSequenceParam.luma_width;
							pSrcU += m_stInputYUVSequenceParam.chroma_width;
							pSrcV += m_stInputYUVSequenceParam.chroma_width;
						}
						else if (m_decimate_coeff > 0)
						{
							if ((dstRow % m_decimate_coeff) == (m_decimate_coeff - 1))
							{
								pSrcY += m_stInputYUVSequenceParam.luma_width;
								pSrcU += m_stInputYUVSequenceParam.chroma_width;
								pSrcV += m_stInputYUVSequenceParam.chroma_width;
							}
						}
						else
						{
							pSrcY += -(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff);
							pSrcU += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
							pSrcV += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
						}
					}
				}
				else
				{
					assert(0);
				}
			}
			else if (m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', 'U', 'Y', '2'))			//YUY2	4:2:2, Packed Mode, FourCC code = 0x32595559
			{
				if (
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '2', 'B')) ||			//YUV	4:2:2, Planar Mode
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('4', '2', '2', 'P'))				//YVU	4:2:2, Planar Mode
					)
				{
					uint8_t *pSrcY, *pSrcU, *pSrcV;

					if (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '2', 'B'))		//YUV 4:2:2 Planar
					{
						pSrcY = m_pucInputYUVFrameBuf;
						pSrcU = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
						pSrcV = pSrcU + m_stInputYUVSequenceParam.chroma_plane_size;
					}
					else  //YVU 4:2:2 Planar   422P
					{
						pSrcY = m_pucInputYUVFrameBuf;
						pSrcV = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
						pSrcU = pSrcV + m_stInputYUVSequenceParam.chroma_plane_size;
					}

					uint8_t* pDstLine = m_pucOutputYUVFrameBuf;

					for (int dstRow = 0; dstRow < m_stOutputYUVSequenceParam.luma_height; dstRow++)
					{
						for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.luma_width; dstCol++)
						{
							int srcLumaCol = dstCol;
							int srcChormaCol = dstCol / 2;

							if (m_decimate_coeff > 0)
							{
								srcLumaCol /= m_decimate_coeff;
								srcChormaCol /= m_decimate_coeff;
							}
							else if (m_decimate_coeff < 0)
							{
								srcLumaCol *= -m_decimate_coeff;
								srcChormaCol *= -m_decimate_coeff;
							}

							pDstLine[2 * dstCol + 0] = pSrcY[srcLumaCol];

							if (dstCol % 2 == 0)
							{
								pDstLine[2 * dstCol + 1] = pSrcU[srcChormaCol];
							}
							else
							{
								pDstLine[2 * dstCol + 1] = pSrcV[srcChormaCol];
							}
						}

						pDstLine += (m_stOutputYUVSequenceParam.luma_width << 1);

						if (m_decimate_coeff == 0)
						{
							pSrcY += m_stInputYUVSequenceParam.luma_width;
							pSrcU += m_stInputYUVSequenceParam.chroma_width;
							pSrcV += m_stInputYUVSequenceParam.chroma_width;
						}
						else if (m_decimate_coeff > 0)
						{
							if ((dstRow % m_decimate_coeff) == (m_decimate_coeff - 1))
							{
								pSrcY += m_stInputYUVSequenceParam.luma_width;
								pSrcU += m_stInputYUVSequenceParam.chroma_width;
								pSrcV += m_stInputYUVSequenceParam.chroma_width;
							}
						}
						else
						{
							pSrcY += -(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff);
							pSrcU += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
							pSrcV += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
						}
					}
				}
				else
				{
					assert(0);
				}
			}
			else if (m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('U', 'Y', 'V', 'Y'))			//UYVY	4:2:2, Packed Mode, FourCC code = 0x59565955
			{
				if (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('U', 'Y', 'V', 'Y'))
				{
					uint8_t* pDstLine = m_pucOutputYUVFrameBuf;
					uint8_t* pSrcLine = m_pucInputYUVFrameBuf;

					for (int dstRow = 0; dstRow < m_stOutputYUVSequenceParam.luma_height; dstRow++)
					{
						for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.luma_width; dstCol++)
						{
							int srcLumaCol = dstCol;

							if (m_decimate_coeff > 0)
							{
								srcLumaCol /= m_decimate_coeff;
							}
							else if (m_decimate_coeff < 0)
							{
								srcLumaCol *= -m_decimate_coeff;
							}

							int srcBufOffset = (srcLumaCol << 1);
							int dstBufOffset = (dstCol << 1);

							pDstLine[dstBufOffset + 1] = pSrcLine[srcBufOffset + 1];
						}
						for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.luma_width; dstCol += 2)
						{
							int srcChromaCol = dstCol/2;

							if (m_decimate_coeff > 0)
							{
								srcChromaCol /= m_decimate_coeff;
							}
							else if (m_decimate_coeff < 0)
							{
								srcChromaCol *= -m_decimate_coeff;
							}

							int srcUBufOffset = (((srcChromaCol << 1) + 0) << 1);
							int dstBufOffset = (dstCol << 1);
							pDstLine[dstBufOffset + 0] = pSrcLine[srcUBufOffset + 0];

							int srcVBufOffset = (((srcChromaCol << 1) + 1) << 1);
							dstBufOffset = ((dstCol + 1) << 1);

							pDstLine[dstBufOffset + 0] = pSrcLine[srcVBufOffset + 0];
						}

						pDstLine += (m_stOutputYUVSequenceParam.luma_width << 1);

						if (m_decimate_coeff == 0)
						{
							pSrcLine += (m_stInputYUVSequenceParam.luma_width << 1);
						}
						else if (m_decimate_coeff > 0)
						{
							if ((dstRow % m_decimate_coeff) == (m_decimate_coeff - 1))
							{
								pSrcLine += (m_stInputYUVSequenceParam.luma_width << 1);
							}
						}
						else
						{
							pSrcLine += (-(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff) << 1);
						}
					}
				}
				else
				{
					assert(0);
				}
			}
			else if (
				(m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', 'V', '1', '2')) ||			//YUY2	4:2:0, Planar Mode, FourCC code = 0x32315659
				(m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', '4', '2', '0')) ||			//I420	4:2:0, Planar Mode, FourCC code = 0x30323449
				(m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', 'Y', 'U', 'V'))				//IYUV	4:2:0, Planar Mode, FourCC code = 0x56555949
				)
			{
				if (
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', 'V', '1', '2')) ||			//YUY2	4:2:0, Planar Mode, FourCC code = 0x32315659
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', '4', '2', '0')) ||			//I420	4:2:0, Planar Mode, FourCC code = 0x30323449
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', 'Y', 'U', 'V'))				//IYUV	4:2:0, Planar Mode, FourCC code = 0x56555949
					)
				{
					uint8_t* pSrcLuma = m_pucInputYUVFrameBuf;
					uint8_t* pSrcChroma = pSrcLuma + m_stInputYUVSequenceParam.luma_plane_size;

					uint8_t* pDstLuma = m_pucOutputYUVFrameBuf;
					uint8_t* pDstChroma = pDstLuma + m_stOutputYUVSequenceParam.luma_plane_size;

					for (int dstRow = 0; dstRow < m_stOutputYUVSequenceParam.luma_height; dstRow++)
					{
						for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.luma_width; dstCol++)
						{
							int srcLumaCol = dstCol;

							if (m_decimate_coeff > 0)
							{
								srcLumaCol /= m_decimate_coeff;
							}
							else if (m_decimate_coeff < 0)
							{
								srcLumaCol *= -m_decimate_coeff;
							}

							pDstLuma[dstCol] = pSrcLuma[srcLumaCol];
						}
						for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.chroma_width; dstCol++)
						{
							int chromaCol = dstCol;

							if (m_decimate_coeff > 0)
							{
								chromaCol /= m_decimate_coeff;
							}
							else if (m_decimate_coeff < 0)
							{
								chromaCol *= -m_decimate_coeff;
							}

							pDstChroma[dstCol] = pSrcChroma[chromaCol];
						}

						pDstLuma += m_stOutputYUVSequenceParam.luma_width;
						pDstChroma += m_stOutputYUVSequenceParam.chroma_width;

						if (m_decimate_coeff == 0)
						{
							pSrcLuma += m_stInputYUVSequenceParam.luma_width;
							pSrcChroma += m_stInputYUVSequenceParam.chroma_width;
						}
						else if (m_decimate_coeff > 0)
						{
							if ((dstRow % m_decimate_coeff) == (m_decimate_coeff - 1))
							{
								pSrcLuma += m_stInputYUVSequenceParam.luma_width;
								pSrcChroma += m_stInputYUVSequenceParam.chroma_width;
							}
						}
						else
						{
							pSrcLuma += -(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff);
							pSrcChroma += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
						}
					}
				}
				else
				{
					assert(0);
				}

				//if (m_decimate_coeff == 0)
				//{
				//	memcpy(m_pucOutputYUVFrameBuf, m_pucInputYUVFrameBuf, m_nInputYUVFrameSize);
				//}
				//else if (m_decimate_coeff > 0)
				//{
				//	PICTURE_Enlarge(pSrcLuma, m_stInputYUVSequenceParam.luma_width, m_stInputYUVSequenceParam.luma_height,
				//		pDstLuma, m_stOutputYUVSequenceParam.luma_width, m_stOutputYUVSequenceParam.luma_height, m_decimate_coeff);

				//	PICTURE_Enlarge(pSrcChroma, m_stInputYUVSequenceParam.chroma_width, (m_stInputYUVSequenceParam.chroma_height << 1),
				//		pDstChroma, m_stOutputYUVSequenceParam.chroma_width, (m_stOutputYUVSequenceParam.chroma_height << 1), m_decimate_coeff);
				//}
				//else if (m_decimate_coeff < 0)
				//{
				//	PICTURE_Reduce(pSrcLuma, m_stInputYUVSequenceParam.luma_width, m_stInputYUVSequenceParam.luma_height,
				//		pDstLuma, m_stOutputYUVSequenceParam.luma_width, m_stOutputYUVSequenceParam.luma_height, -m_decimate_coeff);

				//	PICTURE_Reduce(pSrcChroma, m_stInputYUVSequenceParam.chroma_width, (m_stInputYUVSequenceParam.chroma_height << 1),
				//		pDstChroma, m_stOutputYUVSequenceParam.chroma_width, (m_stOutputYUVSequenceParam.chroma_height << 1), -m_decimate_coeff);
				//}
			}
#else

			if ((m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '4', 'B')) ||			//Y-U-V	4:4:4 Planar
				(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('4', '4', '4', 'P'))			//Y-V-U	4:4:4 Planar
				)
			{
				//YUV(YVU) 4:4:4 planar format convert to AYUV 4:4:4:4 Packed format

				assert(m_stInputYUVSequenceParam.luma_plane_size == m_stInputYUVSequenceParam.chroma_plane_size);
				assert(m_stOutputYUVSequenceParam.luma_plane_size == m_stOutputYUVSequenceParam.chroma_plane_size);
				assert(m_stOutputYUVSequenceParam.alpha_plane_size == m_stOutputYUVSequenceParam.alpha_plane_size);

				uint8_t R, G, B;
				uint8_t Y, U, V;
				uint8_t *pSrcY, *pSrcU, *pSrcV;

				if (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '4', 'B'))		//YUV 4:4:4 Planar
				{
					pSrcY = m_pucInputYUVFrameBuf;
					pSrcU = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
					pSrcV = pSrcU + m_stInputYUVSequenceParam.chroma_plane_size;
				}
				else  //YVU 4:4:4 Planar   444P
				{
					pSrcY = m_pucInputYUVFrameBuf;
					pSrcV = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
					pSrcU = pSrcV + m_stInputYUVSequenceParam.chroma_plane_size;
				}

				uint8_t* pDstLine = m_pucOutputYUVFrameBuf;
				for (int row = 0; row < m_stOutputYUVSequenceParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputYUVSequenceParam.luma_width; col++)
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
						GRAPHICS_yuv2rgb(m_stOutputYUVSequenceParam.nColorSpace, Y, U, V, &R, &G, &B);

						int colOffset = (col << 2);
						pDstLine[colOffset + 0] = R;
						pDstLine[colOffset + 1] = G;
						pDstLine[colOffset + 2] = B;
						pDstLine[colOffset + 3] = 0;
					}
					pDstLine += (m_stOutputYUVSequenceParam.luma_width << 2);

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stInputYUVSequenceParam.luma_width;
						pSrcU += m_stInputYUVSequenceParam.chroma_width;
						pSrcV += m_stInputYUVSequenceParam.chroma_width;
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stInputYUVSequenceParam.luma_width;
							pSrcU += m_stInputYUVSequenceParam.chroma_width;
							pSrcV += m_stInputYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff);
						pSrcU += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
						pSrcV += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
					}
				}
			}
			else if ((m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '2', 'B')) ||		//YUV  4:2:2 Planar
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('4', '2', '2', 'P')))			//YVU  4:2:2 Planar
			{
				//YUV(YVU) 4:2:2 planar format convert to AYUV 4:4:4:4 Packed format

				uint8_t R, G, B;
				uint8_t Y, U, V;
				uint8_t *pSrcY, *pSrcU, *pSrcV;

				if (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '2', 'B'))		//YUV 4:2:2 Planar
				{
					pSrcY = m_pucInputYUVFrameBuf;
					pSrcU = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
					pSrcV = pSrcU + m_stInputYUVSequenceParam.chroma_plane_size;
				}
				else  //YVU 4:2:2 Planar   422P
				{
					pSrcY = m_pucInputYUVFrameBuf;
					pSrcV = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
					pSrcU = pSrcV + m_stInputYUVSequenceParam.chroma_plane_size;
				}

				uint8_t* pDstLine = m_pucOutputYUVFrameBuf;

				for (int row = 0; row < m_stOutputYUVSequenceParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputYUVSequenceParam.luma_width; col++)
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
						GRAPHICS_yuv2rgb(m_stOutputYUVSequenceParam.nColorSpace, Y, U, V, &R, &G, &B);

						int colOffset = (col << 2);
						pDstLine[colOffset + 0] = R;
						pDstLine[colOffset + 1] = G;
						pDstLine[colOffset + 2] = B;
						pDstLine[colOffset + 3] = 0;
					}
					pDstLine += (m_stOutputYUVSequenceParam.luma_width << 2);

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stInputYUVSequenceParam.luma_width;
						pSrcU += m_stInputYUVSequenceParam.chroma_width;
						pSrcV += m_stInputYUVSequenceParam.chroma_width;
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stInputYUVSequenceParam.luma_width;
							pSrcU += m_stInputYUVSequenceParam.chroma_width;
							pSrcV += m_stInputYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff);
						pSrcU += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
						pSrcV += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
					}
				}
			}
			else if ((m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', 'Y', 'U', 'V')) ||		//YUV  4:2:0 Planar
					 (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', '4', '2', '0')) ||		//YUV  4:2:0 Planar
					 (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', 'V', '1', '2'))			//YVU  4:2:0 Planar
					 )
			{
				//YUV(YVU) 4:2:0 planar format convert to AYUV 4:4:4:4 Packed format
				uint8_t R, G, B;
				uint8_t Y, U, V;
				uint8_t *pSrcY, *pSrcU, *pSrcV;

				if ((m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', 'Y', 'U', 'V')) ||		//YUV 4:2:0 Planar
					(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('I', '4', '2', '0'))		//YUV  4:2:0 Planar
					)
				{
					pSrcY = m_pucInputYUVFrameBuf;
					pSrcU = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
					pSrcV = pSrcU + m_stInputYUVSequenceParam.chroma_plane_size;
				}
				else  //YVU 4:2:0 Planar   YV12
				{
					pSrcY = m_pucInputYUVFrameBuf;
					pSrcV = pSrcY + m_stInputYUVSequenceParam.luma_plane_size;
					pSrcU = pSrcV + m_stInputYUVSequenceParam.chroma_plane_size;
				}

				uint8_t* pDstLine = m_pucOutputYUVFrameBuf;

				for (int row = 0; row < m_stOutputYUVSequenceParam.luma_height; row++)
				{
					for (int col = 0; col < m_stOutputYUVSequenceParam.luma_width; col++)
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
						GRAPHICS_yuv2rgb(m_stOutputYUVSequenceParam.nColorSpace, Y, U, V, &R, &G, &B);

						int colOffset = (col << 2);
						pDstLine[colOffset + 0] = R;
						pDstLine[colOffset + 1] = G;
						pDstLine[colOffset + 2] = B;
						pDstLine[colOffset + 3] = 0;
					}
					pDstLine += (m_stOutputYUVSequenceParam.luma_width << 2);

					if (m_decimate_coeff == 0)
					{
						pSrcY += m_stInputYUVSequenceParam.luma_width;
						if (row % 2 == 1)
						{
							pSrcU += m_stInputYUVSequenceParam.chroma_width;
							pSrcV += m_stInputYUVSequenceParam.chroma_width;
						}
					}
					else if (m_decimate_coeff > 0)
					{
						if ((row % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcY += m_stInputYUVSequenceParam.luma_width;
						}
						int chroma_decimate_coeff = m_decimate_coeff * 2;
						if ((row % chroma_decimate_coeff) == (chroma_decimate_coeff - 1))
						{
							pSrcU += m_stInputYUVSequenceParam.chroma_width;
							pSrcV += m_stInputYUVSequenceParam.chroma_width;
						}
					}
					else
					{
						pSrcY += -(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff);
						if (row % 2 == 1)
						{
							pSrcU += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
							pSrcV += -(m_stInputYUVSequenceParam.chroma_width * m_decimate_coeff);
						}
					}

				}
			}
			else if (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('U', 'Y', 'V', 'Y')) 		//UYUV  4:2:2 Packed
			{
				//YUV(YVU) 4:2:2 packed format convert to AYUV 4:4:4:4 Packed format
				uint8_t* pDstLine = m_pucOutputYUVFrameBuf;
				uint8_t* pSrcLine = m_pucInputYUVFrameBuf;

				for (int dstRow = 0; dstRow < m_stOutputYUVSequenceParam.luma_height; dstRow++)
				{
					for (int dstCol = 0; dstCol < m_stOutputYUVSequenceParam.luma_width; dstCol++)
					{
						int srcLumaCol = dstCol;
						int srcChromaCol = dstCol / 2;

						if (m_decimate_coeff > 0)
						{
							srcLumaCol /= m_decimate_coeff;
							srcChromaCol /= m_decimate_coeff;
						}
						else if (m_decimate_coeff < 0)
						{
							srcLumaCol *= -m_decimate_coeff;
							srcChromaCol *= -m_decimate_coeff;
						}

						int srcYBufOffset = (srcLumaCol << 1);
						int srcUBufOffset = (((srcChromaCol << 1) + 0) << 1);
						int srcVBufOffset = (((srcChromaCol << 1) + 1) << 1);

						uint8_t R, G, B;
						uint8_t Y, U, V;

						Y = pSrcLine[srcYBufOffset + 1];
						U = pSrcLine[srcUBufOffset + 0];
						V = pSrcLine[srcVBufOffset + 0];

						GRAPHICS_yuv2rgb(m_stOutputYUVSequenceParam.nColorSpace, Y, U, V, &R, &G, &B);

						int colOffset = (dstCol << 2);
						pDstLine[colOffset + 0] = R;
						pDstLine[colOffset + 1] = G;
						pDstLine[colOffset + 2] = B;
						pDstLine[colOffset + 3] = 0;
					}

					pDstLine += (m_stOutputYUVSequenceParam.luma_width << 2);

					if (m_decimate_coeff == 0)
					{
						pSrcLine += (m_stInputYUVSequenceParam.luma_width << 1);
					}
					else if (m_decimate_coeff > 0)
					{
						if ((dstRow % m_decimate_coeff) == (m_decimate_coeff - 1))
						{
							pSrcLine += (m_stInputYUVSequenceParam.luma_width << 1);
						}
					}
					else
					{
						pSrcLine += (-(m_stInputYUVSequenceParam.luma_width * m_decimate_coeff) << 1);
					}
				}
			}
			else
			{
				assert(0);
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
				m_callback_report_yuv_luma_stats(m_hwnd_for_caller, (WPARAM)m_pucOutputYUVFrameBuf, (LPARAM)m_nOutputYUVFrameSize);
			}

			if (m_callback_report_yuv_chroma_stats != NULL)
			{
				m_callback_report_yuv_chroma_stats(m_hwnd_for_caller, (WPARAM)m_pucOutputYUVFrameBuf, (LPARAM)m_nOutputYUVFrameSize);
			}
		}

		//FeedToDirectDraw(m_pucOutputFrameBuf, m_nOutputFrameSize, &m_stOutputFrameParams);
		HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(m_pucOutputYUVFrameBuf, m_nOutputYUVFrameSize);

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
	return m_stInputYUVSequenceParam.framerate;
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
			m_stOutputYUVSequenceParam.luma_width = m_stInputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.luma_height = m_stInputYUVSequenceParam.luma_height;
#if RENDER_IN_AUTO_YUV_MODE
			m_stOutputYUVSequenceParam.chroma_width = m_stInputYUVSequenceParam.chroma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stInputYUVSequenceParam.chroma_height;
			m_stOutputYUVSequenceParam.chroma_width = m_stInputYUVSequenceParam.chroma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stInputYUVSequenceParam.chroma_height;
#else
			m_stOutputYUVSequenceParam.chroma_width = m_stOutputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stOutputYUVSequenceParam.luma_height;
#endif
		}
		else if (display_decimate_coeff > 0)
		{
			m_stOutputYUVSequenceParam.luma_width = m_stInputYUVSequenceParam.luma_width * display_decimate_coeff;
			m_stOutputYUVSequenceParam.luma_height = m_stInputYUVSequenceParam.luma_height * display_decimate_coeff;
#if RENDER_IN_AUTO_YUV_MODE
			m_stOutputYUVSequenceParam.chroma_width = m_stInputYUVSequenceParam.chroma_width * display_decimate_coeff;
			m_stOutputYUVSequenceParam.chroma_height = m_stInputYUVSequenceParam.chroma_height * display_decimate_coeff;
			m_stOutputYUVSequenceParam.chroma_width = m_stInputYUVSequenceParam.chroma_width * display_decimate_coeff;
			m_stOutputYUVSequenceParam.chroma_height = m_stInputYUVSequenceParam.chroma_height * display_decimate_coeff;
#else
			m_stOutputYUVSequenceParam.chroma_width = m_stOutputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stOutputYUVSequenceParam.luma_height;
#endif
		}
		else if (display_decimate_coeff < 0)
		{
			m_stOutputYUVSequenceParam.luma_width = -m_stInputYUVSequenceParam.luma_width / display_decimate_coeff;
			m_stOutputYUVSequenceParam.luma_height = -m_stInputYUVSequenceParam.luma_height / display_decimate_coeff;
#if RENDER_IN_AUTO_YUV_MODE
			m_stOutputYUVSequenceParam.chroma_width = -m_stInputYUVSequenceParam.chroma_width / display_decimate_coeff;
			m_stOutputYUVSequenceParam.chroma_height = -m_stInputYUVSequenceParam.chroma_height / display_decimate_coeff;
			m_stOutputYUVSequenceParam.chroma_width = -m_stInputYUVSequenceParam.chroma_width / display_decimate_coeff;
			m_stOutputYUVSequenceParam.chroma_height = -m_stInputYUVSequenceParam.chroma_height / display_decimate_coeff;
#else
			m_stOutputYUVSequenceParam.chroma_width = m_stOutputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stOutputYUVSequenceParam.luma_height;
#endif
		}

		if (m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('A', 'Y', 'U', 'V'))
		{
			m_stOutputYUVSequenceParam.alpha_width = m_stOutputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.alpha_height = m_stOutputYUVSequenceParam.luma_height;
		}
		else
		{
			m_stOutputYUVSequenceParam.alpha_width = 0;
			m_stOutputYUVSequenceParam.alpha_height = 0;
		}

		m_pDirectDraw->CloseVideo();
		m_pDirectDraw->OpenVideo(m_hwnd_for_caller, m_stOutputYUVSequenceParam.luma_width, m_stOutputYUVSequenceParam.luma_height, m_stOutputYUVSequenceParam.dwFourCC);

		if (m_pucOutputYUVFrameBuf != NULL)
		{
			free(m_pucOutputYUVFrameBuf);

			m_stOutputYUVSequenceParam.luma_plane_size = m_stOutputYUVSequenceParam.luma_width * m_stOutputYUVSequenceParam.luma_height;
			m_stOutputYUVSequenceParam.chroma_plane_size = m_stOutputYUVSequenceParam.chroma_width * m_stOutputYUVSequenceParam.chroma_height;
			m_stOutputYUVSequenceParam.alpha_plane_size = m_stOutputYUVSequenceParam.alpha_width * m_stOutputYUVSequenceParam.alpha_height;

			m_nOutputYUVFrameSize = m_stOutputYUVSequenceParam.luma_plane_size +
				m_stOutputYUVSequenceParam.chroma_plane_size + 
				m_stOutputYUVSequenceParam.chroma_plane_size +
				m_stOutputYUVSequenceParam.alpha_plane_size;			//ARGB 4 plane

			m_pucOutputYUVFrameBuf = (uint8_t*)malloc(m_nOutputYUVFrameSize);			//RGB 3 plane
			memset(m_pucOutputYUVFrameBuf, 0x00, m_nOutputYUVFrameSize);
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
		*pnwidth = m_stOutputYUVSequenceParam.luma_width;
		*pnheight = m_stOutputYUVSequenceParam.luma_height;
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




