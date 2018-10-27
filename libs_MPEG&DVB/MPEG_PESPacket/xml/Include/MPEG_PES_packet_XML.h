#ifndef __MPEG2_PACKET_ELEMENTARY_STREAM_XML_H__
#define __MPEG2_PACKET_ELEMENTARY_STREAM_XML_H__

#include "../../compile.h"

#include "../../Include/MPEG_PES_packet.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT	int	MPEG_decode_PES_packet_to_xml(uint8_t *pes_buf, int pes_size, HALForXMLDoc* pxmlDoc = NULL, PES_packet_t* pPES_packet = NULL);
_CDL_EXPORT int	MPEG_present_PES_packet_to_xml(HALForXMLDoc* pxmlDoc, PES_packet_t* pPES_packet);

#endif

