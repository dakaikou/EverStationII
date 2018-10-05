#include <string.h>
#include <assert.h>

#include "../nativeInclude/DVB_SI_section.h"
#include "../nativeInclude/DVB_table_id.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"
#include "../nativeInclude/DVB_SI_Utilities.h"
#include "../xmlInclude/DVB_SI_section_XML.h"

/////////////////////////////////////////////
int DVB_SI_TDT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, time_date_section_t* ptdt_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[64];
	char	pszComment[64];

	if ((pxmlDoc != NULL) && (ptdt_section != NULL))
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//���ڵ�
		sprintf_s(pszField, sizeof(pszField), "time_date_section(table_id=0x%02X)", ptdt_section->table_id);
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, pszField);
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, ptdt_section->section_length + 3);

		//if (rtcode != SECTION_PARSE_NO_ERROR)
		//{
		//	sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
		//	XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		//}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", ptdt_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", ptdt_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved_future_use", ptdt_section->reserved_future_use, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "reserved", ptdt_section->reserved, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_length", ptdt_section->section_length, 12, "uimsbf", NULL);

		DVB_SI_NumericCoding2Text_UTCTime(ptdt_section->UTC_time, pszComment, sizeof(pszComment));
		XMLDOC_NewElementForX64Bits(pxmlDoc, pxmlRootNode, "UTC_time", ptdt_section->UTC_time, 40, "bslbf", pszComment);
	}

	return rtcode;
}

int DVB_SI_TDT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, time_date_section_t* pTDTSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	time_date_section_t* ptdt_section = (pTDTSection == NULL) ? new time_date_section_t : pTDTSection;
	rtcode = DVB_SI_TDT_DecodeSection(section_buf, section_size, ptdt_section);
	rtcode = DVB_SI_TDT_PresentSection_to_XML(pxmlDoc, ptdt_section);

	if (pTDTSection == NULL)
	{
		//˵��ptdt_sectionָ����ʱ���䣬��������ǰ��Ҫ�ͷ�
		delete ptdt_section;
	}

	return rtcode;
}