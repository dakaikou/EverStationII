#ifndef __MPEG_DVB_COMMON_XML_H__
#define __MPEG_DVB_COMMON_XML_H__

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

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_Common.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB int report_descriptor_length_error_to_xml(uint8_t *buf, int length, int max_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_MPEG_DVB_LIB int present_reserved_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, reserved_descriptor_t* preserved_descriptor = NULL);

XML_MPEG_DVB_LIB int MPEG_DVB_present_reserved_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, reserved_descriptor_t* preserved_descriptor);

#endif