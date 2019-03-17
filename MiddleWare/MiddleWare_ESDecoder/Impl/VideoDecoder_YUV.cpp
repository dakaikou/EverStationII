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

int CYUV_VideoDecoder::Open(uint32_t dwStreamType, const char* pszFileName, const YUV_SEQUENCE_PARAM_t* pstYuvSequenceParam)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	rtcode = CVESDecoder::Open(dwStreamType, pszFileName);

	if (rtcode == ESDECODER_NO_ERROR)
	{
		if (pstYuvSequenceParam != NULL)
		{
			memcpy(&m_stYUVSequenceParam, pstYuvSequenceParam, sizeof(YUV_SEQUENCE_PARAM_t));

			m_nYUVFrameSize = m_stYUVSequenceParam.luma_plane_size +
				m_stYUVSequenceParam.chroma_plane_size +
				m_stYUVSequenceParam.chroma_plane_size;			//YUV 3 plane

			m_pucYUVFrameBuf = (uint8_t*)malloc(m_nYUVFrameSize);
			memset(m_pucYUVFrameBuf, 0x00, m_nYUVFrameSize);

			m_decimate_coeff = 0;

			m_stOutputPlaneParam.luma_width = m_stYUVSequenceParam.luma_width;
			m_stOutputPlaneParam.luma_height = m_stYUVSequenceParam.luma_height;
			m_stOutputPlaneParam.chroma_width = m_stYUVSequenceParam.chroma_width;
			m_stOutputPlaneParam.chroma_height = m_stYUVSequenceParam.chroma_height;
			m_stOutputPlaneParam.luma_plane_size = m_stYUVSequenceParam.luma_plane_size;
			m_stOutputPlaneParam.chroma_plane_size = m_stYUVSequenceParam.chroma_plane_size;
			m_stOutputPlaneParam.framerate = m_stYUVSequenceParam.framerate;
			m_stOutputPlaneParam.nColorSpace = m_stYUVSequenceParam.nColorSpace;
			m_stOutputPlaneParam.quantizationBits = m_stYUVSequenceParam.quantizationBits;
			m_stOutputPlaneParam.dwFourCC = m_stYUVSequenceParam.dwFourCC;

			m_nOutputPlaneSize = m_stOutputPlaneParam.luma_plane_size + 
				m_stOutputPlaneParam.chroma_plane_size + 
				m_stOutputPlaneParam.chroma_plane_size;

			m_pucOutputPlaneBuf = (uint8_t*)malloc(m_nOutputPlaneSize);			//RGB 3 plane
			memset(m_pucOutputPlaneBuf, 0x00, m_nOutputPlaneSize);

			m_nTotalFrameCount = (int)(m_nFileTotalSize / m_nYUVFrameSize);
			m_nFrameEndPos = m_nFileTotalSize - (m_nFileTotalSize % m_nYUVFrameSize);
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
					rdsize = _read(m_hFile, m_pucYUVFrameBuf, m_nYUVFrameSize);
					assert(rdsize == m_nYUVFrameSize);

					m_nCurReadPos += rdsize;

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
	
	filepos -= (m_nYUVFrameSize * (1+1));
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

	filepos += m_nYUVFrameSize * (n-1);

	//if move exceed the last frame, than seek at the last frame
	if (filepos > (m_nFileTotalSize - m_nYUVFrameSize))
	{
		filepos = (m_nFileTotalSize - m_nYUVFrameSize);
	}
	filepos -= (filepos % m_nYUVFrameSize);
	_lseeki64(m_hFile, filepos, SEEK_SET);
	m_nCurReadPos = filepos;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_BackwardNPicture(int n)
{
	int64_t filepos = m_nCurReadPos;

	filepos -= m_nYUVFrameSize * (n+1);
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
	int64_t filepos = m_nFileTotalSize - m_nYUVFrameSize;
	filepos -= (filepos % m_nYUVFrameSize);

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
	offset -= (offset % m_nYUVFrameSize);
	_lseeki64(m_hFile, offset, SEEK_SET);
	m_nCurReadPos = offset;

	return Preview_Forward1Picture();
}

int CYUV_VideoDecoder::Preview_Picture(int nFrameNum)
{
	int64_t		 offset;

	offset = (int64_t)(nFrameNum * m_nYUVFrameSize);
	_lseeki64(m_hFile, offset, SEEK_SET);
	m_nCurReadPos = offset;

	return Preview_Forward1Picture();
}

int	CYUV_VideoDecoder::Preview_CurPicture(void)
{
	int64_t filepos = m_nCurReadPos;

	filepos -= m_nYUVFrameSize;
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






