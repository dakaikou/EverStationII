#ifndef _DVB_MHP_SECTION_XML_H_
#define _DVB_MHP_SECTION_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../../Include/DVB_MHP_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT	int	DVB_MHP_AIT_DecodeSection_to_XML(uint8_t *section_buf, int section_length, HALForXMLDoc* pxmlDoc, application_information_section_t* pait_section = NULL);
_CDL_EXPORT	int DVB_MHP_AIT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, application_information_section_t* pait_section);

#endif
