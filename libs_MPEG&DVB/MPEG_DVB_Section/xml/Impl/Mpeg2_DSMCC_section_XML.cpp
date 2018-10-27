#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../Include/Mpeg2_DSMCC_section_XML.h"
#include "../Include/dsmcc/Mpeg2_DSMCC_UNM_DII_XML.h"
#include "../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_XML.h"
#include "../Include/dsmcc/Mpeg2_DSMCC_DDM_DDB_XML.h"

//#include "libs_Math/Include/CRC_32.h"

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
	char					pszMnemonic[16];

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	if ((pxmlDoc != NULL) && (pdsmcc_section != NULL))
	{
		//根节点
		sprintf_s(pszField, sizeof(pszField), "DSMCC_section(table_id=0x%02X)", pdsmcc_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, pdsmcc_section->dsmcc_section_length + 3);

		if ((pdsmcc_section->table_id >= 0x38) && (pdsmcc_section->table_id <= 0x3F))
		{
			pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL);

			pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL);
			pxmlDoc->NewElementForBits(pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL);
			pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL);
			pxmlDoc->NewElementForBits(pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL);

			pxmlDoc->NewElementForBits(pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", "transaction_id<15:0>");

			pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL);
			pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL);
			pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL);

			pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL);

			pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL);

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
				XMLElement* pxmlMessageNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, section_payload_length);

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
				XMLElement* pxmlMessageHeaderNode = pxmlDoc->NewBranchElement(pxmlMessageNode, pszField, NULL, message_header_length);

				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, "protocolDiscriminator", pdsmccMessageHeader->protocolDiscriminator, 1, NULL);

				MPEG2_DSMCC_NumericCoding2Text_dsmccType(pdsmccMessageHeader->dsmccType, pszComment, sizeof(pszComment));
				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, "dsmccType", pdsmccMessageHeader->dsmccType, 1, pszComment);

				MPEG2_DSMCC_NumericCoding2Text_messageId(pdsmccMessageHeader->messageId, pszComment, sizeof(pszComment));
				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, "messageId", pdsmccMessageHeader->messageId, 2, pszComment);

				if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
				{
					sprintf_s(pszField, sizeof(pszField), "downloadId()");
				}
				else
				{
					sprintf_s(pszField, sizeof(pszField), "transactionId()");
				}
				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, pszField, pdsmccMessageHeader->TxOrDnloadID, 4, NULL);

				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, "reserved", pdsmccMessageHeader->reserved, 1, NULL);

				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, "adaptationLength", pdsmccMessageHeader->adaptationLength, 1, NULL);

				pxmlDoc->NewElementForByteMode(pxmlMessageHeaderNode, "messageLength", pdsmccMessageHeader->messageLength, 2, NULL);

				assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
				//messageLength解析错误，将是灾难性的

				if (pdsmccMessageHeader->adaptationLength > 0)
				{
					XMLElement* pxmlAdaptationHeaderNode = pxmlDoc->NewBranchElement(pxmlMessageHeaderNode, "dsmccAdaptationHeader()", NULL, pdsmccMessageHeader->adaptationLength);

					pxmlDoc->NewElementForByteMode(pxmlAdaptationHeaderNode, "adaptationType", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType, 1, NULL);

					if (pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataLength > 0)
					{
						pxmlDoc->NewElementForByteBuf(pxmlAdaptationHeaderNode, "adaptationDataByte[ ]", pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataLength, NULL);
					}
				}

				if (pdsmcc_section->dsmccMessagePayloadLength > 0)
				{
					if (pdsmccMessageHeader->messageId == 0x1002)			//DII
					{
						pxmlMessageNode->SetAttribute("comment", "DownloadInfoIndication");
						sprintf_s(pszField, sizeof(pszField), "dsmccMessagePayload[ ]");
					}
					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
					{
						pxmlMessageNode->SetAttribute("comment", "DownloadServerInitiate");
						sprintf_s(pszField, sizeof(pszField), "dsmccMessagePayload[ ]");
					}
					else if (pdsmccMessageHeader->messageId == 0x1003)
					{
						pxmlMessageNode->SetAttribute("comment", "DownloadDataBlock");
						sprintf_s(pszField, sizeof(pszField), "dsmccDownloadDataPayload[ ]");
					}
					else
					{
						sprintf_s(pszComment, sizeof(pszComment), "unknown messageId: 0x%04X", pdsmccMessageHeader->messageId);
						pxmlMessageNode->SetAttribute("comment", pszComment);

						sprintf_s(pszField, sizeof(pszField), "dsmccMessagePayload[ ]");
					}

					XMLElement* pxmlMessagePayloadNode = pxmlDoc->NewBranchElement(pxmlMessageNode, pszField, NULL, pdsmcc_section->dsmccMessagePayloadLength);

					if (pdsmccMessageHeader->messageId == 0x1002)			//DII
					{
						rtcode = MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication_to_xml(pdsmcc_section->dsmccMessagePayloadBuf, pdsmcc_section->dsmccMessagePayloadLength, pxmlDoc, pxmlMessagePayloadNode);
					}
					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
					{
						rtcode = MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate_to_xml(pdsmcc_section->dsmccMessagePayloadBuf, pdsmcc_section->dsmccMessagePayloadLength, pxmlDoc, pxmlMessagePayloadNode);
					}
					else if (pdsmccMessageHeader->messageId == 0x1003)
					{
						rtcode = MPEG2_DSMCC_DDM_DecodeDownloadDataBlock_to_xml(pdsmcc_section->dsmccMessagePayloadBuf, pdsmcc_section->dsmccMessagePayloadLength, pxmlDoc, pxmlMessagePayloadNode);
					}
					else
					{
						pxmlDoc->NewElementForByteBuf(pxmlMessagePayloadNode, "byte_char[ ]", pdsmcc_section->dsmccMessagePayloadBuf, pdsmcc_section->dsmccMessagePayloadLength, NULL);
					}
				}
			}
			else
			{
				assert(0);
			}

			if (pdsmcc_section->section_syntax_indicator == 1)
			{
				sprintf_s(pszField, sizeof(pszField), "CRC_32");
				sprintf_s(pszMnemonic, sizeof(pszMnemonic), "rpchof");
			}
			else
			{
				sprintf_s(pszField, sizeof(pszField), "checksum");
				sprintf_s(pszMnemonic, sizeof(pszMnemonic), "uimsbf");
			}

			XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, pszField, pdsmcc_section->encodedCheckValue, 32, pszMnemonic, NULL);
			if (pdsmcc_section->recalculatedCheckValue != pdsmcc_section->encodedCheckValue)
			{
				sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pdsmcc_section->recalculatedCheckValue);
				pxmlCrcNode->SetAttribute("error", pszComment);
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
