#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/MPEG2_table_id.h"
#include "../../Include/Mpeg2_PSI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/Mpeg2_PSI_section_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int MPEG2_PSI_TSDT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, TS_description_section_t* pTSDTSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	//U8*		old_buf;
	S32		move_length;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		loop_length;
	S32		reserved_count;
	BITS_t	bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRoot = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("TS_description_section()");
		pxmlDoc->InsertEndChild(pxmlRoot);
		pxmlDoc->UpdateBufMark(pxmlRoot, buf, buf + length);

		if ((buf != NULL) && (length >= MPEG2_PSI_TSDT_SECTION_MIN_SIZE) && (length <= MPEG2_PSI_TSDT_SECTION_MAX_SIZE))
		{
			//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
			//这是内部处理的策略问题，不是必须这么做的
			unsigned int CRC_32_verify = Encode_CRC_32(buf, length - 4);
			unsigned int CRC_32_code = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				TS_description_section_t* ptsdt_section = (pTSDTSection != NULL) ? pTSDTSection : new TS_description_section_t;
				memset(ptsdt_section, 0x00, sizeof(TS_description_section_t));

				BITS_map(&bs, buf, length);

				ptsdt_section->table_id = BITS_get(&bs, 8);
				assert(ptsdt_section->table_id == TABLE_ID_TSDT);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "table_id", ptsdt_section->table_id, 8, "uimsbf", NULL, &bs);

				ptsdt_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_syntax_indicator", ptsdt_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);
				ptsdt_section->reserved_future_use = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved_future_use", ptsdt_section->reserved_future_use, 1, "bslbf", NULL, &bs);
				ptsdt_section->reserved0 = BITS_get(&bs, 4);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved0", ptsdt_section->reserved0, 4, "bslbf", NULL, &bs);

				ptsdt_section->section_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_length", ptsdt_section->section_length, 12, "bslbf", NULL, &bs);

				ptsdt_section->reserved1 = BITS_get(&bs, 18);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved1", ptsdt_section->section_length, 18, "bslbf", NULL, &bs);

				ptsdt_section->version_number = BITS_get(&bs, 5);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "version_number", ptsdt_section->version_number, 5, "uimsbf", NULL, &bs);

				ptsdt_section->current_next_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "current_next_indicator", ptsdt_section->current_next_indicator, 1, "bslbf", NULL, &bs);

				ptsdt_section->section_number = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_number", ptsdt_section->section_number, 8, "uimsbf", NULL, &bs);

				ptsdt_section->last_section_number = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "last_section_number", ptsdt_section->last_section_number, 8, "uimsbf", NULL, &bs);

				loop_length = ptsdt_section->section_length - 9;
				reserved_count = 0;
				if (loop_length > 0)
				{
					ptemp = bs.p_cur;
					BITS_byteSkip(&bs, loop_length);

					while ((loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
					{
						descriptor_tag = ptemp[0];
						descriptor_length = ptemp[1];
						move_length = descriptor_length + 2;

						ptsdt_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
						ptsdt_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
						ptsdt_section->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
						ptsdt_section->reserved_descriptor[reserved_count].descriptor_size = move_length;

						reserved_count++;

						ptemp += move_length;
						loop_length -= move_length;
					}
				}
				ptsdt_section->reserved_count = reserved_count;

				unsigned int CRC_32 = BITS_get(&bs, 32);
				assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
				ptsdt_section->CRC_32 = CRC_32_code;
				ptsdt_section->CRC_32_verify = CRC_32_verify;
				tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRoot, "CRC_32", ptsdt_section->CRC_32, 32, "rpchof", NULL, &bs);

				if (CRC_32_verify != CRC_32_code)
				{
					pxmlCrcNode->SetAttribute("error", "Error!");
					pxmlRoot->SetAttribute("error", "Error!");
				}

				if (pTSDTSection == NULL)
				{
					//说明ppat_section指针临时分配，函数返回前需要释放
					delete ptsdt_section;
				}
			}
			//else
			//{
			//	pxmlDoc->NewTitleElement(pRoot, "section buffer CRC error!");
			//	rtcode = SECTION_PARSE_CRC_ERROR;
			//}
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section buffer parameters error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


