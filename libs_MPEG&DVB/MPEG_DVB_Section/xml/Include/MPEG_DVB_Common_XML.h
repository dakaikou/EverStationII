#ifndef __MPEG_DVB_COMMON_XML_H__
#define __MPEG_DVB_COMMON_XML_H__

#include <stdint.h>

#include "../../compile.h"
#include "../../Include/MPEG_DVB_Common.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int report_descriptor_length_error_to_xml(uint8_t *buf, int length, int max_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
_CDL_EXPORT	int present_reserved_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, reserved_descriptor_t* preserved_descriptor = NULL);

_CDL_EXPORT	int MPEG_DVB_present_reserved_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, reserved_descriptor_t* preserved_descriptor);

#endif