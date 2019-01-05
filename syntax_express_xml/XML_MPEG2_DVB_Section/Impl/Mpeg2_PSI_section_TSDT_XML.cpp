#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "../Include/Mpeg2_PSI_section_XML.h"
#include "../Include/MPEG_DVB_Common_XML.h"
#include "../Include/DVB_SI_Descriptor_XML.h"

/////////////////////////////////////////////
int MPEG2_PSI_TSDT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, TS_description_section_t* ptsdt_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (ptsdt_section != NULL))
	{
		char pszField[48];
		char pszComment[128];

		//根节点
		sprintf_s(pszField, sizeof(pszField), "TS_description_section(table_id=0x%02X)", ptsdt_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField, NULL, ptsdt_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", ptsdt_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", ptsdt_section->section_syntax_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved_future_use", ptsdt_section->reserved_future_use, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved0", ptsdt_section->reserved0, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", ptsdt_section->section_length, 12, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved1", ptsdt_section->section_length, 18, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "version_number", ptsdt_section->version_number, 5, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", ptsdt_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", ptsdt_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", ptsdt_section->last_section_number, 8, "uimsbf", NULL);

		int loop_length = ptsdt_section->section_length - 9;
		if (loop_length > 0)
		{
			uint16_t descriptor_tag;
			int		 descriptor_length;
			uint8_t* descriptor_buf;
			int		 descriptor_size;

			sprintf_s(pszField, sizeof(pszField), "TS_description()");
			XMLElement* pxmlDescriptionNode = pxmlDoc->NewBranchElement(pxmlRootNode, pszField, NULL, loop_length);

			for (int descriptor_index = 0; descriptor_index < ptsdt_section->TS_descriptor_count; descriptor_index++)
			{
				descriptor_buf = ptsdt_section->TS_descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = ptsdt_section->TS_descriptors[descriptor_index].descriptor_tag;
				descriptor_length = ptsdt_section->TS_descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
				case DVB_SI_TRANSPORT_STREAM_DESCRIPTOR:
					DVB_SI_decode_transport_stream_descriptor_to_xml(descriptor_buf, descriptor_size, pxmlDoc, pxmlDescriptionNode);
					break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlDescriptionNode, ptsdt_section->TS_descriptors + descriptor_index);
					break;
				}
			}
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, "CRC_32", ptsdt_section->CRC_32, 32, "rpchof", NULL);

		if (ptsdt_section->CRC_32_recalculated != ptsdt_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08x", ptsdt_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}


int MPEG2_PSI_TSDT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, TALForXMLDoc* pxmlDoc, TS_description_section_t* pTSDTSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	TS_description_section_t* ptsdt_section = (pTSDTSection != NULL) ? pTSDTSection : new TS_description_section_t;
	rtcode = MPEG2_PSI_TSDT_DecodeSection(section_buf, section_size, ptsdt_section);
	rtcode = MPEG2_PSI_TSDT_PresentSection_to_XML(pxmlDoc, ptsdt_section);

	if (pTSDTSection == NULL)
	{
		//说明ptsdt_section指针临时分配，函数返回前需要释放
		delete ptsdt_section;
	}

	return rtcode;
}


