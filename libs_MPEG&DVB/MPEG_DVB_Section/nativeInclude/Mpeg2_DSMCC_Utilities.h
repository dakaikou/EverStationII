#ifndef _MPEG2_DSMCC_UTILITIES_H_
#define _MPEG2_DSMCC_UTILITIES_H_

#include "../compile.h"

#include "HAL\HAL_Sys\Include\INTTYPES.H"
#include "Mpeg2_DSMCC_section.h"

#include <stdint.h>

//dsmccœ‡πÿ
_CDL_EXPORT int MPEG2_DSMCC_NumericCoding2Text_dsmccType(uint8_t dsmccType, char* pszText, int strSize);
_CDL_EXPORT int MPEG2_DSMCC_NumericCoding2Text_messageId(uint16_t messageId, char* pszText, int strSize);

//_CDL_EXPORT S32 DSMCC_CarouselTypeIdLookup(U8 carousel_type_id, S8* pszText, S32 strSize);
_CDL_EXPORT int MPEG2_DSMCC_NumericCoding2Text_byteOrder(uint8_t byte_order, char* pszText, int strSize);
_CDL_EXPORT int MPEG2_DSMCC_NumericCoding2Text_tapUse(uint16_t use, char* pszText, int strSize);
_CDL_EXPORT int MPEG2_DSMCC_NumericCoding2Text_tagId(uint32_t tag_id, char* pszText, int strSize);
_CDL_EXPORT int MPEG2_DSMCC_NumericCoding2Text_bindingType(uint8_t bindingType, char* pszText, int strSize);

#endif
