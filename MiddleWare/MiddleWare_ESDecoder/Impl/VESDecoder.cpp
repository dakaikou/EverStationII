#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
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
	//m_pucOutputFrameBuf = NULL;

	m_pDirectDraw = NULL;

	//direct draw settings
	memset(&m_VidDecodeInfo, 0x00, sizeof(VIDEO_DECODE_Params_t));

	m_VidDecodeInfo.size = sizeof(VIDEO_DECODE_Params_t);
	m_VidDecodeInfo.getparams = 0;

	m_callback_report_yuv_luma_stats = NULL;
	m_hwnd_for_caller = NULL;

#if USE_FIFO_ACCESS_MUTEX
	m_hFifoAccess = NULL;
#endif
	m_bCommandRun = 0;
	m_bRunning = 0;

	m_bSourceDataAvailable = 0;
}

CVESDecoder::~CVESDecoder()
{
	assert(m_pDirectDraw == NULL);
}

//void CVESDecoder::Reset(void)
//{
//	m_VidDecodeInfo.getparams = 0;
//
//	CESDecoder::Reset();
//}

int CVESDecoder::Open(uint32_t dwStreamType, const char* pszFileName, const YUV_SOURCE_PARAM_t* psourceInfo)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	if (psourceInfo != NULL)
	{
		m_VidDecodeInfo.size = sizeof(VIDEO_DECODE_Params_t);

		//information got from caller
		m_VidDecodeInfo.display_framerate = psourceInfo->framerate;
		m_VidDecodeInfo.display_width = psourceInfo->luma_width;
		m_VidDecodeInfo.display_height = psourceInfo->luma_height;
		m_VidDecodeInfo.display_decimate_coeff = 0;

		m_VidDecodeInfo.source_FourCC = psourceInfo->dwFourCC;

		//strcpy_s(m_VidDecodeInfo.source_pszFourCC, sizeof(m_VidDecodeInfo.source_pszFourCC), psourceInfo->pszFourCC);

		m_VidDecodeInfo.source_luma_width = psourceInfo->luma_width;
		m_VidDecodeInfo.source_luma_height = psourceInfo->luma_height;

		m_VidDecodeInfo.source_bpp = psourceInfo->quantizationBits;

		//calculation
		m_VidDecodeInfo.luma_pix_count = m_VidDecodeInfo.source_luma_width * m_VidDecodeInfo.source_luma_height;
		m_VidDecodeInfo.luma_buf_size = m_VidDecodeInfo.luma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

		switch (m_VidDecodeInfo.source_FourCC)
		{
		case 0x56555949:		//IYUV
			m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
			m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
			m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
			m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
			m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

			break;

		case 0x30323449:		//I420
			m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
			m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
			m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
			m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
			m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

			break;

		case 0x32315659:		//YV12
			m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_2;
			m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
			m_VidDecodeInfo.source_chroma_height = m_VidDecodeInfo.source_luma_height;
			m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
			m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);

			break;

		default:
			m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
			m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
			m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
			m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
			m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
			break;
		}
		//else if (strcmp(m_VidDecodeInfo.source_pszFourCC, "YUY2") == 0)
		//{
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_2;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = m_VidDecodeInfo.source_luma_height;
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		//}
		//else
		//{
		//	m_VidDecodeInfo.source_chroma_format = CHROMA_FORMAT_4_2_0;
		//	m_VidDecodeInfo.source_chroma_width = (m_VidDecodeInfo.source_luma_width >> 1);
		//	m_VidDecodeInfo.source_chroma_height = (m_VidDecodeInfo.source_luma_height >> 1);
		//	m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
		//	m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * (m_VidDecodeInfo.source_bpp / 8);
		//}

		m_VidDecodeInfo.frame_buf_size = m_VidDecodeInfo.luma_buf_size + m_VidDecodeInfo.chroma_buf_size + m_VidDecodeInfo.chroma_buf_size;

		m_pucSourceFrameBuf = (uint8_t*)malloc(m_VidDecodeInfo.frame_buf_size);
		memset(m_pucSourceFrameBuf, 0x00, m_VidDecodeInfo.frame_buf_size);
		m_bSourceDataAvailable = 0;

		rtcode = CESDecoder::Open(dwStreamType, pszFileName);
	}
	else
	{
		rtcode = ESDECODER_PARAMETER_ERROR;
	}

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
	m_bSourceDataAvailable = 0;

	rtcode = CESDecoder::Close();

	return rtcode;
}

int CVESDecoder::AttachWnd(HWND hWnd, int(*callback_luma)(HWND, WPARAM, LPARAM), int(*callback_chroma)(HWND, WPARAM, LPARAM))
{
	HRESULT ddRval = -1;

	m_hwnd_for_caller = hWnd;

	assert(m_pDirectDraw == NULL);
	m_pDirectDraw = new CTALForDirectDraw;
	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_VidDecodeInfo.display_width, m_VidDecodeInfo.display_height, m_VidDecodeInfo.source_FourCC);

	if (callback_luma != NULL)
	{
		m_callback_report_yuv_luma_stats = callback_luma;
	}
	if (callback_chroma != NULL)
	{
		m_callback_report_yuv_chroma_stats = callback_chroma;
	}

#if USE_FIFO_ACCESS_MUTEX
	m_hFifoAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif
	m_bCommandRun = true;
	m_bRunning = false;
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_frame_process, (LPVOID)this, 0, 0);

	return ddRval;
}

int CVESDecoder::DetachWnd(HWND hWnd)
{
	m_bCommandRun = false;

	while (m_bRunning == 1)
	{
		Sleep(10);
	}

	//assert(hWnd == m_hVidWnd);
#if USE_FIFO_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hFifoAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif

#if USE_FIFO_ACCESS_MUTEX
		::CloseHandle(m_hFifoAccess);
		m_hFifoAccess = NULL;
	}
#endif

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

int CVESDecoder::FeedToDirectDraw(void)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;
	assert(m_pDirectDraw != NULL);

	FRAME_PARAMS_t stFrameParams;
	uint8_t* pucOutputFrameBuf = (uint8_t*)malloc(m_VidDecodeInfo.frame_buf_size);

#if USE_FIFO_ACCESS_MUTEX
	uint32_t wait_state = ::WaitForSingleObject(m_hFifoAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		stFrameParams.luma_width = m_VidDecodeInfo.source_luma_width;
		stFrameParams.luma_height = m_VidDecodeInfo.source_luma_height;
		stFrameParams.chroma_width = m_VidDecodeInfo.source_chroma_width;
		stFrameParams.chroma_height = m_VidDecodeInfo.source_chroma_height;

		//very simple image processing
		memcpy(pucOutputFrameBuf, m_pucSourceFrameBuf, m_VidDecodeInfo.frame_buf_size);

		m_bSourceDataAvailable = 0;
#if USE_FIFO_ACCESS_MUTEX
		::ReleaseMutex(m_hFifoAccess);
	}
#endif

	HRESULT ddRval = m_pDirectDraw->FeedToOffScreenSurface(pucOutputFrameBuf, m_VidDecodeInfo.frame_buf_size, &stFrameParams);

	if (m_callback_report_yuv_luma_stats != NULL)
	{
		m_callback_report_yuv_luma_stats(m_hwnd_for_caller, 0x45678912, 0x23456789);
	}

	if (m_callback_report_yuv_chroma_stats != NULL)
	{
		m_callback_report_yuv_chroma_stats(m_hwnd_for_caller, 0x86091335, 0x67521189);
	}

	free(pucOutputFrameBuf);

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

void CVESDecoder::ToggleCanvas(void)
{
	assert(m_pDirectDraw != NULL);

	m_VidDecodeInfo.display_decimate_coeff += 2;
	if (m_VidDecodeInfo.display_decimate_coeff > 4)
	{
		m_VidDecodeInfo.display_decimate_coeff = -4;
	}

	if (m_VidDecodeInfo.display_decimate_coeff == 0)
	{
		m_VidDecodeInfo.display_width = m_VidDecodeInfo.source_luma_width;
		m_VidDecodeInfo.display_height = m_VidDecodeInfo.source_luma_height;
	}
	else if (m_VidDecodeInfo.display_decimate_coeff > 0)
	{
		m_VidDecodeInfo.display_width = m_VidDecodeInfo.source_luma_width * m_VidDecodeInfo.display_decimate_coeff;
		m_VidDecodeInfo.display_height = m_VidDecodeInfo.source_luma_height * m_VidDecodeInfo.display_decimate_coeff;
	}
	else if (m_VidDecodeInfo.display_decimate_coeff < 0)
	{
		m_VidDecodeInfo.display_width = - m_VidDecodeInfo.source_luma_width / m_VidDecodeInfo.display_decimate_coeff;
		m_VidDecodeInfo.display_height = - m_VidDecodeInfo.source_luma_height / m_VidDecodeInfo.display_decimate_coeff;
	}

	m_pDirectDraw->CloseVideo();
	m_pDirectDraw->OpenVideo(m_hwnd_for_caller, m_VidDecodeInfo.display_width, m_VidDecodeInfo.display_height, m_VidDecodeInfo.source_FourCC);

	//FeedToDirectDraw();

	//m_pDirectDraw->ToggleCanvas();
}

void CVESDecoder::ToggleView(void)
{
	assert(m_pDirectDraw != NULL);

	m_pDirectDraw->ToggleView();
}


uint32_t thread_frame_process(LPVOID lpParam)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	CVESDecoder* pDecoder = (CVESDecoder*)lpParam;

	if (pDecoder != NULL)
	{
		pDecoder->m_bRunning = 1;
		while (pDecoder->m_bCommandRun)
		{
			if (pDecoder->m_bSourceDataAvailable)
			{
				pDecoder->FeedToDirectDraw();
			}
		}
		pDecoder->m_bRunning = 0;

		rtcode = ESDECODER_NO_ERROR;
	}

	return rtcode;
}
