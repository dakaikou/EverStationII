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
	int		    display_width;
	int			display_height;
	double		display_framerate;

	int			source_chroma_format;		//CHROMA_FORMAT_MONO、CHROMA_FORMAT_4_2_0、CHROMA_FORMAT_4_2_2、CHROMA_FORMAT_4_4_4
	char		source_pszFourCC[5];

	int			source_luma_width;
	int			source_luma_height;

	int			source_quantization_bits;

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

	int		AttachWnd(HWND hWnd);
	int		DetachWnd(HWND hWnd);

protected:

	VIDEO_DECODE_Params_t	m_VidDecodeInfo;
	uint8_t*				m_pucOutputFrameBuf;

private:		//direct audio output

	//direct draw variables
	CTALForDirectDraw*		m_pDirectDraw;

protected:
public:
	
	int FeedToDirectDraw(void);
	int DirectDraw_Paint(void);
	//void GetDecodeInfo(Video_decode_info_t* pparam);
	double GetDisplayFrameRate(void);

	virtual void ToggleGrid(void);
	virtual void SaveSnapshot(const char* dstfilename);
	virtual void EnlargeVideo(void);
public:
	~CVESDecoder();
};

#endif

