#ifndef __API_AVS_VIDEO_DECODER_H__
#define __API_AVS_VIDEO_DECODER_H__

#include <stdint.h>

#include "../Compile.h"

//_CDL_EXPORT void AVS_Video_DecodeLevelCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void AVS_Video_DecodeProfileCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void AVS_Video_DecodeChromaFormatCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void AVS_Video_DecodeSamplePrecisionCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void AVS_Video_DecodeAspectRatioCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void AVS_Video_DecodeFrameRateCodetoText(U8 code, S8* pszText, S32 strSize);
//_CDL_EXPORT void AVS_Video_DecodeVideoFormatCodetoText(U8 code, S8* pszText, S32 strSize);

_CDL_EXPORT void AVS_DecodePictureCodingTypetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void AVS_DecodeProgressiveFrameCodetoText(uint8_t code, char* pszText, int strSize);
_CDL_EXPORT void AVS_DecodePictureStructureCodetoText(uint8_t code, char* pszText, int strSize);


#endif

