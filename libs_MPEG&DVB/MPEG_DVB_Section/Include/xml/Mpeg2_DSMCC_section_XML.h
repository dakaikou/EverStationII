#ifndef _MPEG_DSMCC_SECTION_XML_H_
#define _MPEG_DSMCC_SECTION_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../Mpeg2_DSMCC_section.h"
#include "../Mpeg2_DSMCC_section_UNM.h"
#include "../Mpeg2_DSMCC_section_DDM.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

//无法预测IOR()结构体的长度，除非计算一遍，因此第一个参数串入比特串指针
int	MPEG2_DSMCC_DecodeIOR_to_xml(BITS_t* pbs, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, IOP::IOR_t* pIOR = NULL);

int	MPEG2_DSMCC_DecodeGroupInfoIndication_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication = NULL);
int	MPEG2_DSMCC_DecodeServiceGatewayInfo_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo = NULL);

int	MPEG2_DSMCC_DecodeDownloadServerInitiate_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate = NULL);
int	MPEG2_DSMCC_DecodeDownloadInfoIndication_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication = NULL);
int MPEG2_DSMCC_DecodeDownloadDataBlock_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock = NULL);

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, dsmcc_section_t* pDSMCCSection = NULL);
_CDL_EXPORT	int MPEG2_DSMCC_UNM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, dsmcc_unm_section_t* pDSMCCSection = NULL);
_CDL_EXPORT	int MPEG2_DSMCC_DDM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, dsmcc_ddm_section_t* pDSMCCSection = NULL);

#endif
