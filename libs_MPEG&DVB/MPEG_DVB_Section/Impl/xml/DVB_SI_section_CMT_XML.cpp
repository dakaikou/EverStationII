#include <string.h>
#include <stdlib.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/DVB_SI_section_XML.h"

//#include "libs_Math/Include/CRC_32.h"
//
//#ifndef min
//#define min(a,b)  (((a)<(b))?(a):(b))
//#endif

int DVB_SI_CMT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, CA_message_section_t* pCMTSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char pszComment[128];

	CA_message_section_t* pcmt_section = (pCMTSection != NULL) ? pCMTSection : new CA_message_section_t;
	rtcode = DVB_SI_CMT_DecodeSection(section_buf, section_size, pcmt_section);

	if (pxmlDoc != NULL)
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* xmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, xmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "CA_message_section()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, section_size);

		sprintf_s(pszComment, sizeof(pszComment), "%d字节", section_size);
		XMLNODE_SetAttribute(pxmlRootNode, "comment", pszComment);

		if (rtcode != SECTION_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pcmt_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pcmt_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "DVB_reserved", pcmt_section->DVB_reserved, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "ISO_reserved", pcmt_section->ISO_reserved, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CA_section_length", pcmt_section->CA_section_length, 12, "uimsbf", NULL);

		if (pcmt_section->CA_section_length > 0)
		{
			XMLDOC_NewElementForBytes(pxmlDoc, pxmlRootNode, "CA_data_byte[ ]", pcmt_section->CA_data_byte, pcmt_section->CA_section_length, NULL);
		}
	}

	if (pCMTSection == NULL)
	{
		//说明ppat_section指针临时分配，函数返回前需要释放
		delete pcmt_section;
	}

	return rtcode;
}

