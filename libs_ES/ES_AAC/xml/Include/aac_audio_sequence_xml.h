#ifndef __KERNEL_AAC_AUDIO_SEQUENCE_XML_H__
#define __KERNEL_AAC_AUDIO_SEQUENCE_XML_H__

#include "../../compile.h"
#include "../../Include/aac_audio_sequence.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT int aac_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);
_CDL_EXPORT int aac_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);

_CDL_EXPORT int	aac_decode_adts_fixed_header_to_xml(uint8_t* adts_fixed_header_buf, int adts_fixed_header_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, AAC_adts_fixed_header_t* padts_fixed_header = NULL);
_CDL_EXPORT int	aac_decode_adts_variable_header_to_xml(uint8_t* adts_variable_header_buf, int adts_variable_header_length, AAC_adts_variable_header_t* padts_variable_header);
_CDL_EXPORT int	aac_decode_adts_error_check_to_xml(uint8_t* adts_error_check_buf, int adts_error_check_length, AAC_adts_error_check_t* padts_error_check);


#endif

