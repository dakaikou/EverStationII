#ifndef _MPEG_DSMCC_UNM_DII_XML_H_
#define _MPEG_DSMCC_UNM_DII_XML_H_

#include "Mpeg2_DSMCC_UNM_DII_DC_XML.h"
#include "Mpeg2_DSMCC_UNM_DII_OC_XML.h"

#include "../../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII.h"

int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication = NULL);
int	MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication);

#endif
