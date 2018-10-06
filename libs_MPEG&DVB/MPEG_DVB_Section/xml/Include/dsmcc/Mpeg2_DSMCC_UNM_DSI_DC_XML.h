#ifndef _MPEG_DSMCC_UNM_DSI_DC_XML_H_
#define _MPEG_DSMCC_UNM_DSI_DC_XML_H_

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int	MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication = NULL);
int	MPEG2_DSMCC_DSI_DC_PresentGroupInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication);

#endif
