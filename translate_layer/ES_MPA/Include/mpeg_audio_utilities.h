#ifndef __API_MPEG_AUDIO_UTILITIES_H__
#define __API_MPEG_AUDIO_UTILITIES_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPA_EXPORT
#       define MPA_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPA_IMPORT)
#       define MPA_UTILITY_LIB __declspec(dllimport)
#   else
#       define MPA_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPA_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MPA_UTILITY_LIB
#endif

#include <stdint.h>

//public functions
MPA_UTILITY_LIB int M2A_DecodeID(uint8_t ID, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeSamplingFrequency(uint8_t ID, uint8_t frequency, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeBitrate(uint8_t ID, uint8_t layer, uint8_t bitrate_index, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeMode(uint8_t mode, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeLayer(uint8_t layer, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeEmphasis(uint8_t emphasis, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeCopyright(uint8_t copyright, char* pszText, int strSize);
MPA_UTILITY_LIB int M2A_DecodeOriginalOrCopy(uint8_t original_copy, char* pszText, int strSize);

#endif

