#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/MPEG2_table_id.h"
#include "../../Include/Mpeg2_PSI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/Mpeg2_PSI_section_XML.h"
#include "../../Include/xml/MPEG_DVB_Common_XML.h"
#include "../../Include/xml/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int MPEG2_PSI_TSDT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, TS_description_section_t* ptsdt_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (ptsdt_section != NULL))
	{
		char pszField[48];
		char pszComment[128];

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* xmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, xmlDeclaration);

		//根节点
		sprintf_s(pszField, sizeof(pszField), "TS_description_section(table_id=0x%02X)", ptsdt_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, ptsdt_section->section_length + 3);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", ptsdt_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", ptsdt_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", ptsdt_section->reserved_future_use, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved0", ptsdt_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", ptsdt_section->section_length, 12, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved1", ptsdt_section->section_length, 18, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "version_number", ptsdt_section->version_number, 5, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "current_next_indicator", ptsdt_section->current_next_indicator, 1, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_number", ptsdt_section->section_number, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "last_section_number", ptsdt_section->last_section_number, 8, "uimsbf", NULL);

		int loop_length = ptsdt_section->section_length - 9;
		if (loop_length > 0)
		{
			uint16_t descriptor_tag;
			int		 descriptor_length;
			uint8_t* descriptor_buf;
			int		 descriptor_size;

			sprintf_s(pszField, sizeof(pszField), "TS_description()");
			XMLElement* pxmlDescriptionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlDescriptionNode, loop_length);

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

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", ptsdt_section->CRC_32, 32, "rpchof", NULL);

		if (ptsdt_section->CRC_32_recalculated != ptsdt_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08x", ptsdt_section->CRC_32_recalculated);
			XMLNODE_SetAttribute(pxmlCrcNode, "error", pszComment);
		}
	}

	return rtcode;
}


int MPEG2_PSI_TSDT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, TS_description_section_t* pTSDTSection)
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


