#ifndef __API_MPEG_AUDIO_SEQUENCE_XML_H__
#define __API_MPEG_AUDIO_SEQUENCE_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_MPA_EXPORT
#       define XML_ES_MPA_LIB __declspec(dllexport)
#   elif defined(_XML_ES_MPA_IMPORT)
#       define XML_ES_MPA_LIB __declspec(dllimport)
#   else
#       define XML_ES_MPA_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_MPA_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_MPA_LIB
#endif

#include <stdint.h>

#include "translate_layer/ES_MPA/Include/mpeg_audio_sequence.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

//public functions
XML_ES_MPA_LIB int mpga_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_MPA_LIB int mpga_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
//XML_ES_MPA_LIB int	mpga_decode_frame_header_to_xml(uint8_t* es_header_buf, int es_header_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPA_header_t* pmpa_header = NULL);

XML_ES_MPA_LIB int	mpga_decode_frame_to_xml(uint8_t* es_frame_buf, int es_frame_size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPA_frame_t* pmpa_frame = NULL);
XML_ES_MPA_LIB int	mpga_present_frame_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPA_frame_t* pmpa_frame);

#endif

