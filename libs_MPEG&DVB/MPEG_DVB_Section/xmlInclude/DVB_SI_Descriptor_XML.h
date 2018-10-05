#ifndef __DVB_SI_DESCRIPTOR_XML_H__
#define __DVB_SI_DESCRIPTOR_XML_H__

#include "../../compile.h"

#include "../nativeInclude/DVB_SI_Descriptor.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

/*TAG = DVB_SI_NETWORK_NAME_DESCRIPTOR						0x40	*/
_CDL_EXPORT int DVB_SI_decode_network_name_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, network_name_descriptor_t* pnetwork_name_descriptor = NULL);

/*TAG = DVB_SI_SERVICE_LIST_DESCRIPTOR						0x41	*/
_CDL_EXPORT int DVB_SI_decode_service_list_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, service_list_descriptor_t* pservice_list_descriptor = NULL);

/*TAG = DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR			0x43	*/
_CDL_EXPORT int DVB_SI_decode_satellite_delivery_system_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, satellite_delivery_system_descriptor_t* psatellite_delivery_system_descriptor = NULL);

/*TAG = DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR				0x44	*/
_CDL_EXPORT int DVB_SI_decode_cable_delivery_system_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, cable_delivery_system_descriptor_t* pcable_delivery_system_descriptor = NULL);

/*TAG = DVB_SI_BOUQUET_NAME_DESCRIPTOR				0x47		*/
_CDL_EXPORT int DVB_SI_decode_bouquet_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, bouquet_name_descriptor_t* pbouquet_name_descriptor = NULL);

/*TAG = DVB_SI_SERVICE_DESCRIPTOR				0x48	*/
_CDL_EXPORT int DVB_SI_decode_service_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, service_descriptor_t* pservice_descriptor = NULL);

/*TAG = DVB_SI_SHORT_EVENT_DESCRIPTOR					0x4D	*/
_CDL_EXPORT int DVB_SI_decode_short_event_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, short_event_descriptor_t* pshort_event_descriptor = NULL);

/*TAG = DVB_SI_STREAM_IDENTIFIER_DESCRIPTOR				0x52	*/
_CDL_EXPORT int DVB_SI_decode_stream_identifier_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, stream_identifier_descriptor_t* pstream_identifier_descriptor = NULL);

/*TAG = DVB_SI_CA_IDENTIFIER_DESCRIPTOR					0x53	*/
_CDL_EXPORT int DVB_SI_decode_CA_identifier_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, CA_identifier_descriptor_t* pCA_identifier_descriptor = NULL);

/*TAG = DVB_SI_CONTENT_DESCRIPTOR				0x54	*/
_CDL_EXPORT int DVB_SI_decode_content_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, content_descriptor_t* pcontent_descriptor = NULL);

/*TAG = DVB_SI_PARENTAL_RATING_DESCRIPTOR			0x55	*/
_CDL_EXPORT int DVB_SI_decode_parental_rating_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, parental_rating_descriptor_t* pparental_rating_descriptor = NULL);

/*TAG = DVB_SI_TELETEXT_DESCRIPTOR				0x56		*/
_CDL_EXPORT int DVB_SI_decode_teletext_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, teletext_descriptor_t* pteletext_descriptor = NULL);

/*TAG = DVB_SI_SUBTITLING_DESCRIPTOR				0x59		*/
_CDL_EXPORT int DVB_SI_decode_subtitling_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, subtitling_descriptor_t* psubtitling_descriptor = NULL);

/*TAG = DVB_SI_LOCAL_TIME_OFFSET_DESCRIPTOR   0x58*/
_CDL_EXPORT int DVB_SI_decode_local_time_offset_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, local_time_offset_descriptor_t* plocal_time_offset_descriptor = NULL);

/*TAG = DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR				0x5B*/
_CDL_EXPORT int DVB_SI_decode_multilingual_network_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_network_name_descriptor_t* pmultilingual_network_name_descriptor = NULL);

/*TAG = DVB_SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR				0x5C*/
_CDL_EXPORT int DVB_SI_decode_multilingual_bouquet_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_bouquet_name_descriptor_t* pmultilingual_bouquet_name_descriptor = NULL);

/*TAG = DVB_SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR			0x5D*/
_CDL_EXPORT int DVB_SI_decode_multilingual_service_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_service_name_descriptor_t* pmultilingual_service_name_descriptor = NULL);

/*TAG = DVB_SI_MULTILINGUAL_COMPONENT_DESCRIPTOR			0x5E	*/
_CDL_EXPORT int DVB_SI_decode_multilingual_component_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multilingual_component_descriptor_t* pmultilingual_component_descriptor = NULL);

/*TAG = DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR			0x5F	*/
_CDL_EXPORT int DVB_SI_decode_private_data_specifier_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, private_data_specifier_descriptor_t* pprivate_data_specifier_descriptor = NULL);

/*TAG = DVB_SI_DATA_BROADCAST_DESCRIPTOR		0x64		*/
_CDL_EXPORT int DVB_SI_decode_data_broadcast_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_broadcast_descriptor_t* pdata_broadcast_descriptor = NULL);

/*TAG = DVB_SI_DATA_BROADCAST_ID_DESCRIPTOR		0x66*/
_CDL_EXPORT int DVB_SI_decode_data_broadcast_id_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_broadcast_id_descriptor_t* pdata_broadcast_id_descriptor = NULL);

/*TAG = SI_TRANSPORT_STREAM_DESCRIPTOR		0x67*/
_CDL_EXPORT int DVB_SI_decode_transport_stream_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, transport_stream_descriptor_t* ptransport_stream_descriptor = NULL);


/*TAG = DVB_SI_AC3_DESCRIPTOR			0x6A*/
//在TS 102 366中定义
_CDL_EXPORT int DVB_SI_decode_ac3_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, ac3_descriptor_t* pac3_descriptor = NULL);


#endif