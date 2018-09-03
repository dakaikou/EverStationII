#ifndef __MPEG2_TS_PACKET_XML_H__
#define __MPEG2_TS_PACKET_XML_H__

#include "../../compile.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "../Mpeg2_TS_packet.h"

_CDL_EXPORT int MPEG_decode_TS_adaptation_field_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, adaptation_field_t* padaptation_field = NULL);
_CDL_EXPORT int MPEG_decode_TS_packet_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, transport_packet_t* ptransport_packet = NULL);


#endif		//__MPEG2_TS_PACKET_XML_H__