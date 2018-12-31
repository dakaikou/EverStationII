#ifndef __API_ES_SEGMENT_TOOLS_H__
#define __API_ES_SEGMENT_TOOLS_H__

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

#include <stdint.h>

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

MW_ES_LIB int	ES_get_next_segment(uint8_t* es_buf, int es_length, ES_segment_t* psegment);


#endif

