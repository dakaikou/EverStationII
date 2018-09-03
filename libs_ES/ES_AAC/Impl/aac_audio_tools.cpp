#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Include/aac_audio_tools.h"


S32 AAC_DecodeID(U8 ID, S8* pszText, S32 size)
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

S32 AAC_DecodeProfile(U8 ID, U8 profile, S8* pszText, S32 size)
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
static S32 AAC_audio_sampling_frequency_table[16] = 
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

S32 AAC_DecodeSamplingFrequency(U8 sampling_frequency_index, S8* pszText, S32 size)
{
	S32	frequency = AAC_audio_sampling_frequency_table[sampling_frequency_index];

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

S32 AAC_DecodeChannelConfig(U8 channel_configuration, S8* pszText, S32 size)
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


