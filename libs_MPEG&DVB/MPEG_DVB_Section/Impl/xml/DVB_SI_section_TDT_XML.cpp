#include <string.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/xml/DVB_SI_section_XML.h"

/////////////////////////////////////////////
int DVB_SI_TDT_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, time_date_section_t* pTDTSection)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszTemp[64];
	BITS_t  bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* pxmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(pxmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("time_date_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", section_size);
		pxmlRootNode->SetAttribute("comment", pszTemp);

		if ((section_buf != NULL) && (section_size >= DVB_SI_TDT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_TDT_SECTION_MAX_SIZE))
		{
			time_date_section_t* ptdt_section = (pTDTSection == NULL) ? new time_date_section_t : pTDTSection;
			memset(ptdt_section, 0x00, sizeof(time_date_section_t));

			BITS_map(&bs, section_buf, section_size);

			ptdt_section->table_id = BITS_get(&bs, 8);
			assert(ptdt_section->table_id == TABLE_ID_TDT);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", ptdt_section->table_id, 8, "uimsbf", NULL, &bs);

			ptdt_section->section_syntax_indicator = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", ptdt_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);
			ptdt_section->reserved_future_use = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved_future_use", ptdt_section->reserved_future_use, 1, "bslbf", NULL, &bs);
			ptdt_section->reserved = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved", ptdt_section->reserved, 2, "bslbf", NULL, &bs);

			ptdt_section->section_length = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", ptdt_section->section_length, 12, "uimsbf", NULL, &bs);

			ptdt_section->UTC_time = BITS_x64_get(&bs, 40);
			DVB_SI_NumericCoding2Text_UTCTime(ptdt_section->UTC_time, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElementX64Mode(pxmlRootNode, "UTC_time", ptdt_section->UTC_time, 40, "bslbf", pszTemp, &bs);

			if (pTDTSection == NULL)
			{
				//说明ptdt_section指针临时分配，函数返回前需要释放
				delete ptdt_section;
			}
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section buffer parameters error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
