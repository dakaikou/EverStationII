#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
//#include <windows.h>
//#include <io.h>
//#include <fcntl.h>

//#include "../../ES_Include/video_common.h"
#include "../Include/h264_video_utilities.h"

void H264_DecodeProfileCodetoText(uint8_t code, char* pszText, int size)
{
	switch (code)
	{
	case 66:
		strcpy_s(pszText, size, "Baseline Profile");
		break;
	case 77:
		strcpy_s(pszText, size, "Main Profile");
		break;
	case 88:
		strcpy_s(pszText, size, "Extended Profile");
		break;
	case 100:
		strcpy_s(pszText, size, "High Profile");
		break;
	case 110:
		strcpy_s(pszText, size, "High 4:2:0/10b Profile");
		break;
	case 122:
		strcpy_s(pszText, size, "High 4:2:2/10b Profile");
		break;
	case 144:
		strcpy_s(pszText, size, "High 4:4:4/12b Profile");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void H264_DecodeLevelCodetoText(uint8_t code, char* pszText, int size)
{
	int	high = (int)code / 10;
	int	low = code - high * 10;

//	if (low == 0)
//	{
//		sprintf_s(pszText, "%d", high);
//	}
//	else
//	{
		sprintf_s(pszText, size, "Level %d.%d", high, low);
//	}
}

void H264_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		//case CHROMA_FORMAT_MONO:
		//	strcpy_s(pszText, size, "monochrome");
		//	break;
		//case CHROMA_FORMAT_4_2_0:
		//	strcpy_s(pszText, size, "4:2:0");
		//	break;
		//case CHROMA_FORMAT_4_2_2:
		//	strcpy_s(pszText, size, "4:2:2");
		//	break;
		//case CHROMA_FORMAT_4_4_4:
		//	strcpy_s(pszText, size, "4:4:4");
		//	break;
		default:
			strcpy_s(pszText, size, "Unknown");
			break;
		}
	}
}

void H264_DecodeEntropyCodingModetoText(uint8_t code, char* pszText, int size)
{
	if (pszText != NULL)
	{
		if (code)
		{
			strcpy_s(pszText, size, "CABAC");
		}
		else
		{
			strcpy_s(pszText, size, "CAVLC");
		}
	}
}

void H264_DecodeSliceGroupMapTypetoText(uint8_t code, char* pszText, int size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "interleaved slice groups");
			break;

		case 1:
			strcpy_s(pszText, size, "dispersed slice groups");
			break;

		case 2:
			strcpy_s(pszText, size, "foreground and leftover slice groups");
			break;

		case 3:
		case 4:
		case 5:
			strcpy_s(pszText, size, "changing slice groups");
			break;

		case 6:
			strcpy_s(pszText, size, "explicit assignment of slice groups");
			break;

		default:
			strcpy_s(pszText, size, "unknown slice groups");
			break;
		}
	}
}

void H264_DecodeSliceTypetoText(uint8_t code, char* pszText, int size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "P(P slice)");
			break;

		case 1:
			strcpy_s(pszText, size, "B(B slice)");
			break;

		case 2:
			strcpy_s(pszText, size, "I(I slice)");
			break;

		case 3:
			strcpy_s(pszText, size, "SP(SP slice)");
			break;

		case 4:
			strcpy_s(pszText, size, "SI(SI slice)");
			break;

		case 5:
			strcpy_s(pszText, size, "P(P Slice)");
			break;

		case 6:
			strcpy_s(pszText, size, "B(B Slice)");
			break;

		case 7:
			strcpy_s(pszText, size, "I(I Slice)");
			break;

		case 8:
			strcpy_s(pszText, size, "SP(SP Slice)");
			break;

		case 9:
			strcpy_s(pszText, size, "SI(SI Slice)");
			break;

		default:
			strcpy_s(pszText, size, "unknown slice_type");
			break;
		}
	}
}

