#ifndef __MPEG2_TS_PACKET_XML_H__
#define __MPEG2_TS_PACKET_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_TSPACKET_EXPORT
#       define XML_TSPACKET_LIB __declspec(dllexport)
#   elif defined(_XML_TSPACKET_IMPORT)
#       define XML_TSPACKET_LIB __declspec(dllimport)
#   else
#       define XML_TSPACKET_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_TSPACKET_LIB __attribute__((visibility("default")))
#else
#   define XML_TSPACKET_LIB
#endif

#include "thirdparty_AL\TALForXML\Include\TALForXML.h"
#include "translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_packet.h"

using namespace tinyxml2;

//XML_TSPACKET_LIB int MPEG_decode_TS_adaptation_field_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, adaptation_field_t* padaptation_field = NULL);
XML_TSPACKET_LIB int MPEG_decode_TS_packet_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, transport_packet_t* ptransport_packet = NULL);
XML_TSPACKET_LIB int MPEG_present_TS_packet_to_XML(TALForXMLDoc* pxmlDoc, transport_packet_t* ptransport_packet);


#endif		//__MPEG2_TS_PACKET_XML_H__