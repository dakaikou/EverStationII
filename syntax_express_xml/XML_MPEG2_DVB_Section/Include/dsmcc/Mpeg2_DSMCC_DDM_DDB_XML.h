#ifndef _MPEG_DSMCC_DDM_DDB_XML_H_
#define _MPEG_DSMCC_DDM_DDB_XML_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_MPEG_DVB_EXPORT
#       define XML_MPEG_DVB_LIB __declspec(dllexport)
#   elif defined(_XML_MPEG_DVB_IMPORT)
#       define XML_MPEG_DVB_LIB __declspec(dllimport)
#   else
#       define XML_MPEG_DVB_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_MPEG_DVB_LIB __attribute__((visibility("default")))
#else
#   define XML_MPEG_DVB_LIB
#endif

#include <stdint.h>


#include "Mpeg2_DSMCC_DDM_DDB_OC_XML.h"
#include "Mpeg2_DSMCC_DDM_DDB_DC_XML.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_DDM_DDB.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB int	MPEG2_DSMCC_DDM_DecodeDownloadDataBlock_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock = NULL);
XML_MPEG_DVB_LIB int MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock);

#endif
