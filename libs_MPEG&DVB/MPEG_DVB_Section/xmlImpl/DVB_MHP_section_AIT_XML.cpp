#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../nativeInclude/DVB_MHP_section.h"
#include "../nativeInclude/DVB_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "../xmlInclude/MPEG_DVB_Common_XML.h"
#include "../xmlInclude/DVB_MHP_section_XML.h"
#include "../xmlInclude/DVB_MHP_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_MHP_AIT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, application_information_section_t* pAITSection)
{
	S32	 rtcode = SECTION_PARSE_NO_ERROR;
	S32  application_loop_length;
	S32  descriptor_loop_length;
	U8*  pl1temp;
	U8*  pl2temp;
	U8*  old_app_ptr;
	//U8*  old_buf;
	S8	 N;
	S8	 reserved_count;

	U16	 descriptor_tag;
	U8	 descriptor_length;
	U16	 move_length;

	BITS_t bs;

	char pszTemp[64];

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRoot = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("application_information_section()");
		pxmlDoc->InsertEndChild(pxmlRoot);
		pxmlDoc->UpdateBufMark(pxmlRoot, buf, buf + length);

		//AIT section最小长度16字节
		if ((buf != NULL) && (length >= DVB_MHP_AIT_SECTION_MIN_SIZE) && (length <= DVB_MHP_AIT_SECTION_MAX_SIZE))
		{
			unsigned int CRC_32_verify = Encode_CRC_32(buf, length - 4);
			unsigned int CRC_32_code = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				application_information_section_t* pait_section = (pAITSection != NULL) ? pAITSection : new application_information_section_t;
				memset(pait_section, 0x00, sizeof(application_information_section_t));

				BITS_map(&bs, buf, length);

				pait_section->table_id = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "table_id", pait_section->table_id, 8, "uimsbf", NULL, &bs);

				pait_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_syntax_indicator", pait_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);
				pait_section->reserved_future_use0 = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved_future_use", pait_section->reserved_future_use0, 1, "bslbf", NULL, &bs);
				pait_section->reserved0 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved", pait_section->reserved0, 2, "bslbf", NULL, &bs);

				pait_section->section_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_length", pait_section->section_length, 12, "uimsbf", NULL, &bs);

				assert((pait_section->section_length <= 1021));

				pait_section->test_application_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "test_application_flag", pait_section->test_application_flag, 1, "bslbf", NULL, &bs);

				pait_section->application_type = BITS_get(&bs, 15);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "application_type", pait_section->application_type, 15, "uimsbf", NULL, &bs);

				pait_section->reserved1 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved", pait_section->reserved1, 2, "bslbf", NULL, &bs);
				pait_section->version_number = BITS_get(&bs, 5);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "version_number", pait_section->version_number, 5, "uimsbf", NULL, &bs);
				pait_section->current_next_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "current_next_indicator", pait_section->current_next_indicator, 1, "bslbf", NULL, &bs);

				pait_section->section_number = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section_number", pait_section->section_number, 8, "uimsbf", NULL, &bs);

				pait_section->last_section_number = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "last_section_number", pait_section->last_section_number, 8, "uimsbf", NULL, &bs);

				pait_section->reserved_future_use1 = BITS_get(&bs, 4);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved_future_use1", pait_section->reserved_future_use1, 4, "bslbf", NULL, &bs);
				pait_section->common_descriptors_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "common_descriptors_length", pait_section->common_descriptors_length, 12, "uimsbf", NULL, &bs);

				reserved_count = 0;
				if (pait_section->common_descriptors_length > 0)
				{
					pl1temp = bs.p_cur;
					BITS_byteSkip(&bs, pait_section->common_descriptors_length);

					tinyxml2::XMLElement* pxmlDescriptorsNode = pxmlDoc->NewKeyValuePairElement(pxmlRoot, "AIT描述符循环()", -1, -1, NULL, NULL, &bs);

					descriptor_loop_length = pait_section->common_descriptors_length;
					while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
					{
						descriptor_tag = (0x2000 | pl1temp[0]);
						descriptor_length = pl1temp[1];
						move_length = descriptor_length + 2;

						pait_section->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
						pait_section->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
						pait_section->reserved_descriptor[reserved_count].descriptor_buf = pl1temp;
						pait_section->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

						switch (descriptor_tag)
						{
						case DVB_MHP_TRANSPORT_PROTOCOL_DESCRIPTOR:
							DVB_MHP_decode_transport_protocol_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlDescriptorsNode);
							break;
						default:
							decode_reserved_descriptor_to_xml(pl1temp, move_length, pxmlDoc, pxmlDescriptorsNode);

							break;
						}

						reserved_count++;

						pl1temp += move_length;
						descriptor_loop_length -= move_length;
					}
				}
				pait_section->reserved_count = reserved_count;

				pait_section->reserved_future_use2 = BITS_get(&bs, 4);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "reserved_future_use", pait_section->reserved_future_use2, 4, "bslbf", NULL, &bs);

				pait_section->application_loop_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRoot, "application_loop_length", pait_section->application_loop_length, 12, "uimsbf", NULL, &bs);

				N = 0;
				if (pait_section->application_loop_length > 0)
				{
					pl1temp = bs.p_cur;
					BITS_byteSkip(&bs, pait_section->application_loop_length);
					tinyxml2::XMLElement* pxmlApplicationLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRoot, "应用循环()", -1, -1, NULL, NULL, &bs);

					application_loop_length = pait_section->application_loop_length;
					//while ((application_loop_length >= 9) && (N < MAX_APPLICATIONS))
					//{
					//	old_app_ptr = pl1temp;
					//	tinyxml2::XMLElement* pxmlApplicationNode = pxmlDoc->NewTitleElement(pxmlApplicationLoopNode, "应用");

					//	application_t* pApplication = pait_section->applications + N;

					//	//解析applications
					//	old_buf = pl1temp;
					//	tinyxml2::XMLElement* pxmlIdentifierNode = pxmlDoc->NewSyntaxElement(pxmlApplicationNode, "application_identifier()", -1, NULL, -1, old_buf, pl1temp + 6);

					//	pApplication->application_identifier.organisation_id = *pl1temp++;
					//	pApplication->application_identifier.organisation_id <<= 8;
					//	pApplication->application_identifier.organisation_id |= *pl1temp++;
					//	pApplication->application_identifier.organisation_id <<= 8;
					//	pApplication->application_identifier.organisation_id |= *pl1temp++;
					//	pApplication->application_identifier.organisation_id <<= 8;
					//	pApplication->application_identifier.organisation_id |= *pl1temp++;
					//	pxmlDoc->NewSyntaxElement(pxmlIdentifierNode, "organisation_id", 32, "uimsbf", pApplication->application_identifier.organisation_id, old_buf, pl1temp);

					//	old_buf = pl1temp;
					//	pApplication->application_identifier.application_id = *pl1temp++;
					//	pApplication->application_identifier.application_id <<= 8;
					//	pApplication->application_identifier.application_id |= *pl1temp++;
					//	pxmlDoc->NewSyntaxElement(pxmlIdentifierNode, "application_id", 16, "uimsbf", pApplication->application_identifier.application_id, old_buf, pl1temp);

					//	old_buf = pl1temp;
					//	pApplication->application_control_code = *pl1temp++;
					//	pxmlDoc->NewSyntaxElement(pxmlApplicationNode, "application_control_code", 8, "uimsbf", pApplication->application_control_code, old_buf, pl1temp);

					//	old_buf = pl1temp;
					//	pApplication->reserved_future_use = (*pl1temp & 0xf0) >> 4;
					//	pxmlDoc->NewSyntaxElement(pxmlApplicationNode, "reserved_future_use", 4, "bslbf", pApplication->reserved_future_use, old_buf, pl1temp);
					//	pApplication->application_descriptors_loop_length = (*pl1temp++ & 0x0f) << 8;
					//	pApplication->application_descriptors_loop_length |= *pl1temp++;
					//	pxmlDoc->NewSyntaxElement(pxmlApplicationNode, "application_descriptors_loop_length", 12, "uimsbf", pApplication->application_descriptors_loop_length, old_buf, pl1temp);

					//	reserved_count = 0;
					//	if (pApplication->application_descriptors_loop_length > 0)
					//	{
					//		pl2temp = pl1temp;
					//		pl1temp += pApplication->application_descriptors_loop_length;

					//		tinyxml2::XMLElement* pxmlDescriptorLoopNode = pxmlDoc->NewSyntaxElement(pxmlApplicationNode, "应用描述符循环()", -1, NULL, -1, pl2temp, pl1temp);

					//		descriptor_loop_length = pApplication->application_descriptors_loop_length;
					//		while ((descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
					//		{
					//			descriptor_tag = (0x2000 | pl2temp[0]);
					//			descriptor_length = pl2temp[1];
					//			move_length = descriptor_length + 2;

					//			pApplication->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
					//			pApplication->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
					//			pApplication->reserved_descriptor[reserved_count].descriptor_buf = pl2temp;
					//			pApplication->reserved_descriptor[reserved_count].descriptor_size = (uint8_t)move_length;

					//			switch (descriptor_tag)
					//			{
					//			case DVB_MHP_APPLICATION_DESCRIPTOR:
					//				DVB_MHP_decode_application_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
					//				break;
					//			case DVB_MHP_APPLICATION_NAME_DESCRIPTOR:
					//				DVB_MHP_decode_application_name_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
					//				break;
					//			case DVB_MHP_TRANSPORT_PROTOCOL_DESCRIPTOR:
					//				DVB_MHP_decode_transport_protocol_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
					//				break;
					//			case DVB_MHP_DVB_J_APPLICATION_LOCATION_DESCRIPTOR:
					//				DVB_MHP_decode_dvb_j_application_location_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
					//				break;
					//			default:
					//				decode_reserved_descriptor_to_xml(pl2temp, move_length, pxmlDoc, pxmlDescriptorLoopNode);

					//				break;
					//			}

					//			reserved_count++;
					//			pl2temp += move_length;
					//			descriptor_loop_length -= move_length;
					//		}
					//	}
					//	pApplication->reserved_count = reserved_count;

					//	pxmlDoc->UpdateBufMark(pxmlApplicationNode, old_app_ptr, pl1temp);
					//	sprintf_s(pszTemp, sizeof(pszTemp), "ID=0x%04X", pApplication->application_identifier.application_id);
					//	pxmlApplicationNode->SetAttribute("comment", pszTemp);

					//	N++;
					//	application_loop_length -= (9 + pApplication->application_descriptors_loop_length);
					//}
				}
				pait_section->N = N;

				unsigned int CRC_32 = BITS_get(&bs, 32);
				assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
				pait_section->CRC_32 = CRC_32_code;
				pait_section->CRC_32_verify = CRC_32_verify;
				tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRoot, "CRC_32", pait_section->CRC_32, 32, "rpchof", NULL, &bs);

				if (CRC_32_verify != CRC_32_code)
				{
					pxmlCrcNode->SetAttribute("error", "Error!");
					pxmlRoot->SetAttribute("error", "Error!");
				}

				if (pAITSection == NULL)
				{
					//说明pait_section指针临时分配，函数返回前需要释放
					delete pait_section;
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
			pxmlDoc->NewKeyValuePairElement(pxmlRoot, "section buffer parameter error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

