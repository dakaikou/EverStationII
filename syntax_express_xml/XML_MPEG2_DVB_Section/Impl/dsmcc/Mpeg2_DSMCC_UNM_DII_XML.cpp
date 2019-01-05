#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_XML.h"

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/dsmcc/Mpeg2_DSMCC_UNM_DII.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication_to_xml(uint8_t* buf, int size, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDII)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	DownloadInfoIndication_t* pDownloadInfoIndication = (pDII != NULL) ? pDII : new DownloadInfoIndication_t;
	rtcode = MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(buf, size, pDownloadInfoIndication);
	rtcode = MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(pxmlDoc, pxmlParentNode, pDownloadInfoIndication);

	if (pDII == NULL)
	{
		//说明pDownloadInfoIndication指针临时分配，函数返回前需要释放
		delete pDownloadInfoIndication;
	}

	return rtcode;
}

int	MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	char		pszField[96];
	char		pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDownloadInfoIndication != NULL))
	{
		//XMLElement* pxmlSessionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "DownloadInfoIndication()", NULL);
		XMLElement* pxmlSessionNode = pxmlParentNode;

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "downloadId", pDownloadInfoIndication->downloadId, 4, NULL);

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "blockSize", pDownloadInfoIndication->blockSize, 2, NULL);

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "windowSize", pDownloadInfoIndication->windowSize, 1, NULL);

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "ackPeriod", pDownloadInfoIndication->ackPeriod, 1, NULL);

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "tCDownloadWindow", pDownloadInfoIndication->tCDownloadWindow, 4, NULL);

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "tCDownloadScenario", pDownloadInfoIndication->tCDownloadScenario, 4, NULL);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadInfoIndication->compatibilityDescriptor);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlSessionNode, "compatibilityDescriptor()", NULL, 2 + pcompatibilityDescriptor->compatibilityDescriptorLength);

		pxmlDoc->NewElementForByteMode(pxmlDescriptorNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);

		if (pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "compatibilityDescriptorBuf[ ]", pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
		}

		//defined in EN 301192
		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "numberOfModules", pDownloadInfoIndication->numberOfModules, 2, NULL);

		if (pDownloadInfoIndication->numberOfModules > 0)
		{
			for (int moduleIndex = 0; moduleIndex < pDownloadInfoIndication->numberOfModules; moduleIndex++)
			{
				ModuleInfo_t* pmoduleInfo = pDownloadInfoIndication->ModuleInfo + moduleIndex;

				sprintf_s(pszField, sizeof(pszField), "MODULE[%d]()", moduleIndex);
				sprintf_s(pszComment, "ID=0x%04X, size=%d, ver=%d", pmoduleInfo->moduleId, pmoduleInfo->moduleSize, pmoduleInfo->moduleVersion);
				XMLElement* pxmlModuleNode = pxmlDoc->NewBranchElement(pxmlSessionNode, pszField, pszComment, 8 + pmoduleInfo->moduleInfoLength);

				pxmlDoc->NewElementForByteMode(pxmlModuleNode, "moduleId", pmoduleInfo->moduleId, 2, NULL);

				pxmlDoc->NewElementForByteMode(pxmlModuleNode, "moduleSize", pmoduleInfo->moduleSize, 4, NULL);

				pxmlDoc->NewElementForByteMode(pxmlModuleNode, "moduleVersion", pmoduleInfo->moduleVersion, 1, NULL);

				pxmlDoc->NewElementForByteMode(pxmlModuleNode, "moduleInfoLength", pmoduleInfo->moduleInfoLength, 1, NULL);

				if (pmoduleInfo->moduleInfoLength > 0)
				{
					if (pmoduleInfo->data_broadcast_type == 0x0006)			//DC
					{
						sprintf_s(pszField, sizeof(pszField), "moduleInfoByte[DC]");
					}
					else if (pmoduleInfo->data_broadcast_type == 0x0007)			//OC
					{
						sprintf_s(pszField, sizeof(pszField), "moduleInfoByte[OC]");
					}
					else
					{
						sprintf_s(pszField, sizeof(pszField), "Unknown");
					}

					XMLElement* pxmlModuleInfoByte = pxmlDoc->NewBranchElement(pxmlModuleNode, pszField, NULL, pmoduleInfo->moduleInfoLength);

					if (pmoduleInfo->data_broadcast_type == 0x0006)			//DC
					{
						MPEG2_DSMCC_DII_DC_DecodeModuleInfo_to_xml(pmoduleInfo->moduleInfoByte, pmoduleInfo->moduleInfoLength, pxmlDoc, pxmlModuleInfoByte);
					}
					else if (pmoduleInfo->data_broadcast_type == 0x0007)			//OC
					{
						MPEG2_DSMCC_DII_OC_DecodeModuleInfo_to_xml(pmoduleInfo->moduleInfoByte, pmoduleInfo->moduleInfoLength, pxmlDoc, pxmlModuleInfoByte);
					}
					else
					{
						pxmlDoc->NewElementForByteBuf(pxmlModuleInfoByte, "moduleInfoByte[ ]", pmoduleInfo->moduleInfoByte, pmoduleInfo->moduleInfoLength, NULL);
					}
				}
			}
		}

		pxmlDoc->NewElementForByteMode(pxmlSessionNode, "privateDataLength", pDownloadInfoIndication->privateDataLength, 2, NULL);

		if (pDownloadInfoIndication->privateDataLength > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlSessionNode, "privateDataByte[ ]", pDownloadInfoIndication->privateDataByte, pDownloadInfoIndication->privateDataLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

