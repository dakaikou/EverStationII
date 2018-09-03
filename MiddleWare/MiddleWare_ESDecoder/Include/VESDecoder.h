#ifndef __API_VES_DECODER_H__
#define __API_VES_DECODER_H__

#include <windows.h>
#include <mmsystem.h>

#include "../Compile.h"
#include "./common/video_common.h"

#include "ESDecoder.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "HAL/HAL_DirectX/Include/HALForDirectDraw.h"

class _CDL_EXPORT CVESDecoder : public CESDecoder
{
public:
	CVESDecoder(void);

	S32		OpenVideo(HWND hWnd);
	S32		CloseVideo(void);
	void		Reset(void);

protected:

	HWND					m_hVidWnd;
	Video_decode_info_t		m_VidDecodeInfo;

private:		//direct audio output

	//direct draw variables
	CHALForDirectDraw*		m_pDirectDraw;

	S32						m_nDebugFrameCount;
	U32						m_dwTimeCount;


protected:
public:
	
	S32 DirectDraw_Render_yuv(void);
	S32 DirectDraw_Paint(void);

public:
	~CVESDecoder();
};

#endif

