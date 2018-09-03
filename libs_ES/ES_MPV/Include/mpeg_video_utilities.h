#ifndef __API_MPEG_VIDEO_DECODER_H__
#define __API_MPEG_VIDEO_DECODER_H__

#include <stdio.h>
#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../Compile.h"

/*13818-2: Video*/
_CDL_EXPORT void MPGV_DecodeProgressiveSequenceCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeProgressiveFrameCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeChromaFormatCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeFrameRateCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeAspectRatioCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeExtensionStartCodeIdentifiertoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeLevelCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeProfileCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeVideoFormatCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeColourPrimariesCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeTransferCharacteristicsCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodeMatrixCoefficientsCodetoText(U8 code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodePictureStructureCodetoText(U8 code, S8* pszText, S32 strSize);

_CDL_EXPORT void MPGV_DecodeTimeCodetoText(U32 time_code, S8* pszText, S32 strSize);
_CDL_EXPORT void MPGV_DecodePictureCodingTypetoText(U8 code, S8* pszText, S32 strSize);


#endif

