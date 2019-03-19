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

#include "./common/video_common.h"

#include "ESDecoder.h"

#define USE_FRAMEBUF_ACCESS_MUTEX		1	
#define USE_FRAMERATE_CONTROLL			0

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
	int			 nColorSpace;				//709\2020
	double		 framerate;

	int			 quantizationBits;			//8\10\12

} OUTPUT_YUV_SEQUENCE_PARAM_t;

//typedef enum
//{
//	CHROMA_FORMAT_MONO = 0,
//	CHROMA_FORMAT_4_2_0,
//	CHROMA_FORMAT_4_2_2,
//	CHROMA_FORMAT_4_4_4
//} CHROMA_FORMAT_e;

//typedef struct
//{
//	int			size;				//the byte counts for this structure
//	int			getparams;			//determine whether these parameters are usable
//
//	int			display_grid_size;	//0、8、16、32、64
//	int		    display_Y_width;
//	int			display_Y_height;
//	int		    display_U_width;
//	int			display_U_height;
//	int		    display_V_width;
//	int			display_V_height;
//	int			display_decimate_coeff;		//-4\-2\0\2\4
//	double		display_framerate;
//
//	int			source_chroma_format;		//CHROMA_FORMAT_MONO、CHROMA_FORMAT_4_2_0、CHROMA_FORMAT_4_2_2、CHROMA_FORMAT_4_4_4
//	//char		source_pszFourCC[5];
//	unsigned int source_FourCC;
//
//	int			source_luma_width;
//	int			source_luma_height;
//
//	int			source_bpp;
//
//	int			source_chroma_width;
//	int			source_chroma_height;
//
//	int			luma_buf_size;
//	int			luma_pix_count;
//	uint8_t*	pucY;
//
//	int			chroma_buf_size;
//	int			chroma_pix_count;
//	uint8_t*	pucU;
//	uint8_t*	pucV;
//
//	int			frame_buf_size;							//luma_buf_size + chroma_buf_size + chroma_buf_size
//
//	int			mb_width;								//mpeg_sequence_header & mpeg_sequence_extension	
//	int			mb_height_frame;						//mpeg_sequence_header & mpeg_sequence_extension	
//	int			mb_height_field;						//mpeg_sequence_header & mpeg_sequence_extension	
//	int			mb_count;
//	int			luma_mbw;
//	int			luma_mbh;
//	int			chroma_mbw;
//	int			chroma_mbh;
//
//	int			blockcount;
//
//} VIDEO_DECODE_Params_t;

class MW_ES_LIB CVESDecoder : public CESDecoder
{
public:
	CVESDecoder(void);

	int		Open(uint32_t dwStreamType, const char* pszFileName);
	int		Close(void);

	int		AttachWnd(HWND hWnd, int(*callback_luma)(HWND, WPARAM, LPARAM) = NULL, int(*callback_chroma)(HWND, WPARAM, LPARAM) = NULL);
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

	//RECT					m_rcWnd;

	INPUT_YUV_SEQUENCE_PARAM_t	m_stInputYUVSequenceParam;
	uint8_t*					m_pucInputYUVFrameBuf;
	int							m_nInputYUVFrameSize;

	OUTPUT_YUV_SEQUENCE_PARAM_t	m_stOutputYUVSequenceParam;
	uint8_t*					m_pucOutputYUVFrameBuf;
	int							m_nOutputYUVFrameSize;

	HWND	m_hwnd_for_caller;
	int(*m_callback_report_yuv_luma_stats)(HWND hWnd, WPARAM wParam, LPARAM lParam);
	int(*m_callback_report_yuv_chroma_stats)(HWND hWnd, WPARAM wParam, LPARAM lParam);

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

MW_ES_LIB int PICTURE_Enlarge(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int coeff);
MW_ES_LIB int PICTURE_Reduce(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int coeff);
MW_ES_LIB double PICTURE_psnr(uint8_t* reference, uint8_t* working, int size);

MW_ES_LIB int PICTURE_yuv2rgb(int colorSpace, uint8_t Y, uint8_t U, uint8_t V, uint8_t* R, uint8_t* G, uint8_t* B);

#endif

