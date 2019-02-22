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
	int		percent = 100;

	if (m_nCurReadPos < m_nFileTotalSize)
	{
		if (m_dwStreamType == (STREAM_FILE | YUV_FILE_YUV))
		{
#if USE_FRAMEBUF_ACCESS_MUTEX
			uint32_t wait_state = ::WaitForSingleObject(m_hFrameBufAccess, INFINITE);
			if (wait_state == WAIT_OBJECT_0)
			{
#endif
			rdsize = _read(m_hFile, m_pucSourceFrameBuf, m_VidDecodeInfo.frame_buf_size);
			if (rdsize < m_VidDecodeInfo.frame_buf_size)
			{
				assert(0);
			}

			m_nCurReadPos += m_VidDecodeInfo.frame_buf_size;

			percent = (int)(m_nCurReadPos * 100.0 / m_nFileTotalSize);

			m_bSourceDataAvailable = 1;

#if USE_FRAMEBUF_ACCESS_MUTEX
			::ReleaseMutex(m_hFrameBufAccess);
			}
#endif
		}
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

int	CYUV_VideoDecoder::Preview_CurPicture(void)
{
	int64_t filepos = m_nCurReadPos;

	filepos -= m_VidDecodeInfo.frame_buf_size;
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

//void DecodeFourCC2Text(char* pszFourCC, char* pszText, int size)
//{
//	if ((pszFourCC != NULL) && (pszText != NULL))
//	{
//		if (strcmp(pszFourCC, "IYUV") == 0)
//		{
//			strcpy_s(pszText, size, "YUV4:2:0	Y-U-V");
//		}
//		else if (strcmp(pszFourCC, "YV12") == 0)
//		{
//			strcpy_s(pszText, size, "YUV4:2:0	Y-V-U");
//		}
//		else if (strcmp(pszFourCC, "YUY2") == 0)
//		{
//			strcpy_s(pszText, size, "YUV4:2:2	Y-U-Y-V");
//		}
//		else if (strcmp(pszFourCC, "YV16") == 0)
//		{
//			strcpy_s(pszText, size, "YUV4:2:2	Y-V-U");
//		}
//		else
//		{
//			strcpy_s(pszText, size, "");
//		}
//	}
//}






