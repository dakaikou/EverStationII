#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>

#include "MiddleWare_Tools/Include/MiddleWare_Tools.h"
#include "../Include/AudioDecoder_WAVE.h"

CWAVE_AudioDecoder::CWAVE_AudioDecoder(void)
{
	av_es_parse = NULL;
	m_pucSampleBuf = NULL;
}

CWAVE_AudioDecoder::~CWAVE_AudioDecoder(void)
{
	if (m_pucSampleBuf != NULL)
	{
		free(m_pucSampleBuf);
	}
}

S32	CWAVE_AudioDecoder::Preview_Init(void)
{
	S32				rtcode = 0;
	RIFF_t			RIFF;

	if (m_hFile != NULL)
	{
		_lseek(m_hFile, 0, SEEK_SET);
		_read(m_hFile, &RIFF, sizeof(RIFF_t));

		m_nFileStartPos = _tell(m_hFile);

		memcpy(&m_waveHeader, &(RIFF.u.waveFormat), sizeof(formatchunk_t));

		::SendMessage(m_hMsgWnd, WM_UPDATE_WAVE_HEADER, (WPARAM)&m_waveHeader, NULL);
	}
	else
	{
		rtcode = 1;
	}

	return rtcode;
}

S32	CWAVE_AudioDecoder::Preview_FirstFrame(void)
{
	if (m_hFile != NULL)
	{
		_lseek(m_hFile, m_nFileStartPos, SEEK_SET);
	}

	return Preview_GetFileRatio();
}

S32	CWAVE_AudioDecoder::Preview_NextFrame(int mute)
{
	S32				rdsize;

	if (m_hFile != NULL)
	{
		if (mute)
		{
			_lseek(m_hFile, m_waveHeader.dwavgbytespersec, SEEK_CUR);
		}
		else
		{
			if (m_pucSampleBuf != NULL)
			{
				rdsize = _read(m_hFile, m_pucSampleBuf, m_waveHeader.dwavgbytespersec);
				if (rdsize > 0)
				{
					if (m_AudDecodeInfo.getparams)
					{
						CAESDecoder::DirectSound_Render(m_pucSampleBuf, rdsize);
					}
				}
			}
		}
	}

	return Preview_GetFileRatio();
}

S32 CWAVE_AudioDecoder::Preview_AtPercent(int nPercent)
{
	Preview_SetFileRatio(nPercent);

	return Preview_GetFileRatio();
}

S32	CWAVE_AudioDecoder::Open(HWND hMsgWnd, S32 nStreamType, S8* pszFileName)
{
	CESDecoder::Open(hMsgWnd, nStreamType, pszFileName);

	if (m_nStreamType & STREAM_FILE)
	{
		Preview_Init();
	}

	return 0;
}

S32	CWAVE_AudioDecoder::Close(void)
{
	return CESDecoder::Close();
}

void CWAVE_AudioDecoder::OpenAudio(HWND hWnd)
{
	assert(m_AudDecodeInfo.getparams == 0);

	m_AudDecodeInfo.nch = m_waveHeader.wchannels;
	m_AudDecodeInfo.sampling_rate = m_waveHeader.dwsamplespersec;
	m_AudDecodeInfo.bitspersample = m_waveHeader.wbitspersample;

	m_AudDecodeInfo.getparams = 1;

	m_pucSampleBuf = (U8*)malloc(m_waveHeader.dwavgbytespersec);

	CAESDecoder::OpenAudio(hWnd);
}

void CWAVE_AudioDecoder::CloseAudio(void)
{
	assert(m_AudDecodeInfo.getparams == 1);

	if (m_pucSampleBuf != NULL)
	{
		free(m_pucSampleBuf);
		m_pucSampleBuf = NULL;
	}

	CAESDecoder::CloseAudio();
}


