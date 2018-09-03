#ifndef __API_H264_VIDEO_UTILITIES_H__
#define __API_H264_VIDEO_UTILITIES_H__

#include <stdint.h>
#include "../Compile.h"

_CDL_EXPORT void H264_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void H264_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void H264_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void H264_DecodeEntropyCodingModetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void H264_DecodeSliceGroupMapTypetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void H264_DecodeSliceTypetoText(uint8_t code, char* pszText, int strSize);

#endif

