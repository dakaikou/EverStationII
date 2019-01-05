#ifndef __DVB_DATA_BROADCAST_XML_H__
#define __DVB_DATA_BROADCAST_XML_H__

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

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_DataBroadcast.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

int DVB_Databroadcast_present_multiprotocol_encapsulation_info_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiprotocol_encapsulation_info_t* pmultiprotocol_encapsulation_info);

int DVB_Databroadcast_present_data_carousel_info_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_carousel_info_t* pdata_carousel_info);

int DVB_Databroadcast_present_object_carousel_info_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, object_carousel_info_t* pobject_carousel_info);

int DVB_Databroadcast_present_IP_MAC_notification_info_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IP_MAC_notification_info_t* pIP_MAC_notification_info);

#endif