#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_XML.h"

#include "../../../Include/MPEG_DVB_ErrorCode.h"
#include "../../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII.h"

/////////////////////////////////////////////
int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication_to_xml(uint8_t* buf, int size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDII)
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

int	MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	char		pszField[96];
	char		pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDownloadInfoIndication != NULL))
	{
		//XMLElement* pxmlSessionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "DownloadInfoIndication()", NULL);
		XMLElement* pxmlSessionNode = pxmlParentNode;

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "downloadId", pDownloadInfoIndication->downloadId, 4, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "blockSize", pDownloadInfoIndication->blockSize, 2, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "windowSize", pDownloadInfoIndication->windowSize, 1, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "ackPeriod", pDownloadInfoIndication->ackPeriod, 1, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "tCDownloadWindow", pDownloadInfoIndication->tCDownloadWindow, 4, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "tCDownloadScenario", pDownloadInfoIndication->tCDownloadScenario, 4, NULL);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadInfoIndication->compatibilityDescriptor);
		XMLElement* pxmlDescriptorNode = XMLDOC_NewElementForString(pxmlDoc, pxmlSessionNode, "compatibilityDescriptor()", NULL);
		XMLNODE_SetFieldLength(pxmlDescriptorNode, 2 + pcompatibilityDescriptor->compatibilityDescriptorLength);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlDescriptorNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);

		if (pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlDescriptorNode, "compatibilityDescriptorBuf[ ]", pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
		}

		//defined in EN 301192
		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "numberOfModules", pDownloadInfoIndication->numberOfModules, 2, NULL);

		if (pDownloadInfoIndication->numberOfModules > 0)
		{
			for (int moduleIndex = 0; moduleIndex < pDownloadInfoIndication->numberOfModules; moduleIndex++)
			{
				ModuleInfo_t* pmoduleInfo = pDownloadInfoIndication->ModuleInfo + moduleIndex;

				sprintf_s(pszField, sizeof(pszField), "MODULE[%d]()", moduleIndex);
				sprintf_s(pszComment, "ID=0x%04X, size=%d, ver=%d", pmoduleInfo->moduleId, pmoduleInfo->moduleSize, pmoduleInfo->moduleVersion);
				XMLElement* pxmlModuleNode = XMLDOC_NewElementForString(pxmlDoc, pxmlSessionNode, pszField, pszComment);
				XMLNODE_SetFieldLength(pxmlModuleNode, 8 + pmoduleInfo->moduleInfoLength);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "moduleId", pmoduleInfo->moduleId, 2, NULL);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "moduleSize", pmoduleInfo->moduleSize, 4, NULL);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "moduleVersion", pmoduleInfo->moduleVersion, 1, NULL);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "moduleInfoLength", pmoduleInfo->moduleInfoLength, 1, NULL);

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

					XMLElement* pxmlModuleInfoByte = XMLDOC_NewElementForString(pxmlDoc, pxmlModuleNode, pszField, NULL);
					XMLNODE_SetFieldLength(pxmlModuleInfoByte, pmoduleInfo->moduleInfoLength);

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
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlModuleInfoByte, "moduleInfoByte[ ]", pmoduleInfo->moduleInfoByte, pmoduleInfo->moduleInfoLength, NULL);
					}
				}
			}
		}

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "privateDataLength", pDownloadInfoIndication->privateDataLength, 2, NULL);

		if (pDownloadInfoIndication->privateDataLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSessionNode, "privateDataByte[ ]", pDownloadInfoIndication->privateDataByte, pDownloadInfoIndication->privateDataLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

