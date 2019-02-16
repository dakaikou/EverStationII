#ifndef _VIDEO_YUV2RGB_H_
#define _VIDEO_YUV2RGB_H_

typedef enum
{
	YUV_FILE_UNKNOWN = 0,
	YUV_FILE_YUV,
	YUV_FILE_DY_DU_DV,
	YUV_FILE_DYUV,
} YUV_FILE_TYPE_e;

typedef struct
{
	int			 luma_width;
	int			 luma_height;
	//char		pszFourCC[5];
	unsigned int dwFourCC;
	double		 framerate;

	int			quantizationBits;

} YUV_SOURCE_PARAM_t;



#endif

