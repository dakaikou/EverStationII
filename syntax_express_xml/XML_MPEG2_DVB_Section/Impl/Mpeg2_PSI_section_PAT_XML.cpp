#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "../Include/Mpeg2_PSI_section_XML.h"

int MPEG2_PSI_PAT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, program_association_section_t* ppat_section)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (ppat_section != NULL))
	{
		char pszField[128];
		char pszPidStyle[16];
		char pszComment[128];

		//根节点
		sprintf_s(pszField, sizeof(pszField), "program_association_section(table_id=0x%02X)", ppat_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, ppat_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", ppat_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", ppat_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "'0'", ppat_section->reserved_future_use, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", ppat_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", ppat_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_stream_id", ppat_section->transport_stream_id, 16, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", ppat_section->reserved1, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", ppat_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", ppat_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", ppat_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", ppat_section->last_section_number, 8, "uimsbf", NULL);

		int program_map_length = ppat_section->section_length - 5 - 4;
		if (program_map_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "节目映射循环(共 %d 项)\0", ppat_section->program_map_count);
			XMLElement* pxmlPmtLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, program_map_length);

			for (int map_index = 0; map_index < ppat_section->program_map_count; map_index++)
			{
				PROGRAM_MAP_DESCRIPTION_t* pstProgram = ppat_section->astProgramMaps + map_index;

				if (pstProgram->program_number == 0)
				{
					sprintf_s(pszPidStyle, sizeof(pszPidStyle), "NIT_PID\0");
				}
				else
				{
					sprintf_s(pszPidStyle, sizeof(pszPidStyle), "PMT_PID\0");
				}
				sprintf_s(pszField, sizeof(pszField), "节目映射[%d](<program_number=%d, %s=0x%04X>)\0", map_index, pstProgram->program_number, pszPidStyle, pstProgram->program_map_PID);

				XMLElement* pxmlMapNode = pxmlDoc->NewBranchElement(pxmlPmtLoopNode, pszField, NULL, 4);

				pxmlDoc->NewElementForBits(pxmlMapNode, "program_number", pstProgram->program_number, 16, "uimsbf", NULL);

				pxmlDoc->NewElementForBits(pxmlMapNode, "reserved", pstProgram->reserved, 3, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlMapNode, pszPidStyle, pstProgram->program_map_PID, 13, "uimsbf", NULL);
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", ppat_section->CRC_32, 32, "rpchof", NULL);
		if (ppat_section->CRC_32_recalculated != ppat_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08x", ppat_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_PSI_PAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, TALForXMLDoc* pxmlDoc, program_association_section_t* pPATSection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	program_association_section_t* ppat_section = (pPATSection != NULL) ? pPATSection : new program_association_section_t;
	rtcode = MPEG2_PSI_PAT_DecodeSection(section_buf, section_size, ppat_section);
	rtcode = MPEG2_PSI_PAT_PresentSection_to_XML(pxmlDoc, ppat_section);

	if (pPATSection == NULL)
	{
		//说明ppat_section指针临时分配，函数返回前需要释放
		delete ppat_section;
	}

	return rtcode;
}



