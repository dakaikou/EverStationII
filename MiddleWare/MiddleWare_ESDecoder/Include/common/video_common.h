#ifndef _VIDEO_YUV2RGB_H_
#define _VIDEO_YUV2RGB_H_

#define	VID_YUV2RGB_FastAlgorithm			1

typedef enum
{
	CHROMA_FORMAT_MONO			= 0,
	CHROMA_FORMAT_4_2_0,
	CHROMA_FORMAT_4_2_2,
	CHROMA_FORMAT_4_4_4
} CHROMA_FORMAT_e;

typedef enum
{
	YUV_FILE_UNKNOWN = 0,
	YUV_FILE_YUV,
	YUV_FILE_DY_DU_DV,
	YUV_FILE_DYUV,
} YUV_FILE_TYPE_e;

typedef enum
{
	CC_Y		= 0,		//0
	CC_Cb,					//1
	CC_Cr					//2
} CC_TYPE_e;

typedef struct
{
	int			size;				//the byte counts for this structure
	int			getparams;			//determine whether these parameters are usable

	int			graph_with_grid;

	int			chroma_format;
	char		pszFourCC[5];

	double		framerate;

	int			luma_width;
	int			luma_height;
	int			luma_buf_size;
	int			luma_pix_count;
	//uint8_t*	pucY;

	int			chroma_width;
	int			chroma_height;
	int			chroma_buf_size;
	int			chroma_pix_count;
	//uint8_t*	pucU;
	//uint8_t*	pucV;

	int			frame_buf_size;

	int		    display_width;
	int			display_height;

	int			mb_width;								//mpeg_sequence_header & mpeg_sequence_extension	
	int			mb_height_frame;						//mpeg_sequence_header & mpeg_sequence_extension	
	int			mb_height_field;						//mpeg_sequence_header & mpeg_sequence_extension	
	int			mb_count;
	int			luma_mbw;
	int			luma_mbh;
	int			chroma_mbw;
	int			chroma_mbh;

	int			blockcount;

} Video_decode_info_t, *pVideo_decode_info_t;


#endif

