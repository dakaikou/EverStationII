#ifndef __API_DIRECT_SOUND_WRAPPER_H__
#define __API_DIRECT_SOUND_WRAPPER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _TAL_DIRECTX_EXPORT
#       define TAL_DIRECTX_LIB __declspec(dllexport)
#   elif defined(_TAL_DIRECTX_IMPORT)
#       define TAL_DIRECTX_LIB __declspec(dllimport)
#   else
#       define TAL_DIRECTX_LIB
#   endif
#elif __GNUC__ >= 4
#   define TAL_DIRECTX_LIB __attribute__((visibility("default")))
#else
#   define TAL_DIRECTX_LIB
#endif

#include <windows.h>
#include <mmsystem.h>
#include <stdint.h>

#include "thirdparty_libs/directx/include/dsound.h"

#define DS_NOTIFY_COUNT			2

typedef struct
{
	int		nch;				//channel counts
	int		sampling_rate;		//sampling rate
	int		bitspersample;		//bit counts for one sample

} DSound_AudioInfo_t;

class TAL_DIRECTX_LIB CTALForDirectSound
{
public:
	CTALForDirectSound(void);

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

	int			m_bPlaying;
	int			m_nNotifyLength;
	int			m_nWriteInnerOffset;
	int			m_nWriteTotalOffset;
	int			m_nWriteTotalSize;
	int			m_nNotifyIndex;

	int DirectSound_AppData(LPDIRECTSOUNDBUFFER8 lpDsb,						 // The buffer.
							DWORD dwOffset,              // Our own write cursor.
							LPBYTE lpbSoundData,         // Start of our data.
							DWORD dwSoundBytes);          // Size of block to copy.
protected:
	
	int DirectSound_Init(void);
	int DirectSound_Reset(void);
	int DirectSound_Term(void);
public:
	int DirectSound_Render(uint8_t* buf, int length);

public:
	~CTALForDirectSound();
};

#endif	//__API_DIRECT_SOUND_WRAPPER_H__

