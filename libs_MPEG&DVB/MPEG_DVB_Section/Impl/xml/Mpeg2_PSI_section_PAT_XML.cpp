#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/Mpeg2_PSI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/Mpeg2_PSI_section_XML.h"

#include "libs_Math/Include/CRC_32.h"

int MPEG2_PSI_PAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, program_association_section_t* pPATSection)
{
	S32	 rtcode = SECTION_PARSE_NO_ERROR;
	S32  N;
	S32  program_loop_length;
	BITS_t bs;

	char pszComment[128];

	PROGRAM_MAP_DESCRIPTION_t* pProgram;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("program_association_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= MPEG2_PSI_PAT_SECTION_MIN_SIZE) && (section_size <= MPEG2_PSI_PAT_SECTION_MAX_SIZE))
		{
			//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
			//这是内部处理的策略问题，不是必须这么做的
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			//if (CRC_32_verify == CRC_32_code)
			{
				BITS_map(&bs, section_buf, section_size);

				program_association_section_t* ppat_section = (pPATSection != NULL) ? pPATSection : new program_association_section_t;
				memset(ppat_section, 0x00, sizeof(program_association_section_t));

				ppat_section->table_id = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", ppat_section->table_id, 8, "uimsbf", NULL, &bs);

				ppat_section->section_syntax_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", ppat_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);
				ppat_section->reserved_future_use = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "'0'", ppat_section->reserved_future_use, 1, "bslbf", NULL, &bs);
				ppat_section->reserved0 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ppat_section->reserved0, 2, "bslbf", NULL, &bs);

				ppat_section->section_length = BITS_get(&bs, 12);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", ppat_section->section_length, 12, "uimsbf", NULL, &bs);

				ppat_section->transport_stream_id = BITS_get(&bs, 16);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "transport_stream_id", ppat_section->transport_stream_id, 16, "uimsbf", NULL, &bs);

				ppat_section->reserved1 = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ppat_section->reserved1, 2, "bslbf", NULL, &bs);

				ppat_section->version_number = BITS_get(&bs, 5);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "version_number", ppat_section->version_number, 5, "uimsbf", NULL, &bs);

				ppat_section->current_next_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", ppat_section->current_next_indicator, 1, "bslbf", NULL, &bs);

				ppat_section->section_number = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", ppat_section->section_number, 8, "uimsbf", NULL, &bs);

				ppat_section->last_section_number = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", ppat_section->last_section_number, 8, "uimsbf", NULL, &bs);

				N = 0;
				program_loop_length = ppat_section->section_length - 9;
				if (program_loop_length > 0)
				{
					uint8_t* loop_start_ptr = bs.p_cur;

					BITS_byteSkip(&bs, program_loop_length);
					tinyxml2::XMLElement* pxmlPmtLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "节目-PMT_PID 映射循环()");
					pxmlDoc->UpdateBufMark(pxmlPmtLoopNode, bs.p_old, bs.p_cur);

					BITS_t program_loop_bs;
					BITS_map(&program_loop_bs, loop_start_ptr, program_loop_length);

					pProgram = ppat_section->astProgram;
					while ((program_loop_length >= 4) && (N < MAX_PROGRAMS_PER_PAT_SECTION))		//N为啥需要小于64？ chendelin  2018/5/19
					{
						uint8_t* program_map_start_ptr = program_loop_bs.p_cur;
						tinyxml2::XMLElement* pxmlMapNode = pxmlDoc->NewKeyValuePairElement(pxmlPmtLoopNode, "节目-PMT_PID 映射");

						pProgram->program_number = BITS_get(&program_loop_bs, 16);
						pxmlDoc->NewKeyValuePairElement(pxmlMapNode, "program_number", pProgram->program_number, 16, "uimsbf", NULL, &program_loop_bs);

						pProgram->reserved = BITS_get(&program_loop_bs, 3);
						pxmlDoc->NewKeyValuePairElement(pxmlMapNode, "reserved", pProgram->reserved, 3, "bslbf", NULL, &program_loop_bs);

						pProgram->program_map_PID = BITS_get(&program_loop_bs, 13);
						if (pProgram->program_number == 0)
						{
							pxmlDoc->NewKeyValuePairElement(pxmlMapNode, "network_PID", pProgram->program_map_PID, 13, "uimsbf", NULL, &program_loop_bs);
							sprintf_s(pszComment, sizeof(pszComment), "program_number = %d -- NIT_PID=0x%04X\0", pProgram->program_number, pProgram->program_map_PID);
						}
						else
						{
							pxmlDoc->NewKeyValuePairElement(pxmlMapNode, "program_map_PID", pProgram->program_map_PID, 13, "uimsbf", NULL, &program_loop_bs);
							sprintf_s(pszComment, sizeof(pszComment), "program_number = %d -- PMT_PID=0x%04X\0", pProgram->program_number, pProgram->program_map_PID);
						}

						pxmlMapNode->SetAttribute("comment", pszComment);
						pxmlDoc->UpdateBufMark(pxmlMapNode, program_map_start_ptr, program_loop_bs.p_cur);

						program_loop_length -= 4;
						N++;
						pProgram++;
					}

					sprintf_s(pszComment, sizeof(pszComment), "共 %d项映射\0", N);
					pxmlPmtLoopNode->SetAttribute("comment", pszComment);

				}
				ppat_section->N = N;

				unsigned int CRC_32 = BITS_get(&bs, 32);
				assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
				ppat_section->CRC_32 = CRC_32_code;
				ppat_section->CRC_32_verify = CRC_32_verify;
				tinyxml2::XMLElement* pxmlCrcNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CRC_32", ppat_section->CRC_32, 32, "rpchof", NULL, &bs);

				if (CRC_32_verify != CRC_32_code)
				{
					pxmlCrcNode->SetAttribute("error", "Error!");
					pxmlRootNode->SetAttribute("error", "CRC_32 Error!");
				}

				if (pPATSection == NULL)
				{
					//说明ppat_section指针临时分配，函数返回前需要释放
					delete ppat_section;
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



