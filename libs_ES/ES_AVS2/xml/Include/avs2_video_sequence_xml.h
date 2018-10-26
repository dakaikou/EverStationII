#ifndef _AVS2_VIDEO_SEQUENCE_XML_H_
#define _AVS2_VIDEO_SEQUENCE_XML_H_

#include <stdint.h>

#include "../../compile.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int avs2_present_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, HALForXMLDoc* pxmlDoc);
_CDL_EXPORT int avs2_present_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, HALForXMLDoc* pxmlDoc);


#endif

