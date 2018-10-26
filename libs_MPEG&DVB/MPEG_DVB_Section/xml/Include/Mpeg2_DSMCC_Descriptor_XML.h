#ifndef __MPEG_DSMCC_DESCRIPTOR_XML_H__
#define __MPEG_DSMCC_DESCRIPTOR_XML_H__

#include <stdint.h>

#include "../../compile.h"

#include "../../Include/Mpeg2_DSMCC_Descriptor.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int MPEG2_DSMCC_decode_name_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, name_descriptor_t* pname_descriptor = NULL);
_CDL_EXPORT int MPEG2_DSMCC_present_name_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, name_descriptor_t* pname_descriptor);

_CDL_EXPORT int MPEG2_DSMCC_decode_location_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, location_descriptor_t* plocation_descriptor = NULL);
_CDL_EXPORT int MPEG2_DSMCC_present_location_descriptor_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, location_descriptor_t* plocation_descriptor);

#endif