#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/Mpeg2_PSI_section.h"
#include "../../Include/Mpeg2_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/Mpeg2_PSI_section_XML.h"
#include "../../Include/xml/Mpeg2_PSI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

int MPEG2_PSI_CAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, CA_section_t* pCATSection)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		move_length;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		loop_length;
	S32		descriptor_loop_length;
	S32		reserved_count;
	char	pszTemp[64];

	BITS_t  bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", section_size);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("CA_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		pxmlRootNode->SetAttribute("comment", pszTemp);

		if ((section_buf != NULL) && (section_size >= MPEG2_PSI_CAT_SECTION_MIN_SIZE) && (section_size <= MPEG2_PSI_CAT_SECTION_MAX_SIZE))
		{
			//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
			//这是内部处理的策略问题，不是必须这么做的
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				CA_section_t* pcat_section = (pCATSection == NULL) ? new CA_section_t : pCATSection;
				memset(pcat_section, 0x00, sizeof(CA_section_t));

				BITS_map(&bs, section_buf, section_size);

				pcat_section->table_id = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pcat_section->table_id, 8, "uimsbf", NULL, &bs);

				pcat_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pcat_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

				if (pcat_section->section_syntax_indicator == 1)
				{
					pcat_section->reserved_future_use = BITS_get(&bs, 1);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "'0'", pcat_section->reserved_future_use, 1, "bslbf", NULL, &bs);
					pcat_section->reserved0 = BITS_get(&bs, 2);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pcat_section->reserved0, 2, "bslbf", NULL, &bs);

					pcat_section->section_length = BITS_get(&bs, 12);
					pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", pcat_section->section_length, 12, "uimsbf", NULL, &bs);

					if ((pcat_section->section_length >= (MPEG2_PSI_CAT_SECTION_MIN_SIZE - 3)) && (pcat_section->section_length <= (MPEG2_PSI_CAT_SECTION_MAX_SIZE - 3)))
					{
						pcat_section->reserved1 = BITS_get(&bs, 18);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", pcat_section->reserved1, 18, "bslbf", NULL, &bs);

						pcat_section->version_number = BITS_get(&bs, 5);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", pcat_section->version_number, 5, "uimsbf", NULL, &bs);

						pcat_section->current_next_indicator = BITS_get(&bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", pcat_section->current_next_indicator, 1, "bslbf", NULL, &bs);

						pcat_section->section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", pcat_section->section_number, 8, "uimsbf", NULL, &bs);

						pcat_section->last_section_number = BITS_get(&bs, 8);
						pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", pcat_section->last_section_number, 8, "uimsbf", NULL, &bs);

						if (pcat_section->section_number <= pcat_section->last_section_number)			//PMT语法特征点
						{
							descriptor_loop_length = pcat_section->section_length - 9;
							reserved_count = 0;
							if (descriptor_loop_length > 0)
							{
								ptemp = bs.p_cur;
								BITS_byteSkip(&bs, descriptor_loop_length);

								tinyxml2::XMLElement* pxmlCADescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CA描述符循环()", -1, -1, NULL, NULL, &bs);

								loop_length = descriptor_loop_length;
								while ((loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
								{
									descriptor_tag = ptemp[0];
									descriptor_length = ptemp[1];
									move_length = descriptor_length + 2;

									pcat_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
									pcat_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
									pcat_section->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
									pcat_section->reserved_descriptor[reserved_count].descriptor_size = move_length;

									switch (descriptor_tag)
									{
									case MPEG2_PSI_CA_DESCRIPTOR:
										MPEG2_PSI_decode_CA_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlCADescriptorLoopNode);
										break;
									default:
										decode_reserved_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlCADescriptorLoopNode);
										break;
									}

									reserved_count++;

									ptemp += move_length;
									loop_length -= move_length;
								}
							}
							pcat_section->reserved_count = reserved_count;

							unsigned int CRC_32 = BITS_get(&bs, 32);
							assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
							pcat_section->CRC_32 = CRC_32_code;
							pcat_section->CRC_32_verify = CRC_32_verify;
							tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", pcat_section->CRC_32, 32, "rpchof", NULL, &bs);

							if (CRC_32_verify != CRC_32_code)
							{
								sprintf_s(pszTemp, sizeof(pszTemp), "CRC_32 Error! Should be 0x%08X", pcat_section->CRC_32_verify);
								pxmlCrcNode->SetAttribute("error", pszTemp);
								pxmlRootNode->SetAttribute("error", pszTemp);
							}
						}
						else
						{
							sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_number = 0x%02X, last_section_number = 0x%02X", pcat_section->section_number, pcat_section->last_section_number);
							pxmlRootNode->SetAttribute("error", pszTemp);

							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}
					}
					else
					{
						sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_length = %d", pcat_section->section_length);
						pxmlRootNode->SetAttribute("error", pszTemp);

						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					sprintf_s(pszTemp, sizeof(pszTemp), "section syntax error! incorrect section_syntax_indicator = %d", pcat_section->section_syntax_indicator);
					pxmlRootNode->SetAttribute("error", pszTemp);

					rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}

				if (pCATSection == NULL)
				{
					//说明pcat_section指针临时分配，函数返回前需要释放
					delete pcat_section;
				}
			}
			//else
			//{
			//	pxmlDoc->NewTitleElement(pxmlRoot, "section buffer CRC error!");
			//	rtcode = SECTION_PARSE_CRC_ERROR;
			//}
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section buffer parameters error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}



