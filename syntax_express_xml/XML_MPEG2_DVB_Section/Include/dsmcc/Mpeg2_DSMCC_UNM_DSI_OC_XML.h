#ifndef _MPEG_DSMCC_UNM_DSI_OC_XML_H_
#define _MPEG_DSMCC_UNM_DSI_OC_XML_H_

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_OC.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

int	MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo = NULL);
int	MPEG2_DSMCC_DSI_OC_PresentServiceGatewayInfo_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo);


#endif
