#ifndef _VIDEO_YUV2RGB_H_
#define _VIDEO_YUV2RGB_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"

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
	CC_Y		= 0,		//0
	CC_Cb,					//1
	CC_Cr					//2
} CC_TYPE_e;

typedef struct
{
	S32		size;				//the byte counts for this structure
	S32		getparams;			//determine whether these parameters are usable

	S32		graph_with_grid;

	S32		chroma_format;
	S8		pszFourCC[5];

	S32		luma_width;
	S32		luma_height;
	S32		luma_buf_size;
	S32		luma_pix_count;
	U8*		pucY;

	S32		chroma_width;
	S32		chroma_height;
	S32		chroma_buf_size;
	S32		chroma_pix_count;
	U8*		pucU;
	U8*		pucV;

	S32		frame_buf_size;

	S32	    display_width;
	S32		display_height;

	S32		mb_width;								//mpeg_sequence_header & mpeg_sequence_extension	
	S32		mb_height_frame;						//mpeg_sequence_header & mpeg_sequence_extension	
	S32		mb_height_field;						//mpeg_sequence_header & mpeg_sequence_extension	
	S32		mb_count;
	S32		luma_mbw;
	S32		luma_mbh;
	S32		chroma_mbw;
	S32		chroma_mbh;

	S32		blockcount;

} Video_decode_info_t, *pVideo_decode_info_t;


#endif

