#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "thirdparty_AL\TALForDirectX\Include\TALForDirectDraw.h"

#include "../Include/common/video_common.h"
#include "../Include/VESDecoder.h"

CVESDecoder::CVESDecoder(void)
{
	m_hVidWnd = NULL;
	m_pDirectDraw = NULL;

	//direct draw settings
	memset(&m_VidDecodeInfo, 0x00, sizeof(Video_decode_info_t));

	m_VidDecodeInfo.size = sizeof(Video_decode_info_t);
	m_VidDecodeInfo.getparams = 0;

}

CVESDecoder::~CVESDecoder()
{
	assert(m_pDirectDraw == NULL);
}

void CVESDecoder::Reset(void)
{
	m_VidDecodeInfo.getparams = 0;

	CESDecoder::Reset();
}

int CVESDecoder::OpenVideo(HWND hWnd)
{
	HRESULT ddRval;

	m_hVidWnd = hWnd;
	m_nDebugFrameCount = 0;
	m_dwTimeCount = 0x00000000;

	m_pDirectDraw = new CTALForDirectDraw;

	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_VidDecodeInfo.luma_width, m_VidDecodeInfo.luma_height, m_VidDecodeInfo.pszFourCC);

	return ddRval;
}

int CVESDecoder::CloseVideo(void)
{
	m_hVidWnd = NULL;

	HRESULT ddRval = m_pDirectDraw->CloseVideo();
	delete m_pDirectDraw;
	m_pDirectDraw = NULL;

	return ddRval;
}

int CVESDecoder::DirectDraw_Render_yuv(void)
{
	assert(m_pDirectDraw != NULL);

	HRESULT ddRval = m_pDirectDraw->RenderYUV(m_VidDecodeInfo.pucY, m_VidDecodeInfo.luma_buf_size,
										m_VidDecodeInfo.pucU, m_VidDecodeInfo.chroma_buf_size, 
										m_VidDecodeInfo.pucV, m_VidDecodeInfo.chroma_buf_size);

	m_nDebugFrameCount++;
	if (m_dwTimeCount == 0x00000000)
	{
		m_dwTimeCount = ::GetTickCount();
	}
	else
	{
		DWORD curTick = ::GetTickCount();
		DWORD dwTickDiff = curTick - m_dwTimeCount;

		if (dwTickDiff >= 2000)
		{
			uint32_t fps = (m_nDebugFrameCount - 1) * 2000000 / dwTickDiff;

			//::SendMessage(m_hVidWnd, WM_REPORT_VIDEO_DECODE_FPS, (WPARAM)fps, NULL);		chendelin

			m_nDebugFrameCount = 0;
			m_dwTimeCount = curTick;
		}
	}

	return ddRval;
}

int CVESDecoder::DirectDraw_Paint(void)
{
	assert(m_pDirectDraw != NULL);

	HRESULT	ddRval = m_pDirectDraw->RePaint();
	
	return ddRval;
}

