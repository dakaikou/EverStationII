#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../Include/dra_audio_utilities.h"


//S32 AAC_DecodeID(U8 ID, S8* pszText, S32 size)
//{
//	if (pszText)
//	{
//		switch (ID)
//		{
//		case 1:
//			strcpy_s(pszText, size, "MPEG-2 AAC");
//			break;
//		default:
//			strcpy_s(pszText, size, "MPEG-4 AAC");
//			break;
//		}
//	}
//	return 0;
//}
//
//S32 AAC_DecodeProfile(U8 ID, U8 profile, S8* pszText, S32 size)
//{
//	if (pszText)
//	{
//		if (ID == 1)			//mpeg-2 AAC audio
//		{
//			switch (profile)
//			{
//			case 0:
//				sprintf_s(pszText, size, "Main profile");
//				break;
//			case 1:
//				sprintf_s(pszText, size, "Low Complexity profile(LC)");
//				break;
//			case 2:
//				sprintf_s(pszText, size, "Scalable Sample Rate profile(SSR)");
//				break;
//			case 3:
//				sprintf_s(pszText, size, "reserved");
//				break;
//			}
//		}
//		else
//		{
//			switch (profile)
//			{
//			case 0:
//				sprintf_s(pszText, size, "AAC MAIN");
//				break;
//			case 1:
//				sprintf_s(pszText, size, "AAC LC");
//				break;
//			case 2:
//				sprintf_s(pszText, size, "AAC SSR");
//				break;
//			case 3:
//				sprintf_s(pszText, size, "AAC LTP");
//				break;
//			}
//		}
//	}
//
//	return 0;
//}

/*units: Hz*/
static int DRA_audio_sampling_frequency_table[16] = 
{
	 8000,		//0
	11025,		//1
	12000,		//2
	16000,		//3
	22050,		//4
	24000,		//5
	32000,		//6
	44100,		//7
	48000,		//8
	88200,		//9
	96000,		//a
   176400,		//b
   192000,
	   -1,
	   -1,
	   -1
};

int DRA_DecodeFrmHeaderType(uint8_t nFrmHeaderType, char* pszText, int strSize)
{
	if (nFrmHeaderType == 0)
	{
		strcpy_s(pszText, strSize, "常规帧头");
	}
	else
	{
		strcpy_s(pszText, strSize, "扩展帧头");
	}

	return 0;
}

int DRA_DecodeSamplingFrequency(uint8_t sample_rate_index, char* pszText, int strSize)
{
	int	frequency = DRA_audio_sampling_frequency_table[sample_rate_index];

	if (pszText)
	{
		if (sample_rate_index <= 12)
		{
			sprintf_s(pszText, strSize, "%d Hz", frequency);
		}
		else if (sample_rate_index <= 15)
		{
			strcpy_s(pszText, strSize, "reserved");
		}
		else
		{
			strcpy_s(pszText, strSize, "escape value");
		}
	}

	return frequency;
}

//S32 AAC_DecodeChannelConfig(U8 channel_configuration, S8* pszText, S32 size)
//{
//	if (pszText)
//	{
//		switch (channel_configuration)
//		{
//		case 0:
//			strcpy_s(pszText, size, "");
//			break;
//		case 1:
//			strcpy_s(pszText, size, "C");
//			break;
//		case 2:
//			strcpy_s(pszText, size, "LR");
//			break;
//		case 3:
//			strcpy_s(pszText, size, "C LR");
//			break;
//		case 4:
//			strcpy_s(pszText, size, "C LR S");
//			break;
//		case 5:
//			strcpy_s(pszText, size, "C LR LsRs");
//			break;
//		case 6:
//			strcpy_s(pszText, size, "C LR LsRs + Sub");
//			break;
//		case 7:
//			strcpy_s(pszText, size, "C LR LoRo LsRs + Sub");
//			break;
//		default:
//			strcpy_s(pszText, size, "reserved");
//			break;
//		}
//	}
//	return 0;
//}


