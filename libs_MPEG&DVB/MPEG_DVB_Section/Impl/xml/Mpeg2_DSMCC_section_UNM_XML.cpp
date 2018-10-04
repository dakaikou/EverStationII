#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_DSMCC_section_UNM.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/xml/Mpeg2_DSMCC_section_XML.h"
#include "../../Include/xml/Mpeg2_DSMCC_descriptor_XML.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
int MPEG2_DSMCC_UNM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_unm_section_t* pDSMCCSection)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	dsmcc_unm_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_unm_section_t;
	rtcode = MPEG2_DSMCC_UNM_DecodeSection(section_buf, section_size, pdsmcc_section);
	rtcode = MPEG2_DSMCC_UNM_PresentSection_to_XML(pxmlDoc, pdsmcc_section);

	if (pDSMCCSection == NULL)
	{
		//说明pdsmcc_section指针临时分配，函数返回前需要释放
		delete pdsmcc_section;
	}

	return rtcode;
}

int MPEG2_DSMCC_UNM_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_unm_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	char					pszComment[128];
	char					pszField[64];

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	DownloadServerInitiate_t*			pDownloadServerInitiate;
	DownloadInfoIndication_t*			pDownloadInfoIndication;

	if ((pxmlDoc != NULL) && (pdsmcc_section != NULL))
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* xmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, xmlDeclaration);

		//根节点
		sprintf_s(pszField, sizeof(pszField), "DSMCC_section(table_id=0x%02X)", pdsmcc_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, pdsmcc_section->dsmcc_section_length + 3);

		if ((pdsmcc_section->table_id >= 0x38) && (pdsmcc_section->table_id <= 0x3F))
		{
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", "transaction_id<15:0>");

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL);

			int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;

			if (pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM)
			{
				XMLElement* pxmlUNMNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "userNetworkMessage()", NULL);
				XMLNODE_SetFieldLength(pxmlUNMNode, section_payload_length);

				//包含DownloadServerInitiate、DownloadInfoIndication、DownloadCancel

				pdsmccMessageHeader = &(pdsmcc_section->dsmccMessageHeader);
				int message_header_length = 12 + pdsmccMessageHeader->adaptationLength;
				XMLElement* pxmlHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlUNMNode, "dsmccMessageHeader()", NULL);
				XMLNODE_SetFieldLength(pxmlHeaderNode, message_header_length);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "protocolDiscriminator", pdsmccMessageHeader->protocolDiscriminator, 1, NULL);

				MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccMessageHeader->dsmccType, pszComment, sizeof(pszComment));
				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "dsmccType", pdsmccMessageHeader->dsmccType, 1, pszComment);

				MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccMessageHeader->messageId, pszComment, sizeof(pszComment));
				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageId", pdsmccMessageHeader->messageId, 2, pszComment);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "transactionId", pdsmccMessageHeader->transactionId, 4, NULL);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "reserved", pdsmccMessageHeader->reserved, 1, NULL);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "adaptationLength", pdsmccMessageHeader->adaptationLength, 1, NULL);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageLength", pdsmccMessageHeader->messageLength, 2, NULL);

				assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
				//messageLength解析错误，将是灾难性的

				if (pdsmccMessageHeader->adaptationLength > 0)
				{
					XMLElement* pxmlAdaptationHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlHeaderNode, "dsmccAdaptationHeader()", NULL);
					XMLNODE_SetFieldLength(pxmlAdaptationHeaderNode, pdsmccMessageHeader->adaptationLength);

					XMLDOC_NewElementForByteMode(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationType", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType, 1, NULL);

					if (pdsmccMessageHeader->dsmccAdaptationHeader.N > 0)
					{
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, pdsmccMessageHeader->dsmccAdaptationHeader.N, NULL);
					}
				}

				int msg_payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
				if (msg_payload_length > 0)
				{
					if (pdsmccMessageHeader->messageId == 0x1002)			//DII
					{
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadInfoIndication");

						pDownloadInfoIndication = &(pdsmcc_section->u.DownloadInfoIndication);
						rtcode = MPEG2_DSMCC_PresentDownloadInfoIndication_to_xml(pxmlDoc, pxmlUNMNode, pDownloadInfoIndication);
					}
					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
					{
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadServerInitiate");

						pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);
						rtcode = MPEG2_DSMCC_PresentDownloadServerInitiate_to_xml(pxmlDoc, pxmlUNMNode, pDownloadServerInitiate);
					}
					else
					{
						sprintf_s(pszComment, sizeof(pszComment), "unknown messageId: 0x%04X", pdsmccMessageHeader->messageId);
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", pszComment);

						XMLElement* pxmlPayloadNode = XMLDOC_NewElementForString(pxmlDoc, pxmlUNMNode, "messagePayload[ ]", NULL);
						XMLNODE_SetFieldLength(pxmlPayloadNode, msg_payload_length);
					}
				}
			}
			else
			{
				assert(0);
			}

			if (pdsmcc_section->section_syntax_indicator == 1)
			{
				XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", pdsmcc_section->encode.CRC_32, 32, "rpchof", NULL);

				if (pdsmcc_section->recalculated.CRC_32 != pdsmcc_section->encode.CRC_32)
				{
					sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculated.CRC_32);
					pxmlCrcNode->SetAttribute("error", pszComment);
				}
			}
			else
			{
				XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "checksum", pdsmcc_section->encode.checksum, 32, "uimsbf", NULL);

				if (pdsmcc_section->recalculated.checksum != pdsmcc_section->encode.checksum)
				{
					sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculated.checksum);
					pxmlCrcNode->SetAttribute("error", pszComment);
				}
			}
		}
		else
		{
			sprintf_s(pszComment, sizeof(pszComment), "section syntax error! incorrect table_id = 0x%02X", pdsmcc_section->table_id);
			pxmlRootNode->SetAttribute("error", pszComment);
			rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}

int	MPEG2_DSMCC_PresentGroupInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, GroupInfoIndication_t* pGroupInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pGroupInfoIndication != NULL))
	{
		XMLElement* pxmlGroupsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "GroupInfoIndication()", NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupsNode, "NumberOfGroups", pGroupInfoIndication->NumberOfGroups, 2, NULL);

		for (int group_index = 0; group_index < pGroupInfoIndication->NumberOfGroups; group_index++)
		{
			GroupInfo_t* pGroupInfo = pGroupInfoIndication->GroupInfo + group_index;
			sprintf_s(pszField, sizeof(pszField), "GROUP[%d](ID:0x%08X)", group_index, pGroupInfo->GroupId);

			XMLElement* pxmlGroupNode = XMLDOC_NewElementForString(pxmlDoc, pxmlGroupsNode, pszField, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupNode, "GroupId", pGroupInfo->GroupId, 4, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupNode, "GroupSize", pGroupInfo->GroupSize, 4, NULL);

			compatibilityDescriptor_t* pcompatibilityDescriptor = &(pGroupInfo->GroupCompatibility);
			XMLElement* pxmlGroupCompatibilityNode = XMLDOC_NewElementForString(pxmlDoc, pxmlGroupNode, "GroupCompatibility()", NULL);
			XMLNODE_SetFieldLength(pxmlGroupCompatibilityNode, 2 + pcompatibilityDescriptor->compatibilityDescriptorLength);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupCompatibilityNode, "compatibilityDescriptorLength", pcompatibilityDescriptor->compatibilityDescriptorLength, 2, NULL);
			if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
			{
				XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlGroupCompatibilityNode, "compatibilityDescriptorBuf[ ]", pcompatibilityDescriptor->compatibilityDescriptorBuf, pcompatibilityDescriptor->compatibilityDescriptorLength, NULL);
			}

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupNode, "GroupInfoLength", pGroupInfo->GroupInfoLength, 2, NULL);

			if (pGroupInfo->GroupInfoLength > 0)
			{
				XMLElement* pxmlDescriptionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlGroupNode, "GroupInfo()", NULL);

				for (int descriptor_index = 0; descriptor_index < pGroupInfo->group_descriptor_count; descriptor_index++)
				{
					uint8_t* descriptor_buf = pGroupInfo->group_descriptors[descriptor_index].descriptor_buf;
					uint8_t descriptor_tag = pGroupInfo->group_descriptors[descriptor_index].descriptor_tag;
					uint8_t descriptor_length = pGroupInfo->group_descriptors[descriptor_index].descriptor_length;
					int descriptor_size = descriptor_length + 2;

					switch (descriptor_tag)
					{
						//case DVB_SI_NETWORK_NAME_DESCRIPTOR:
						//	DVB_SI_decode_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
						//	break;
						//case DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR:
						//	DVB_SI_decode_multilingual_network_name_descriptor_to_xml(pl1temp, descriptor_size, pxmlDoc, pxmlProgramInfoNode);
						//	break;
					default:
						MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlDescriptionNode, pGroupInfo->group_descriptors + descriptor_index);
						break;
					}
				}

			}
		}

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlGroupsNode, "PrivateDataLength", pGroupInfoIndication->PrivateDataLength, 2, NULL);
		if (pGroupInfoIndication->PrivateDataLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlGroupsNode, "privateDataByte[ ]", pGroupInfoIndication->privateDataByte, pGroupInfoIndication->PrivateDataLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


int	MPEG2_DSMCC_PresentServiceGatewayInfo_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ServiceGatewayInfo_t* pServiceGatewayInfo)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszTemp[96];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pServiceGatewayInfo != NULL))
	{
		XMLElement* pxmlSGINode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "ServiceGatewayInfo()", NULL);

		//解析IOP::IOR()
		MPEG2_DSMCC_PresentIOR_to_xml(pxmlDoc, pxmlSGINode, &(pServiceGatewayInfo->IOR));

		XMLDOC_NewElementForBits(pxmlDoc, pxmlSGINode, "downloadTaps_count", pServiceGatewayInfo->downloadTaps_count, 8, "uimsbf", "N1");
		if (pServiceGatewayInfo->downloadTaps_count > 0)
		{
			assert(0);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlSGINode, "serviceContextList_count", pServiceGatewayInfo->serviceContextList_count, 8, "uimsbf", "N2");
		if (pServiceGatewayInfo->serviceContextList_count > 0)
		{
			//pxmlDoc->NewKeyValuePairElement(pxmlSGINode, "serviceContextList_data_byte()", ptemp, pServiceGatewayInfo->serviceContextList_count, NULL, &bs);
			assert(0);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlSGINode, "userInfoLength", pServiceGatewayInfo->userInfoLength, 16, "uimsbf", "N3");
		if (pServiceGatewayInfo->userInfoLength > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSGINode, "userInfo_data_byte[ ]", pServiceGatewayInfo->userInfo_data_byte, pServiceGatewayInfo->userInfoLength, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_PresentDownloadServerInitiate_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	int			remain_length;
	int			copy_length;
	char		pszTemp[96];
	char		pszStyle[64];
	BYTES_t		bytes;

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
				rtcode = MPEG2_DSMCC_PresentGroupInfoIndication_to_xml(pxmlDoc, pxmlSessionNode, pGroupInfoIndication);
			}
			else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)		//OC
			{
				pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);
				rtcode = MPEG2_DSMCC_PresentServiceGatewayInfo_to_xml(pxmlDoc, pxmlSessionNode, pServiceGatewayInfo);
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

int	MPEG2_DSMCC_PresentDownloadInfoIndication_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadInfoIndication_t* pDownloadInfoIndication)
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
