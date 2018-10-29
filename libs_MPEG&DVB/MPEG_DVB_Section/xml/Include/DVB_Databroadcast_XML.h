#ifndef __DVB_DATA_BROADCAST_XML_H__
#define __DVB_DATA_BROADCAST_XML_H__

#include "../../compile.h"

#include "../../Include/DVB_DataBroadcast.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int DVB_Databroadcast_present_multiprotocol_encapsulation_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiprotocol_encapsulation_info_t* pmultiprotocol_encapsulation_info);

int DVB_Databroadcast_present_data_carousel_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_carousel_info_t* pdata_carousel_info);

int DVB_Databroadcast_present_object_carousel_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, object_carousel_info_t* pobject_carousel_info);

int DVB_Databroadcast_present_IP_MAC_notification_info_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IP_MAC_notification_info_t* pIP_MAC_notification_info);

#endif