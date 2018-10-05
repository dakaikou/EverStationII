#ifndef _DVB_MHP_SECTION_XML_H_
#define _DVB_MHP_SECTION_XML_H_

#include <stdint.h>

#include "../compile.h"

#include "../nativeInclude/DVB_MHP_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT	int	DVB_MHP_AIT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, application_information_section_t* pait_section = NULL);

#endif
