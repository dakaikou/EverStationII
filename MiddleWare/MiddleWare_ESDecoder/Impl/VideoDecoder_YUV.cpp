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
	m_nFrameEndPos = 0;
}

CYUV_VideoDecoder::~CYUV_VideoDecoder(void)
{
}

int CYUV_VideoDecoder::Open(uint32_t dwStreamType, const char* pszFileName, const INPUT_YUV_SEQUENCE_PARAM_t* pstYuvSequenceParam)
{
	int rtcode = ESDECODER_UNKNOWN_ERROR;

	rtcode = CVESDecoder::Open(dwStreamType, pszFileName);

	if (rtcode == ESDECODER_NO_ERROR)
	{
		if (pstYuvSequenceParam != NULL)
		{
			memcpy(&m_stInputYUVSequenceParam, pstYuvSequenceParam, sizeof(INPUT_YUV_SEQUENCE_PARAM_t));

			m_nInputYUVFrameSize = m_stInputYUVSequenceParam.luma_plane_size +
				m_stInputYUVSequenceParam.chroma_plane_size +
				m_stInputYUVSequenceParam.chroma_plane_size;			//YUV 3 plane

			m_nTotalFrameCount = (int)(m_nFileTotalSize / m_nInputYUVFrameSize);
			m_nFrameEndPos = m_nFileTotalSize - (m_nFileTotalSize % m_nInputYUVFrameSize);

			m_pucInputYUVFrameBuf = (uint8_t*)malloc(m_nInputYUVFrameSize);
			if (m_pucInputYUVFrameBuf != NULL)
			{
				memset(m_pucInputYUVFrameBuf, 0x00, m_nInputYUVFrameSize);
			}

			m_decimate_coeff = 0;

			m_stOutputYUVSequenceParam.luma_width = m_stInputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.luma_height = m_stInputYUVSequenceParam.luma_height;
			m_stOutputYUVSequenceParam.framerate = m_stInputYUVSequenceParam.framerate;
			m_stOutputYUVSequenceParam.nColorSpace = m_stInputYUVSequenceParam.nColorSpace;
			m_stOutputYUVSequenceParam.quantizationBits = m_stInputYUVSequenceParam.quantizationBits;
#if RENDER_IN_AUTO_YUV_MODE
			if ((m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '4', 'B')) ||		//YUV 4:4:4 Planar   format
				(m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('4', '4', '4', 'P'))			//YVU 4:4:4 Planar	 format
				)
			{
				//Planar format changes to Packed format
				m_stOutputYUVSequenceParam.dwFourCC = MAKEFOURCC('A', 'Y', 'U', 'V');		//Packed
			}
			else if ((m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('Y', '4', '2', 'B')) ||		//YUV 4:2:2 Planar   format
				     (m_stInputYUVSequenceParam.dwFourCC == MAKEFOURCC('4', '2', '2', 'P'))			//YVU 4:2:2 Planar	 format
				)
			{
				//Planar format changes to Packed format
				m_stOutputYUVSequenceParam.dwFourCC = MAKEFOURCC('Y', 'U', 'Y', '2');		//Packed
			}
			else
			{
				m_stOutputYUVSequenceParam.dwFourCC = m_stInputYUVSequenceParam.dwFourCC;
			}

			m_stOutputYUVSequenceParam.chroma_width = m_stInputYUVSequenceParam.chroma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stInputYUVSequenceParam.chroma_height;

			if (m_stOutputYUVSequenceParam.dwFourCC == MAKEFOURCC('A', 'Y', 'U', 'V'))
			{
				m_stOutputYUVSequenceParam.alpha_width = m_stInputYUVSequenceParam.luma_width;
				m_stOutputYUVSequenceParam.alpha_height = m_stInputYUVSequenceParam.luma_height;
			}
			else
			{
				m_stOutputYUVSequenceParam.alpha_width = 0;
				m_stOutputYUVSequenceParam.alpha_height = 0;
			}
#else
			m_stOutputYUVSequenceParam.dwFourCC = MAKEFOURCC('A', 'Y', 'U', 'V');		//Packed 4:4:4 format

			m_stOutputYUVSequenceParam.chroma_width = m_stInputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.chroma_height = m_stInputYUVSequenceParam.luma_height;

			m_stOutputYUVSequenceParam.alpha_width = m_stInputYUVSequenceParam.luma_width;
			m_stOutputYUVSequenceParam.alpha_height = m_stInputYUVSequenceParam.luma_height;

#endif
			m_stOutputYUVSequenceParam.luma_plane_size = m_stOutputYUVSequenceParam.luma_width * m_stOutputYUVSequenceParam.luma_height;
			m_stOutputYUVSequenceParam.chroma_plane_size = m_stOutputYUVSequenceParam.chroma_width * m_stOutputYUVSequenceParam.chroma_height;
			m_stOutputYUVSequenceParam.alpha_plane_size = m_stOutputYUVSequenceParam.alpha_width * m_stOutputYUVSequenceParam.alpha_height;

			m_nOutputYUVFrameSize = m_stOutputYUVSequenceParam.luma_plane_size +
				m_stOutputYUVSequenceParam.chroma_plane_size +
				m_stOutputYUVSequenceParam.chroma_plane_size +
				m_stOutputYUVSequenceParam.alpha_plane_size;

			m_pucOutputYUVFrameBuf = (uint8_t*)malloc(m_nOutputYUVFrameSize);			//RGB 3 plane
			if (m_pucOutputYUVFrameBuf != NULL)
			{
				memset(m_pucOutputYUVFrameBuf, 0x00, m_nOutputYUVFrameSize);
			}
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
					rdsize = _read(m_hFile, m_pucInputYUVFrameBuf, m_nInputYUVFrameSize);
					assert(rdsize == m_nInputYUVFrameSize);

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
	
	filepos -= (m_nInputYUVFrameSize * (1+1));
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

	filepos += m_nInputYUVFrameSize * (n-1);

	//if move exceed the last frame, than seek at the last frame
	if (filepos > (m_nFileTotalSize - m_nInputYUVFrameSize))
	{
		filepos = (m_nFileTotalSize - m_nInputYUVFrameSize);
	}
	filepos -= (filepos % m_nInputYUVFrameSize);
	_lseeki64(m_hFile, filepos, SEEK_SET);
	m_nCurReadPos = filepos;

	return Preview_Forward1Picture();
}

//implementation of virtual fuction
int CYUV_VideoDecoder::Preview_BackwardNPicture(int n)
{
	int64_t filepos = m_nCurReadPos;

	filepos -= m_nInputYUVFrameSize * (n+1);
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
	int64_t filepos = m_nFileTotalSize - m_nInputYUVFrameSize;
	filepos -= (filepos % m_nInputYUVFrameSize);

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
	offset -= (offset % m_nInputYUVFrameSize);
	_lseeki64(m_hFile, offset, SEEK_SET);
	m_nCurReadPos = offset;

	return Preview_Forward1Picture();
}

int CYUV_VideoDecoder::Preview_Picture(int nFrameNum)
{
	int64_t		 offset;

	offset = (int64_t)(nFrameNum * m_nInputYUVFrameSize);
	_lseeki64(m_hFile, offset, SEEK_SET);
	m_nCurReadPos = offset;

	return Preview_Forward1Picture();
}

int	CYUV_VideoDecoder::Preview_CurPicture(void)
{
	int64_t filepos = m_nCurReadPos;

	filepos -= m_nInputYUVFrameSize;
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






