#ifndef _MPEG_DSMCC_UNM_DSI_DC_XML_H_
#define _MPEG_DSMCC_UNM_DSI_DC_XML_H_

#include "translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_DC.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

int	MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication = NULL);
int	MPEG2_DSMCC_DSI_DC_PresentGroupInfoIndication_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication);

#endif
