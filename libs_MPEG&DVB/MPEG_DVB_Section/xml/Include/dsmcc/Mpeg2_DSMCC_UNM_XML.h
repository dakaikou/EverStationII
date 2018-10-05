#ifndef _MPEG_DSMCC_UNM_XML_H_
#define _MPEG_DSMCC_UNM_XML_H_

#include <stdint.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int	MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate);
int	MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication);


#endif
