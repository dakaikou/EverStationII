#ifndef __API_AVS_VIDEO_DECODER_H__
#define __API_AVS_VIDEO_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _AVS2_EXPORT
#       define AVS2_UTILITY_LIB __declspec(dllexport)
#   elif defined(_AVS2_IMPORT)
#       define AVS2_UTILITY_LIB __declspec(dllimport)
#   else
#       define AVS2_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define AVS2_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define AVS2_UTILITY_LIB
#endif

#include <stdint.h>

AVS2_UTILITY_LIB void AVS_Video_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
AVS2_UTILITY_LIB void AVS_Video_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
AVS2_UTILITY_LIB void AVS_Video_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
AVS2_UTILITY_LIB void AVS_Video_DecodeSamplePrecisionCodetoText(uint8_t code, char* pszText, int strSize);
AVS2_UTILITY_LIB void AVS_Video_DecodeAspectRatioCodetoText(uint8_t code, char* pszText, int strSize);
AVS2_UTILITY_LIB void AVS_Video_DecodeFrameRateCodetoText(uint8_t code, char* pszText, int strSize);
AVS2_UTILITY_LIB void AVS_Video_DecodeVideoFormatCodetoText(uint8_t code, char* pszText, int strSize);

#endif

