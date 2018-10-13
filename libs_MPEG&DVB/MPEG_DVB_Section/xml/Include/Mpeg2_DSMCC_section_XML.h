#ifndef _MPEG_DSMCC_SECTION_XML_H_
#define _MPEG_DSMCC_SECTION_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT	int MPEG2_DSMCC_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_section_t* pDSMCCSection = NULL);
_CDL_EXPORT	int MPEG2_DSMCC_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_section_t* pdsmcc_section);


#endif
