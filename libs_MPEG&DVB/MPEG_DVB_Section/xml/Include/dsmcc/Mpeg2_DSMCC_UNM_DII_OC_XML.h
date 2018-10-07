#ifndef _MPEG_DSMCC_UNM_DII_OC_XML_H_
#define _MPEG_DSMCC_UNM_DII_OC_XML_H_

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_OC.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int	MPEG2_DSMCC_DII_OC_DecodeModuleInfo_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::ModuleInfo_t* pModuleInfo = NULL);
int	MPEG2_DSMCC_DII_OC_PresentModuleInfo_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::ModuleInfo_t* pModuleInfo);

#endif
