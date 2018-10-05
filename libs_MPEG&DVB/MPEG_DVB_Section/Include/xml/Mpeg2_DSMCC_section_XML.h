#ifndef _MPEG_DSMCC_SECTION_XML_H_
#define _MPEG_DSMCC_SECTION_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../Mpeg2_DSMCC_section.h"
#include "../dsmcc/Mpeg2_DSMCC_UNM.h"
#include "../dsmcc/Mpeg2_DSMCC_DDM.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_PresentIOR_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IOP::IOR_t* pIOR);

int	MPEG2_DSMCC_DSI_PresentGroupInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication);
int	MPEG2_DSMCC_DSI_PresentServiceGatewayInfo_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo);

int	MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate);
int	MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication);

int MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock);

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeSection_to_XML(uint8_t *section_buf, int section_length, XMLDocForMpegSyntax* pxmlDoc, dsmcc_section_t* pDSMCCSection = NULL);
_CDL_EXPORT	int	MPEG2_DSMCC_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_section_t* pdsmcc_section);

_CDL_EXPORT	int MPEG2_DSMCC_UNM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_unm_section_t* pDSMCCSection = NULL);
_CDL_EXPORT	int MPEG2_DSMCC_UNM_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_unm_section_t* pdsmcc_section);

_CDL_EXPORT	int MPEG2_DSMCC_DDM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_ddm_section_t* pDSMCCSection = NULL);
_CDL_EXPORT	int MPEG2_DSMCC_DDM_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_ddm_section_t* pdsmcc_section);


#endif
