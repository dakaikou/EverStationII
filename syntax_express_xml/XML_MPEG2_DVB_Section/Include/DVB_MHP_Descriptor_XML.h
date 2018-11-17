#ifndef __DVB_MHP_DESCRIPTOR_XML_H__
#define __DVB_MHP_DESCRIPTOR_XML_H__

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

#include "translate_layer/MPEG2_DVB_Section/Include/DVB_MHP_Descriptor.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB int DVB_MHP_decode_application_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_descriptor_t* papplication_descriptor = NULL);
XML_MPEG_DVB_LIB int DVB_MHP_present_application_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_descriptor_t* papplication_descriptor);

XML_MPEG_DVB_LIB int DVB_MHP_decode_application_name_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_name_descriptor_t* papplication_name_descriptor = NULL);
XML_MPEG_DVB_LIB int DVB_MHP_present_application_name_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, application_name_descriptor_t* papplication_name_descriptor);

XML_MPEG_DVB_LIB int DVB_MHP_decode_transport_protocol_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, transport_protocol_descriptor_t* ptransport_protocol_descriptor = NULL);
XML_MPEG_DVB_LIB int DVB_MHP_present_transport_protocol_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, transport_protocol_descriptor_t* ptransport_protocol_descriptor);

XML_MPEG_DVB_LIB int DVB_MHP_decode_dvb_j_application_location_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor = NULL);
XML_MPEG_DVB_LIB int DVB_MHP_present_dvb_j_application_location_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor);

#endif