#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "../Include/MiddleWare_Utilities_Video.h"

static WIDTH_HEIGHT_t	width_height_tab[] =
{
{ 128, 96 },			//SQCIF
{ 176, 144 },			//QCIF
{ 320, 240 },			//QVGA
{ 352, 240 },			//525 SIF
{ 352, 288 },			//CIF
{ 352, 480 },			//525 HHR
{ 352, 576 },			//625 HHR
{ 640, 480 },			//VGA
{ 704, 480 },			//525 4SIF
{ 720, 480 },			//525 SD
{ 704, 576 },			//4CIF
{ 720, 576 },			//625 SD
{ 800, 600 },			//SVGA
{ 1024, 768 },			//XGA
{ 1280, 720 },			//720p HD
{ 1280, 960 },			//4VGA
{ 1280, 1024 },			//SXGA
{ 1408, 960 },			//525 16SIF
{ 1408, 1152 },			//16 CIF
{ 1600, 1200 },			//4SVGA
{ 1920, 1088 },			//1080HD
{ 2048, 1024 },			//2Kx1K
{ 2048, 1088 },			//2Kx1080
{ 2048, 1536 },			//4XGA
{ 2560, 1920 },			//16VGA
{ 3616, 1536 },			//3616x1536
{ 3672, 1536 },			//3672x1536
{ 4096, 2048 },			//4Kx2K
{ 4096, 2304 },			//4096x2304
};

int VIDEO_get_width_and_height_info(int format_index, WIDTH_HEIGHT_t* pwh, char* pszText, int size)
{
	if (pwh != NULL)
	{
		if ((format_index >= 0) && (format_index < VIDEO_FORMAT_MAX))
		{
			pwh->width = width_height_tab[format_index].width;
			pwh->height = width_height_tab[format_index].height;

		}
		else
		{
			pwh->width = 0;
			pwh->height = 0;
		}
	}

	if (pszText != NULL)
	{
		switch (format_index)
		{
		case VIDEO_SQCIF:
			sprintf_s(pszText, size, "SQCIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_QCIF:
			sprintf_s(pszText, size, "QCIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_QVGA:
			sprintf_s(pszText, size, "QVGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_525_CIF:
			sprintf_s(pszText, size, "525_CIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_CIF:
			sprintf_s(pszText, size, "CIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_525_HHR:
			sprintf_s(pszText, size, "525_HHR: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_625_HHR:
			sprintf_s(pszText, size, "625_HHR: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_VGA:
			sprintf_s(pszText, size, "VGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_525_4SIF:
			sprintf_s(pszText, size, "525_4SIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_525_SD:
			sprintf_s(pszText, size, "525_SD: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_4CIF:
			sprintf_s(pszText, size, "4CIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_625_SD:
			sprintf_s(pszText, size, "625_SD: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_SVGA:
			sprintf_s(pszText, size, "SVGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_XGA:
			sprintf_s(pszText, size, "XGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_720P_HD:
			sprintf_s(pszText, size, "720p HD: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_4VGA:
			sprintf_s(pszText, size, "4VGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_SXGA:
			sprintf_s(pszText, size, "SXGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_525_16SIF:
			sprintf_s(pszText, size, "525 16SIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_16CIF:
			sprintf_s(pszText, size, "16CIF: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_4SVGA:
			sprintf_s(pszText, size, "4SVGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_1080_HD:
			sprintf_s(pszText, size, "1080 HD: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_2Kx1K:
			sprintf_s(pszText, size, "2Kx1K: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_2Kx1080:
			sprintf_s(pszText, size, "2Kx1080: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_4XGA:
			sprintf_s(pszText, size, "4XGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_16VGA:
			sprintf_s(pszText, size, "16VGA: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_3616x1536:
			sprintf_s(pszText, size, "3616x1536: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_3672x1536:
			sprintf_s(pszText, size, "3672x1536: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_4Kx2K:
			sprintf_s(pszText, size, "4Kx2K: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		case VIDEO_4096x2304:
			sprintf_s(pszText, size, "4096x2304: (%dx%d)", width_height_tab[format_index].width, width_height_tab[format_index].height);
			break;
		default:
			strcpy_s(pszText, size, "wrong video format");
			break;
		}
	}

	return 0;
}



