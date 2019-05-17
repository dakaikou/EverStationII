#ifndef __API_VES_DECODER_H__
#define __API_VES_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_ES_EXPORT
#       define MW_ES_LIB __declspec(dllexport)
#   elif defined(_MW_ES_IMPORT)
#       define MW_ES_LIB __declspec(dllimport)
#   else
#       define MW_ES_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_ES_LIB __attribute__((visibility("default")))
#else
#   define MW_ES_LIB
#endif

#include <windows.h>
#include <mmsystem.h>

#include "thirdparty_abstract_layer\TALForDirectX\Include\TALForDirectDraw.h"

#include "ESDecoder.h"

#define USE_FRAMEBUF_ACCESS_MUTEX		1	
#define USE_FRAMERATE_CONTROLL			0

#define RENDER_IN_AUTO_YUV_MODE			0

#define WM_STATISTIC_LUMA			WM_USER + 0x4bbc
#define WM_STATISTIC_CHROMA			WM_USER + 0x4645

typedef struct
{
	int			 luma_width;
	int			 luma_height;
	int			 luma_plane_size;

	int			 chroma_width;
	int			 chroma_height;
	int			 chroma_plane_size;

	unsigned int dwFourCC;					//Setup according to the  YUV File Format
	int			 nColorSpace;				//709\2020
	double		 framerate;

	int			quantizationBits;			//8\10\12

} INPUT_YUV_SEQUENCE_PARAM_t;

typedef struct
{
	int			 luma_width;
	int			 luma_height;
	int			 luma_plane_size;

	int			 chroma_width;
	int			 chroma_height;
	int			 chroma_plane_size;

	int			 alpha_width;
	int			 alpha_height;
	int			 alpha_plane_size;

	unsigned int dwFourCC;					//Limit to DirectX's ability
	int			 nColorSpace;				//NTSC\PAL\709\2020
	double		 framerate;

	int			 quantizationBits;			//8\10\12

} OUTPUT_YUV_SEQUENCE_PARAM_t;

class MW_ES_LIB CVESDecoder : public CESDecoder
{
public:
	CVESDecoder(void);

	int		Open(uint32_t dwStreamType, const char* pszFileName);
	int		Close(void);

	//int		AttachWnd(HWND hWnd, int(*callback_luma)(HWND, WPARAM, LPARAM) = NULL, int(*callback_chroma)(HWND, uint8_t*, int, int, DWORD) = NULL);
	int		AttachWnd(HWND hWnd);
	int		DetachWnd(HWND hWnd);
	int     SetClientRect(RECT rcClient);

	void    StartFrameProcessThread(void);
	void    StopFrameProcessThread(void);

	int			m_bFrameProcessControllStatus;
	int			m_bFrameProcessResponseStatus;

#if USE_FRAMEBUF_ACCESS_MUTEX
	HANDLE		m_hSourceFrameBufAccess;
	HANDLE		m_hOutputFrameBufAccess;
#endif

	HANDLE		m_hSourceFrameBufEmptyEvent;
	HANDLE		m_hSourceFrameBufFullEvent;

protected:

	INPUT_YUV_SEQUENCE_PARAM_t	m_stInputYUVSequenceParam;
	uint8_t*					m_pucInputYUVFrameBuf;
	int							m_nInputYUVFrameSize;

	OUTPUT_YUV_SEQUENCE_PARAM_t	m_stOutputYUVSequenceParam;
	uint8_t*					m_pucOutputYUVFrameBuf;
	int							m_nOutputYUVFrameSize;

	HWND	m_hwnd_for_caller;
	//int(*m_callback_report_yuv_luma_stats)(HWND hWnd, WPARAM wParam, LPARAM lParam);
	//int(*m_callback_report_yuv_chroma_stats)(HWND hWnd, uint8_t* ucFrameBuf, int width, int height, DWORD dwFourCC);

private:		//direct audio output

	//direct draw variables
	CTALForDirectDraw*		m_pDirectDraw;
	int CanvasSetup(int decimate_coeff);

protected:
	//int			m_nCanvasWidth;
	//int			m_nCanvasHeight;
	//int			m_dCanvasEnlargeCoeff;		//-4/-2/0/2/4
	int				m_decimate_coeff;
public:
	
	int DirectDraw_RePaint(void);

	int FrameProcessAndFeedToDirectDraw(void);
	//int FeedToDirectDraw(const LPBYTE lpRGBBuf, int frameSize);
	double GetDisplayFrameRate(void);
	int GetCanvasWH(int* pnwidth, int* pnheight);

	virtual void ToggleGrid(void);
	//virtual void ToggleView(void);
	virtual int CanvasEnlarge(void);
	virtual int CanvasReduce(void);
	virtual void SaveSnapshot(const char* dstfilename);
public:
	~CVESDecoder();
};

uint32_t thread_frame_process(LPVOID lpParam);


#endif

