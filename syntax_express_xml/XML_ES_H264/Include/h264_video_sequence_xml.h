#ifndef _H264_VIDEO_SEQUENCE_DECODER_H_
#define _H264_VIDEO_SEQUENCE_DECODER_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_H264_EXPORT
#       define XML_ES_H264_LIB __declspec(dllexport)
#   elif defined(_XML_ES_H264_IMPORT)
#       define XML_ES_H264_LIB __declspec(dllimport)
#   else
#       define XML_ES_H264_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_H264_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_H264_LIB
#endif

#include <stdint.h>

#include "translate_layer/ES_H264/Include/h264_video_sequence.h"

#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

XML_ES_H264_LIB int h264_present_unaligned_nal_to_xml(uint8_t* segment_buf, int prefix_length, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_H264_LIB int h264_present_unknown_nal_to_xml(uint8_t* segment_buf, int prefix_length, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);

#endif

