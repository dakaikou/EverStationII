#ifndef _MPEG_DSMCC_UNM_DSI_XML_H_
#define _MPEG_DSMCC_UNM_DSI_XML_H_

#include <stdint.h>

#include "Mpeg2_DSMCC_UNM_DSI_DC_XML.h"
#include "Mpeg2_DSMCC_UNM_DSI_OC_XML.h"

#include "../../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI.h"

int	MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate = NULL);
int	MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate);

#endif
