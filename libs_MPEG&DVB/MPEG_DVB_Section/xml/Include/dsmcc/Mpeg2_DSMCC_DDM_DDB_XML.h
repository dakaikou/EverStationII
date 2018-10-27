#ifndef _MPEG_DSMCC_DDM_DDB_XML_H_
#define _MPEG_DSMCC_DDM_DDB_XML_H_

#include "Mpeg2_DSMCC_DDM_DDB_OC_XML.h"
#include "Mpeg2_DSMCC_DDM_DDB_DC_XML.h"

#include "../../../Include/dsmcc/Mpeg2_DSMCC_DDM_DDB.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int	MPEG2_DSMCC_DDM_DecodeDownloadDataBlock_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock = NULL);
_CDL_EXPORT int MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock);

#endif
