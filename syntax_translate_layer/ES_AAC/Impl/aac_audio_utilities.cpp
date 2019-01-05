#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Include/aac_audio_utilities.h"


int AAC_DecodeID(uint8_t ID, char* pszText, int size)
{
	if (pszText)
	{
		switch (ID)
		{
		case 1:
			strcpy_s(pszText, size, "MPEG-2 AAC");
			break;
		default:
			strcpy_s(pszText, size, "MPEG-4 AAC");
			break;
		}
	}
	return 0;
}

int AAC_DecodeProfile(uint8_t ID, uint8_t profile, char* pszText, int size)
{
	if (pszText)
	{
		if (ID == 1)			//mpeg-2 AAC audio
		{
			switch (profile)
			{
			case 0:
				sprintf_s(pszText, size, "Main profile");
				break;
			case 1:
				sprintf_s(pszText, size, "Low Complexity profile(LC)");
				break;
			case 2:
				sprintf_s(pszText, size, "Scalable Sample Rate profile(SSR)");
				break;
			case 3:
				sprintf_s(pszText, size, "reserved");
				break;
			}
		}
		else
		{
			switch (profile)
			{
			case 0:
				sprintf_s(pszText, size, "AAC MAIN");
				break;
			case 1:
				sprintf_s(pszText, size, "AAC LC");
				break;
			case 2:
				sprintf_s(pszText, size, "AAC SSR");
				break;
			case 3:
				sprintf_s(pszText, size, "AAC LTP");
				break;
			}
		}
	}

	return 0;
}

/*units: Hz*/
static int AAC_audio_sampling_frequency_table[16] = 
{
	96000,		//0
	88200,		//1
	64000,		//2
	48000,		//3
	44100,		//4
	32000,		//5
	24000,		//6
	22050,		//7
	16000,		//8
	12000,		//9
	11025,		//a
	 8000,		//b
	   -1,
	   -1,
	   -1,
	   -1
};

int AAC_DecodeSamplingFrequency(uint8_t sampling_frequency_index, char* pszText, int size)
{
	int	frequency = AAC_audio_sampling_frequency_table[sampling_frequency_index];

	if (pszText)
	{
		if (sampling_frequency_index <= 0xb)
		{
			sprintf_s(pszText, size, "%d Hz", frequency);
		}
		else if (sampling_frequency_index <= 0xf)
		{
			strcpy_s(pszText, size, "reserved");
		}
		else
		{
			strcpy_s(pszText, size, "escape value");
		}
	}

	return frequency;
}

int AAC_DecodeChannelConfig(uint8_t channel_configuration, char* pszText, int size)
{
	if (pszText)
	{
		switch (channel_configuration)
		{
		case 0:
			strcpy_s(pszText, size, "");
			break;
		case 1:
			strcpy_s(pszText, size, "C");
			break;
		case 2:
			strcpy_s(pszText, size, "LR");
			break;
		case 3:
			strcpy_s(pszText, size, "C LR");
			break;
		case 4:
			strcpy_s(pszText, size, "C LR S");
			break;
		case 5:
			strcpy_s(pszText, size, "C LR LsRs");
			break;
		case 6:
			strcpy_s(pszText, size, "C LR LsRs + Sub");
			break;
		case 7:
			strcpy_s(pszText, size, "C LR LoRo LsRs + Sub");
			break;
		default:
			strcpy_s(pszText, size, "reserved");
			break;
		}
	}
	return 0;
}


