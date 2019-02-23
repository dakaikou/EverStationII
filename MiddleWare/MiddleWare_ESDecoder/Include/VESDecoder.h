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

typedef enum
{
	CHROMA_FORMAT_MONO = 0,
	CHROMA_FORMAT_4_2_0,
	CHROMA_FORMAT_4_2_2,
	CHROMA_FORMAT_4_4_4
} CHROMA_FORMAT_e;

typedef struct
{
	int			size;				//the byte counts for this structure
	int			getparams;			//determine whether these parameters are usable

	int			display_grid_size;	//0、8、16、32、64
	int		    display_Y_width;
	int			display_Y_height;
	int		    display_U_width;
	int			display_U_height;
	int		    display_V_width;
	int			display_V_height;
	int			display_decimate_coeff;		//-4\-2\0\2\4
	double		display_framerate;

	int			source_chroma_format;		//CHROMA_FORMAT_MONO、CHROMA_FORMAT_4_2_0、CHROMA_FORMAT_4_2_2、CHROMA_FORMAT_4_4_4
	//char		source_pszFourCC[5];
	unsigned int source_FourCC;

	int			source_luma_width;
	int			source_luma_height;

	int			source_bpp;

	int			source_chroma_width;
	int			source_chroma_height;

	int			luma_buf_size;
	int			luma_pix_count;
	uint8_t*	pucY;

	int			chroma_buf_size;
	int			chroma_pix_count;
	uint8_t*	pucU;
	uint8_t*	pucV;

	int			frame_buf_size;							//luma_buf_size + chroma_buf_size + chroma_buf_size

	int			mb_width;								//mpeg_sequence_header & mpeg_sequence_extension	
	int			mb_height_frame;						//mpeg_sequence_header & mpeg_sequence_extension	
	int			mb_height_field;						//mpeg_sequence_header & mpeg_sequence_extension	
	int			mb_count;
	int			luma_mbw;
	int			luma_mbh;
	int			chroma_mbw;
	int			chroma_mbh;

	int			blockcount;

} VIDEO_DECODE_Params_t;

class MW_ES_LIB CVESDecoder : public CESDecoder
{
public:
	CVESDecoder(void);

	int		Open(uint32_t dwStreamType, const char* pszFileName, const YUV_SOURCE_PARAM_t* psource_info = NULL);
	int		Close(void);

	int		AttachWnd(HWND hWnd, int(*callback_luma)(HWND, WPARAM, LPARAM) = NULL, int(*callback_chroma)(HWND, WPARAM, LPARAM) = NULL);
	int		DetachWnd(HWND hWnd);

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

	VIDEO_DECODE_Params_t	m_VidDecodeInfo;
	uint8_t*				m_pucSourceFrameBuf;
	int						m_nSourceFrameSize;

	uint8_t*				m_pucOutputFrameBuf;
	int						m_nOutputFrameSize;
	FRAME_PARAMS_t			m_stOutputFrameParams;

	HWND	m_hwnd_for_caller;
	int(*m_callback_report_yuv_luma_stats)(HWND hWnd, WPARAM wParam, LPARAM lParam);
	int(*m_callback_report_yuv_chroma_stats)(HWND hWnd, WPARAM wParam, LPARAM lParam);

private:		//direct audio output

	//direct draw variables
	CTALForDirectDraw*		m_pDirectDraw;

protected:
	//int			m_nCanvasWidth;
	//int			m_nCanvasHeight;
	//int			m_dCanvasEnlargeCoeff;		//-4/-2/0/2/4
public:
	
	int DirectDraw_RePaint(void);

	int FrameProcessAndFeedToDirectDraw(void);
	int FeedToDirectDraw(const LPBYTE lpFrameBuf, int frameSize, const FRAME_PARAMS_t* pstFrameParams);
	double GetDisplayFrameRate(void);

	virtual void ToggleGrid(void);
	virtual void ToggleView(void);
	virtual void ToggleCanvas(void);
	virtual void SaveSnapshot(const char* dstfilename);
public:
	~CVESDecoder();
};

uint32_t thread_frame_process(LPVOID lpParam);

int PICTURE_Enlarge(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int coeff);
int PICTURE_Reduce(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int coeff);

#endif

