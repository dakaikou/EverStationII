#ifndef __MPEG2_PACKET_ELEMENTARY_STREAM_XML_H__
#define __MPEG2_PACKET_ELEMENTARY_STREAM_XML_H__

#include "../../compile.h"

#include "../MPEG_PES_packet.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT	int	MPEG_decode_PES_packet_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, PES_packet_t* pPES_packet = NULL);

#endif

