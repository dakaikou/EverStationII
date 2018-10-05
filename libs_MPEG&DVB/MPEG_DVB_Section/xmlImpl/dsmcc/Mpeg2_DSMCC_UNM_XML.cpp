#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../nativeInclude/Mpeg2_DSMCC_Utilities.h"
#include "../../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "../../xmlInclude/dsmcc/MPEG2_DSMCC_UNM_XML.h"
#include "../../xmlInclude/dsmcc/MPEG2_DSMCC_UNM_DSI_XML.h"

/////////////////////////////////////////////

int	MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	//char		pszTemp[96];
	//char		pszStyle[64];

	GroupInfoIndication_t*			pGroupInfoIndication;
	ServiceGatewayInfo_t*			pServiceGatewayInfo;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDownloadServerInitiate != NULL))
	{
		//XMLElement* pxmlSessionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "DownloadServerInitiate()", NULL);
		XMLElement* pxmlSessionNode = pxmlParentNode;

		XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSessionNode, "serverId", pDownloadServerInitiate->serverId, 20, NULL);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadServerInitiate->compatibilityDescriptor);
		XMLElement* pxmlDescriptorNode = XMLDOC_NewElementForString(pxmlDoc, pxmlSessionNode, "compatibilityDescriptor()", NULL);
		XMLNODE_SetFieldLength(pxmlDescriptorNode, 2 + pcompatibilityDescriptor->compatibilityDescriptorLength);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlDescriptorNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);

		if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlDescriptorNode, "compatibilityDescriptorBuf[ ]", 
				pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
		}

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "privateDataLength", pDownloadServerInitiate->privateDataLength, 2, NULL);

		//defined in EN 301 192
		//若privateDataLength解析错误，将导致灾难性错误
		if (pDownloadServerInitiate->privateDataLength > 0)
		{
			if (pDownloadServerInitiate->data_broadcast_type == 0x0006)		//DC
			{
				pGroupInfoIndication = &(pDownloadServerInitiate->u.GroupInfoIndication);
				rtcode = MPEG2_DSMCC_DSI_DC_PresentGroupInfoIndication_to_xml(pxmlDoc, pxmlSessionNode, pGroupInfoIndication);
			}
			else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)		//OC
			{
				pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);
				rtcode = MPEG2_DSMCC_DSI_OC_PresentServiceGatewayInfo_to_xml(pxmlDoc, pxmlSessionNode, pServiceGatewayInfo);
			}
			else
			{
				//如何判断privateDataByte载荷的类型？
				XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSessionNode, "privateDataByte[ ]", pDownloadServerInitiate->u.privateDataByte, pDownloadServerInitiate->privateDataLength, NULL);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	char		pszField[96];
	char		pszComment[128];

	DC_moduleInfo_t*	pDC_moduleInfo;
	BIOP_moduleInfo_t*	pBIOP_moduleInfo;

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
				moduleInfo_t* pmoduleInfo = pDownloadInfoIndication->moduleInfo + moduleIndex;

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
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlModuleNode, "moduleInfoByte[DC]", NULL, pmoduleInfo->moduleInfoLength, NULL);
						//			pDC_moduleInfo = &(pDownloadInfoIndication->moduleInfo[n].u.DC_moduleInfo);

					//			reserved_count = 0;
					//			uint8_t* ptemp = moduleInfoByte;
					//			int remain_length = moduleInfoLength;
					//			int descriptor_count = 0;
					//			while ((remain_length >= 2) && (descriptor_count < MAX_RESERVED_DESCRIPTORS))
					//			{
					//				descriptor_tag = (ptemp[0] | 0x3000);
					//				descriptor_length = ptemp[1];
					//				move_length = descriptor_length + 2;

					//				//截成一段一段的描述符
					//				pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_tag = descriptor_tag;
					//				pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_length = descriptor_length;
					//				pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_buf = ptemp;
					//				pDC_moduleInfo->reserved_descriptor[descriptor_count].descriptor_size = (uint8_t)move_length;

					//				switch (descriptor_tag)
					//				{
					//				case MPEG2_DSMCC_NAME_DESCRIPTOR:
					//					rtcode = MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->name_descriptor));
					//					break;
					//				case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
					//					rtcode = MPEG2_DSMCC_decode_location_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->location_descriptor));
					//					break;
					//				default:

					//					//if (descriptor_tag == 0x3081)
					//					//{
					//					//	if (pDC_moduleInfo->name_descriptor.descriptor_tag == 0x00)
					//					//	{
					//					//		rtcode = MPEG2_DSMCC_decode_name_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode, &(pDC_moduleInfo->name_descriptor));
					//					//	}
					//					//}
					//					//else
					//				{
					//					decode_reserved_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlModuleNode);
					//				}

					//				break;
					//				}

					//				descriptor_count++;

					//				ptemp += move_length;
					//				remain_length -= move_length;
					//			}

					//			pDC_moduleInfo->reserved_count = reserved_count;
					}
					else if (pmoduleInfo->data_broadcast_type == 0x0007)
					{
						pBIOP_moduleInfo = &(pmoduleInfo->u.BIOP_moduleInfo);

						XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "moduleTimeOut", pBIOP_moduleInfo->moduleTimeOut, 4, NULL);

						XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "blockTimeOut", pBIOP_moduleInfo->blockTimeOut, 4, NULL);

						XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "minBlockTime", pBIOP_moduleInfo->minBlockTime, 4, NULL);

						XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "taps_count", pBIOP_moduleInfo->taps_count, 1, NULL);

						for (int tap_index = 0; tap_index < pBIOP_moduleInfo->taps_count; tap_index++)
						{
							sprintf_s(pszField, sizeof(pszField), "TAP[%d]()", tap_index);
							XMLElement* pxmlTapNode = XMLDOC_NewElementForString(pxmlDoc, pxmlModuleNode, pszField, NULL);
							XMLNODE_SetFieldLength(pxmlTapNode, 7 + pBIOP_moduleInfo->TAP[tap_index].selector_length);

							XMLDOC_NewElementForByteMode(pxmlDoc, pxmlTapNode, "ID", pBIOP_moduleInfo->TAP[tap_index].id, 2, NULL);

							MPEG2_DSMCC_NumericCoding2Text_tapUse(pBIOP_moduleInfo->TAP[tap_index].use, pszComment, sizeof(pszComment));
							XMLDOC_NewElementForByteMode(pxmlDoc, pxmlTapNode, "use", pBIOP_moduleInfo->TAP[tap_index].use, 2, pszComment);

							XMLDOC_NewElementForByteMode(pxmlDoc, pxmlTapNode, "association_tag", pBIOP_moduleInfo->TAP[tap_index].association_tag, 2, NULL);

							XMLDOC_NewElementForByteMode(pxmlDoc, pxmlTapNode, "selector_length", pBIOP_moduleInfo->TAP[tap_index].selector_length, 1, NULL);
						}

						XMLDOC_NewElementForByteMode(pxmlDoc, pxmlModuleNode, "userInfoLength", pBIOP_moduleInfo->userInfoLength, 1, NULL);

						if (pBIOP_moduleInfo->userInfoLength > 0)
						{
							XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlModuleNode, "module_info_byte[ ]", pBIOP_moduleInfo->userInfo_data_byte, pBIOP_moduleInfo->userInfoLength, NULL);
						}
					}
					else
					{
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlModuleNode, "moduleInfoByte[ ]", NULL, pmoduleInfo->moduleInfoLength, NULL);
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
