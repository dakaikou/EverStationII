#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../Include/Mpeg2_DSMCC_section_XML.h"
#include "../Include/dsmcc/Mpeg2_DSMCC_UNM_XML.h"
#include "../Include/dsmcc/Mpeg2_DSMCC_DDM_XML.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//int MPEG2_DSMCC_UNM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_section_t* pDSMCCSection)
//{
//	int						rtcode = SECTION_PARSE_NO_ERROR;
//
//	dsmcc_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_section_t;
//	rtcode = MPEG2_DSMCC_DecodeSection(section_buf, section_size, pdsmcc_section);
//	rtcode = MPEG2_DSMCC_UNM_PresentSection_to_XML(pxmlDoc, pdsmcc_section);
//
//	if (pDSMCCSection == NULL)
//	{
//		//说明pdsmcc_section指针临时分配，函数返回前需要释放
//		delete pdsmcc_section;
//	}
//
//	return rtcode;
//}

//int MPEG2_DSMCC_UNM_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_section_t* pdsmcc_section)
//{
//	int						rtcode = SECTION_PARSE_NO_ERROR;
//	char					pszComment[128];
//	char					pszField[64];
//
//	dsmccMessageHeader_t*				pdsmccMessageHeader;
//
//	DownloadServerInitiate_t*			pDownloadServerInitiate;
//	DownloadInfoIndication_t*			pDownloadInfoIndication;
//
//	if ((pxmlDoc != NULL) && (pdsmcc_section != NULL))
//	{
//		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";
//
//		XMLDeclaration* xmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
//		XMLDOC_InsertFirstChild(pxmlDoc, xmlDeclaration);
//
//		//根节点
//		sprintf_s(pszField, sizeof(pszField), "DSMCC_section(table_id=0x%02X)", pdsmcc_section->table_id);
//		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
//		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
//		XMLNODE_SetFieldLength(pxmlRootNode, pdsmcc_section->dsmcc_section_length + 3);
//
//		if ((pdsmcc_section->table_id >= 0x38) && (pdsmcc_section->table_id <= 0x3F))
//		{
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL);
//
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL);
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL);
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL);
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL);
//
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", "transaction_id<15:0>");
//
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL);
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL);
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL);
//
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL);
//
//			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL);
//
//			int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
//
//			if (pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM)
//			{
//				XMLElement* pxmlUNMNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "userNetworkMessage()", NULL);
//				XMLNODE_SetFieldLength(pxmlUNMNode, section_payload_length);
//
//				//包含DownloadServerInitiate、DownloadInfoIndication、DownloadCancel
//
//				pdsmccMessageHeader = &(pdsmcc_section->dsmccMessageHeader);
//				int message_header_length = 12 + pdsmccMessageHeader->adaptationLength;
//				XMLElement* pxmlHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlUNMNode, "dsmccMessageHeader()", NULL);
//				XMLNODE_SetFieldLength(pxmlHeaderNode, message_header_length);
//
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "protocolDiscriminator", pdsmccMessageHeader->protocolDiscriminator, 1, NULL);
//
//				MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccMessageHeader->dsmccType, pszComment, sizeof(pszComment));
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "dsmccType", pdsmccMessageHeader->dsmccType, 1, pszComment);
//
//				MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccMessageHeader->messageId, pszComment, sizeof(pszComment));
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageId", pdsmccMessageHeader->messageId, 2, pszComment);
//
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "transactionId", pdsmccMessageHeader->TxOrDnloadID, 4, NULL);
//
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "reserved", pdsmccMessageHeader->reserved, 1, NULL);
//
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "adaptationLength", pdsmccMessageHeader->adaptationLength, 1, NULL);
//
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageLength", pdsmccMessageHeader->messageLength, 2, NULL);
//
//				assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
//				//messageLength解析错误，将是灾难性的
//
//				if (pdsmccMessageHeader->adaptationLength > 0)
//				{
//					XMLElement* pxmlAdaptationHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlHeaderNode, "dsmccAdaptationHeader()", NULL);
//					XMLNODE_SetFieldLength(pxmlAdaptationHeaderNode, pdsmccMessageHeader->adaptationLength);
//
//					XMLDOC_NewElementForByteMode(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationType", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType, 1, NULL);
//
//					if (pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataLength > 0)
//					{
//						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataLength, NULL);
//					}
//				}
//
//				int msg_payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
//				if (msg_payload_length > 0)
//				{
//					if (pdsmccMessageHeader->messageId == 0x1002)			//DII
//					{
//						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadInfoIndication");
//
//						pDownloadInfoIndication = &(pdsmcc_section->u.DownloadInfoIndication);
//						rtcode = MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(pxmlDoc, pxmlUNMNode, pDownloadInfoIndication);
//					}
//					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
//					{
//						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadServerInitiate");
//
//						pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);
//						rtcode = MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(pxmlDoc, pxmlUNMNode, pDownloadServerInitiate);
//					}
//					else
//					{
//						sprintf_s(pszComment, sizeof(pszComment), "unknown messageId: 0x%04X", pdsmccMessageHeader->messageId);
//						XMLNODE_SetAttribute(pxmlUNMNode, "comment", pszComment);
//
//						XMLElement* pxmlPayloadNode = XMLDOC_NewElementForString(pxmlDoc, pxmlUNMNode, "messagePayload[ ]", NULL);
//						XMLNODE_SetFieldLength(pxmlPayloadNode, msg_payload_length);
//					}
//				}
//			}
//			else
//			{
//				assert(0);
//			}
//
//			if (pdsmcc_section->section_syntax_indicator == 1)
//			{
//				XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", pdsmcc_section->encode.CRC_32, 32, "rpchof", NULL);
//
//				if (pdsmcc_section->recalculated.CRC_32 != pdsmcc_section->encode.CRC_32)
//				{
//					sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculated.CRC_32);
//					pxmlCrcNode->SetAttribute("error", pszComment);
//				}
//			}
//			else
//			{
//				XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "checksum", pdsmcc_section->encode.checksum, 32, "uimsbf", NULL);
//
//				if (pdsmcc_section->recalculated.checksum != pdsmcc_section->encode.checksum)
//				{
//					sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculated.checksum);
//					pxmlCrcNode->SetAttribute("error", pszComment);
//				}
//			}
//		}
//		else
//		{
//			sprintf_s(pszComment, sizeof(pszComment), "section syntax error! incorrect table_id = 0x%02X", pdsmcc_section->table_id);
//			pxmlRootNode->SetAttribute("error", pszComment);
//			rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
//		}
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//输入参数错误
//	}
//
//	return rtcode;
//}


//int MPEG2_DSMCC_DDM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_section_t* pDSMCCSection)
//{
//	int		rtcode = SECTION_PARSE_NO_ERROR;
//
//	dsmcc_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_section_t;
//	rtcode = MPEG2_DSMCC_DecodeSection(section_buf, section_size, pdsmcc_section);
//	rtcode = MPEG2_DSMCC_DDM_PresentSection_to_XML(pxmlDoc, pdsmcc_section);
//
//	if (pDSMCCSection == NULL)
//	{
//		//说明pdsmcc_section指针临时分配，函数返回前需要释放
//		delete pdsmcc_section;
//	}
//
//	return rtcode;
//}

//int MPEG2_DSMCC_DDM_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_section_t* pdsmcc_section)
//{
//	int						rtcode = SECTION_PARSE_NO_ERROR;
//	char					pszField[64];
//	char					pszComment[64];
//
//	dsmccMessageHeader_t*			pdsmccDownloadDataHeader;
//	DownloadDataBlock_t*			pDownloadDataBlock;
//
//	if ((pxmlDoc != NULL) && (pdsmcc_section != NULL))
//	{
//		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";
//
//		XMLDeclaration* xmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
//		XMLDOC_InsertFirstChild(pxmlDoc, xmlDeclaration);
//
//		//根节点
//		sprintf_s(pszField, sizeof(pszField), "DSMCC_section(table_id=0x%02X)", pdsmcc_section->table_id);
//		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
//		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
//		XMLNODE_SetFieldLength(pxmlRootNode, pdsmcc_section->dsmcc_section_length + 3);
//
//		assert(pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL);
//
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL);
//
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", "moduleId");
//
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL);
//
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL);
//
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL);
//
//		int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
//
//		if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
//		{
//			XMLElement* pxmlDDMNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "downloadDataMessage()", NULL);
//			XMLNODE_SetFieldLength(pxmlDDMNode, section_payload_length);
//
//			pdsmccDownloadDataHeader = &(pdsmcc_section->dsmccMessageHeader);
//			int message_header_length = 12 + pdsmccDownloadDataHeader->adaptationLength;
//			XMLElement* pxmlHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlDDMNode, "dsmccDownloadDataHeader()", NULL);
//			XMLNODE_SetFieldLength(pxmlHeaderNode, message_header_length);
//
//			//包含DownloadDataBlock
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "protocolDiscriminator", pdsmccDownloadDataHeader->protocolDiscriminator, 1, NULL);
//
//			MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccDownloadDataHeader->dsmccType, pszComment, sizeof(pszComment));
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "dsmccType", pdsmccDownloadDataHeader->dsmccType, 1, pszComment);
//
//			MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccDownloadDataHeader->messageId, pszComment, sizeof(pszComment));
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageId", pdsmccDownloadDataHeader->messageId, 2, pszComment);
//
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "downloadId", pdsmccDownloadDataHeader->TxOrDnloadID, 4, NULL);
//
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "reserved", pdsmccDownloadDataHeader->reserved, 1, NULL);
//
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "adaptationLength", pdsmccDownloadDataHeader->adaptationLength, 1, NULL);
//
//			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageLength", pdsmccDownloadDataHeader->messageLength, 2, NULL);
//
//			assert(pdsmccDownloadDataHeader->adaptationLength <= pdsmccDownloadDataHeader->messageLength);
//			//messageLength解析错误，将是灾难性的
//
//			if (pdsmccDownloadDataHeader->adaptationLength > 0)
//			{
//				XMLElement* pxmlAdaptationHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlHeaderNode, "dsmccAdaptationHeader()", NULL);
//				XMLNODE_SetFieldLength(pxmlAdaptationHeaderNode, pdsmccDownloadDataHeader->adaptationLength);
//
//				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationType", pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationType, 1, NULL);
//
//				if (pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationDataLength > 0)
//				{
//					XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationDataByte, pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationDataLength, NULL);
//				}
//			}
//
//			int msg_payload_length = pdsmccDownloadDataHeader->messageLength - pdsmccDownloadDataHeader->adaptationLength;
//			if (msg_payload_length > 0)
//			{
//				if (pdsmccDownloadDataHeader->messageId == 0x1003)
//				{
//					XMLNODE_SetAttribute(pxmlDDMNode, "comment", "DownloadDataBlock");
//
//					pDownloadDataBlock = &(pdsmcc_section->u.DownloadDataBlock);
//					rtcode = MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(pxmlDoc, pxmlDDMNode, pDownloadDataBlock);
//				}
//				else
//				{
//					sprintf_s(pszComment, sizeof(pszComment), "unknown messageId: 0x%04X", pdsmccDownloadDataHeader->messageId);
//					XMLNODE_SetAttribute(pxmlDDMNode, "comment", pszComment);
//
//					XMLElement* pxmlPayloadNode = XMLDOC_NewElementForString(pxmlDoc, pxmlDDMNode, "messagePayload[ ]", NULL);
//					XMLNODE_SetFieldLength(pxmlPayloadNode, msg_payload_length);
//				}
//			}
//
//			if (pdsmcc_section->section_syntax_indicator == 1)
//			{
//				XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", pdsmcc_section->encode.CRC_32, 32, "rpchof", NULL);
//
//				if (pdsmcc_section->recalculated.CRC_32 != pdsmcc_section->encode.CRC_32)
//				{
//					sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculated.CRC_32);
//					pxmlCrcNode->SetAttribute("error", pszComment);
//				}
//			}
//			else
//			{
//				XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "checksum", pdsmcc_section->encode.checksum, 32, "uimsbf", NULL);
//
//				if (pdsmcc_section->recalculated.checksum != pdsmcc_section->encode.checksum)
//				{
//					sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculated.checksum);
//					pxmlCrcNode->SetAttribute("error", pszComment);
//				}
//			}
//		}
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;				//
//	}
//
//	return rtcode;
//}


int MPEG2_DSMCC_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_section_t* pDSMCCSection)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	dsmcc_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_section_t;
	rtcode = MPEG2_DSMCC_DecodeSection(section_buf, section_size, pdsmcc_section);
	rtcode = MPEG2_DSMCC_PresentSection_to_XML(pxmlDoc, pdsmcc_section);

	if (pDSMCCSection == NULL)
	{
		//说明pdsmcc_section指针临时分配，函数返回前需要释放
		delete pdsmcc_section;
	}

	return rtcode;
}

int MPEG2_DSMCC_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	char					pszComment[128];
	char					pszField[64];

	dsmccMessageHeader_t*				pdsmccMessageHeader;

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

			if ((pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM) ||
				(pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM))
			{
				if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
				{
					sprintf_s(pszField, sizeof(pszField), "downloadDataMessage()");
				}
				else
				{
					sprintf_s(pszField, sizeof(pszField), "userNetworkMessage()");
				}
				XMLElement* pxmlUNMNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlUNMNode, section_payload_length);

				//包含DownloadServerInitiate、DownloadInfoIndication、DownloadCancel

				pdsmccMessageHeader = &(pdsmcc_section->dsmccMessageHeader);
				int message_header_length = 12 + pdsmccMessageHeader->adaptationLength;

				if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
				{
					sprintf_s(pszField, sizeof(pszField), "dsmccDownloadDataHeader()");
				}
				else
				{
					sprintf_s(pszField, sizeof(pszField), "dsmccMessageHeader()");
				}
				XMLElement* pxmlHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlUNMNode, pszField, NULL);
				XMLNODE_SetFieldLength(pxmlHeaderNode, message_header_length);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "protocolDiscriminator", pdsmccMessageHeader->protocolDiscriminator, 1, NULL);

				MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccMessageHeader->dsmccType, pszComment, sizeof(pszComment));
				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "dsmccType", pdsmccMessageHeader->dsmccType, 1, pszComment);

				MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccMessageHeader->messageId, pszComment, sizeof(pszComment));
				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageId", pdsmccMessageHeader->messageId, 2, pszComment);

				if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
				{
					sprintf_s(pszField, sizeof(pszField), "downloadId()");
				}
				else
				{
					sprintf_s(pszField, sizeof(pszField), "transactionId()");
				}
				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, pszField, pdsmccMessageHeader->TxOrDnloadID, 4, NULL);

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

					if (pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataLength > 0)
					{
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataLength, NULL);
					}
				}

				int msg_payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
				if (msg_payload_length > 0)
				{
					if (pdsmccMessageHeader->messageId == 0x1002)			//DII
					{
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadInfoIndication");

						DownloadInfoIndication_t* pDownloadInfoIndication = &(pdsmcc_section->u.DownloadInfoIndication);
						rtcode = MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(pxmlDoc, pxmlUNMNode, pDownloadInfoIndication);
					}
					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
					{
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadServerInitiate");

						DownloadServerInitiate_t* pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);
						rtcode = MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(pxmlDoc, pxmlUNMNode, pDownloadServerInitiate);
					}
					else if (pdsmccMessageHeader->messageId == 0x1003)
					{
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadDataBlock");
					
						DownloadDataBlock_t* pDownloadDataBlock = &(pdsmcc_section->u.DownloadDataBlock);
						rtcode = MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(pxmlDoc, pxmlUNMNode, pDownloadDataBlock);
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
