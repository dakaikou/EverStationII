#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../nativeInclude/Mpeg2_DSMCC_section.h"
#include "../nativeInclude/Mpeg2_DSMCC_Utilities.h"
#include "../nativeInclude/Mpeg2_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "../xmlInclude/Mpeg2_DSMCC_section_XML.h"
#include "../xmlInclude/dsmcc/Mpeg2_DSMCC_UNM_XML.h"
#include "../xmlInclude/dsmcc/Mpeg2_DSMCC_DDM_XML.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////
int MPEG2_DSMCC_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, dsmcc_section_t* pDSMCCSection)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	int						payload_length;
	uint8_t*				ptemp;
	//int						stream_error;
	char					pszTemp[64];

	//uint8_t						section_syntax_indicator;

	BITS_t   bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		//���ڵ�
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("DSMCC_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (section_size <= MPEG2_DSMCC_SECTION_MAX_SIZE))
		{
			uint8_t table_id = section_buf[0];

			if ((table_id >= 0x38) && (table_id <= 0x3F))
			{
				dsmcc_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_section_t;
				memset(pdsmcc_section, 0x00, sizeof(dsmcc_section_t));

				unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
				unsigned int CRC_32_code = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

				//section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

				//stream_error = 0;
				//if (section_syntax_indicator == 1)
				//{
				//	if (pdsmcc_section->CRC_32_verify != pdsmcc_section->CRC_32)
				//	{
				//		stream_error = 1;
				//	}
				//}
				//else
				//{
				//	//û��У��checksum, �Ժ�����

				//	//			assert((length % 4) == 0);
				//}

				BITS_map(&bs, section_buf, section_size);

				pdsmcc_section->table_id = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pdsmcc_section->table_id, 8, "uimsbf", NULL, &bs);

				pdsmcc_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pdsmcc_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

				pdsmcc_section->private_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "private_indicator", pdsmcc_section->private_indicator, 1, "bslbf", NULL, &bs);

				pdsmcc_section->reserved0 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pdsmcc_section->reserved0, 2, "bslbf", NULL, &bs);

				pdsmcc_section->dsmcc_section_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "dsmcc_section_length", pdsmcc_section->dsmcc_section_length, 12, "uimsbf", NULL, &bs);

				if ((pdsmcc_section->dsmcc_section_length + 3) == section_size)
				{
					pdsmcc_section->table_id_extension = BITS_get(&bs, 16);
					tinyxml2::XMLElement* pxmlTableIDExtNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id_extension", pdsmcc_section->table_id_extension, 16, "uimsbf", NULL, &bs);
					if (table_id == TABLE_ID_DSMCC_UNM)
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "transaction_id <15:0>");
						pxmlTableIDExtNode->SetAttribute("comment", pszTemp);
					}
					else if (table_id == TABLE_ID_DSMCC_DDM)
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "moduleId");
						pxmlTableIDExtNode->SetAttribute("comment", pszTemp);
					}

					pdsmcc_section->reserved1 = BITS_get(&bs, 2);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pdsmcc_section->reserved1, 2, "bslbf", NULL, &bs);

					pdsmcc_section->version_number = BITS_get(&bs, 5);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", pdsmcc_section->version_number, 5, "uimsbf", NULL, &bs);

					pdsmcc_section->current_next_indicator = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", pdsmcc_section->current_next_indicator, 1, "bslbf", NULL, &bs);

					pdsmcc_section->section_number = BITS_get(&bs, 8);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", pdsmcc_section->section_number, 8, "uimsbf", NULL, &bs);

					pdsmcc_section->last_section_number = BITS_get(&bs, 8);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", pdsmcc_section->last_section_number, 8, "uimsbf", NULL, &bs);

					if (pdsmcc_section->section_number <= pdsmcc_section->last_section_number)			//DSMCC�﷨������
					{
						payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
						ptemp = bs.p_cur;
						BITS_byteSkip(&bs, payload_length);
						tinyxml2::XMLElement* pxmlMessageNode = NULL;

						BYTES_t bytes;
						BYTES_map(&bytes, ptemp, payload_length);

						if (pdsmcc_section->table_id == TABLE_ID_DSMCC_MPE)			//0x3A
						{
							pxmlMessageNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "LLCSNAP()", -1, -1, NULL, NULL, &bs);
						}
						else if (pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM)
						{
							pxmlMessageNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "userNetworkMessage()", -1, -1, NULL, NULL, &bs);
						}
						else if (pdsmcc_section->table_id == TABLE_ID_DSMCC_DDM)
						{
							pxmlMessageNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "downloadDataMessage()", -1, -1, NULL, NULL, &bs);
						}
						else if (pdsmcc_section->table_id == TABLE_ID_DSMCC_SD)
						{
							pxmlMessageNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "DSMCC_descriptor_list()", -1, -1, NULL, NULL, &bs);
						}
						//else if (pdsmcc_section->table_id == TABLE_ID_DSMCC_PVT)
						//{
						//	pxmlMessageNode = pxmlDoc->NewSyntaxElement(pxmlRootNode, "private_data_byte()", -1, NULL, -1, &bs);
						//}
						else
						{
							pxmlMessageNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "Unknow ()", -1, -1, NULL, NULL, &bs);
						}

						unsigned int CRC_32 = BITS_get(&bs, 32);
						assert(CRC_32 == CRC_32_code);			//�ٴ�У�飬�������������ָ��ƫ���Ƿ��д�
						pdsmcc_section->CRC_32 = CRC_32_code;
						pdsmcc_section->CRC_32_verify = CRC_32_verify;
						tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", pdsmcc_section->CRC_32, 32, "rpchof", NULL, &bs);

						if (CRC_32_verify != CRC_32_code)
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", pdsmcc_section->CRC_32_verify);
							pxmlCrcNode->SetAttribute("error", pszTemp);
							pxmlRootNode->SetAttribute("error", pszTemp);
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! section_number = 0x%02X, last_section_number = 0x%02X", pdsmcc_section->section_number, pdsmcc_section->last_section_number);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "syntax error! section_size = %d, dsmcc_section_length =%d", section_size, pdsmcc_section->dsmcc_section_length);
					pxmlRootNode->SetAttribute("error", pszTemp);
					rtcode = SECTION_PARSE_LENGTH_ERROR;				//section���ȴ���
				}

				if (pDSMCCSection == NULL)
				{
					//˵��pdsmcc_sectionָ����ʱ���䣬��������ǰ��Ҫ�ͷ�
					delete pdsmcc_section;
				}
			}
			else
			{
				sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect table_id = 0x%02X", table_id);
				pxmlRootNode->SetAttribute("error", pszTemp);
				rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id��������
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "section buffer parameters error! section_size = %d", section_size);
			pxmlRootNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//�����������
	}

	return rtcode;
}

int MPEG2_DSMCC_UNM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_unm_section_t* pDSMCCSection)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	dsmcc_unm_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_unm_section_t;
	rtcode = MPEG2_DSMCC_UNM_DecodeSection(section_buf, section_size, pdsmcc_section);
	rtcode = MPEG2_DSMCC_UNM_PresentSection_to_XML(pxmlDoc, pdsmcc_section);

	if (pDSMCCSection == NULL)
	{
		//˵��pdsmcc_sectionָ����ʱ���䣬��������ǰ��Ҫ�ͷ�
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

		//���ڵ�
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

				//����DownloadServerInitiate��DownloadInfoIndication��DownloadCancel

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
				//messageLength�������󣬽��������Ե�

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
						rtcode = MPEG2_DSMCC_UNM_PresentDownloadInfoIndication_to_xml(pxmlDoc, pxmlUNMNode, pDownloadInfoIndication);
					}
					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
					{
						XMLNODE_SetAttribute(pxmlUNMNode, "comment", "DownloadServerInitiate");

						pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);
						rtcode = MPEG2_DSMCC_UNM_PresentDownloadServerInitiate_to_xml(pxmlDoc, pxmlUNMNode, pDownloadServerInitiate);
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
			rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id��������
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//�����������
	}

	return rtcode;
}


int MPEG2_DSMCC_DDM_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, dsmcc_ddm_section_t* pDSMCCSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	dsmcc_ddm_section_t* pdsmcc_section = (pDSMCCSection != NULL) ? pDSMCCSection : new dsmcc_ddm_section_t;
	rtcode = MPEG2_DSMCC_DDM_DecodeSection(section_buf, section_size, pdsmcc_section);
	rtcode = MPEG2_DSMCC_DDM_PresentSection_to_XML(pxmlDoc, pdsmcc_section);

	if (pDSMCCSection == NULL)
	{
		//˵��pdsmcc_sectionָ����ʱ���䣬��������ǰ��Ҫ�ͷ�
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

		//���ڵ�
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

			//����DownloadDataBlock
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
			//messageLength�������󣬽��������Ե�

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
					rtcode = MPEG2_DSMCC_DDM_PresentDownloadDataBlock_to_xml(pxmlDoc, pxmlDDMNode, pDownloadDataBlock);
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


