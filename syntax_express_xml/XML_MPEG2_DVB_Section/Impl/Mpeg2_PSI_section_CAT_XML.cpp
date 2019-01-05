#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "../Include/MPEG_DVB_Common_XML.h"
#include "../Include/Mpeg2_PSI_section_XML.h"
#include "../Include/Mpeg2_PSI_Descriptor_XML.h"

int MPEG2_PSI_CAT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, CA_section_t* pcat_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[128];
	char	pszComment[128];

	if ((pxmlDoc != NULL) && (pcat_section != NULL))
	{
		//根节点
		sprintf_s(pszField, sizeof(pszField), "CA_section(table_id=0x%02X)", pcat_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, pcat_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", pcat_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", pcat_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "'0'", pcat_section->reserved_future_use, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pcat_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", pcat_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved", pcat_section->reserved1, 18, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", pcat_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", pcat_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", pcat_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", pcat_section->last_section_number, 8, "uimsbf", NULL);

		int ca_loop_length = pcat_section->section_length - 9;
		if (ca_loop_length > 0)
		{
			uint16_t descriptor_tag;
			int		 descriptor_length;
			uint8_t* descriptor_buf;
			int		 descriptor_size;

			sprintf_s(pszField, sizeof(pszField), "CA_description()");
			XMLElement* pxmlCADescriptorsLoopNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, ca_loop_length);

			for (int descriptor_index = 0; descriptor_index < pcat_section->CA_descriptor_count; descriptor_index++)
			{
				descriptor_buf = pcat_section->CA_descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = pcat_section->CA_descriptors[descriptor_index].descriptor_tag;
				descriptor_length = pcat_section->CA_descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
				case MPEG2_PSI_CA_DESCRIPTOR:
					MPEG2_PSI_decode_CA_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlCADescriptorsLoopNode);
					break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlCADescriptorsLoopNode, pcat_section->CA_descriptors + descriptor_index);
					break;
				}
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", pcat_section->CRC_32, 32, "rpchof", NULL);
		if (pcat_section->CRC_32_recalculated != pcat_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08x", pcat_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}

int MPEG2_PSI_CAT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, TALForXMLDoc* pxmlDoc, CA_section_t* pCATSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	CA_section_t* pcat_section = (pCATSection == NULL) ? new CA_section_t : pCATSection;
	rtcode = MPEG2_PSI_CAT_DecodeSection(section_buf, section_size, pcat_section);
	rtcode = MPEG2_PSI_CAT_PresentSection_to_XML(pxmlDoc, pcat_section);

	if (pCATSection == NULL)
	{
		//说明pcat_section指针临时分配，函数返回前需要释放
		delete pcat_section;
	}

	return rtcode;
}



