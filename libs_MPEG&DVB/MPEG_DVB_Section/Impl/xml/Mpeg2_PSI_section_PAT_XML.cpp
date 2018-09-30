#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/Mpeg2_PSI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/Mpeg2_PSI_section_XML.h"

#include "libs_Math/Include/CRC_32.h"

int MPEG2_PSI_PAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, program_association_section_t* pPATSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	program_association_section_t* ppat_section = (pPATSection != NULL) ? pPATSection : new program_association_section_t;
	rtcode = MPEG2_PSI_PAT_DecodeSection(section_buf, section_size, ppat_section);

	if (pxmlDoc != NULL)
	{
		char pszTemp[128];
		char pszComment[128];

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "program_association_section()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetStartEndPtr(pxmlRootNode, 0, section_size);

		BITS_TRACER_t bs_tracer;
		bs_tracer.offset = 0;
		bs_tracer.i_left = 8;

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", ppat_section->table_id, 8, "uimsbf", NULL, &bs_tracer);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", ppat_section->section_syntax_indicator, 1, "bslbf", NULL, &bs_tracer);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "'0'", ppat_section->reserved_future_use, 1, "bslbf", NULL, &bs_tracer);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppat_section->reserved0, 2, "bslbf", NULL, &bs_tracer);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", ppat_section->section_length, 12, "uimsbf", NULL, &bs_tracer);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_stream_id", ppat_section->transport_stream_id, 16, "uimsbf", NULL, &bs_tracer);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppat_section->reserved1, 2, "bslbf", NULL, &bs_tracer);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", ppat_section->version_number, 5, "uimsbf", NULL, &bs_tracer);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", ppat_section->current_next_indicator, 1, "bslbf", NULL, &bs_tracer);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", ppat_section->section_number, 8, "uimsbf", NULL, &bs_tracer);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", ppat_section->last_section_number, 8, "uimsbf", NULL, &bs_tracer);

		if (ppat_section->N > 0)
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "节目号-PMT_PID 映射循环( 共 %d项)\0", ppat_section->N);
			XMLElement* pxmlPmtLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszTemp);

			int loop_length = section_size - 8 - 4;
			XMLNODE_SetStartEndPtr(pxmlPmtLoopNode, bs_tracer.offset, loop_length);

			BITS_TRACER_t loop_bs_tracer;
			loop_bs_tracer.offset = bs_tracer.offset;
			loop_bs_tracer.i_left = 8;

			bs_tracer.offset += loop_length;
			bs_tracer.i_left = 8;

			PROGRAM_MAP_DESCRIPTION_t* pstProgram = ppat_section->astProgram;
			for (int i = 0; i < ppat_section->N; i++)		
			{
				sprintf_s(pszTemp, sizeof(pszTemp), "节目号-PMT_PID 映射[%d]\0", i);
				XMLElement* pxmlMapNode = XMLDOC_NewElementForString(pxmlDoc, pxmlPmtLoopNode, pszTemp);
				XMLNODE_SetStartEndPtr(pxmlMapNode, loop_bs_tracer.offset, 4);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, "program_number", pstProgram->program_number, 16, "uimsbf", NULL, &loop_bs_tracer);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, "reserved", pstProgram->reserved, 3, "bslbf", NULL, &loop_bs_tracer);

				if (pstProgram->program_number == 0)
				{
					XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, "network_PID", pstProgram->program_map_PID, 13, "uimsbf", NULL, &loop_bs_tracer);
					sprintf_s(pszComment, sizeof(pszComment), "program_number = %d <--> NIT_PID=0x%04X\0", pstProgram->program_number, pstProgram->program_map_PID);
				}
				else
				{
					XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, "program_map_PID", pstProgram->program_map_PID, 13, "uimsbf", NULL, &loop_bs_tracer);
					sprintf_s(pszComment, sizeof(pszComment), "program_number = %d <--> PMT_PID=0x%04X\0", pstProgram->program_number, pstProgram->program_map_PID);
				}

				XMLNODE_SetAttribute(pxmlMapNode, "comment", pszComment);

				pstProgram++;
			}
		}

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", ppat_section->CRC_32, 32, "rpchof", NULL, &bs_tracer);

		if (ppat_section->CRC_32_verify != ppat_section->CRC_32)
		{
			XMLNODE_SetAttribute(pxmlCrcNode, "error", "Error!");
			XMLNODE_SetAttribute(pxmlRootNode, "error", "CRC_32 Error!");
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	if (pPATSection == NULL)
	{
		//说明ppat_section指针临时分配，函数返回前需要释放
		delete ppat_section;
	}

	return rtcode;
}



