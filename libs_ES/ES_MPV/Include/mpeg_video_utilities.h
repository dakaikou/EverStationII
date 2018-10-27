#ifndef __API_MPEG_VIDEO_DECODER_H__
#define __API_MPEG_VIDEO_DECODER_H__

#include <stdio.h>
#include <stdint.h>
#include "../Compile.h"

/*13818-2: Video*/
_CDL_EXPORT void MPGV_DecodeProgressiveSequenceCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeProgressiveFrameCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeFrameRateCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeAspectRatioCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeExtensionStartCodeIdentifiertoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeVideoFormatCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeColourPrimariesCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeTransferCharacteristicsCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeMatrixCoefficientsCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodePictureStructureCodetoText(uint8_t code, char* pszText, int strSize);

_CDL_EXPORT void MPGV_DecodeTimeCodetoText(uint32_t time_code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodePictureCodingTypetoText(uint8_t code, char* pszText, int strSize);


#endif

