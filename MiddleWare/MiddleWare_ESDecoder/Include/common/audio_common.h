#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"

typedef struct
{
	S32		size;				//the byte counts for this structure
	S32		getparams;			//determine whether these parameters are usable

	S32		nch;				//channel counts
	S32		sampling_rate;		//sampling rate
	S32		bitspersample;		//bit counts for one sample

	S32		frame_buf_size;		//frame buf length

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

