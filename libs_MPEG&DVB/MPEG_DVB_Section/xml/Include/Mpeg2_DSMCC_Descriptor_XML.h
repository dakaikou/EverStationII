#ifndef __MPEG_DSMCC_DESCRIPTOR_XML_H__
#define __MPEG_DSMCC_DESCRIPTOR_XML_H__

#include "../../compile.h"

#include "../../Include/Mpeg2_DSMCC_Descriptor.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT int MPEG2_DSMCC_decode_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, name_descriptor_t* pname_descriptor = NULL);
_CDL_EXPORT int MPEG2_DSMCC_decode_location_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, location_descriptor_t* plocation_descriptor = NULL);

#endif