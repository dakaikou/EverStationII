#ifndef __AC3_AUDIO_SEQUENCE_XML_H__
#define __AC3_AUDIO_SEQUENCE_XML_H__

#include "../../compile.h"

#include "../../Include/ac3_audio_sequence.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int ac3_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
_CDL_EXPORT int ac3_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
_CDL_EXPORT int	ac3_decode_syncinfo_to_xml(uint8_t* syncinfo_buf, int syncinfo_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AC3_syncinfo_t* pac3_syncinfo = NULL);

_CDL_EXPORT int	ac3_decode_syncframe_to_xml(uint8_t* es_frame_buf, int es_frame_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AC3_syncframe_t* pac3_syncframe = NULL);

#endif

