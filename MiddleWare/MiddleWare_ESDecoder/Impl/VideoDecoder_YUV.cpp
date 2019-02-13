#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "../Include/VideoDecoder_YUV.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"

CYUV_VideoDecoder::CYUV_VideoDecoder(void)
{
}

CYUV_VideoDecoder::~CYUV_VideoDecoder(void)
{
}

//implementation of virtual fuction
int	CYUV_VideoDecoder::Preview_beEOF(void)
{
	int	bEOF = FALSE;

	int64_t endpos = m_nFileTotalSize - (m_nFileTotalSize % m_VidDecodeInfo.frame_buf_size);

	if (m_nCurReadPos >= endpos)
	{
		bEOF = TRUE;
	}

	return bEOF;
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_Forward1Picture(void)
{
	int		rdsize;
	//int		end_of_file = 0;
	//int		seek_count = 0;
	int		percent = 100;

	if (m_nCurReadPos < m_nFileTotalSize)
	{
		if (m_dwStreamType == (STREAM_FILE | YUV_FILE_YUV))
		{
			rdsize = _read(m_hFile, m_pucOutputFrameBuf[CC_Y], m_VidDecodeInfo.luma_buf_size);
			if (rdsize < m_VidDecodeInfo.luma_buf_size)
			{
				//end_of_file = 1;
				assert(0);
			}

			rdsize = _read(m_hFile, m_pucOutputFrameBuf[CC_Cb], m_VidDecodeInfo.chroma_buf_size);
			if (rdsize < m_VidDecodeInfo.chroma_buf_size)
			{
				//end of the file
				//end_of_file = 1;
				assert(0);
			}

			rdsize = _read(m_hFile, m_pucOutputFrameBuf[CC_Cr], m_VidDecodeInfo.chroma_buf_size);
			if (rdsize < m_VidDecodeInfo.chroma_buf_size)
			{
				//end of the file
				//end_of_file = 1;
				assert(0);
			}

			//m_nCurReadPos = _telli64(m_hFile);
			m_nCurReadPos += m_VidDecodeInfo.frame_buf_size;

			percent = (int)(m_nCurReadPos * 100.0 / m_nFileTotalSize);
		}
		//else if (m_dwStreamType == (STREAM_FILE | YUV_FILE_DY_DU_DV))
		//{
		//	char pszFileName[256];

		//	sprintf_s(pszFileName, "%s.y", m_pszFileName);
		//	FILE* fpY = NULL;
		//	fopen_s(&fpY, pszFileName, "rb");
		//	if (fpY != NULL)
		//	{
		//		fread(m_pucOutputFrameBuf[CC_Y], 1, m_VidDecodeInfo.luma_buf_size, fpY);
		//		fclose(fpY);
		//	}

		//	sprintf_s(pszFileName, "%s.u", m_pszFileName);
		//	FILE* fpU = NULL;
		//	fopen_s(&fpU, pszFileName, "rb");
		//	if (fpU != NULL)
		//	{
		//		fread(m_pucOutputFrameBuf[CC_Cb], 1, m_VidDecodeInfo.chroma_buf_size, fpU);
		//		fclose(fpU);
		//	}

		//	sprintf_s(pszFileName, "%s.v", m_pszFileName);
		//	FILE* fpV = NULL;
		//	fopen_s(&fpV, pszFileName, "rb");
		//	if (fpV != NULL)
		//	{
		//		fread(m_pucOutputFrameBuf[CC_Cr], 1, m_VidDecodeInfo.chroma_buf_size, fpV);
		//		fclose(fpV);

		//	}
		//}

		DirectDraw_Render_yuv();
	}

	return percent;	
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_Backward1Picture(void)
{
	int64_t filepos = m_nCurReadPos;
	
	filepos -= (m_VidDecodeInfo.frame_buf_size * (1+1));
	if (filepos < 0)
	{
		_lseeki64(m_hFile, 0, SEEK_SET);
		m_nCurReadPos = 0;
	}
	else
	{
		_lseeki64(m_hFile, filepos, SEEK_SET);
		m_nCurReadPos = filepos;
	}

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_ForwardNPicture(int n)
{
	int64_t filepos = m_nCurReadPos;

	filepos += m_VidDecodeInfo.frame_buf_size * (n-1);

	//if move exceed the last frame, than seek at the last frame
	if (filepos > (m_nFileTotalSize - m_VidDecodeInfo.frame_buf_size))
	{
		filepos = (m_nFileTotalSize - m_VidDecodeInfo.frame_buf_size);
	}
	filepos -= (filepos % m_VidDecodeInfo.frame_buf_size);
	_lseeki64(m_hFile, filepos, SEEK_SET);
	m_nCurReadPos = filepos;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_BackwardNPicture(int n)
{
	int64_t filepos = m_nCurReadPos;

	filepos -= m_VidDecodeInfo.frame_buf_size * (n+1);
	if (filepos < 0)
	{
		filepos = 0;
	}
	_lseeki64(m_hFile, filepos, SEEK_SET);
	m_nCurReadPos = filepos;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_LastPicture(void)
{
	int64_t filepos = m_nFileTotalSize - m_VidDecodeInfo.frame_buf_size;
	filepos -= (filepos % m_VidDecodeInfo.frame_buf_size);

	_lseeki64(m_hFile, filepos, SEEK_SET);
	m_nCurReadPos = filepos;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_FirstPicture(void)
{
	_lseeki64(m_hFile, 0, SEEK_SET);
	m_nCurReadPos = 0;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_SeekAtPercent(int nPercent)
{
	int64_t		 offset;

	offset = (int64_t)((m_nFileTotalSize / 100.0)  * nPercent);
	offset -= (offset % m_VidDecodeInfo.frame_buf_size);
	_lseeki64(m_hFile, offset, SEEK_SET);
	m_nCurReadPos = offset;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
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

void CYUV_VideoDecoder::SaveSnapshot(const char* dstfilename)
{
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






