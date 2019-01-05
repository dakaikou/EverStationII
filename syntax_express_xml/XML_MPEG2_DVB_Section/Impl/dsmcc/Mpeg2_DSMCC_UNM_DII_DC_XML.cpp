#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_DC_XML.h"
#include "../../Include/MPEG_DVB_Common_XML.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_DII_DC_DecodeModuleInfo_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DC_ModuleInfo_t* pModuleInfo)
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

int	MPEG2_DSMCC_DII_DC_PresentModuleInfo_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DC_ModuleInfo_t* pDC_moduleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDC_moduleInfo != NULL))
	{
		XMLElement* pxmlModuleNode = pxmlParentNode;

		for (int descriptor_index = 0; descriptor_index < pDC_moduleInfo->module_descriptor_count; descriptor_index++)
		{
			uint8_t* descriptor_buf = pDC_moduleInfo->module_descriptors[descriptor_index].descriptor_buf;
			uint16_t descriptor_tag = pDC_moduleInfo->module_descriptors[descriptor_index].descriptor_tag;
			uint8_t descriptor_length = pDC_moduleInfo->module_descriptors[descriptor_index].descriptor_length;
			int descriptor_size = descriptor_length + 2;

			switch (descriptor_tag)
			{
			//case DVB_SI_NETWORK_NAME_DESCRIPTOR:
			//	DVB_SI_decode_network_name_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlModuleNode);
			//	break;
				//case DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:
				//	DVB_SI_decode_multilingual_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
				//	break;
			default:
				MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlModuleNode, pDC_moduleInfo->module_descriptors + descriptor_index);
				break;
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
