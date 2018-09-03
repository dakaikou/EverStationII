#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <io.h>
#include <fcntl.h>
#include <assert.h>

#include "../Include/mpeg_video_utilities.h"

#define CHINESE_VERSION		1

void MPGV_DecodeProgressiveSequenceCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText)
	{
		if (code == 0)
		{
			strcpy_s(pszText, size, "±àÂëÐòÁÐ¿É°üÀ¨(ÖðÐÐ/¸ôÐÐ)Ö¡Í¼ÏñºÍ³¡Í¼Ïñ");
		}
		else
		{
			strcpy_s(pszText, size, "±àÂëÐòÁÐ½ö°üÀ¨ÖðÐÐÖ¡Í¼Ïñ");
		}
	}
}

void MPGV_DecodeProgressiveFrameCodetoText(U8 code, S8* pszText, S32 size)
{
	if (pszText)
	{
		if (code == 0)
		{
			strcpy_s(pszText, size, "Ö¡µÄÁ½³¡ÊÇ¸ôÐÐ³¡");
		}
		else
		{
			strcpy_s(pszText, size, "Ö¡µÄÁ½³¡À´×ÔÍ¬Ò»Ê±¿Ì");
		}
	}
}

void MPGV_DecodeFrameRateCodetoText(U8 code, S8* pszText, S32 size)
{
#if CHINESE_VERSION
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "½ûÖ¹");
		break;
	case 1:
		strcpy_s(pszText, size, "23.976 Ö¡/Ãë");
		break;
	case 2:
		strcpy_s(pszText, size, "24 Ö¡/Ãë");
		break;
	case 3:
		strcpy_s(pszText, size, "25 Ö¡/Ãë");
		break;
	case 4:
		strcpy_s(pszText, size, "29.97 Ö¡/Ãë");
		break;
	case 5:
		strcpy_s(pszText, size, "30 Ö¡/Ãë");
		break;
	case 6:
		strcpy_s(pszText, size, "50 Ö¡/Ãë");
		break;
	case 7:
		strcpy_s(pszText, size, "59.94 Ö¡/Ãë ");
		break;
	case 8:
		strcpy_s(pszText, size, "60 Ö¡/Ãë");
		break;
	default:
		strcpy_s(pszText, size, "±£Áô");
		break;
	}
#else
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "forbidden");
		break;
	case 1:
		strcpy_s(pszText, size, "23.976 frame/s");
		break;
	case 2:
		strcpy_s(pszText, size, "24 frame/s");
		break;
	case 3:
		strcpy_s(pszText, size, "25 frame/s");
		break;
	case 4:
		strcpy_s(pszText, size, "29.97 frame/s");
		break;
	case 5:
		strcpy_s(pszText, size, "30 frame/s");
		break;
	case 6:
		strcpy_s(pszText, size, "50 frame/s");
		break;
	case 7:
		strcpy_s(pszText, size, "59.94 frame/s");
		break;
	case 8:
		strcpy_s(pszText, size, "60 frame/s");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
#endif

}

void MPGV_DecodeAspectRatioCodetoText(U8 code, S8* pszText, S32 size)
{
#if CHINESE_VERSION
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "½ûÖ¹");
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
		strcpy_s(pszText, size, "±£Áô");
		break;
	}
#else
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "forbidden");
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
		strcpy_s(pszText, size, "reserved");
		break;
	}
#endif

}

void MPGV_DecodePictureStructureCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "±£Áô");
		break;
	case 1:
		strcpy_s(pszText, size, "¶¥³¡");
		break;
	case 2:
		strcpy_s(pszText, size, "µ×³¡");
		break;
	case 3:
		strcpy_s(pszText, size, "Ö¡Í¼Ïñ");
		break;
	default:
		strcpy_s(pszText, size, "½ûÖ¹");
		break;
	}
}

void MPGV_DecodeChromaFormatCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "reserved");
		break;
	case 1:
		strcpy_s(pszText, size, "4:2:0");
		break;
	case 2:
		strcpy_s(pszText, size, "4:2:2");
		break;
	case 3:
		strcpy_s(pszText, size, "4:2:4");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodeVideoFormatCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "component");
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
		strcpy_s(pszText, size, "4:2:4");
		break;
	case 6:
		strcpy_s(pszText, size, "Unspecified video format");
		break;
	case 7:
		strcpy_s(pszText, size, "reserved");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodeColourPrimariesCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "forbidden");
		break;
	case 1:
		strcpy_s(pszText, size, "ITU-R BT.709");
		break;
	case 2:
		strcpy_s(pszText, size, "Unspecified Video");
		break;
	case 3:
		strcpy_s(pszText, size, "reserved");
		break;
	case 4:
		strcpy_s(pszText, size, "ITU-R BT.470-2");
		break;
	case 5:
		strcpy_s(pszText, size, "ITU-R BT.470-2");
		break;
	case 6:
		strcpy_s(pszText, size, "SMPTE 170M");
		break;
	case 7:
		strcpy_s(pszText, size, "SMPTE 240M(1987)");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;

	}
}

void MPGV_DecodeTransferCharacteristicsCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "forbidden");
		break;
	case 1:
		strcpy_s(pszText, size, "ITU-R BT.709");
		break;
	case 2:
		strcpy_s(pszText, size, "Unspecified Video");
		break;
	case 3:
		strcpy_s(pszText, size, "reserved");
		break;
	case 4:
		strcpy_s(pszText, size, "ITU-R BT.470-2");
		break;
	case 5:
		strcpy_s(pszText, size, "ITU-R BT.470-2");
		break;
	case 6:
		strcpy_s(pszText, size, "SMPTE 170M");
		break;
	case 7:
		strcpy_s(pszText, size, "SMPTE 240M(1987)");
		break;
	case 8:
		strcpy_s(pszText, size, "Linear transfer characteristics");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;

	}
}

void MPGV_DecodeMatrixCoefficientsCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "forbidden");
		break;
	case 1:
		strcpy_s(pszText, size, "ITU-R BT.709");
		break;
	case 2:
		strcpy_s(pszText, size, "Unspecified Video");
		break;
	case 3:
		strcpy_s(pszText, size, "reserved");
		break;
	case 4:
		strcpy_s(pszText, size, "FCC");
		break;
	case 5:
		strcpy_s(pszText, size, "ITU-R BT.470-2");
		break;
	case 6:
		strcpy_s(pszText, size, "SMPTE 170M");
		break;
	case 7:
		strcpy_s(pszText, size, "SMPTE 240M(1987)");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;

	}
}

void MPGV_DecodeProfileCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "reserved");
		break;
	case 1:
		strcpy_s(pszText, size, "High");
		break;
	case 2:
		strcpy_s(pszText, size, "Spatially Scalable");
		break;
	case 3:
		strcpy_s(pszText, size, "SNR Scalable");
		break;
	case 4:
		strcpy_s(pszText, size, "Main");
		break;
	case 5:
		strcpy_s(pszText, size, "Simple");
		break;
	case 6:
	case 7:
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodeLevelCodetoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 4:
		strcpy_s(pszText, size, "High");
		break;
	case 6:
		strcpy_s(pszText, size, "High 1440");
		break;
	case 8:
		strcpy_s(pszText, size, "Main");
		break;
	case 10:
		strcpy_s(pszText, size, "Low");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodeExtensionStartCodeIdentifiertoText(U8 code, S8* pszText, S32 size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "reserved");
		break;
	case 1:
		strcpy_s(pszText, size, "Sequence Extension ID");
		break;
	case 2:
		strcpy_s(pszText, size, "Sequence Display Extension ID");
		break;
	case 3:
		strcpy_s(pszText, size, "Quant Matrix Extension ID");
		break;
	case 4:
		strcpy_s(pszText, size, "Copyright Extension ID");
		break;
	case 5:
		strcpy_s(pszText, size, "Sequence Scalable Extension ID");
		break;
	case 6:
		strcpy_s(pszText, size, "reserved");
		break;
	case 7:
		strcpy_s(pszText, size, "Picture Display Extension ID");
		break;
	case 8:
		strcpy_s(pszText, size, "Picutre Coding Extension ID");
		break;
	case 9:
		strcpy_s(pszText, size, "Picture Spatial Scalable Extension ID");
		break;
	case 10:
		strcpy_s(pszText, size, "Picture Temporal Scalable Extension ID");
		break;
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodePictureCodingTypetoText(U8 code, S8* pszText, S32 size)
{
#if CHINESE_VERSION
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "½ûÖ¹");
		break;
	case 1:
		strcpy_s(pszText, size, "Ö¡ÄÚ±àÂë(I)");
		break;
	case 2:
		strcpy_s(pszText, size, "Ô¤²â±àÂë(P)");
		break;
	case 3:
		strcpy_s(pszText, size, "Ë«ÏòÔ¤²â±àÂë(B)");
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	default:
		strcpy_s(pszText, size, "±£Áô");
		break;
	}
#else
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "forbidden");
		break;
	case 1:
		strcpy_s(pszText, size, "intra-coded(I)");
		break;
	case 2:
		strcpy_s(pszText, size, "predictive-coded(P)");
		break;
	case 3:
		strcpy_s(pszText, size, "bidirectionally-predictive-coded(B)");
		break;
	case 4:
	case 5:
	case 6:
	case 7:
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
#endif
}





