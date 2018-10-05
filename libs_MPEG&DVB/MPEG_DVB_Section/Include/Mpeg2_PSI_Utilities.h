#ifndef _MPEG2_PSI_UTILITIES_H_
#define _MPEG2_PSI_UTILITIES_H_

#include "../compile.h"

#include <stdint.h>

/*13818-2: Video*/
//_CDL_EXPORT void MPGV_DecodeProgressiveSequenceCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodeProgressiveFrameCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeFrameRateCodetoText(uint8_t code, char* pszText, int strSize);
//_CDL_EXPORT void MPGV_DecodeAspectRatioCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodeExtensionStartCodeIdentifiertoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void MPGV_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
//_CDL_EXPORT void MPGV_DecodeVideoFormatCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodeColourPrimariesCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodeTransferCharacteristicsCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodeMatrixCoefficientsCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodePictureStructureCodetoText(U8 code, S8* pszText, S32 strSize);
//
//_CDL_EXPORT void MPGV_DecodeTimeCodetoText(U32 time_code, S8* pszText, S32 strSize);
//_CDL_EXPORT void MPGV_DecodePictureCodingTypetoText(U8 code, S8* pszText, S32 strSize);

_CDL_EXPORT int MPEG2_PSI_NumericCoding2Text_StreamType(uint8_t stream_type, uint8_t subtype, char* pszText, int strSize);
_CDL_EXPORT uint8_t GetTSPayloadClassByStreamType(uint8_t stream_type, uint8_t sub_type);


#endif
