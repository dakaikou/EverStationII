#include <string.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_Utilities.h"
#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/DVB_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"
#include "../xmlInclude/MPEG_DVB_Common_XML.h"
#include "../xmlInclude/DVB_SI_section_XML.h"
#include "../xmlInclude/DVB_SI_Descriptor_XML.h"

#include "libs_Math/Include/CRC_32.h"

int DVB_SI_TOT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, time_offset_section_t* ptot_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	uint16_t descriptor_tag;
	int		 descriptor_length;
	uint8_t* descriptor_buf;
	int		 descriptor_size;

	char	 pszField[128];
	char	 pszComment[128];

	if ((pxmlDoc != NULL) && (ptot_section != NULL))
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根节点
		sprintf_s(pszField, sizeof(pszField), "time_offset_section(table_id=0x%02X)", ptot_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, ptot_section->section_length + 3);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", ptot_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", ptot_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", ptot_section->reserved_future_use, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ptot_section->reserved0, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", ptot_section->section_length, 12, "uimsbf", NULL);

		DVB_SI_NumericCoding2Text_UTCTime(ptot_section->UTC_time, pszComment, sizeof(pszComment));
		XMLDOC_NewElementForX64Bits(pxmlDoc, pxmlRootNode, "UTC_time", ptot_section->UTC_time, 40, "bslbf", pszComment);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved1", ptot_section->reserved1, 4, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "descriptors_loop_length", ptot_section->descriptors_loop_length, 12, "uimsbf", NULL);

		if (ptot_section->descriptors_loop_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "descriptors_loop()");
			XMLElement* pxmlDescriptorsLoopNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, pszField, NULL);
			XMLNODE_SetFieldLength(pxmlDescriptorsLoopNode, ptot_section->descriptors_loop_length);

			for (int descriptor_index = 0; descriptor_index < ptot_section->descriptor_count; descriptor_index++)
			{
				descriptor_buf = ptot_section->descriptors[descriptor_index].descriptor_buf;
				descriptor_tag = ptot_section->descriptors[descriptor_index].descriptor_tag;
				descriptor_length = ptot_section->descriptors[descriptor_index].descriptor_length;
				descriptor_size = descriptor_length + 2;

				switch (descriptor_tag)
				{
					//case DVB_SI_LOCAL_TIME_OFFSET_DESCRIPTOR:
					//	DVB_SI_decode_local_time_offset_descriptor_to_xml(ptemp, move_length, pxmlDoc, pxmlDescriptorLoopNode);
					//	break;
				default:
					MPEG_DVB_present_reserved_descriptor_to_xml(pxmlDoc, pxmlDescriptorsLoopNode, ptot_section->descriptors + descriptor_index);
					break;
				}
			}
		}

		XMLElement* pxmlCrcNode = XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CRC_32", ptot_section->CRC_32, 32, "rpchof", NULL);
		if (ptot_section->CRC_32_recalculated != ptot_section->CRC_32)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", ptot_section->CRC_32_recalculated);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}

	return rtcode;
}

int DVB_SI_TOT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, time_offset_section_t* pTOTSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	time_offset_section_t* ptot_section = (pTOTSection != NULL) ? pTOTSection : new time_offset_section_t;
	rtcode = DVB_SI_TOT_DecodeSection(section_buf, section_size, ptot_section);
	rtcode = DVB_SI_TOT_PresentSection_to_XML(pxmlDoc, ptot_section);

	if (pTOTSection == NULL)
	{
		//说明上面的ptot_section指针为临时分配，函数返回前需要释放
		delete ptot_section;
	}

	return rtcode;
}

