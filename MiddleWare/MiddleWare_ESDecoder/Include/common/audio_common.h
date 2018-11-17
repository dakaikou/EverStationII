#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_

#include <stdint.h>

typedef struct
{
	int		size;				//the byte counts for this structure
	int		getparams;			//determine whether these parameters are usable

	int		nch;				//channel counts
	int		sampling_rate;		//sampling rate
	int		bitspersample;		//bit counts for one sample

	int		frame_buf_size;		//frame buf length

} Audio_decode_info_t, *pAudio_decode_info_t;

typedef struct 
{
	short          wformattag;
	unsigned short wchannels;
	unsigned long  dwsamplespersec;
	unsigned long  dwavgbytespersec;
	unsigned short wblockalign;
	unsigned short wbitspersample;

} formatchunk_t;

#endif

