#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "../Include/ESDecoder_ErrorCode.h"
#include "../Include/VideoDecoder_YUV.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"

CYUV_VideoDecoder::CYUV_VideoDecoder(void)
{
	m_nTotalFrameCount = 0;
}

CYUV_VideoDecoder::~CYUV_VideoDecoder(void)
{
}

int CYUV_VideoDecoder::Open(uint32_t dwStreamType, const char* pszFileName, const YUV_SERIAL_PARAM_t* pYuvSerialParam)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	rtcode = CVESDecoder::Open(dwStreamType, pszFileName);

	if (rtcode == ESDECODER_NO_ERROR)
	{
		if (pYuvSerialParam != NULL)
		{
			m_VidDecodeInfo.size = sizeof(VIDEO_DECODE_Params_t);

			//information got from caller

			m_VidDecodeInfo.source_FourCC = pYuvSerialParam->dwFourCC;
			//strcpy_s(m_VidDecodeInfo.source_pszFourCC, sizeof(m_VidDecodeInfo.source_pszFourCC), psourceInfo->pszFourCC);

			m_VidDecodeInfo.source_luma_width = pYuvSerialParam->luma_width;
			m_VidDecodeInfo.source_luma_height = pYuvSerialParam->luma_height;
			m_VidDecodeInfo.source_chroma_width = pYuvSerialParam->chroma_width;
			m_VidDecodeInfo.source_chroma_height = pYuvSerialParam->chroma_height;

			m_VidDecodeInfo.source_bpp = pYuvSerialParam->quantizationBits;

			//calculation
			m_VidDecodeInfo.luma_pix_count = m_VidDecodeInfo.source_luma_width * m_VidDecodeInfo.source_luma_height;
			m_VidDecodeInfo.luma_buf_size = m_VidDecodeInfo.luma_pix_count * m_VidDecodeInfo.source_bpp / 8;
			m_VidDecodeInfo.chroma_pix_count = m_VidDecodeInfo.source_chroma_width * m_VidDecodeInfo.source_chroma_height;
			m_VidDecodeInfo.chroma_buf_size = m_VidDecodeInfo.chroma_pix_count * m_VidDecodeInfo.source_bpp / 8;

			m_VidDecodeInfo.frame_buf_size = m_VidDecodeInfo.luma_buf_size + m_VidDecodeInfo.chroma_buf_size + m_VidDecodeInfo.chroma_buf_size;

			//display parameters
			m_VidDecodeInfo.display_framerate = pYuvSerialParam->framerate;

			m_VidDecodeInfo.display_Y_width = pYuvSerialParam->luma_width;
			m_VidDecodeInfo.display_Y_height = pYuvSerialParam->luma_height;

			m_VidDecodeInfo.display_U_width = m_VidDecodeInfo.source_chroma_width;
			m_VidDecodeInfo.display_U_height = m_VidDecodeInfo.source_chroma_height;

			m_VidDecodeInfo.display_V_width = m_VidDecodeInfo.source_chroma_width;
			m_VidDecodeInfo.display_V_height = m_VidDecodeInfo.source_chroma_height;

			m_VidDecodeInfo.display_decimate_coeff = 0;

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

			m_nSourceFrameSize = m_VidDecodeInfo.frame_buf_size;
			m_pucSourceFrameBuf = (uint8_t*)malloc(m_nSourceFrameSize);
			memset(m_pucSourceFrameBuf, 0x00, m_nSourceFrameSize);

			m_nTotalFrameCount = (int)(m_nFileTotalSize / m_VidDecodeInfo.frame_buf_size);
			m_nFrameEndPos = m_nFileTotalSize - (m_nFileTotalSize % m_VidDecodeInfo.frame_buf_size);
		}
		else
		{
			rtcode = ESDECODER_PARAMETER_ERROR;
		}
	}

	return rtcode;
}

int	CYUV_VideoDecoder::GetFrameCount(void)
{
	return m_nTotalFrameCount;
}

//implementation of virtual fuction
int	CYUV_VideoDecoder::Preview_beEOF(void)
{
	int	bEOF = (m_nCurReadPos >= m_nFrameEndPos) ? TRUE : FALSE;

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
			uint32_t wait_state0 = ::WaitForSingleObject(m_hSourceFrameBufEmptyEvent, INFINITE);
			if (wait_state0 == WAIT_OBJECT_0)
			{
				::ResetEvent(m_hSourceFrameBufEmptyEvent);

//#if USE_FRAMEBUF_ACCESS_MUTEX
//				uint32_t wait_state = ::WaitForSingleObject(m_hSourceFrameBufAccess, INFINITE);
//				if (wait_state == WAIT_OBJECT_0)
//				{
//#endif
					rdsize = _read(m_hFile, m_pucSourceFrameBuf, m_VidDecodeInfo.frame_buf_size);
					if (rdsize < m_VidDecodeInfo.frame_buf_size)
					{
						assert(0);
					}

					m_nCurReadPos += m_VidDecodeInfo.frame_buf_size;

					percent = (int)(m_nCurReadPos * 100.0 / m_nFileTotalSize);

					//m_bSourceDataAvailable = 1;

//#if USE_FRAMEBUF_ACCESS_MUTEX
//					::ReleaseMutex(m_hSourceFrameBufAccess);
//				}
//#endif

				::SetEvent(m_hSourceFrameBufFullEvent);
			}
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

int CYUV_VideoDecoder::Preview_Picture(int nFrameNum)
{
	int64_t		 offset;

	offset = (int64_t)(nFrameNum * m_VidDecodeInfo.frame_buf_size);
	//offset -= (offset % m_VidDecodeInfo.frame_buf_size);
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






