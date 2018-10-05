#ifndef __MPEG2_TS_PACKET_XML_H__
#define __MPEG2_TS_PACKET_XML_H__

#include "../../compile.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"
#include "../../Include/Mpeg2_TS_packet.h"

using namespace tinyxml2;

//_CDL_EXPORT int MPEG_decode_TS_adaptation_field_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, adaptation_field_t* padaptation_field = NULL);
_CDL_EXPORT int MPEG_decode_TS_packet_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, transport_packet_t* ptransport_packet = NULL);


#endif		//__MPEG2_TS_PACKET_XML_H__