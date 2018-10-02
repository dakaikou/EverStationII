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
		char pszField[128];
		char pszTemp[128];
		char pszComment[128];

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "program_association_section()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, section_size);

		sprintf_s(pszComment, sizeof(pszComment), "%d字节", section_size);
		XMLNODE_SetAttribute(pxmlRootNode, "comment", pszComment);

		if (rtcode != SECTION_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", ppat_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", ppat_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "'0'", ppat_section->reserved_future_use, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppat_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", ppat_section->section_length, 12, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_stream_id", ppat_section->transport_stream_id, 16, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ppat_section->reserved1, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", ppat_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", ppat_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", ppat_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", ppat_section->last_section_number, 8, "uimsbf", NULL);

		int sync_offset = 8;
		if (ppat_section->program_map_count > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "节目映射循环( 共 %d 项)\0", ppat_section->program_map_count);
			XMLElement* pxmlPmtLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);

			int loop_length = section_size - 8 - 4;
			XMLNODE_SetFieldLength(pxmlPmtLoopNode, loop_length);

			PROGRAM_MAP_DESCRIPTION_t* pstProgram = ppat_section->astProgramMaps;
			for (int i = 0; i < ppat_section->program_map_count; i++)
			{
				if (pstProgram->program_number == 0)
				{
					sprintf_s(pszField, sizeof(pszField), "NIT_PID\0");
				}
				else
				{
					sprintf_s(pszField, sizeof(pszField), "PMT_PID\0");
				}
				sprintf_s(pszTemp, sizeof(pszTemp), "节目映射[%d](<program_number=%d, %s=0x%04X>)\0", i, pstProgram->program_number, pszField, pstProgram->program_map_PID);

				XMLElement* pxmlMapNode = XMLDOC_NewElementForString(pxmlDoc, pxmlPmtLoopNode, pszTemp, NULL);
				XMLNODE_SetFieldLength(pxmlMapNode, 4);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, "program_number", pstProgram->program_number, 16, "uimsbf", NULL);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, "reserved", pstProgram->reserved, 3, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlMapNode, pszField, pstProgram->program_map_PID, 13, "uimsbf", NULL);

				pstProgram++;
				sync_offset += 4;
			}
		}

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", ppat_section->CRC_32, 32, "rpchof", NULL);

		if (ppat_section->CRC_32_recalculated != ppat_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08x", ppat_section->CRC_32_recalculated);
			XMLNODE_SetAttribute(pxmlCrcNode, "error", pszComment);
		}
	}

	if (pPATSection == NULL)
	{
		//说明ppat_section指针临时分配，函数返回前需要释放
		delete ppat_section;
	}

	return rtcode;
}



