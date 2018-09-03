#ifndef _AVS_VIDEO_SEQUENCE_XML_H_
#define _AVS_VIDEO_SEQUENCE_XML_H_

#include <stdint.h>

#include "../../compile.h"
#include "../avs_video_sequence.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT int avs_decode_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);
_CDL_EXPORT int avs_decode_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);

_CDL_EXPORT int	avs_decode_pb_picture_header_to_xml(uint8_t* nal_buf, int nal_length, AVS_global_param_t* pparam, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, AVS_pb_picture_header_t* ppb_picture_header = NULL);
_CDL_EXPORT int	avs_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, AVS_global_param_t* pparam, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, AVS_slice_t* pslice = NULL);


#endif

