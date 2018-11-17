#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>

#include "../Include/VideoDecoder_YUV.h"

CYUV_VideoDecoder::CYUV_VideoDecoder(void)
{
	m_pucFrameBuf[CC_Y] = NULL;
	m_pucFrameBuf[CC_Cb] = NULL;
	m_pucFrameBuf[CC_Cr] = NULL;

//	memset(&m_VidDecodeInfo, 0x00, sizeof(Video_decode_info_t));
}

CYUV_VideoDecoder::~CYUV_VideoDecoder(void)
{
	Reset();
}

int CYUV_VideoDecoder::Open(int nFileType, char* pszFileName, Video_decode_info_t* pdecode_info)
{
	m_nFileType = nFileType;
	strcpy_s(m_pszFileName, pszFileName);

	memcpy(&m_VidDecodeInfo, pdecode_info, sizeof(Video_decode_info_t));

	m_pucFrameBuf[CC_Y] = (uint8_t*)malloc(m_VidDecodeInfo.luma_buf_size);
	m_pucFrameBuf[CC_Cb] = (uint8_t*)malloc(m_VidDecodeInfo.chroma_buf_size);
	m_pucFrameBuf[CC_Cr] = (uint8_t*)malloc(m_VidDecodeInfo.chroma_buf_size);
	memset(m_pucFrameBuf[CC_Y], 0x00, m_VidDecodeInfo.luma_buf_size);
	memset(m_pucFrameBuf[CC_Cb], 0x80, m_VidDecodeInfo.chroma_buf_size);
	memset(m_pucFrameBuf[CC_Cr], 0x80, m_VidDecodeInfo.chroma_buf_size);

	m_VidDecodeInfo.pucY = m_pucFrameBuf[CC_Y];
	m_VidDecodeInfo.pucU = m_pucFrameBuf[CC_Cb];
	m_VidDecodeInfo.pucV = m_pucFrameBuf[CC_Cr];

	if (m_nFileType == YUV_FILE_YUV)
	{
		_sopen_s(&m_hFile, m_pszFileName, _O_BINARY | _O_RDONLY, _SH_DENYWR, 0);
		if (m_hFile > 0)
		{
			_lseeki64(m_hFile, 0, SEEK_END);
			m_nFileTotalSize = _tell(m_hFile);

			_lseek(m_hFile, 0, SEEK_SET);
		}
	}

	return 0;
}

int CYUV_VideoDecoder::Close(void)
{
	Reset();

	return 0;
}

void CYUV_VideoDecoder::Reset(void)
{
	int		 i;

	memset(&m_VidDecodeInfo, 0x00, sizeof(Video_decode_info_t));

	for (i = 0; i < 3; i++)
	{
		if (m_pucFrameBuf[i] != NULL)
		{
			free(m_pucFrameBuf[i]);
			m_pucFrameBuf[i] = NULL;
		}
	}

	if (m_hFile > 0)
	{
		_close(m_hFile);
		m_hFile = 0;
	}
}

int CYUV_VideoDecoder::OpenVideo(HWND hWnd, char* pszFourCC, int strSize)
{
//	if (pwidth != NULL)
//	{
//		*pwidth = m_VidDecodeInfo.display_width;
//	}
//	if (pheight != NULL)
//	{
//		*pheight = m_VidDecodeInfo.display_height;
//	}
//	if (pszFourCC != NULL)
//	{
//		strcpy_s(pszFourCC, m_VidDecodeInfo.pszFourCC);
//	}

	return CVESDecoder::OpenVideo(hWnd);
}

int CYUV_VideoDecoder::CloseVideo(void)
{
	return CVESDecoder::CloseVideo();
}

int CYUV_VideoDecoder::Preview_NextPicture(void)
{
	int		rdsize;
	int		end_of_file = 0;
	int		seek_count = 0;

	if (_tell(m_hFile) < m_nFileTotalSize)
	{
		if (m_nFileType == YUV_FILE_YUV)
		{
//			if (end_of_file)
//			{
//				_lseeki64(m_hFile, 0, SEEK_SET);
//				seek_count ++;
//				end_of_file = 0;
//			}

			rdsize = _read(m_hFile, m_pucFrameBuf[CC_Y], m_VidDecodeInfo.luma_buf_size);
			if (rdsize < m_VidDecodeInfo.luma_buf_size)
			{
				end_of_file = 1;
			}

			rdsize = _read(m_hFile, m_pucFrameBuf[CC_Cb], m_VidDecodeInfo.chroma_buf_size);
			if (rdsize < m_VidDecodeInfo.chroma_buf_size)
			{
				//end of the file
				end_of_file = 1;
			}

			rdsize = _read(m_hFile, m_pucFrameBuf[CC_Cr], m_VidDecodeInfo.chroma_buf_size);
			if (rdsize < m_VidDecodeInfo.chroma_buf_size)
			{
				//end of the file
				end_of_file = 1;
			}
		}
		else if (m_nFileType == YUV_FILE_DY_DU_DV)
		{
			char pszFileName[256];

			sprintf_s(pszFileName, "%s.y", m_pszFileName);
			FILE* fpY = NULL;
			fopen_s(&fpY, pszFileName, "rb");
			if (fpY != NULL)
			{
				fread(m_pucFrameBuf[CC_Y], 1, m_VidDecodeInfo.luma_buf_size, fpY);
				fclose(fpY);
			}

			sprintf_s(pszFileName, "%s.u", m_pszFileName);
			FILE* fpU = NULL;
			fopen_s(&fpU, pszFileName, "rb");
			if (fpU != NULL)
			{
				fread(m_pucFrameBuf[CC_Cb], 1, m_VidDecodeInfo.chroma_buf_size, fpU);
				fclose(fpU);
			}

			sprintf_s(pszFileName, "%s.v", m_pszFileName);
			FILE* fpV = NULL;
			fopen_s(&fpV, pszFileName, "rb");
			if (fpV != NULL)
			{
				fread(m_pucFrameBuf[CC_Cr], 1, m_VidDecodeInfo.chroma_buf_size, fpV);
				fclose(fpV);

			}
		}

		DirectDraw_Render_yuv();
//		::SendMessage(m_hVidWnd, WM_UPDATE_PICTURE, (WPARAM)&m_VidDecodeInfo, NULL);
	}

	int percent = 0;

	if (m_nFileTotalSize > 0)
	{
		int filepos = _tell(m_hFile);
		
		percent = (int)(filepos * 100 / m_nFileTotalSize);
	}

	return percent;	
}

int	CYUV_VideoDecoder::Preview_EOF(void)
{
	int	bEOF = FALSE;

	int filepos = _tell(m_hFile);
	int endpos = m_nFileTotalSize - (m_nFileTotalSize % m_VidDecodeInfo.frame_buf_size);

	if (filepos >= endpos)
	{
		bEOF = TRUE;
	}

	return bEOF;
}

int CYUV_VideoDecoder::Preview_PrePicture(void)
{
	int64_t filepos = _telli64(m_hFile);
	
	filepos -= (m_VidDecodeInfo.frame_buf_size << 1);
	if (filepos < 0)
	{
		_lseeki64(m_hFile, 0, SEEK_SET);
	}
	else
	{
		_lseeki64(m_hFile, filepos, SEEK_SET);
	}

	return Preview_NextPicture();
}

int CYUV_VideoDecoder::Preview_ForwardPicture(void)
{
	int filepos = _tell(m_hFile);

	filepos += m_VidDecodeInfo.frame_buf_size * 5;
	if (filepos > (m_nFileTotalSize - m_VidDecodeInfo.frame_buf_size))
	{
		filepos = (m_nFileTotalSize - m_VidDecodeInfo.frame_buf_size);
	}
	filepos -= (filepos % m_VidDecodeInfo.frame_buf_size);
	_lseek(m_hFile, filepos, SEEK_SET);

	return Preview_NextPicture();
}

int CYUV_VideoDecoder::Preview_BackwardPicture(void)
{
	int64_t filepos = _telli64(m_hFile);

	filepos -= m_VidDecodeInfo.frame_buf_size * 5;
	if (filepos < 0)
	{
		filepos = 0;
	}
	_lseeki64(m_hFile, filepos, SEEK_SET);

	return Preview_NextPicture();
}

int CYUV_VideoDecoder::Preview_LastPicture(void)
{
	int filepos = m_nFileTotalSize - m_VidDecodeInfo.frame_buf_size;
	
	filepos -= filepos % m_VidDecodeInfo.frame_buf_size;

	_lseek(m_hFile, filepos, SEEK_SET);

	return Preview_NextPicture();
}

int CYUV_VideoDecoder::Preview_FirstPicture(void)
{
	_lseek(m_hFile, 0, SEEK_SET);

	return Preview_NextPicture();
}

int CYUV_VideoDecoder::Preview_AtPercent(int nPercent)
{
	int		 offset;

	offset = m_nFileTotalSize * nPercent / 100;
	offset -= offset % m_VidDecodeInfo.frame_buf_size;
	_lseek(m_hFile, offset, SEEK_SET);

	return Preview_NextPicture();
}

void CYUV_VideoDecoder::SetGrid(void)
{
/*	
	U8*		buf = NULL;
	S32		width;
	S32		height;

	mpeg_video_setgrid();

	if (m_hWnd != NULL)
	{
		buf = mpeg_video_output(&width, &height);

		if (buf != NULL)
		{
			if (callback_videooutput_Render_bmp != NULL)
			{
				callback_videooutput_Render_bmp(m_hDC, buf, width, height);
			}
		}
	}
*/	
}

int	CYUV_VideoDecoder::Get_decode_info(Video_decode_info_t* pdecode_info)
{
	int	rtcode = 0;

	if (pdecode_info != NULL)
	{
		memcpy(pdecode_info, &m_VidDecodeInfo, sizeof(Video_decode_info_t));

		//¨º1¨®??a??¡¤?¡¤¡§?¨¦¨°?¨¨¡¤¡À¡ê¨®|¨®?3¨¬D¨°???¨¹¦Ì?¦Ì??¨²o?¦Ì??¡À¡À?¡ê??T?????¡äDT??2??¨¢???¦Ì?¨²o?¦Ì???DD
	}
	else
	{
		rtcode = -1;
	}

	return rtcode;
}

void DecodeFourCC2Text(char* pszFourCC, char* pszText, int size)
{
	if ((pszFourCC != NULL) && (pszText != NULL))
	{
		if (strcmp(pszFourCC, "IYUV") == 0)
		{
			strcpy_s(pszText, size, "YUV4:2:0	Y-U-V");
		}
		else if (strcmp(pszFourCC, "YV12") == 0)
		{
			strcpy_s(pszText, size, "YUV4:2:0	Y-V-U");
		}
		else if (strcmp(pszFourCC, "YUY2") == 0)
		{
			strcpy_s(pszText, size, "YUV4:2:2	Y-U-Y-V");
		}
		else if (strcmp(pszFourCC, "YV16") == 0)
		{
			strcpy_s(pszText, size, "YUV4:2:2	Y-V-U");
		}
		else
		{
			strcpy_s(pszText, size, "");
		}
	}
}






