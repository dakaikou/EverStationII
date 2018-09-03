#ifndef __DVB_MHP_DESCRIPTOR_XML_H__
#define __DVB_MHP_DESCRIPTOR_XML_H__

#include <stdint.h>
#include "../../compile.h"

#include "../DVB_MHP_Descriptor.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT int DVB_MHP_decode_application_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, application_descriptor_t* papplication_descriptor = NULL);
_CDL_EXPORT int DVB_MHP_decode_application_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, application_name_descriptor_t* papplication_name_descriptor = NULL);
_CDL_EXPORT int DVB_MHP_decode_transport_protocol_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, transport_protocol_descriptor_t* ptransport_protocol_descriptor = NULL);
_CDL_EXPORT int DVB_MHP_decode_dvb_j_application_location_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor = NULL);

#endif