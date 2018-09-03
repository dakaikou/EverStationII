#ifndef __API_DIRECT_SOUND_WRAPPER_H__
#define __API_DIRECT_SOUND_WRAPPER_H__

#include <windows.h>
#include <mmsystem.h>

#include "../Compile.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "thirdparty_libs/directx/include/dsound.h"

#define DS_NOTIFY_COUNT			2

typedef struct
{
	S32		nch;				//channel counts
	S32		sampling_rate;		//sampling rate
	S32		bitspersample;		//bit counts for one sample

} DSound_AudioInfo_t;

class _CDL_EXPORT CHALForDirectSound
{
public:
	CHALForDirectSound(void);

protected:

	HWND		m_hAudWnd;
	DSound_AudioInfo_t		m_AudInfo;

public:

	virtual void OpenAudio(HWND hWnd, DSound_AudioInfo_t* pAudInfo);
	virtual void CloseAudio(void);

private:		//direct audio output

	//direct draw variables
	LPDIRECTSOUND8			m_lpds; 
	DWORD					m_dwSpeakerConfig;	
	LPDIRECTSOUNDBUFFER8	m_lpBuf;
	HANDLE					m_hStopEvent[DS_NOTIFY_COUNT];

	S32			m_bPlaying;
	S32			m_nNotifyLength;
	S32			m_nWriteInnerOffset;
	S32			m_nWriteTotalOffset;
	S32			m_nWriteTotalSize;
	S32			m_nNotifyIndex;

	S32 DirectSound_AppData(LPDIRECTSOUNDBUFFER8 lpDsb,						 // The buffer.
							DWORD dwOffset,              // Our own write cursor.
							LPBYTE lpbSoundData,         // Start of our data.
							DWORD dwSoundBytes);          // Size of block to copy.
protected:
	
	S32 DirectSound_Init(void);
	S32 DirectSound_Reset(void);
	S32 DirectSound_Term(void);
public:
	S32 DirectSound_Render(U8* buf, S32 length);

public:
	~CHALForDirectSound();
};

#endif	//__API_DIRECT_SOUND_WRAPPER_H__

