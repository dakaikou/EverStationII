#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "../Include/ESDecoder.h"
#include "../Include/ESDecoder_ErrorCode.h"

CESDecoder::CESDecoder(void)
{
	m_hFile = -1;
	memset(m_pszFileName, sizeof(m_pszFileName), 0x00);
	m_dwStreamType = 0;
}

CESDecoder::~CESDecoder()
{
}

int CESDecoder::Open(uint32_t dwStreamType, const char* pszFileName)
{
	m_dwStreamType = dwStreamType;

	if (m_dwStreamType & STREAM_FILE)
	{
		strcpy_s(m_pszFileName, pszFileName);

		_sopen_s(&m_hFile, m_pszFileName, _O_BINARY | _O_RDONLY, _SH_DENYWR, 0);

		_lseeki64(m_hFile, 0, SEEK_END);
		m_nFileTotalSize = _telli64(m_hFile);

		_lseeki64(m_hFile, 0, SEEK_SET);
		m_nFileStartPos = 0;
		m_nCurReadPos = 0;
	}

	return 0;
}

int CESDecoder::Close()
{
	if (m_dwStreamType & STREAM_FILE)
	{
		if (m_hFile != -1)
		{
			_close(m_hFile);
			m_hFile = -1;
		}
		memset(m_pszFileName, sizeof(m_pszFileName), 0x00);
	}

	return 0;
}

int	CESDecoder::GetTitle(char* pszTitle, int strLen)
{
	int rtcode = ESDECODER_NO_ERROR;

	if (strlen(m_pszFileName) > 0)
	{
		strcpy_s(pszTitle, strLen, m_pszFileName);
	}
	else
	{
		rtcode = ESDECODER_UNKNOWN_ERROR;
	}

	return rtcode;
}

int CESDecoder::Preview_FirstPicture(void)
{
	return -1;
}

int CESDecoder::Preview_LastPicture(void)
{
	return -1;
}

int CESDecoder::Preview_Forward1Picture(void)
{
	return -1;
}

int CESDecoder::Preview_Backward1Picture(void)
{
	return -1;
}

int CESDecoder::Preview_ForwardNPicture(int n)
{
	return -1;
}

int CESDecoder::Preview_BackwardNPicture(int n)
{
	return -1;
}

int CESDecoder::Preview_SeekAtPercent(int nPercent)
{
	return -1;
}

int CESDecoder::Preview_beEOF(void)
{
	return -1;
}

int	CESDecoder::Preview_CurPicture(void)
{
	return -1;
}
