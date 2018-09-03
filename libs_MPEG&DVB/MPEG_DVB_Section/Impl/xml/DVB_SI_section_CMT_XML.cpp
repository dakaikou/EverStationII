#include <string.h>
#include <stdlib.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/xml/DVB_SI_section_XML.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int DVB_SI_CMT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, CA_message_section_t* pCMTSection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	int		copy_length;
	BITS_t	bs;
	//char	pszTemp[64];

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("CA_message_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= DVB_SI_CMT_SECTION_MIN_SIZE))
		{
			CA_message_section_t* pcmt_section = (pCMTSection != NULL) ? pCMTSection : new CA_message_section_t;
			memset(pcmt_section, 0x00, sizeof(CA_message_section_t));

			BITS_map(&bs, section_buf, section_size);

			pcmt_section->table_id = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pcmt_section->table_id, 8, "uimsbf", NULL, &bs);

			pcmt_section->section_syntax_indicator = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pcmt_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

			pcmt_section->DVB_reserved = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "DVB_reserved", pcmt_section->DVB_reserved, 1, "bslbf", NULL, &bs);

			pcmt_section->ISO_reserved = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "ISO_reserved", pcmt_section->ISO_reserved, 2, "bslbf", NULL, &bs);

			pcmt_section->CA_section_length = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CA_section_length", pcmt_section->CA_section_length, 12, "uimsbf", NULL, &bs);

			if (pcmt_section->CA_section_length > 0)
			{
				copy_length = min(pcmt_section->CA_section_length, 64);
				memcpy(pcmt_section->CA_data_byte, bs.p_cur, copy_length);
				BITS_byteSkip(&bs, pcmt_section->CA_section_length);

				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "CA_data_byte[ ]", pcmt_section->CA_data_byte, pcmt_section->CA_section_length, NULL, &bs);
			}

			if (pCMTSection == NULL)
			{
				//说明ppat_section指针临时分配，函数返回前需要释放
				delete pcmt_section;
			}
		}
		else
		{
			pxmlRootNode->SetAttribute("error", "section buffer parameters error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

