#include <string.h>
#include <stdlib.h>
#include <assert.h>

/////////////////////////////////////////////
#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_OC_XML.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_Utilities.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_DII_OC_DecodeModuleInfo_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::ModuleInfo_t* pModuleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	BIOP::ModuleInfo_t* pOC_ModuleInfo = (pModuleInfo != NULL) ? pModuleInfo : new BIOP::ModuleInfo_t;
	rtcode = MPEG2_DSMCC_DII_OC_DecodeModuleInfo(buf, size, pOC_ModuleInfo);
	rtcode = MPEG2_DSMCC_DII_OC_PresentModuleInfo_to_xml(pxmlDoc, pxmlParentNode, pOC_ModuleInfo);

	if (pModuleInfo == NULL)
	{
		//说明pServiceGatewayInfo指针临时分配，函数返回前需要释放
		delete pOC_ModuleInfo;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DII_OC_PresentModuleInfo_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, BIOP::ModuleInfo_t* pOC_moduleInfo)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	char pszField[64];
	char pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pOC_moduleInfo != NULL))
	{
		XMLElement* pxmlModuleNode = pxmlParentNode;

		pxmlDoc->NewElementForByteMode(pxmlModuleNode, "moduleTimeOut", pOC_moduleInfo->moduleTimeOut, 4, NULL);

		pxmlDoc->NewElementForByteMode(pxmlModuleNode, "blockTimeOut", pOC_moduleInfo->blockTimeOut, 4, NULL);

		pxmlDoc->NewElementForByteMode(pxmlModuleNode, "minBlockTime", pOC_moduleInfo->minBlockTime, 4, NULL);

		pxmlDoc->NewElementForByteMode(pxmlModuleNode, "taps_count", pOC_moduleInfo->taps_count, 1, NULL);

		for (int tap_index = 0; tap_index < pOC_moduleInfo->taps_count; tap_index++)
		{
			sprintf_s(pszField, sizeof(pszField), "TAP[%d]()", tap_index);
			XMLElement* pxmlTapNode = pxmlDoc->NewBranchElement(pxmlModuleNode, pszField, NULL, 7 + pOC_moduleInfo->TAP[tap_index].selector_length);

			pxmlDoc->NewElementForByteMode(pxmlTapNode, "ID", pOC_moduleInfo->TAP[tap_index].id, 2, NULL);

			MPEG2_DSMCC_NumericCoding2Text_tapUse(pOC_moduleInfo->TAP[tap_index].use, pszComment, sizeof(pszComment));
			pxmlDoc->NewElementForByteMode(pxmlTapNode, "use", pOC_moduleInfo->TAP[tap_index].use, 2, pszComment);

			pxmlDoc->NewElementForByteMode(pxmlTapNode, "association_tag", pOC_moduleInfo->TAP[tap_index].association_tag, 2, NULL);

			pxmlDoc->NewElementForByteMode(pxmlTapNode, "selector_length", pOC_moduleInfo->TAP[tap_index].selector_length, 1, NULL);
		}

		pxmlDoc->NewElementForByteMode(pxmlModuleNode, "userInfoLength", pOC_moduleInfo->userInfoLength, 1, NULL);

		if (pOC_moduleInfo->userInfoLength > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlModuleNode, "module_info_byte[ ]", pOC_moduleInfo->userInfo_data_byte, pOC_moduleInfo->userInfoLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

