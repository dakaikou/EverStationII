#ifndef __API_H264_VIDEO_UTILITIES_H__
#define __API_H264_VIDEO_UTILITIES_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _H264_EXPORT
#       define H264_UTILITY_LIB __declspec(dllexport)
#   elif defined(_H264_IMPORT)
#       define H264_UTILITY_LIB __declspec(dllimport)
#   else
#       define H264_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define H264_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define H264_UTILITY_LIB
#endif

#include <stdint.h>

H264_UTILITY_LIB void H264_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
H264_UTILITY_LIB void H264_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
H264_UTILITY_LIB void H264_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
H264_UTILITY_LIB void H264_DecodeEntropyCodingModetoText(uint8_t code, char* pszText, int strSize);
H264_UTILITY_LIB void H264_DecodeSliceGroupMapTypetoText(uint8_t code, char* pszText, int strSize);
H264_UTILITY_LIB void H264_DecodeSliceTypetoText(uint8_t code, char* pszText, int strSize);

#endif

