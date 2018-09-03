#ifndef __API_MPEG_AUDIO_UTILITIES_H__
#define __API_MPEG_AUDIO_UTILITIES_H__

#include <stdint.h>
#include "../Compile.h"

//public functions
_CDL_EXPORT int M2A_DecodeID(uint8_t ID, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeSamplingFrequency(uint8_t ID, uint8_t frequency, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeBitrate(uint8_t ID, uint8_t layer, uint8_t bitrate_index, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeMode(uint8_t mode, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeLayer(uint8_t layer, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeEmphasis(uint8_t emphasis, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeCopyright(uint8_t copyright, char* pszText, int strSize);
_CDL_EXPORT int M2A_DecodeOriginalOrCopy(uint8_t original_copy, char* pszText, int strSize);

#endif

