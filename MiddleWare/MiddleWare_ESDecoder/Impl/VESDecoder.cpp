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
	m_pucSourceFrameBuf = NULL;
	m_pucOutputFrameBuf = NULL;

	m_pDirectDraw = NULL;

	//direct draw settings
	memset(&m_VidDecodeInfo, 0x00, sizeof(VIDEO_DECODE_Params_t));

	m_VidDecodeInfo.size = sizeof(VIDEO_DECODE_Params_t);
	m_VidDecodeInfo.getparams = 0;

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

	//if (psourceInfo != NULL)
	{
		//m_VidDecodeInfo.size = sizeof(VIDEO_DECODE_Params_t);

		////information got from caller
		//m_VidDecodeInfo.display_framerate = psourceInfo->framerate;
		//m_VidDecodeInfo.display_Y_width = psourceInfo->luma_width;
		//m_VidDecodeInfo.display_Y_height = psourceInfo->luma_height;
		//m_VidDecodeInfo.display_decimate_coeff = 0;

		//m_VidDecodeInfo.source_FourCC = psourceInfo->dwFourCC;
		////strcpy_s(m_VidDecodeInfo.source_pszFourCC, sizeof(m_VidDecodeInfo.source_pszFourCC), psourceInfo->pszFourCC);

		//m_VidDecodeInfo.source_luma_width = psourceInfo->luma_width;
		//m_VidDecodeInfo.source_luma_height = psourceInfo->luma_height;

		//m_VidDecodeInfo.source_bpp = psourceInfo->quantizationBits;

		////calculation
		//m_VidDecodeInfo.luma_pix_count = m_VidDecodeInfo.source_luma_width * m_VidDecodeInfo.source_luma_height;
		//m_VidDecodeInfo.luma_buf_size = m_VidDecodeInfo.luma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

		//switch (m_VidDecodeInfo.source_FourCC)
		//{
		//case 0x56555949:		//IYUV
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

		//	break;

		//case 0x30323449:		//I420
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

		//	break;

		//case 0x32315659:		//YV12
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_2;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = m_VidDecodeInfo.source_luma_height;
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

		//	break;

		//default:
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		//	break;
		//}
		////else if (strcmp(m_VidDecodeInfo.source_pszFourCC, "YUY2") == 0)
		////{
		////	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_2;
		////	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		////	m_VidDecodeInfo.source_chroma_height = m_VidDecodeInfo.source_luma_height;
		////	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		////	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		////}
		////else
		////{
		////	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
		////	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		////	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
		////	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		////	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		////}

		//m_VidDecodeInfo.frame_buf_size = m_VidDecodeInfo.luma_buf_size + m_VidDecodeInfo.chroma_buf_size + m_VidDecodeInfo.chroma_buf_size;

		//m_VidDecodeInfo.display_U_width = m_VidDecodeInfo.source_chroma_width;
		//m_VidDecodeInfo.display_U_height = m_VidDecodeInfo.source_chroma_height;

		//m_VidDecodeInfo.display_V_width = m_VidDecodeInfo.source_chroma_width;
		//m_VidDecodeInfo.display_V_height = m_VidDecodeInfo.source_chroma_height;

		//m_stOutputFrameParams.Y_width = m_VidDecodeInfo.display_Y_width;
		//m_stOutputFrameParams.Y_height = m_VidDecodeInfo.display_Y_height;
		//m_stOutputFrameParams.Y_frameSize = m_VidDecodeInfo.display_Y_width * m_VidDecodeInfo.display_Y_height;

		//m_stOutputFrameParams.U_width = m_VidDecodeInfo.display_U_width;
		//m_stOutputFrameParams.U_height = m_VidDecodeInfo.display_U_height;
		//m_stOutputFrameParams.U_frameSize = m_VidDecodeInfo.display_U_width * m_VidDecodeInfo.display_U_height;

		//m_stOutputFrameParams.V_width = m_VidDecodeInfo.display_V_width;
		//m_stOutputFrameParams.V_height = m_VidDecodeInfo.display_V_height;
		//m_stOutputFrameParams.V_frameSize = m_VidDecodeInfo.display_V_width * m_VidDecodeInfo.display_V_height;

		//m_nOutputFrameSize = m_stOutputFrameParams.Y_frameSize + m_stOutputFrameParams.U_frameSize + m_stOutputFrameParams.V_frameSize;			//RGB 3 plane

		//m_pucOutputFrameBuf = (uint8_t*)malloc(m_nOutputFrameSize);			//RGB 3 plane
		//memset(m_pucOutputFrameBuf, 0x00, m_nOutputFrameSize);

		//m_nSourceFrameSize = m_VidDecodeInfo.frame_buf_size;
		//m_pucSourceFrameBuf = (uint8_t*)malloc(m_nSourceFrameSize);
		//memset(m_pucSourceFrameBuf, 0x00, m_nSourceFrameSize);

		rtcode = CESDecoder::Open(dwStreamType, pszFileName);
	}
	//else
	//{
	//	rtcode = ESDECODER_PARAMETER_ERROR;
	//}

	return rtcode;
}

int CVESDecoder::Close(void)
{
	int rtcode = ESDECODER_PARAMETER_ERROR;

	memset(&m_VidDecodeInfo, 0x00, sizeof(VIDEO_DECODE_Params_t));

	if (m_pucSourceFrameBuf != NULL)
	{
		free(m_pucSourceFrameBuf);
		m_pucSourceFrameBuf = NULL;
	}
	//m_bSourceDataAvailable = 0;

	assert(m_bFrameProcessResponseStatus == 0);

	if (m_pucOutputFrameBuf != NULL)
	{
		free(m_pucOutputFrameBuf);
		m_pucOutputFrameBuf = NULL;
	}

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
	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_VidDecodeInfo.display_Y_width, m_VidDecodeInfo.display_Y_height, m_VidDecodeInfo.source_FourCC, m_VidDecodeInfo.display_framerate);

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
			uint8_t* pucSrcY = m_pucSourceFrameBuf;
			uint8_t* pucSrcU = m_pucSourceFrameBuf + m_VidDecodeInfo.luma_buf_size;
			uint8_t* pucSrcV = m_pucSourceFrameBuf + m_VidDecodeInfo.luma_buf_size + m_VidDecodeInfo.chroma_buf_size;

			uint8_t* pucDstY = m_pucOutputFrameBuf;
			uint8_t* pucDstU = m_pucOutputFrameBuf + m_stOutputFrameParams.Y_frameSize;
			uint8_t* pucDstV = m_pucOutputFrameBuf + m_stOutputFrameParams.Y_frameSize + m_stOutputFrameParams.U_frameSize;

			if (m_VidDecodeInfo.display_decimate_coeff == 0)
			{
				assert(m_nOutputFrameSize == m_nSourceFrameSize);
				memcpy(m_pucOutputFrameBuf, m_pucSourceFrameBuf, m_nSourceFrameSize);
			}
			else if (m_VidDecodeInfo.display_decimate_coeff > 0)
			{
				PICTURE_Enlarge(pucSrcY, m_VidDecodeInfo.source_luma_width, m_VidDecodeInfo.source_luma_height,
					pucDstY, m_stOutputFrameParams.Y_width, m_stOutputFrameParams.Y_height, m_VidDecodeInfo.display_decimate_coeff);

				PICTURE_Enlarge(pucSrcU, m_VidDecodeInfo.source_chroma_width, m_VidDecodeInfo.source_chroma_height,
					pucDstU, m_stOutputFrameParams.U_width, m_stOutputFrameParams.U_height, m_VidDecodeInfo.display_decimate_coeff);

				PICTURE_Enlarge(pucSrcV, m_VidDecodeInfo.source_chroma_width, m_VidDecodeInfo.source_chroma_height,
					pucDstV, m_stOutputFrameParams.V_width, m_stOutputFrameParams.V_height, m_VidDecodeInfo.display_decimate_coeff);
			}
			else if (m_VidDecodeInfo.display_decimate_coeff < 0)
			{
				PICTURE_Reduce(pucSrcY, m_VidDecodeInfo.source_luma_width, m_VidDecodeInfo.source_luma_height,
					pucDstY, m_stOutputFrameParams.Y_width, m_stOutputFrameParams.Y_height, -m_VidDecodeInfo.display_decimate_coeff);

				PICTURE_Reduce(pucSrcU, m_VidDecodeInfo.source_chroma_width, m_VidDecodeInfo.source_chroma_height,
					pucDstU, m_stOutputFrameParams.U_width, m_stOutputFrameParams.U_height, -m_VidDecodeInfo.display_decimate_coeff);

				PICTURE_Reduce(pucSrcV, m_VidDecodeInfo.source_chroma_width, m_VidDecodeInfo.source_chroma_height,
					pucDstV, m_stOutputFrameParams.V_width, m_stOutputFrameParams.V_height, -m_VidDecodeInfo.display_decimate_coeff);
			}

#if USE_FRAMEBUF_ACCESS_MUTEX
			::ReleaseMutex(m_hSourceFrameBufAccess);
		}
#endif
		::SetEvent(m_hSourceFrameBufEmptyEvent);

		if (m_callback_report_yuv_luma_stats != NULL)
		{
			m_callback_report_yuv_luma_stats(m_hwnd_for_caller, 0x45678912, 0x23456789);
		}

		if (m_callback_report_yuv_chroma_stats != NULL)
		{
			m_callback_report_yuv_chroma_stats(m_hwnd_for_caller, 0x86091335, 0x67521189);
		}

		//FeedToDirectDraw(m_pucOutputFrameBuf, m_nOutputFrameSize, &m_stOutputFrameParams);
		HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(m_pucOutputFrameBuf, m_nOutputFrameSize, &m_stOutputFrameParams);

#if USE_FRAMEBUF_ACCESS_MUTEX
		::ReleaseMutex(m_hOutputFrameBufAccess);
	}
#endif
	
	//FeedToDirectDraw();

	return rtcode;
}

int CVESDecoder::FeedToDirectDraw(const LPBYTE lpOutputFrameBuf, int frameSize, const FRAME_PARAMS_t* pstFrameParams)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;
	assert(m_pDirectDraw != NULL);

//#if USE_FRAMEBUF_ACCESS_MUTEX
//	uint32_t wait_state = ::WaitForSingleObject(m_hOutputFrameBufAccess, INFINITE);
//	if (wait_state == WAIT_OBJECT_0)
//	{
//#endif
//		//FRAME_PARAMS_t stFrameParams;
//
//		//stFrameParams.luma_width = m_VidDecodeInfo.source_luma_width;
//		//stFrameParams.luma_height = m_VidDecodeInfo.source_luma_height;
//		//stFrameParams.chroma_width = m_VidDecodeInfo.source_chroma_width;
//		//stFrameParams.chroma_height = m_VidDecodeInfo.source_chroma_height;
//
//		//HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(m_pucOutputFrameBuf, m_VidDecodeInfo.frame_buf_size, &stFrameParams);
//#if USE_FRAMEBUF_ACCESS_MUTEX
//		::ReleaseMutex(m_hOutputFrameBufAccess);
//	}
//#endif

	if ((m_pDirectDraw != NULL) && (lpOutputFrameBuf != NULL) && (pstFrameParams != NULL))
	{
		HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(lpOutputFrameBuf, frameSize, pstFrameParams);
	}

	return rtcode;
}

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
	return m_VidDecodeInfo.display_framerate;
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

	m_VidDecodeInfo.display_decimate_coeff += 2;
	if (m_VidDecodeInfo.display_decimate_coeff > 4)
	{
		m_VidDecodeInfo.display_decimate_coeff = -4;
	}

	rtcode = CanvasSetup(m_VidDecodeInfo.display_decimate_coeff);

	return rtcode;
}

int CVESDecoder::CanvasReduce(void)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	m_VidDecodeInfo.display_decimate_coeff -= 2;
	if (m_VidDecodeInfo.display_decimate_coeff < -4)
	{
		m_VidDecodeInfo.display_decimate_coeff = 4;
	}

	rtcode = CanvasSetup(m_VidDecodeInfo.display_decimate_coeff);

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
		if (display_decimate_coeff == 0)
		{
			m_VidDecodeInfo.display_Y_width = m_VidDecodeInfo.source_luma_width;
			m_VidDecodeInfo.display_Y_height = m_VidDecodeInfo.source_luma_height;
			m_VidDecodeInfo.display_U_width = m_VidDecodeInfo.source_chroma_width;
			m_VidDecodeInfo.display_U_height = m_VidDecodeInfo.source_chroma_height;
			m_VidDecodeInfo.display_V_width = m_VidDecodeInfo.source_chroma_width;
			m_VidDecodeInfo.display_V_height = m_VidDecodeInfo.source_chroma_height;
		}
		else if (display_decimate_coeff > 0)
		{
			m_VidDecodeInfo.display_Y_width = m_VidDecodeInfo.source_luma_width * display_decimate_coeff;
			m_VidDecodeInfo.display_Y_height = m_VidDecodeInfo.source_luma_height * display_decimate_coeff;
			m_VidDecodeInfo.display_U_width = m_VidDecodeInfo.source_chroma_width * display_decimate_coeff;
			m_VidDecodeInfo.display_U_height = m_VidDecodeInfo.source_chroma_height * display_decimate_coeff;
			m_VidDecodeInfo.display_V_width = m_VidDecodeInfo.source_chroma_width * display_decimate_coeff;
			m_VidDecodeInfo.display_V_height = m_VidDecodeInfo.source_chroma_height * display_decimate_coeff;
		}
		else if (display_decimate_coeff < 0)
		{
			m_VidDecodeInfo.display_Y_width = -m_VidDecodeInfo.source_luma_width / display_decimate_coeff;
			m_VidDecodeInfo.display_Y_height = -m_VidDecodeInfo.source_luma_height / display_decimate_coeff;
			m_VidDecodeInfo.display_U_width = -m_VidDecodeInfo.source_chroma_width / display_decimate_coeff;
			m_VidDecodeInfo.display_U_height = -m_VidDecodeInfo.source_chroma_height / display_decimate_coeff;
			m_VidDecodeInfo.display_V_width = -m_VidDecodeInfo.source_chroma_width / display_decimate_coeff;
			m_VidDecodeInfo.display_V_height = -m_VidDecodeInfo.source_chroma_height / display_decimate_coeff;
		}

		m_pDirectDraw->CloseVideo();
		m_pDirectDraw->OpenVideo(m_hwnd_for_caller, m_VidDecodeInfo.display_Y_width, m_VidDecodeInfo.display_Y_height, m_VidDecodeInfo.source_FourCC, m_VidDecodeInfo.display_framerate);

		if (m_pucOutputFrameBuf != NULL)
		{
			free(m_pucOutputFrameBuf);

			m_stOutputFrameParams.Y_width = m_VidDecodeInfo.display_Y_width;
			m_stOutputFrameParams.Y_height = m_VidDecodeInfo.display_Y_height;
			m_stOutputFrameParams.Y_frameSize = m_VidDecodeInfo.display_Y_width * m_VidDecodeInfo.display_Y_height;

			m_stOutputFrameParams.U_width = m_VidDecodeInfo.display_U_width;
			m_stOutputFrameParams.U_height = m_VidDecodeInfo.display_U_height;
			m_stOutputFrameParams.U_frameSize = m_VidDecodeInfo.display_U_width * m_VidDecodeInfo.display_U_height;

			m_stOutputFrameParams.V_width = m_VidDecodeInfo.display_V_width;
			m_stOutputFrameParams.V_height = m_VidDecodeInfo.display_V_height;
			m_stOutputFrameParams.V_frameSize = m_VidDecodeInfo.display_V_width * m_VidDecodeInfo.display_V_height;

			m_nOutputFrameSize = m_stOutputFrameParams.Y_frameSize + m_stOutputFrameParams.U_frameSize + m_stOutputFrameParams.V_frameSize;			//RGB 3 plane

			m_pucOutputFrameBuf = (uint8_t*)malloc(m_nOutputFrameSize);			//RGB 3 plane
			memset(m_pucOutputFrameBuf, 0x00, m_nOutputFrameSize);
		}

#if USE_FRAMEBUF_ACCESS_MUTEX
		::ReleaseMutex(m_hOutputFrameBufAccess);
	}
#endif

	rtcode = FrameProcessAndFeedToDirectDraw();

	return rtcode;
}

void CVESDecoder::ToggleView(void)
{
	assert(m_pDirectDraw != NULL);

	m_pDirectDraw->ToggleView();
}


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
		double frame_rate = pDecoder->GetDisplayFrameRate();
		double frame_interval = 1000.0 / frame_rate;

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
			//for (int j = 0; j < decimate_coeff; j++)
			{
				int dst_col = 0;
				for (int src_col = 0; src_col < src_w; src_col+= decimate_coeff)
				{
					//for (int i = 0; i < decimate_coeff; i++)
					{
						pDstRowStart[dst_col] = pSrcRowStart[src_col];
					}
					dst_col ++;
				}

				pDstRowStart += dst_w;
			}

			pSrcRowStart += (src_w * decimate_coeff);
		}
	}

	return rtcode;
}
