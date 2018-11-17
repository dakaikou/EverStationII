#ifndef _MPEG_DSMCC_UNM_DII_OC_XML_H_
#define _MPEG_DSMCC_UNM_DII_OC_XML_H_

#include "translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DII_OC.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

int	MPEG2_DSMCC_DII_OC_DecodeModuleInfo_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::ModuleInfo_t* pModuleInfo = NULL);
int	MPEG2_DSMCC_DII_OC_PresentModuleInfo_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::ModuleInfo_t* pModuleInfo);

#endif
