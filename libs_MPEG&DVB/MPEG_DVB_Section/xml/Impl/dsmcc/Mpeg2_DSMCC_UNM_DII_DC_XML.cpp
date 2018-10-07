#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_DC_XML.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_DII_DC_DecodeModuleInfo_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DC_ModuleInfo_t* pModuleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	DC_ModuleInfo_t* pDC_moduleInfo = (pModuleInfo != NULL) ? pModuleInfo : new DC_ModuleInfo_t;
	rtcode = MPEG2_DSMCC_DII_DC_DecodeModuleInfo(buf, size, pDC_moduleInfo);
	rtcode = MPEG2_DSMCC_DII_DC_PresentModuleInfo_to_xml(pxmlDoc, pxmlParentNode, pDC_moduleInfo);

	if (pModuleInfo == NULL)
	{
		//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
		delete pDC_moduleInfo;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DII_DC_PresentModuleInfo_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DC_ModuleInfo_t* pDC_moduleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDC_moduleInfo != NULL))
	{
		XMLElement* pxmlModuleNode = pxmlParentNode;

		//while ((remain_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
		//{
		//	descriptor_tag = (ptemp[0] | 0x3000);
		//	descriptor_length = ptemp[1];
		//	move_length = descriptor_length + 2;

		//	//截成一段一段的描述符
		//	pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
		//	pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
		//	pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_buf = ptemp;
		//	pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

		//	switch (descriptor_tag)
		//	{
		//	case MPEG2_DSMCC_NAME_DESCRIPTOR:
		//		rtcode = MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->name_descriptor));
		//		break;
		//	case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
		//		rtcode = MPEG2_DSMCC_decode_location_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->location_descriptor));
		//		break;
		//	default:

		//		//if (descriptor_tag == 0x3081)
		//		//{
		//		//	if (pDC_moduleInfo->name_descriptor.descriptor_tag == 0x00)
		//		//	{
		//		//		rtcode = MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->name_descriptor));
		//		//	}
		//		//}
		//		//else
		//	{
		//		decode_reserved_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode);
		//	}

		//	break;
		//	}

		//	descriptor_count++;

		//	ptemp += move_length;
		//	remain_length -= move_length;
		//}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
