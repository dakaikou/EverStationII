#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "thirdparty_AL\TALForDirectX\Include\TALForDirectDraw.h"

#include "../Include/common/video_common.h"
#include "../Include/VESDecoder.h"

CVESDecoder::CVESDecoder(void)
{
	m_pucOutputFrameBuf[CC_Y] = NULL;
	m_pucOutputFrameBuf[CC_Cb] = NULL;
	m_pucOutputFrameBuf[CC_Cr] = NULL;

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

//void CVESDecoder::Reset(void)
//{
//	m_VidDecodeInfo.getparams = 0;
//
//	CESDecoder::Reset();
//}

int CVESDecoder::Open(uint32_t dwStreamType, char* pszFileName, Video_decode_info_t* pdecode_info)
{
	//m_nFileType = nFileType;
	//strcpy_s(m_pszFileName, pszFileName);

	memcpy(&m_VidDecodeInfo, pdecode_info, sizeof(Video_decode_info_t));

	m_pucOutputFrameBuf[CC_Y] = (uint8_t*)malloc(m_VidDecodeInfo.luma_buf_size);
	m_pucOutputFrameBuf[CC_Cb] = (uint8_t*)malloc(m_VidDecodeInfo.chroma_buf_size);
	m_pucOutputFrameBuf[CC_Cr] = (uint8_t*)malloc(m_VidDecodeInfo.chroma_buf_size);
	memset(m_pucOutputFrameBuf[CC_Y], 0x00, m_VidDecodeInfo.luma_buf_size);
	memset(m_pucOutputFrameBuf[CC_Cb], 0x80, m_VidDecodeInfo.chroma_buf_size);
	memset(m_pucOutputFrameBuf[CC_Cr], 0x80, m_VidDecodeInfo.chroma_buf_size);

	//if (m_nFileType == YUV_FILE_YUV)
	//{
	//	_sopen_s(&m_hFile, m_pszFileName, _O_BINARY | _O_RDONLY, _SH_DENYWR, 0);
	//	if (m_hFile > 0)
	//	{
	//		_lseeki64(m_hFile, 0, SEEK_END);
	//		m_nFileTotalSize = _telli64(m_hFile);

	//		_lseeki64(m_hFile, 0, SEEK_SET);
	//	}
	//}
	CESDecoder::Open(dwStreamType, pszFileName);

	return 0;
}

int CVESDecoder::Close(void)
{
	int		 i;

	memset(&m_VidDecodeInfo, 0x00, sizeof(Video_decode_info_t));

	for (i = 0; i < 3; i++)
	{
		if (m_pucOutputFrameBuf[i] != NULL)
		{
			free(m_pucOutputFrameBuf[i]);
			m_pucOutputFrameBuf[i] = NULL;
		}
	}

	CESDecoder::Close();
	//if (m_hFile > 0)
	//{
	//	_close(m_hFile);
	//	m_hFile = 0;
	//}

	return 0;
}

HWND CVESDecoder::GetWndHandle(void)
{
	return m_hVidWnd;
}

int CVESDecoder::OpenDirectxWnd(HWND hWnd)
{
	HRESULT ddRval = -1;

	m_hVidWnd = hWnd;
	m_nDebugFrameCount = 0;
	m_dwDebugTimeTick = 0;

	assert(m_pDirectDraw == NULL);
	m_pDirectDraw = new CTALForDirectDraw;
	ddRval = m_pDirectDraw->OpenVideo(hWnd, m_VidDecodeInfo.luma_width, m_VidDecodeInfo.luma_height, m_VidDecodeInfo.pszFourCC);

	return ddRval;
}

int CVESDecoder::CloseDirectxWnd(void)
{
	HRESULT ddRval = -1;

	if (m_pDirectDraw != NULL)
	{
		ddRval = m_pDirectDraw->CloseVideo();
		delete m_pDirectDraw;
		m_pDirectDraw = NULL;
	}
	m_hVidWnd = NULL;

	return ddRval;
}

int CVESDecoder::DirectDraw_Render_yuv(void)
{
	assert(m_pDirectDraw != NULL);

	HRESULT ddRval = m_pDirectDraw->RenderYUV(m_pucOutputFrameBuf[CC_Y], m_VidDecodeInfo.luma_buf_size,
		m_pucOutputFrameBuf[CC_Cb], m_VidDecodeInfo.chroma_buf_size,
		m_pucOutputFrameBuf[CC_Cr], m_VidDecodeInfo.chroma_buf_size);

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

	return ddRval;
}

int CVESDecoder::DirectDraw_Paint(void)
{
	assert(m_pDirectDraw != NULL);

	HRESULT	ddRval = m_pDirectDraw->RePaint();
	
	return ddRval;
}

void CVESDecoder::GetDecodeInfo(Video_decode_info_t* pparam)
{
	if (pparam != NULL)
	{
		memcpy(pparam, &m_VidDecodeInfo, sizeof(Video_decode_info_t));
	}
}

