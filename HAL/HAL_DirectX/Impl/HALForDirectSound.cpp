#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "../Include/HALForDirectSound.h"

CHALForDirectSound::CHALForDirectSound(void)
{
	m_hAudWnd = NULL;
	
	//direct audio settings
	m_lpBuf = NULL;
	m_lpds = NULL;

	//memset(&m_AudDecodeInfo, 0x00, sizeof(Audio_decode_info_t));

	//m_AudDecodeInfo.size = sizeof(Audio_decode_info_t);
	//m_AudDecodeInfo.getparams = 0;
}

CHALForDirectSound::~CHALForDirectSound()
{
}

void CHALForDirectSound::OpenAudio(HWND hWnd, DSound_AudioInfo_t* pAudInfo)
{
	m_hAudWnd = hWnd;

	if (pAudInfo != NULL)		//initialize direct audio only when parameters are successfully got
	{
		memcpy(&m_AudInfo, pAudInfo, sizeof(DSound_AudioInfo_t));
		DirectSound_Init();
	}
}

void CHALForDirectSound::CloseAudio(void)
{
	//if (m_AudDecodeInfo.getparams == 1)		//initialize direct audio only when parameters are successfully got
	//{
		DirectSound_Term();

	//	m_AudDecodeInfo.getparams = 0;
	//}
}

static HRESULT DirectSound_SetNotification(HANDLE* hStopEvent, LPDIRECTSOUNDBUFFER8 ppDsb8, int nNotifyCount, int nNotifyLength)
{
	HRESULT					hr = NULL;

    DSBPOSITIONNOTIFY*		aPosNotify     = NULL; 
    LPDIRECTSOUNDNOTIFY		pDSNotify      = NULL;

	hr = ppDsb8->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&pDSNotify);

	if (SUCCEEDED(hr))
	{ 
	    aPosNotify = new DSBPOSITIONNOTIFY[nNotifyCount];
		if (aPosNotify != NULL)
		{
			for (int i = 0; i < nNotifyCount; i++)
			{
				aPosNotify[i].dwOffset     = nNotifyLength * (i + 1) - 1;
				aPosNotify[i].hEventNotify = hStopEvent[i];             
			}
			hr = pDSNotify->SetNotificationPositions(nNotifyCount, aPosNotify);

			pDSNotify->Release();

			delete aPosNotify;
		}
	}

	return hr;
}

BOOL CHALForDirectSound::DirectSound_AppData(LPDIRECTSOUNDBUFFER8 lpDsb,						 // The buffer.
									DWORD dwOffset,              // Our own write cursor.
									LPBYTE lpbSoundData,         // Start of our data.
									DWORD dwSoundBytes)          // Size of block to copy.
{ 

//#if SAVE_META_DATA_TO_DISK
#if 0

	if (fp_pcm != NULL)
	{
		fwrite(lpbSoundData, dwSoundBytes, sizeof(char), fp_pcm);
	}
#endif


	LPVOID   lpvPtr1; 
	DWORD	 dwBytes1; 
	LPVOID	 lpvPtr2; 
	DWORD	 dwBytes2; 
	HRESULT	 hr; 

	// Obtain memory address of write block. This will be in two parts
	// if the block wraps around.
	
	hr = lpDsb->Lock(dwOffset, dwSoundBytes, &lpvPtr1, &dwBytes1, &lpvPtr2, &dwBytes2, 0); 

//	TRACE("DirectSound: dwOffset = %d, dwBytes1 = %d, dwBytes2 = %d, dwSoundBytes = %d, dwBytes1 + dwBytes2 = %d\n",
//		dwOffset, dwBytes1, dwBytes2, dwSoundBytes, dwBytes1 + dwBytes2);
 
	// If the buffer was lost, restore and retry lock. 
 
	if (DSERR_BUFFERLOST == hr) 
	{ 
		lpDsb->Restore(); 
		hr = lpDsb->Lock(dwOffset, dwSoundBytes, 
							&lpvPtr1, &dwBytes1,
							&lpvPtr2, &dwBytes2, 0); 
	} 
	if (SUCCEEDED(hr)) 
	{ 
		// Write to pointers. 
 
		CopyMemory(lpvPtr1, lpbSoundData, dwBytes1); 
		if (NULL != lpvPtr2) 
		{ 
			CopyMemory(lpvPtr2, lpbSoundData + dwBytes1, dwBytes2); 
		} 
 
		// Release the data back to DirectSound. 
 
		hr = lpDsb->Unlock(lpvPtr1, dwBytes1, lpvPtr2, dwBytes2); 
		if (SUCCEEDED(hr)) 
		{ 
			// Success. 
			return TRUE; 
		} 
	} 
	// Lock, Unlock, or Restore failed. 
 
	return FALSE; 
} 

S32 CHALForDirectSound::DirectSound_Init(void)
{
//	DSCAPS					 dscaps; 
	WAVEFORMATEX			 wfx; 
	DSBUFFERDESC			 dsbdesc; 
	LPDIRECTSOUNDBUFFER		 pDsb = NULL;
	int						 i;

	//check the parameters

	// Set up WAV format structure. 

	memset(&wfx, 0, sizeof(WAVEFORMATEX)); 
	wfx.wFormatTag = WAVE_FORMAT_PCM; 
	wfx.nChannels = m_AudInfo.nch; 
	wfx.nSamplesPerSec = m_AudInfo.sampling_rate;
	wfx.wBitsPerSample = m_AudInfo.bitspersample;
	wfx.nBlockAlign = (wfx.wBitsPerSample * wfx.nChannels) / 8; 
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign; 

	for (i = 0; i < DS_NOTIFY_COUNT; i++)
	{
		m_hStopEvent[i] = ::CreateEvent(NULL, TRUE, TRUE, NULL);			//intial state: manual reset mode, signaled
	}

	HRESULT hr = DirectSoundCreate8(NULL, &m_lpds, NULL);
	
	if (hr == DS_OK)
	{
		hr = m_lpds->SetCooperativeLevel(m_hAudWnd, DSSCL_PRIORITY);
		if (FAILED(hr))
		{
			// Add error-handling here.
//			TRACE("DirectSound SetCooperativeLevel Error.\n");
		}
/*
		dscaps.dwSize = sizeof(DSCAPS); 
		hr = m_lpds->GetCaps(&dscaps); 
		if (FAILED(hr))
		{
			TRACE("DirectSound GetCaps Error.\n");
		}
*/
/*
		hr = m_lpds->GetSpeakerConfig(&m_dwSpeakerConfig);
		if (FAILED(hr))
		{
			// Add error-handling here.
			TRACE("DirectSound GetSpeakerConfig Error.\n");
		}
		else
		{
			switch (m_dwSpeakerConfig)
			{
			case DSSPEAKER_5POINT1:
				TRACE("DirectSound SpeakerConfig: 5.1\n");
				break;
			case DSSPEAKER_DIRECTOUT:
				TRACE("DirectSound SpeakerConfig: DirectOut\n");
				break;
			case DSSPEAKER_HEADPHONE:
				TRACE("DirectSound SpeakerConfig: Headphone\n");
				break;
			case DSSPEAKER_MONO:
				TRACE("DirectSound SpeakerConfig: mono\n");
				break;
			case DSSPEAKER_QUAD:
				TRACE("DirectSound SpeakerConfig: quad\n");
				break;
			case DSSPEAKER_STEREO:
				TRACE("DirectSound SpeakerConfig: stereo\n");
				break;
			case DSSPEAKER_SURROUND:
				TRACE("DirectSound SpeakerConfig: surround\n");
				break;
			case DSSPEAKER_7POINT1:
				TRACE("DirectSound SpeakerConfig: 7.1\n");
				break;
			default:
				TRACE("DirectSound SpeakerConfig: Unknown\n");
				break;
			}
		}

*/
		// Set up DSBUFFERDESC structure. 
		memset(&dsbdesc, 0, sizeof(DSBUFFERDESC)); 
		dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
		dsbdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2;
		dsbdesc.dwBufferBytes = DS_NOTIFY_COUNT * wfx.nAvgBytesPerSec; 
		dsbdesc.lpwfxFormat = &wfx; 

		// Create buffer. 

		hr = m_lpds->CreateSoundBuffer(&dsbdesc, &pDsb, NULL); 
		if (SUCCEEDED(hr)) 
		{ 
			hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)(&m_lpBuf));
			pDsb->Release();

			if (FAILED(hr))
			{
				// Add error-handling here.
//				TRACE("DirectSound CreateBasicBuffer Error.\n");
			}
			else
			{
				m_nNotifyLength = wfx.nAvgBytesPerSec;
				m_nWriteTotalSize = DS_NOTIFY_COUNT * wfx.nAvgBytesPerSec;

				hr = DirectSound_SetNotification(m_hStopEvent, m_lpBuf, DS_NOTIFY_COUNT, m_nNotifyLength);
				if (SUCCEEDED(hr)) 
				{ 
//					TRACE("DirectSound SetStopNotification Success.\n");

					m_nWriteInnerOffset = 0;
					m_nWriteTotalOffset = 0;
					m_bPlaying = 0;
					m_nNotifyIndex = 0;
				}
			}
		}
	}
	else
	{
		switch (hr)
		{
		case DSERR_NODRIVER:
			MessageBox(m_hAudWnd, (LPCWSTR)"没有正确安装声卡驱动", (LPCWSTR)"DirectSound Error", MB_OK);
			break;
		default:
			// Add error-handling here.
			MessageBox(m_hAudWnd, (LPCWSTR)"DirectSound Create Error", (LPCWSTR)"DirectSound Error", MB_OK);
			break;
		}
	}

#if SAVE_META_DATA_TO_DISK
	
	char	pszExeFile[MAX_PATH];
	char	exeDrive[3];
	char	pszAppTempPath[MAX_PATH];
	char	pszMediaPath[MAX_PATH];
	char	pszMediaFile[MAX_PATH];
	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	exeDrive[0] = pszExeFile[0];
	exeDrive[1] = pszExeFile[1];
	exeDrive[2] = '\0';
	sprintf_s(pszAppTempPath, sizeof(pszAppTempPath), "%s\\~EverStationII", exeDrive);
	sprintf_s(pszMediaPath, sizeof(pszMediaPath), "%s\\media", pszAppTempPath);
	::CreateDirectoryA(pszAppTempPath, NULL);
	::CreateDirectoryA(pszDbasePath, NULL);

	sprintf_s(pszMediaFile, sizeof(pszMediaFile), "%s\\pcm.wav", pszMediaPath);

	fp_pcm = fopen(pszMediaFile, "wb");
	fseek(fp_pcm, 0, SEEK_SET);

	fmtchunk.wformattag = wfx.wFormatTag;
	fmtchunk.wchannels = wfx.nChannels;
	fmtchunk.dwsamplespersec = wfx.nSamplesPerSec;
	fmtchunk.dwavgbytespersec = wfx.nAvgBytesPerSec;
	fmtchunk.wblockalign = wfx.nBlockAlign;
	fmtchunk.wbitspersample = wfx.wBitsPerSample;

	pcm_file_size = 0;
	pcm_data_size = 0;

#endif

	return 0;
}

S32 CHALForDirectSound::DirectSound_Reset(void)
{
//	CMainFrame*	pFrame = (CMainFrame*)AfxGetMainWnd();

//	if (pFrame != NULL)
//	{
//		if (pFrame->m_bMPEGAudioOutputOpened == TRUE)
//		{
//			DirectSound_Term();
//			DirectSound_Init(hWnd, paudio_header);
//		}
//	}

	return 0;
}

S32 CHALForDirectSound::DirectSound_Term(void)
{
	int	 i;

	if (m_bPlaying)
	{
		m_bPlaying = 0;

		if (m_lpBuf != NULL)
		{
			m_lpBuf->Stop();
		}
	}

	if (m_lpBuf != NULL)
	{
		m_lpBuf->Release();
		m_lpBuf = NULL;
	}

	if (m_lpds != NULL)
	{
		m_lpds->Release();
		m_lpds = NULL;
	}

	for (i = 0; i < DS_NOTIFY_COUNT; i++)
	{
		::CloseHandle(m_hStopEvent[i]);
		m_hStopEvent[i] = NULL;
	}

#if SAVE_META_DATA_TO_DISK
	
	char		 id[5];
	unsigned int data;

	pcm_data_size = ftell(fp_pcm) - 44;									//减掉44个字节
	pcm_file_size = pcm_data_size + 8 + 8 + sizeof(formatchunk_t);

	fseek(fp_pcm, 0, SEEK_SET);

	strcpy_s(id, "RIFF");
	fwrite(id, 4, sizeof(char), fp_pcm);
	data = pcm_file_size;
	fwrite(&data, 1, sizeof(int), fp_pcm);

	strcpy_s(id, "WAVE");
	fwrite(id, 4, sizeof(char), fp_pcm);

	strcpy_s(id, "fmt ");
	fwrite(id, 4, sizeof(char), fp_pcm);

	data = sizeof(formatchunk_t);
	fwrite(&data, 1, sizeof(int), fp_pcm);

	fwrite(&fmtchunk, 1, sizeof(formatchunk_t), fp_pcm);

	strcpy_s(id, "data");
	fwrite(id, 4, sizeof(char), fp_pcm);

	data = pcm_data_size;
	fwrite(&data, 1, sizeof(int), fp_pcm);

	fclose(fp_pcm);
	fp_pcm = NULL;
#endif

	return 0;
}

S32 CHALForDirectSound::DirectSound_Render(U8* buf, S32 length)
{
	int				write_length = length;
	int				left_length;
	unsigned char*	wrptr = buf;

#if SAVE_META_DATA_TO_DISK

	if (fp_pcm != NULL)
	{
		fwrite(buf, length, sizeof(char), fp_pcm);
	}

#endif

	if (m_lpBuf != NULL)
	{
		do
		{
			::WaitForSingleObject(m_hStopEvent[m_nNotifyIndex], INFINITE);

			left_length = m_nNotifyLength - m_nWriteInnerOffset;
			if (write_length >= left_length)		//beyound the left notify size
			{
				DirectSound_AppData(m_lpBuf, m_nWriteTotalOffset, wrptr, left_length);

				wrptr += left_length;
				m_nWriteTotalOffset += left_length;
				m_nWriteTotalOffset %= m_nWriteTotalSize;
				m_nWriteInnerOffset = 0;

				::ResetEvent(m_hStopEvent[m_nNotifyIndex]);

				m_nNotifyIndex ++;
				m_nNotifyIndex %= DS_NOTIFY_COUNT;

				write_length -= left_length;
			}
			else
			{
				DirectSound_AppData(m_lpBuf, m_nWriteTotalOffset, wrptr, write_length);

				m_nWriteInnerOffset += write_length;
				m_nWriteTotalOffset += write_length;
				write_length = 0;
			}

			if (!m_bPlaying)
			{
				m_lpBuf->SetCurrentPosition(0);
				m_lpBuf->Play(0, 0, DSBPLAY_LOOPING);

				m_bPlaying = 1;
			}

//			TRACE("DirectAudio: running....notify_index = %d\n", m_nNotifyIndex);

		} while (write_length > 0);
	}

	return 0;
}


