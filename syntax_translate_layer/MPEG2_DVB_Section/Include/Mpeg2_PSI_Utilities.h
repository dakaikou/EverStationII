#ifndef _MPEG2_PSI_UTILITIES_H_
#define _MPEG2_PSI_UTILITIES_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define MPEG2_PSI_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define MPEG2_PSI_UTILITY_LIB __declspec(dllimport)
#   else
#       define MPEG2_PSI_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_PSI_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_PSI_UTILITY_LIB
#endif

#include <stdint.h>

/*13818-2: Video*/
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeProgressiveSequenceCodetoText(uint8_t code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeProgressiveFrameCodetoText(uint8_t code, S8* pszText, int strSize);
MPEG2_PSI_UTILITY_LIB void MPGV_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int strSize);
MPEG2_PSI_UTILITY_LIB void MPGV_DecodeFrameRateCodetoText(uint8_t code, char* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeAspectRatioCodetoText(uint8_t code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeExtensionStartCodeIdentifiertoText(uint8_t code, S8* pszText, int strSize);
MPEG2_PSI_UTILITY_LIB void MPGV_DecodeLevelCodetoText(uint8_t code, char* pszText, int strSize);
MPEG2_PSI_UTILITY_LIB void MPGV_DecodeProfileCodetoText(uint8_t code, char* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeVideoFormatCodetoText(uint8_t code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeColourPrimariesCodetoText(uint8_t code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeTransferCharacteristicsCodetoText(uint8_t code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeMatrixCoefficientsCodetoText(uint8_t code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodePictureStructureCodetoText(uint8_t code, S8* pszText, int strSize);
//
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodeTimeCodetoText(uint32_t time_code, S8* pszText, int strSize);
//MPEG2_PSI_UTILITY_LIB void MPGV_DecodePictureCodingTypetoText(uint8_t code, S8* pszText, int strSize);

MPEG2_PSI_UTILITY_LIB int MPEG2_PSI_NumericCoding2Text_StreamType(uint8_t stream_type, uint8_t subtype, char* pszText, int strSize);


#endif
