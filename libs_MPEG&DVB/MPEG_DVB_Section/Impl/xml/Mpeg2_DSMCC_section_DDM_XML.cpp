#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_DSMCC_section_DDM.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/xml/Mpeg2_DSMCC_section_XML.h"

////////////////////////////////////////////

int MPEG2_DSMCC_DDM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_ddm_section_t* pDSMCCSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	dsmcc_ddm_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_ddm_section_t;
	rtcode = MPEG2_DSMCC_DDM_DecodeSection(section_buf, section_size, pdsmcc_section);
	rtcode = MPEG2_DSMCC_DDM_PresentSection_to_XML(pxmlDoc, pdsmcc_section);

	if (pDSMCCSection == NULL)
	{
		//说明pdsmcc_section指针临时分配，函数返回前需要释放
		delete pdsmcc_section;
	}

	return rtcode;
}

int MPEG2_DSMCC_DDM_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, dsmcc_ddm_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	char					pszField[64];
	char					pszComment[64];

	dsmccDownloadDataHeader_t*			pdsmccDownloadDataHeader;
	DownloadDataBlock_t*				pDownloadDataBlock;

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

		assert(pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", "moduleId");

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL);

		int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;

		if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
		{
			XMLElement* pxmlDDMNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "downloadDataMessage()", NULL);
			XMLNODE_SetFieldLength(pxmlDDMNode, section_payload_length);

			pdsmccDownloadDataHeader = &(pdsmcc_section->dsmccDownloadDataHeader);
			int message_header_length = 12 + pdsmccDownloadDataHeader->adaptationLength;
			XMLElement* pxmlHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlDDMNode, "dsmccDownloadDataHeader()", NULL);
			XMLNODE_SetFieldLength(pxmlHeaderNode, message_header_length);

			//包含DownloadDataBlock
			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "protocolDiscriminator", pdsmccDownloadDataHeader->protocolDiscriminator, 1, NULL);

			MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccDownloadDataHeader->dsmccType, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "dsmccType", pdsmccDownloadDataHeader->dsmccType, 1, pszComment);

			MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccDownloadDataHeader->messageId, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageId", pdsmccDownloadDataHeader->messageId, 2, pszComment);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "downloadId", pdsmccDownloadDataHeader->downloadId, 4, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "reserved", pdsmccDownloadDataHeader->reserved, 1, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "adaptationLength", pdsmccDownloadDataHeader->adaptationLength, 1, NULL);

			XMLDOC_NewElementForByteMode(pxmlDoc, pxmlHeaderNode, "messageLength", pdsmccDownloadDataHeader->messageLength, 2, NULL);

			assert(pdsmccDownloadDataHeader->adaptationLength <= pdsmccDownloadDataHeader->messageLength);
			//messageLength解析错误，将是灾难性的

			if (pdsmccDownloadDataHeader->adaptationLength > 0)
			{
				XMLElement* pxmlAdaptationHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlHeaderNode, "dsmccAdaptationHeader()", NULL);
				XMLNODE_SetFieldLength(pxmlAdaptationHeaderNode, pdsmccDownloadDataHeader->adaptationLength);

				XMLDOC_NewElementForByteMode(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationType", pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationType, 1, NULL);

				if (pdsmccDownloadDataHeader->dsmccAdaptationHeader.N > 0)
				{
					XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationDataByte, pdsmccDownloadDataHeader->dsmccAdaptationHeader.N, NULL);
				}
			}

			int msg_payload_length = pdsmccDownloadDataHeader->messageLength - pdsmccDownloadDataHeader->adaptationLength;
			if (msg_payload_length > 0)
			{
				if (pdsmccDownloadDataHeader->messageId == 0x1003)
				{
					XMLNODE_SetAttribute(pxmlDDMNode, "comment", "DownloadDataBlock");

					pDownloadDataBlock = &(pdsmcc_section->DownloadDataBlock);
					rtcode = MPEG2_DSMCC_PresentDownloadDataBlock_to_xml(pxmlDoc, pxmlDDMNode, pDownloadDataBlock);
				}
				else
				{
					sprintf_s(pszComment, sizeof(pszComment), "unknown messageId: 0x%04X", pdsmccDownloadDataHeader->messageId);
					XMLNODE_SetAttribute(pxmlDDMNode, "comment", pszComment);

					XMLElement* pxmlPayloadNode = XMLDOC_NewElementForString(pxmlDoc, pxmlDDMNode, "messagePayload[ ]", NULL);
					XMLNODE_SetFieldLength(pxmlPayloadNode, msg_payload_length);
				}
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
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;				//
	}

	return rtcode;
}


int MPEG2_DSMCC_PresentDownloadDataBlock_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, DownloadDataBlock_t* pDownloadDataBlock)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pDownloadDataBlock != NULL))
	{
		XMLElement* pxmlSessionNode = pxmlParentNode;

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "moduleId", pDownloadDataBlock->moduleId, 2, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "moduleVersion", pDownloadDataBlock->moduleVersion, 1, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "reserved", pDownloadDataBlock->reserved, 1, NULL);

		XMLDOC_NewElementForByteMode(pxmlDoc, pxmlSessionNode, "blockNumber", pDownloadDataBlock->blockNumber, 2, NULL);

		if (pDownloadDataBlock->N > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlSessionNode, "blockDataByte[ ]", pDownloadDataBlock->blockDataByte, pDownloadDataBlock->N, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
