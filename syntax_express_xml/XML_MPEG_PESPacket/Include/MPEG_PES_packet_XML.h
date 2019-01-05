#ifndef __MPEG2_PACKET_ELEMENTARY_STREAM_XML_H__
#define __MPEG2_PACKET_ELEMENTARY_STREAM_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_MPEG_PES_EXPORT
#       define XML_MPEG_PES_LIB __declspec(dllexport)
#   elif defined(_XML_MPEG_PES_IMPORT)
#       define XML_MPEG_PES_LIB __declspec(dllimport)
#   else
#       define XML_MPEG_PES_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_MPEG_PES_LIB __attribute__((visibility("default")))
#else
#   define XML_MPEG_PES_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/MPEG_PESPacket/Include/MPEG_PES_packet.h"

#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_MPEG_PES_LIB int	MPEG_decode_PES_packet_to_xml(uint8_t *pes_buf, int pes_size, TALForXMLDoc* pxmlDoc = NULL, PES_packet_t* pPES_packet = NULL);
XML_MPEG_PES_LIB int	MPEG_present_PES_packet_to_xml(TALForXMLDoc* pxmlDoc, PES_packet_t* pPES_packet);

#endif

