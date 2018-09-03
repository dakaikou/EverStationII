#ifndef __API_ES_SEGMENT_TOOLS_H__
#define __API_ES_SEGMENT_TOOLS_H__

#include <stdint.h>

#include "../Compile.h"

typedef struct
{
	char		 pszFourCC[5];
	unsigned int sync_word;
	int			 match_bytes;
	unsigned int match_mask;
	int			 min_length;

	unsigned char*  segment_start_ptr;			//valid for H264¡¢AVS
	int				segment_length;

	int				prefix_length;

	unsigned char*	nal_buf;
	int				nal_length;

} ES_segment_t, *pES_segment_t;

_CDL_EXPORT int	ES_get_next_segment(uint8_t* es_buf, int es_length, ES_segment_t* psegment);


#endif

