#ifndef _MPEG2_DSMCC_UTILITIES_H_
#define _MPEG2_DSMCC_UTILITIES_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define MPEG2_DSMCC_UTILITY_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define MPEG2_DSMCC_UTILITY_LIB __declspec(dllimport)
#   else
#       define MPEG2_DSMCC_UTILITY_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_DSMCC_UTILITY_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_DSMCC_UTILITY_LIB
#endif

#include "Mpeg2_DSMCC_section.h"

#include <stdint.h>

//dsmccœ‡πÿ
MPEG2_DSMCC_UTILITY_LIB int MPEG2_DSMCC_NumericCoding2Text_dsmccType(uint8_t dsmccType, char* pszText, int strSize);
MPEG2_DSMCC_UTILITY_LIB int MPEG2_DSMCC_NumericCoding2Text_messageId(uint16_t messageId, char* pszText, int strSize);

//MPEG2_DSMCC_UTILITY_LIB int DSMCC_CarouselTypeIdLookup(uint8_t carousel_type_id, S8* pszText, int strSize);
MPEG2_DSMCC_UTILITY_LIB int MPEG2_DSMCC_NumericCoding2Text_byteOrder(uint8_t byte_order, char* pszText, int strSize);
MPEG2_DSMCC_UTILITY_LIB int MPEG2_DSMCC_NumericCoding2Text_tapUse(uint16_t use, char* pszText, int strSize);
MPEG2_DSMCC_UTILITY_LIB int MPEG2_DSMCC_NumericCoding2Text_tagId(uint32_t tag_id, char* pszText, int strSize);
MPEG2_DSMCC_UTILITY_LIB int MPEG2_DSMCC_NumericCoding2Text_bindingType(uint8_t bindingType, char* pszText, int strSize);

#endif
