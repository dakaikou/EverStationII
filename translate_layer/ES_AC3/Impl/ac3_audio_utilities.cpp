#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <assert.h>
#include <io.h>
#include <fcntl.h>

#include "../Include/ac3_audio_utilities.h"
#include "./kernel/AC3_audio_out_internal.h"


#define RESERVE				 -1
#define FREE_BITRATE		 0

const static int LUT_ac3_bitrate[] = 
{ 
	 32000,  40000,  48000,  56000,  64000,  80000,  96000, 112000,
	128000, 160000, 192000, 224000, 256000, 320000, 384000, 448000,
	512000, 576000, 640000
};

const static int LUT_ac3_sampling_rate[4] =
{
	48000, 44100, 32000, RESERVE
};

const static int LUT_ac3_frame_size[4][64] =
{
	{  
		64,  64,  80,  80,  96,  96, 112, 112, 128, 128, 160, 160, 192, 192, 224, 224,  
	   256, 256, 320, 320, 384, 384, 448, 448, 512, 512, 640, 640, 768, 768, 896, 896, 
	   1024, 1024, 1152, 1152, 1280, 1280
	},
	{  
		69,  70,  87,  88, 104, 105, 121, 122, 139, 140, 174, 175, 208, 209, 243, 244,
       278, 279, 348, 349, 417, 418, 487, 488, 557, 558, 696, 697, 835, 836, 975, 976, 
	  1114, 1115, 1253, 1254, 1393, 1394
	},
	{  
		96,  96, 120, 120, 144, 144, 168, 168, 192, 192, 240, 240, 288, 288, 336, 336,
       384, 384, 480, 480, 576, 576, 672, 672, 768, 768, 960, 960, 1152, 1152, 1344, 1344, 
	  1536, 1536, 1728, 1728, 1920, 1920
	},
	{  
		0
	},
};

const static int LUT_ac3_nfchans[8] =
{
    2, 1, 2, 3, 3, 4, 4, 5
};


int AC3_DecodeSamplingFrequency(uint8_t fscod, char* pszText, int size)
{
	int frequency = LUT_ac3_sampling_rate[fscod];

	if (pszText != NULL)
	{
		if (frequency == -1)
		{
			strcpy_s(pszText, size, "reserved");
		}
		else
		{
			sprintf_s(pszText, size, "%6.3f kHz", frequency / 1000.0);
		}
	}

	return frequency;
}

int	AC3_DecodeFrameSize(uint8_t frsizecod, uint8_t fscod, char* pszText, int size)
{
	int		framesize = 0;

	if (pszText)
	{
		framesize = LUT_ac3_frame_size[fscod][frsizecod];
		sprintf_s(pszText, size, "%d words/syncframe", framesize);
	}

	return framesize;
}

int AC3_DecodeBitStreamMode(uint8_t bsmod, uint8_t acmod, char* pszText, int size)
{
	if (pszText != NULL)
	{
		switch (bsmod)
		{
		case 0:
			strcpy_s(pszText, size, "main audio service: complete main (CM)");
			break;
		case 1:
			strcpy_s(pszText, size, "main audio service: music and effects (ME)");
			break;
		case 2:
			strcpy_s(pszText, size, "associated service: visually impaired (VI)");
			break;
		case 3:
			strcpy_s(pszText, size, "associated service: hearing impaired (HI)");
			break;
		case 4:
			strcpy_s(pszText, size, "associated service: dialogue (D)");
			break;
		case 5:
			strcpy_s(pszText, size, "associated service: commentary (C)");
			break;
		case 6:
			strcpy_s(pszText, size, "associated service: emergency (E)");
			break;
		case 7:
			if (acmod == 1)
			{
				strcpy_s(pszText, size, "associated service: voice over (VO)");
			}
			else if ((acmod >= 2) && (acmod <= 7))
			{
				strcpy_s(pszText, size, "main audio service: karaoke");
			}
			else
			{
				strcpy_s(pszText, size, "not allowed");
			}
			break;
		default:
			break;
		}
	}

	return 0;
}

int AC3_DecodeAudioCodingMode(uint8_t acmod, char* pszText, int size)
{
	if (pszText != NULL)
	{
		switch (acmod)
		{
		case 0:
			strcpy_s(pszText, size, "1+1 (Ch1, Ch2)");
			break;
		case 1:
			strcpy_s(pszText, size, "1/0 (C)");
			break;
		case 2:
			strcpy_s(pszText, size, "2/0 (L, R)");
			break;
		case 3:
			strcpy_s(pszText, size, "3/0 (L, C, R)");
			break;
		case 4:
			strcpy_s(pszText, size, "2/1 (L, R, S)");
			break;
		case 5:
			strcpy_s(pszText, size, "3/1 (L, C, R, S)");
			break;
		case 6:
			strcpy_s(pszText, size, "2/2 (L, R, SL, SR)");
			break;
		case 7:
			strcpy_s(pszText, size, "3/2 (L, C, R, SL, SR)");
			break;
		default:
			break;
		}
	}

	return 0;
}


