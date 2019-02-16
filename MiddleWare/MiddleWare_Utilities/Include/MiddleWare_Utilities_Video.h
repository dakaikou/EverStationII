#ifndef __EVERSTATION_MIDDLEWARE_UTILITIES_VIDEO_H__
#define __EVERSTATION_MIDDLEWARE_UTILITIES_VIDEO_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_UTILITY_EXPORT
#       define MW_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MW_UTILITY_IMPORT)
#       define MW_UTILITY_LIB __declspec(dllimport)
#   else
#       define MW_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MW_UTILITY_LIB
#endif

#include <stdint.h>

enum
{
	VIDEO_SQCIF = 0,
	VIDEO_QCIF,
	VIDEO_QVGA,
	VIDEO_525_SIF,
	VIDEO_CIF,
	VIDEO_525_HHR,
	VIDEO_625_HHR,
	VIDEO_VGA,
	VIDEO_525_4SIF,
	VIDEO_525_SD,
	VIDEO_4CIF,
	VIDEO_625_SD,
	VIDEO_SVGA,
	VIDEO_XGA,
	VIDEO_720P_HD,
	VIDEO_4VGA,
	VIDEO_SXGA,
	VIDEO_525_16SIF,
	VIDEO_16CIF,
	VIDEO_4SVGA,
	VIDEO_1080_HD,
	VIDEO_2Kx1K,
	VIDEO_2Kx1080,
	VIDEO_4XGA,
	VIDEO_16VGA,
	VIDEO_3616x1536,
	VIDEO_3672x1536,
	VIDEO_4Kx2K,
	VIDEO_4096x2304,
	VIDEO_FORMAT_MAX
} VIDEO_FORMAT_e;

typedef struct
{
	int		width;
	int		height;

} WIDTH_HEIGHT_t;

MW_UTILITY_LIB int VIDEO_get_width_and_height_info(int format, WIDTH_HEIGHT_t* pwh, char* pszText, int strSize);

#endif

