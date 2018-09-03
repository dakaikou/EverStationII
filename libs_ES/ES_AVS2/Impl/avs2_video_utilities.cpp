#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#include "../Include/VideoDecoder_AVS.h"
#include "../Include/common/video_common.h"


void AVS_Video_DecodeProfileCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0x00:
		strcpy_s(pszText, size, "禁止");
		break;
	case 0x20:
		strcpy_s(pszText, size, "基准档次(Jizhun profile)");
		break;
	default:
		strcpy_s(pszText, size, "保留");
		break;
	}
}

void AVS_Video_DecodeLevelCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0x00:
		strcpy_s(pszText, size, "禁止");
		break;
	case 0x10:
		strcpy_s(pszText, size, "2.0");
		break;
	case 0x20:
		strcpy_s(pszText, size, "4.0");
		break;
	case 0x22:
		strcpy_s(pszText, size, "4.2");
		break;
	case 0x40:
		strcpy_s(pszText, size, "6.0");
		break;
	case 0x42:
		strcpy_s(pszText, size, "6.2");
		break;
	default:
		strcpy_s(pszText, size, "保留");
		break;
	}
}

void AVS_Video_DecodeChromaFormatCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case CHROMA_FORMAT_MONO:
			strcpy_s(pszText, size, "保留");
			break;
		case CHROMA_FORMAT_4_2_0:
			strcpy_s(pszText, size, "4:2:0");
			break;
		case CHROMA_FORMAT_4_2_2:
			strcpy_s(pszText, size, "4:2:2");
			break;
		case CHROMA_FORMAT_4_4_4:
			strcpy_s(pszText, size, "保留");
			break;
		}
	}
}

void AVS_Video_DecodeSamplePrecisionCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "禁止");
			break;
		case 1:
			strcpy_s(pszText, size, "亮度和色度均为8比特精度");
			break;
		default:
			strcpy_s(pszText, size, "保留");
			break;
		}
	}
}

void AVS_Video_DecodeAspectRatioCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "禁止");
			break;
		case 1:
			strcpy_s(pszText, size, "-");
			break;
		case 2:
			strcpy_s(pszText, size, "4:3");
			break;
		case 3:
			strcpy_s(pszText, size, "16:9");
			break;
		case 4:
			strcpy_s(pszText, size, "2.21:1");
			break;
		default:
			strcpy_s(pszText, size, "保留");
			break;
		}
	}
}

void AVS_Video_DecodeFrameRateCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "禁止");
			break;
		case 1:
			strcpy_s(pszText, size, "23.976 帧/秒");
			break;
		case 2:
			strcpy_s(pszText, size, "24 帧/秒");
			break;
		case 3:
			strcpy_s(pszText, size, "25 帧/秒");
			break;
		case 4:
			strcpy_s(pszText, size, "29.97 帧/秒");
			break;
		case 5:
			strcpy_s(pszText, size, "30 帧/秒");
			break;
		case 6:
			strcpy_s(pszText, size, "50 帧/秒");
			break;
		case 7:
			strcpy_s(pszText, size, "59.94 帧/秒");
			break;
		case 8:
			strcpy_s(pszText, size, "60 帧/秒");
			break;
		default:
			strcpy_s(pszText, size, "保留");
			break;
		}
	}
}

void AVS_Video_DecodeVideoFormatCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "分量信号");
			break;
		case 1:
			strcpy_s(pszText, size, "PAL");
			break;
		case 2:
			strcpy_s(pszText, size, "NTSC");
			break;
		case 3:
			strcpy_s(pszText, size, "SECAM");
			break;
		case 4:
			strcpy_s(pszText, size, "MAC");
			break;
		case 5:
			strcpy_s(pszText, size, "未作规定的视频格式");
			break;
		case 6:
			strcpy_s(pszText, size, "保留");
			break;
		case 7:
			strcpy_s(pszText, size, "保留");
			break;
		}
	}
}