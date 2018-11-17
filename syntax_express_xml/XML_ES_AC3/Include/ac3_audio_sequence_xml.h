#ifndef __AC3_AUDIO_SEQUENCE_XML_H__
#define __AC3_AUDIO_SEQUENCE_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_AC3_EXPORT
#       define XML_ES_AC3_LIB __declspec(dllexport)
#   elif defined(_XML_ES_AC3_IMPORT)
#       define XML_ES_AC3_LIB __declspec(dllimport)
#   else
#       define XML_ES_AC3_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_AC3_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_AC3_LIB
#endif

#include "translate_layer/ES_AC3/Include/ac3_audio_sequence.h"

#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

XML_ES_AC3_LIB int ac3_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_AC3_LIB int ac3_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_AC3_LIB int	ac3_decode_syncinfo_to_xml(uint8_t* syncinfo_buf, int syncinfo_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AC3_syncinfo_t* pac3_syncinfo = NULL);

XML_ES_AC3_LIB int	ac3_decode_syncframe_to_xml(uint8_t* es_frame_buf, int es_frame_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AC3_syncframe_t* pac3_syncframe = NULL);

#endif

