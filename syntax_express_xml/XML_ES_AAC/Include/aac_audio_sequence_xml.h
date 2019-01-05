#ifndef __KERNEL_AAC_AUDIO_SEQUENCE_XML_H__
#define __KERNEL_AAC_AUDIO_SEQUENCE_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_AAC_EXPORT
#       define XML_ES_AAC_LIB __declspec(dllexport)
#   elif defined(_XML_ES_AAC_IMPORT)
#       define XML_ES_AAC_LIB __declspec(dllimport)
#   else
#       define XML_ES_AAC_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_AAC_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_AAC_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/ES_AAC/Include/aac_audio_sequence.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_ES_AAC_LIB int aac_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_AAC_LIB int aac_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);

XML_ES_AAC_LIB int	aac_decode_adts_fixed_header_to_xml(uint8_t* adts_fixed_header_buf, int adts_fixed_header_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AAC_adts_fixed_header_t* padts_fixed_header = NULL);
XML_ES_AAC_LIB int	aac_decode_adts_variable_header_to_xml(uint8_t* adts_variable_header_buf, int adts_variable_header_length, AAC_adts_variable_header_t* padts_variable_header);
XML_ES_AAC_LIB int	aac_decode_adts_error_check_to_xml(uint8_t* adts_error_check_buf, int adts_error_check_length, AAC_adts_error_check_t* padts_error_check);


#endif

