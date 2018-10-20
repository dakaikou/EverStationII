#include <string.h>
#include <stdlib.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/DVB_SI_section_XML.h"

int DVB_SI_CMT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, CA_message_section_t* pcmt_section)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;
	char pszField[128];
	char pszComment[128];

	if ((pxmlDoc != NULL) && (pcmt_section != NULL))
	{
		//���ڵ�
		sprintf_s(pszField, sizeof(pszField), "CA_message_section(table_id=0x%02X)", pcmt_section->table_id);
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement(pszField);
		pxmlDoc->SetAnchor(pxmlRootNode);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "table_id", pcmt_section->table_id, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "section_syntax_indicator", pcmt_section->section_syntax_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "DVB_reserved", pcmt_section->DVB_reserved, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "ISO_reserved", pcmt_section->ISO_reserved, 2, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "CA_section_length", pcmt_section->CA_section_length, 12, "uimsbf", NULL);

		if (pcmt_section->CA_section_length > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlRootNode, "CA_data_byte[ ]", pcmt_section->CA_data_byte, pcmt_section->CA_section_length, NULL);
		}

		pxmlDoc->ClearAnchor(pxmlRootNode);
	}

	return rtcode;
}

int DVB_SI_CMT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, CA_message_section_t* pCMTSection)
{
	int  rtcode = SECTION_PARSE_NO_ERROR;

	CA_message_section_t* pcmt_section = (pCMTSection != NULL) ? pCMTSection : new CA_message_section_t;
	rtcode = DVB_SI_CMT_DecodeSection(section_buf, section_size, pcmt_section);
	rtcode = DVB_SI_CMT_PresentSection_to_XML(pxmlDoc, pcmt_section);

	if (pCMTSection == NULL)
	{
		//˵��ppat_sectionָ����ʱ���䣬��������ǰ��Ҫ�ͷ�
		delete pcmt_section;
	}

	return rtcode;
}

