#ifndef __MPEG_PSI_DESCRIPTOR_XML_H__
#define __MPEG_PSI_DESCRIPTOR_XML_H__

#include "../../compile.h"

#include "../Mpeg2_PSI_Descriptor.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

/*TAG = PSI_VIDEO_STREAM_DESCRIPTOR			0x02*/
_CDL_EXPORT int MPEG2_PSI_decode_video_stream_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, video_stream_descriptor_t* pvideo_stream_descriptor = NULL);

/*TAG = PSI_AUDIO_STREAM_DESCRIPTOR			0x03*/
_CDL_EXPORT int MPEG2_PSI_decode_audio_stream_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, audio_stream_descriptor_t* paudio_stream_descriptor = NULL);

/*TAG = PSI_REGISTRATION_DESCRIPTOR						0x05	*/
_CDL_EXPORT int MPEG2_PSI_decode_registration_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, registration_descriptor_t* pregistration_descriptor = NULL);

/*TAG = PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR		0x06*/
_CDL_EXPORT int MPEG2_PSI_decode_data_stream_alignment_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_stream_alignment_descriptor_t* pdata_stream_alignment_descriptor = NULL);

/*TAG = PSI_CA_DESCRIPTOR				0x09*/
_CDL_EXPORT int MPEG2_PSI_decode_CA_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, CA_descriptor_t* pCA_descriptor = NULL);

/*TAG = PSI_ISO_639_LANGUAGE_DESCRIPTOR				0x0A*/
_CDL_EXPORT int MPEG2_PSI_decode_ISO_639_language_descriptor(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, ISO_639_language_descriptor_t* pISO_639_language_descriptor = NULL);

/*TAG = PSI_SYSTEM_CLOCK_DESCRIPTOR			0x0B*/
_CDL_EXPORT int MPEG2_PSI_decode_system_clock_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, system_clock_descriptor_t* psystem_clock_descriptor = NULL);

/*TAG = PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR		0x0C*/
_CDL_EXPORT int MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor = NULL);

/*TAB = PSI_MAXIMUM_BITRATE_DESCRIPTOR				0x0E*/
_CDL_EXPORT int MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor = NULL);

/*TAG = PSI_SMOOTHING_BUFFER_DESCRIPTOR					0x10*/
_CDL_EXPORT int MPEG2_PSI_decode_smoothing_buffer_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor = NULL);

/*TAG = DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR						0x13*/
_CDL_EXPORT int MPEG2_DSMCC_decode_carousel_identifier_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, carousel_identifier_descriptor_t* pcarousel_identifier_descriptor = NULL);

/*TAG = DSMCC_ASSOCIATION_TAG_DESCRIPTOR					0x14*/
_CDL_EXPORT int MPEG2_DSMCC_decode_association_tag_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, association_tag_descriptor_t* passociation_tag_descriptor = NULL);

#endif