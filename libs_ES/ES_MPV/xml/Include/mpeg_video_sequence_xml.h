#ifndef _KERNEL_MPEG_VIDEO_DECODER_XML_H_
#define _KERNEL_MPEG_VIDEO_DECODER_XML_H_

#include "../../compile.h"

#include "../../Include/mpeg_video_sequence.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int mpgv_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);
_CDL_EXPORT int mpgv_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode);

_CDL_EXPORT int mpgv_decode_sequence_header_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* psequence_header = NULL);
_CDL_EXPORT int mpgv_present_sequence_header_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* psequence_header);

_CDL_EXPORT int	mpgv_decode_sequence_extension_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* psequence_extension = NULL);
_CDL_EXPORT int	mpgv_present_sequence_extension_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* psequence_extension);

_CDL_EXPORT int	mpgv_decode_sequence_display_extension_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* psequence_display_extension = NULL);
_CDL_EXPORT int	mpgv_present_sequence_display_extension_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* psequence_display_extension);

_CDL_EXPORT int	mpgv_decode_group_of_pictures_header_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header = NULL);
_CDL_EXPORT int	mpgv_present_group_of_pictures_header_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header);

_CDL_EXPORT int mpgv_decode_picture_header_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* ppicture_header = NULL);
_CDL_EXPORT int mpgv_present_picture_header_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* ppicture_header);

_CDL_EXPORT int	mpgv_decode_picture_coding_extension_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* ppicture_coding_extension = NULL);
_CDL_EXPORT int	mpgv_present_picture_coding_extension_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* ppicture_coding_extension);

_CDL_EXPORT int	mpgv_decode_user_data_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* puser_data = NULL);
_CDL_EXPORT int	mpgv_present_user_data_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* puser_data);

_CDL_EXPORT int	mpgv_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pslice = NULL);
_CDL_EXPORT int	mpgv_present_slice_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pslice);


#endif


