#ifndef __API_MPEG_VIDEO_DECODER_H__
#define __API_MPEG_VIDEO_DECODER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPV_EXPORT
#       define MPV_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPV_IMPORT)
#       define MPV_UTILITY_LIB __declspec(dllimport)
#   else
#       define MPV_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPV_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MPV_UTILITY_LIB
#endif

#include <stdint.h>

#include <stdio.h>
#include <stdint.h>

/*13818-2: Video*/
MPV_UTILITY_LIB void MPGV_DecodeProgressiveSequenceCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeProgressiveFrameCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeFrameRateCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeAspectRatioCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeExtensionStartCodeIdentifiertoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeVideoFormatCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeColourPrimariesCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeTransferCharacteristicsCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodeMatrixCoefficientsCodetoText(uint8_t code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodePictureStructureCodetoText(uint8_t code, char* pszText, int strSize);

MPV_UTILITY_LIB void MPGV_DecodeTimeCodetoText(uint32_t time_code, char* pszText, int strSize);
MPV_UTILITY_LIB void MPGV_DecodePictureCodingTypetoText(uint8_t code, char* pszText, int strSize);


#endif

