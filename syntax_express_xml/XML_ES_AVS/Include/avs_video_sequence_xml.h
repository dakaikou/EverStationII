#ifndef _AVS_VIDEO_SEQUENCE_XML_H_
#define _AVS_VIDEO_SEQUENCE_XML_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_AVS_EXPORT
#       define XML_ES_AVS_LIB __declspec(dllexport)
#   elif defined(_XML_ES_AVS_IMPORT)
#       define XML_ES_AVS_LIB __declspec(dllimport)
#   else
#       define XML_ES_AVS_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_AVS_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_AVS_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/ES_AVS/Include/avs_video_sequence.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_ES_AVS_LIB int avs_present_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_AVS_LIB int avs_present_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);

XML_ES_AVS_LIB int	avs_decode_pb_picture_header_to_xml(uint8_t* nal_buf, int nal_length, AVS_global_param_t* pparam, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AVS_pb_picture_header_t* ppb_picture_header = NULL);
XML_ES_AVS_LIB int	avs_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, AVS_global_param_t* pparam, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AVS_slice_t* pslice = NULL);


#endif

