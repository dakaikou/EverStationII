#ifndef __KERNEL_DRA_AUDIO_SEQUENCE_XML_H__
#define __KERNEL_DRA_AUDIO_SEQUENCE_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_DRA_EXPORT
#       define XML_ES_DRA_LIB __declspec(dllexport)
#   elif defined(_XML_ES_DRA_IMPORT)
#       define XML_ES_DRA_LIB __declspec(dllimport)
#   else
#       define XML_ES_DRA_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_DRA_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_DRA_LIB
#endif

#include <stdint.h>

#include "translate_layer/ES_DRA/Include/dra_audio_sequence.h"

#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

XML_ES_DRA_LIB int dra_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_DRA_LIB int dra_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);

XML_ES_DRA_LIB int	dra_decode_frame_to_xml(uint8_t* adts_fixed_header_buf, int adts_fixed_header_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DRA_Frame_t* pdra_frame = NULL);
//_CDL_EXPORT int	dra_decode_adts_variable_header_to_xml(uint8_t* adts_variable_header_buf, int adts_variable_header_length, AAC_adts_variable_header_t* padts_variable_header);
//_CDL_EXPORT int	dra_decode_adts_error_check_to_xml(uint8_t* adts_error_check_buf, int adts_error_check_length, AAC_adts_error_check_t* padts_error_check);


#endif

