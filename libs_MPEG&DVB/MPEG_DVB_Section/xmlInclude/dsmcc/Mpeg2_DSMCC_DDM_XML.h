#ifndef _MPEG_DSMCC_DDM_XML_H_
#define _MPEG_DSMCC_DDM_XML_H_

#include "../../nativeInclude/dsmcc/Mpeg2_DSMCC_DDM.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

int MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock);


#endif
