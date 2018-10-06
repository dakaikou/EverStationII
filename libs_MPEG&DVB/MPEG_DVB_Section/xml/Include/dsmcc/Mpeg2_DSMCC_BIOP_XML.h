#ifndef _MPEG_DSMCC_COMMON_XML_H_
#define _MPEG_DSMCC_COMMON_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

//�޷�Ԥ��IOR()�ṹ��ĳ��ȣ����Ǽ���һ�飬��˵�һ������������ش�ָ��
int	MPEG2_DSMCC_IOP_PresentIOR_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, IOP::IOR_t* pIOR);

int	MPEG2_DSMCC_IOP_DecodeBIOPProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOPProfileBody_t* pBIOPProfileBody = NULL);
int	MPEG2_DSMCC_IOP_PresentBIOPProfileBody_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOPProfileBody_t* pBIOPProfileBody);

int	MPEG2_DSMCC_IOP_DecodeLiteOptionsProfileBody_to_xml(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, LiteOptionsProfileBody_t* pLiteOptionsProfileBody = NULL);
int	MPEG2_DSMCC_IOP_PresentLiteOptionsProfileBody_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, LiteOptionsProfileBody_t* pLiteOptionsProfileBody);

#endif
