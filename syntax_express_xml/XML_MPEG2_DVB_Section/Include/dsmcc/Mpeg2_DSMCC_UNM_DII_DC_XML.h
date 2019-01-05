#ifndef _MPEG_DSMCC_UNM_DII_DC_XML_H_
#define _MPEG_DSMCC_UNM_DII_DC_XML_H_

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DII_DC.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

int	MPEG2_DSMCC_DII_DC_DecodeModuleInfo_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DC_ModuleInfo_t* pModuleInfo = NULL);
int	MPEG2_DSMCC_DII_DC_PresentModuleInfo_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DC_ModuleInfo_t* pModuleInfo);

#endif
