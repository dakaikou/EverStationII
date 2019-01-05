#ifndef _KERNEL_MPEG_VIDEO_DECODER_XML_H_
#define _KERNEL_MPEG_VIDEO_DECODER_XML_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_ES_MPV_EXPORT
#       define XML_ES_MPV_LIB __declspec(dllexport)
#   elif defined(_XML_ES_MPV_IMPORT)
#       define XML_ES_MPV_LIB __declspec(dllimport)
#   else
#       define XML_ES_MPV_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_ES_MPV_LIB __attribute__((visibility("default")))
#else
#   define XML_ES_MPV_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer/ES_MPV/Include/mpeg_video_sequence.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_ES_MPV_LIB int mpgv_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
XML_ES_MPV_LIB int mpgv_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);

XML_ES_MPV_LIB int mpgv_decode_sequence_header_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* psequence_header = NULL);
XML_ES_MPV_LIB int mpgv_present_sequence_header_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* psequence_header);

XML_ES_MPV_LIB int	mpgv_decode_sequence_extension_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* psequence_extension = NULL);
XML_ES_MPV_LIB int	mpgv_present_sequence_extension_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* psequence_extension);

XML_ES_MPV_LIB int	mpgv_decode_sequence_display_extension_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* psequence_display_extension = NULL);
XML_ES_MPV_LIB int	mpgv_present_sequence_display_extension_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* psequence_display_extension);

XML_ES_MPV_LIB int	mpgv_decode_group_of_pictures_header_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header = NULL);
XML_ES_MPV_LIB int	mpgv_present_group_of_pictures_header_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header);

XML_ES_MPV_LIB int mpgv_decode_picture_header_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* ppicture_header = NULL);
XML_ES_MPV_LIB int mpgv_present_picture_header_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* ppicture_header);

XML_ES_MPV_LIB int	mpgv_decode_picture_coding_extension_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* ppicture_coding_extension = NULL);
XML_ES_MPV_LIB int	mpgv_present_picture_coding_extension_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* ppicture_coding_extension);

XML_ES_MPV_LIB int	mpgv_decode_user_data_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* puser_data = NULL);
XML_ES_MPV_LIB int	mpgv_present_user_data_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* puser_data);

XML_ES_MPV_LIB int	mpgv_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pslice = NULL);
XML_ES_MPV_LIB int	mpgv_present_slice_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pslice);


#endif


