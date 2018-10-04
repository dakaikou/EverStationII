#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#include "../../Include/Mpeg2_DSMCC_section.h"
#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/xml/Mpeg2_DSMCC_section_XML.h"

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
	int						stream_error;
	char					pszTemp[64];

	uint8_t						section_syntax_indicator;

	BITS_t   bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		//根节点
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
				//	//没有校验checksum, 以后增加

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

					if (pdsmcc_section->section_number <= pdsmcc_section->last_section_number)			//DSMCC语法特征点
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
						assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
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
					rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
				}

				if (pDSMCCSection == NULL)
				{
					//说明pdsmcc_section指针临时分配，函数返回前需要释放
					delete pdsmcc_section;
				}
			}
			else
			{
				sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect table_id = 0x%02X", table_id);
				pxmlRootNode->SetAttribute("error", pszTemp);
				rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
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
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}

