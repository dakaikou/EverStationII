#ifndef _MPEG_DSMCC_UNM_DSI_OC_XML_H_
#define _MPEG_DSMCC_UNM_DSI_OC_XML_H_

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_OC.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int	MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo = NULL);
int	MPEG2_DSMCC_DSI_OC_PresentServiceGatewayInfo_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo);


#endif
