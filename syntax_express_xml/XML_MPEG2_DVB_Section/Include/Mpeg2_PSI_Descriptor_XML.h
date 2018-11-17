#ifndef __MPEG_PSI_DESCRIPTOR_XML_H__
#define __MPEG_PSI_DESCRIPTOR_XML_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_MPEG_DVB_EXPORT
#       define XML_MPEG_DVB_LIB __declspec(dllexport)
#   elif defined(_XML_MPEG_DVB_IMPORT)
#       define XML_MPEG_DVB_LIB __declspec(dllimport)
#   else
#       define XML_MPEG_DVB_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_MPEG_DVB_LIB __attribute__((visibility("default")))
#else
#   define XML_MPEG_DVB_LIB
#endif

#include <stdint.h>

#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

/*TAG = PSI_VIDEO_STREAM_DESCRIPTOR			0x02*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_video_stream_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, video_stream_descriptor_t* pvideo_stream_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_video_stream_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, video_stream_descriptor_t* pvideo_stream_descriptor);

/*TAG = PSI_AUDIO_STREAM_DESCRIPTOR			0x03*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_audio_stream_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, audio_stream_descriptor_t* paudio_stream_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_audio_stream_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, audio_stream_descriptor_t* paudio_stream_descriptor);

/*TAG = PSI_REGISTRATION_DESCRIPTOR						0x05	*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_registration_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, registration_descriptor_t* pregistration_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_registration_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, registration_descriptor_t* pregistration_descriptor);

/*TAG = PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR		0x06*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_data_stream_alignment_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_stream_alignment_descriptor_t* pdata_stream_alignment_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_data_stream_alignment_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_stream_alignment_descriptor_t* pdata_stream_alignment_descriptor = NULL);

/*TAG = MPEG2_PSI_CA_DESCRIPTOR				0x09*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_CA_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, CA_descriptor_t* pCA_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_CA_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, CA_descriptor_t* pCA_descriptor);

/*TAG = PSI_ISO_639_LANGUAGE_DESCRIPTOR				0x0A*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_ISO_639_language_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ISO_639_language_descriptor_t* pISO_639_language_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_ISO_639_language_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ISO_639_language_descriptor_t* pISO_639_language_descriptor);

/*TAG = PSI_SYSTEM_CLOCK_DESCRIPTOR			0x0B*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_system_clock_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, system_clock_descriptor_t* psystem_clock_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_system_clock_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, system_clock_descriptor_t* psystem_clock_descriptor);

/*TAG = PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR		0x0C*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_multiplex_buffer_utilization_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor);

/*TAB = PSI_MAXIMUM_BITRATE_DESCRIPTOR				0x0E*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_maximum_bitrate_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor);

/*TAG = PSI_SMOOTHING_BUFFER_DESCRIPTOR					0x10*/
XML_MPEG_DVB_LIB int MPEG2_PSI_decode_smoothing_buffer_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_present_smoothing_buffer_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor);

/*TAG = DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR						0x13*/
XML_MPEG_DVB_LIB int MPEG2_DSMCC_decode_carousel_identifier_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, carousel_identifier_descriptor_t* pcarousel_identifier_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_DSMCC_present_carousel_identifier_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, carousel_identifier_descriptor_t* pcarousel_identifier_descriptor);

/*TAG = DSMCC_ASSOCIATION_TAG_DESCRIPTOR					0x14*/
XML_MPEG_DVB_LIB int MPEG2_DSMCC_decode_association_tag_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, association_tag_descriptor_t* passociation_tag_descriptor = NULL);
XML_MPEG_DVB_LIB int MPEG2_DSMCC_present_association_tag_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, association_tag_descriptor_t* passociation_tag_descriptor);

#endif