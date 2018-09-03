#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/mpeg_audio_utilities.h"
#include "./kernel/Mpeg_audiotables.h"
//#include "./kernel/Mpeg_decodetable.h"
//#include "./kernel/Mpeg_filtertable.h"
#include "./kernel/Mpeg_alloctables.h"


//static AllocTablePtr alloc_table_table[4] =
//{
//    (AllocTablePtr) & alloc_0,
//    (AllocTablePtr) & alloc_1,
//    (AllocTablePtr) & alloc_2,
//    (AllocTablePtr) & alloc_3
//};

//static S32 sblims[4] =
//{
//    27, 30, 8, 12
//};
//
//static int which_table[3][2][16] =			//[sampling_frequency][stero mode][bitrate index]
//{
//    {
//        {0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 1, 1, 1, 1, 1, 0},			/*stero mode*/
//        {0, 2, 2, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}},			/*signal channel mode*/
//    {
//        {0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//        {0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
//    {
//        {0, 3, 3, 3, 3, 3, 3, 0, 0, 0, 1, 1, 1, 1, 1, 0},
//        {0, 3, 3, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}}
//};
//
//static int slen[2][16] =
//{
//    {0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4},
//    {0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3}
//};


/*
 * Two tables for dequantization of layer 2 audio data
 */
//static double c_coeff[17] =
//{
//    1.33333333333, 1.60000000000, 1.14285714286, 
//    1.77777777777, 1.06666666666, 1.03225806452,
//    1.01587301587, 1.00787401575, 1.00392156863,
//    1.00195694716, 1.00097751711, 1.00048851979,
//    1.00024420024, 1.00012208522, 1.00006103888,
//    1.00003051851, 1.00001525902
//};
//
//static double d_coeff[17] =
//{
//    0.50000000000, 0.50000000000, 0.25000000000, 0.50000000000,
//    0.12500000000, 0.06250000000, 0.03125000000, 0.01562500000,
//    0.00781250000, 0.00390625000, 0.00195312500, 0.00097656250,
//    0.00048828125, 0.00024414063, 0.00012207031, 0.00006103516,
//	0.00003051758
//};



/*units: Hz*/
/*
static S32 MPEG_audio_sampling_frequency_table[2][4] = 
{
	{22050, 24000, 16000, -1},
	{44100, 48000, 32000, -1}
};
*/
int M2A_DecodeSamplingFrequency(uint8_t ID, uint8_t sampling_frequency, char* pszText, int size)
{
	int frequency = MPA_sampling_rate_table[ID][sampling_frequency];

	if (pszText != NULL)
	{
		if (frequency == -1)
		{
			strcpy_s(pszText, size, "reserved");
		}
		else
		{
			sprintf_s(pszText, size, "%d Hz", frequency);
		}
	}

	return frequency;
}

int M2A_DecodeBitrate(uint8_t ID, uint8_t layer, uint8_t bitrate_index, char* pszText, int size)
{
	int bitrate = -1;

	if ((layer > 0) && (layer <= 3))
	{
		bitrate = MPA_bit_rate_table[ID][3 - layer][bitrate_index];
	}

	if (pszText != NULL)
	{
		if (bitrate == 0)
		{
			strcpy_s(pszText, size, "free");
		}
		else if (bitrate == -1)
		{
			strcpy_s(pszText, size, "forbidden");
		}
		else
		{
			sprintf_s(pszText, size, "%d bps", bitrate);
		}
	}

	return bitrate;
}

int M2A_DecodeID(uint8_t ID, char* pszText, int size)
{
	if (pszText)
	{
		switch (ID)
		{
		case 1:
			strcpy_s(pszText, size, "ISO/IEC 11172-3");
			break;
		default:
			strcpy_s(pszText, size, "extension to lower sampling frequencies");
			break;
		}
	}
	return 0;
}

int M2A_DecodeMode(uint8_t mode, char* pszText, int size)
{
	if (pszText)
	{
		switch (mode)
		{
		case 0:
			strcpy_s(pszText, size, "stero");
			break;
		case 1:
			strcpy_s(pszText, size, "joint stero");
			break;
		case 2:
			strcpy_s(pszText, size, "dual channel stero");
			break;
		case 3:
			strcpy_s(pszText, size, "single channel");
			break;
		default:
			break;
		}
	}

	return 0;
}

int M2A_DecodeLayer(uint8_t layer, char* pszText, int size)
{
	if (pszText)
	{
		switch (layer)
		{
		case 0:
			strcpy_s(pszText, size, "reserved");
			break;
		case 1:
			strcpy_s(pszText, size, "Layer III");
			break;
		case 2:
			strcpy_s(pszText, size, "Layer II");
			break;
		case 3:
			strcpy_s(pszText, size, "Layer I");
			break;
		default:
			break;
		}
	}

	return 0;
}

int M2A_DecodeEmphasis(uint8_t emphasis, char* pszText, int size)
{
	if (pszText)
	{
		switch (emphasis)
		{
		case 0:
			strcpy_s(pszText, size, "none");
			break;
		case 1:
			strcpy_s(pszText, size, "50/15 microseconds");
			break;
		case 2:
			strcpy_s(pszText, size, "reserved");
			break;
		case 3:
			strcpy_s(pszText, size, "CCITT J.17");
			break;
		default:
			break;
		}
	}

	return 0;
}

int M2A_DecodeCopyright(uint8_t copyright, char* pszText, int size)
{
	if (pszText)
	{
		switch (copyright)
		{
		case 1:
			strcpy_s(pszText, size, "copyright protected");
			break;
		default:
			strcpy_s(pszText, size, "no copyright");
			break;
		}
	}

	return 0;
}

int M2A_DecodeOriginalOrCopy(uint8_t original_or_copy, char* pszText, int size)
{
	if (pszText)
	{
		switch (original_or_copy)
		{
		case 1:
			strcpy_s(pszText, size, "original");
			break;
		default:
			strcpy_s(pszText, size, "copy");
			break;
		}
	}

	return 0;
}



