#ifndef _H264_VIDEO_SEQUENCE_DECODER_H_
#define _H264_VIDEO_SEQUENCE_DECODER_H_

#include "../../Include/h264_video_sequence.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT int h264_decode_unaligned_nal_to_xml(uint8_t* segment_buf, int prefix_length, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);
_CDL_EXPORT int h264_decode_unknown_nal_to_xml(uint8_t* segment_buf, int prefix_length, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode);

#endif

