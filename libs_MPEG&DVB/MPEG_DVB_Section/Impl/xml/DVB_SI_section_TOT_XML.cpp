#include <string.h>
#include <assert.h>

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/DVB_SI_section_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

int DVB_SI_TOT_DecodeSection_to_XML(uint8_t *section_buf, int section_length, XMLDocForMpegSyntax* pxmlDoc, time_offset_section_t* pTOTSection)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	S32		N;
	S32		loop_length;
	U16		descriptor_tag;
	U8		descriptor_length;
	U16		move_length;
	U8*		ptemp;
	char	pszTemp[64];
	BITS_t  bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("time_offset_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_length);

		if ((section_buf != NULL) && (section_length >= DVB_SI_TOT_SECTION_MIN_SIZE) && (section_length <= DVB_SI_TOT_SECTION_MAX_SIZE))
		{
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_length - 4);
			unsigned int CRC_32_code = (section_buf[section_length - 4] << 24) | (section_buf[section_length - 3] << 16) | (section_buf[section_length - 2] << 8) | section_buf[section_length - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				time_offset_section_t* ptot_section = (pTOTSection != NULL) ? pTOTSection : new time_offset_section_t;
				memset(ptot_section, 0x00, sizeof(time_offset_section_t));

				BITS_map(&bs, section_buf, section_length);

				ptot_section->table_id = BITS_get(&bs, 8);
				assert(ptot_section->table_id == TABLE_ID_TOT);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", ptot_section->table_id, 8, "uimsbf", NULL, &bs);

				ptot_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", ptot_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);
				ptot_section->reserved_future_use = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", ptot_section->reserved_future_use, 1, "bslbf", NULL, &bs);
				ptot_section->reserved0 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ptot_section->reserved0, 2, "bslbf", NULL, &bs);

				ptot_section->section_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", ptot_section->section_length, 12, "uimsbf", NULL, &bs);

				ptot_section->UTC_time = BITS_x64_get(&bs, 40);
				DVB_SI_NumericCoding2Text_UTCTime(ptot_section->UTC_time, pszTemp, sizeof(pszTemp));
				pxmlDoc->NewKeyValuePairElementX64Mode(pxmlRootNode, "UTC_time", ptot_section->UTC_time, 40, "bslbf", pszTemp, &bs);

				ptot_section->reserved1 = BITS_get(&bs, 4);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved1", ptot_section->reserved1, 4, "bslbf", NULL, &bs);

				ptot_section->descriptors_loop_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "descriptors_loop_length", ptot_section->descriptors_loop_length, 12, "uimsbf", NULL, &bs);

				N = 0;
				if (ptot_section->descriptors_loop_length > 0)
				{
					ptemp = bs.p_cur;
					BITS_byteSkip(&bs, ptot_section->descriptors_loop_length);
					tinyxml2::XMLElement* pxmlDescriptorLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "descriptors_loop()", -1, -1, NULL, NULL, &bs);

					loop_length = ptot_section->descriptors_loop_length;
					while ((loop_length >= 2) && (N < MAX_RESERVED_DESCRIPTORS))		//N为啥需要小于64？ chendelin  2018/5/19
					{
						descriptor_tag = ptemp[0];
						descriptor_length = ptemp[1];
						move_length = descriptor_length + 2;

						ptot_section->descriptors[N].descriptor_tag = descriptor_tag;
						ptot_section->descriptors[N].descriptor_length = descriptor_length;
						ptot_section->descriptors[N].descriptor_buf = ptemp;
						ptot_section->descriptors[N].descriptor_size = (uint8_t)move_length;

						switch (descriptor_tag)
						{
						case DVB_SI_LOCAL_TIME_OFFSET_DESCRIPTOR:
							DVB_SI_decode_local_time_offset_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
							break;
						default:
							decode_reserved_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptorLoopNode);

							break;
						}

						N++;

						ptemp += move_length;
						loop_length -= move_length;
					}
				}
				ptot_section->N = N;

				unsigned int CRC_32 = BITS_get(&bs, 32);
				assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
				ptot_section->CRC_32 = CRC_32_code;
				ptot_section->CRC_32_verify = CRC_32_verify;
				tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", ptot_section->CRC_32, 32, "rpchof", NULL, &bs);

				if (CRC_32_verify != CRC_32_code)
				{
					pxmlCrcNode->SetAttribute("error", "Error!");
					pxmlRootNode->SetAttribute("error", "CRC_32 Error!");
				}

				if (pTOTSection == NULL)
				{
					//说明上面的ptot_section指针为临时分配，函数返回前需要释放
					delete ptot_section;
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
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section buffer parameter error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

