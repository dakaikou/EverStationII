#ifndef __MPEG_DSMCC_DESCRIPTOR_XML_H__
#define __MPEG_DSMCC_DESCRIPTOR_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_MPEG_DVB_EXPORT
#       define XML_MPEG_DVB_LIB __declspec(dllexport)
#   elif defined(_XML_MPEG_DVB_IMPORT)
#       define XML_MPEG_DVB_LIB __declspec(dllimport)
#   else
#       define XML_MPEG_DVB_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_MPEG_DVB_LIB __attribute__((visibility("default")))
#else
#   define XML_MPEG_DVB_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Descriptor.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB int MPEG2_DSMCC_decode_name_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, name_descriptor_t* pname_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_DSMCC_present_name_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, name_descriptor_t* pname_descriptor);

XML_MPEG_DVB_LIB int MPEG2_DSMCC_decode_location_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, location_descriptor_t* plocation_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_DSMCC_present_location_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, location_descriptor_t* plocation_descriptor);

#endif