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
	m_pucOutputFrameBuf = NULL;

	m_pDirectDraw = NULL;

	//direct draw settings
	memset(&m_VidDecodeInfo, 0x00, sizeof(VIDEO_DECODE_Params_t));

	m_VidDecodeInfo.size = sizeof(VIDEO_DECODE_Params_t);
	m_VidDecodeInfo.getparams = 0;
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

		m_pucOutputFrameBuf = (uint8_t*)malloc(m_VidDecodeInfo.frame_buf_size);
		memset(m_pucOutputFrameBuf, 0x00, m_VidDecodeInfo.frame_buf_size);

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

	if (m_pucOutputFrameBuf != NULL)
	{
		free(m_pucOutputFrameBuf);
		m_pucOutputFrameBuf = NULL;
	}

	rtcode = CESDecoder::Close();

	return rtcode;
}

int CVESDecoder::AttachWnd(HWND hWnd)
{
	HRESULT ddRval = -1;

	assert(m_pDirectDraw == NULL);
	m_pDirectDraw = new CTALForDirectDraw;
	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_VidDecodeInfo.display_width, m_VidDecodeInfo.display_height, m_VidDecodeInfo.source_FourCC);

	return ddRval;
}

int CVESDecoder::DetachWnd(HWND hWnd)
{
	//assert(hWnd == m_hVidWnd);

	HRESULT ddRval = -1;

	if (m_pDirectDraw != NULL)
	{
		ddRval = m_pDirectDraw->CloseVideo();
		delete m_pDirectDraw;
		m_pDirectDraw = NULL;
	}

	return ddRval;
}

int CVESDecoder::FeedToDirectDraw(void)
{
	assert(m_pDirectDraw != NULL);

	FRAME_PARAMS_t stFrameParams;
	stFrameParams.luma_width = m_VidDecodeInfo.source_luma_width;
	stFrameParams.luma_height = m_VidDecodeInfo.source_luma_height;
	stFrameParams.chroma_width = m_VidDecodeInfo.source_chroma_width;
	stFrameParams.chroma_height = m_VidDecodeInfo.source_chroma_height;

	HRESULT ddRval = m_pDirectDraw->RenderYUV(m_pucOutputFrameBuf, m_VidDecodeInfo.frame_buf_size, &stFrameParams);

	return ddRval;
}

int CVESDecoder::DirectDraw_Paint(void)
{
	assert(m_pDirectDraw != NULL);

	HRESULT	ddRval = m_pDirectDraw->RePaint();
	
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
	HRESULT	ddRval = m_pDirectDraw->RePaint();

	return;
}

void CVESDecoder::SaveSnapshot(const char* dstfilename)
{
}

void CVESDecoder::EnlargeVideo(void)
{
	assert(m_pDirectDraw != NULL);

	m_pDirectDraw->ToggleScreen();
	HRESULT	ddRval = m_pDirectDraw->RePaint();
}
