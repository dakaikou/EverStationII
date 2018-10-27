#ifndef __DVB_DATA_BROADCAST_XML_H__
#define __DVB_DATA_BROADCAST_XML_H__

#include "../../compile.h"

#include "../../Include/DVB_DataBroadcast.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int DVB_Databroadcast_decode_data_carousel_info_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_carousel_info_t* pdata_carousel_info = NULL);
int DVB_Databroadcast_decode_object_carousel_info_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, object_carousel_info_t* pobject_carousel_info = NULL);

#endif