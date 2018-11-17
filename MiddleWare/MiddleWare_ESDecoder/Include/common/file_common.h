#ifndef _FILE_COMMON_H_
#define _FILE_COMMON_H_

#include "audio_common.h"
/*
typedef struct 
{
	short          wformattag;
	unsigned short wchannels;
	unsigned long  dwsamplespersec;
	unsigned long  dwavgbytespersec;
	unsigned short wblockalign;
	unsigned short wbitspersample;

} formatchunk_t;
*/
/*
	strcpy(id, "RIFF");
	fwrite(id, 4, sizeof(char), fp_pcm);
	data = pcm_file_size;
	fwrite(&data, 1, sizeof(int), fp_pcm);

	strcpy(id, "WAVE");
	fwrite(id, 4, sizeof(char), fp_pcm);

	strcpy(id, "fmt ");
	fwrite(id, 4, sizeof(char), fp_pcm);

	data = sizeof(formatchunk_t);
	fwrite(&data, 1, sizeof(int), fp_pcm);

	fwrite(&fmtchunk, 1, sizeof(formatchunk_t), fp_pcm);

	strcpy(id, "data");
	fwrite(id, 4, sizeof(char), fp_pcm);

	data = pcm_data_size;
	fwrite(&data, 1, sizeof(int), fp_pcm);
*/

typedef struct
{
	char		ID[4];
	int		file_size;
	char		type[4];
	char		format[4];
	
	int		length;
	union
	{
		formatchunk_t	waveFormat;
	} u;
	
	char		dataseg[4];
	int		datasize;
					
} RIFF_t;

#endif

