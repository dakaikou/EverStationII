#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../../Include/DVB_IPDC_section.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/DVB_IPDC_section_XML.h"

/////////////////////////////////////////////
int DVB_IPDC_MPE_DecodeSection_to_XML(uint8_t *section_buf, int section_size, HALForXMLDoc* pxmlDoc, datagram_section_t* pMPESection)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	datagram_section_t* pmpe_section = (pMPESection != NULL) ? pMPESection : new datagram_section_t;
	rtcode = DVB_IPDC_MPE_DecodeSection(section_buf, section_size, pmpe_section);
	rtcode = DVB_IPDC_MPE_PresentSection_to_XML(pxmlDoc, pmpe_section);

	if (pMPESection == NULL)
	{
		//说明pmpe_section指针临时分配，函数返回前需要释放
		delete pmpe_section;
	}

	return rtcode;
}

int DVB_IPDC_MPE_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, datagram_section_t* pmpe_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;
	char pszComment[128];
	char pszField[64];
	char pszMnemonic[16];

	if ((pxmlDoc != NULL) && (pmpe_section != NULL))
	{
		//根节点
		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement("MPE_datagram_section()", NULL, pmpe_section->section_length + 3);

		pxmlDoc->NewElementForBits(pxmlRootNode, "table_id", pmpe_section->table_id, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_syntax_indicator", pmpe_section->section_syntax_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "private_indicator", pmpe_section->private_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved0", pmpe_section->reserved0, 2, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_length", pmpe_section->section_length, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "MAC_address_6", pmpe_section->MAC_address_6, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "MAC_address_5", pmpe_section->MAC_address_5, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "reserved1", pmpe_section->reserved1, 2, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "payload_scrambling_control", pmpe_section->payload_scrambling_control, 2, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "address_scrambling_control", pmpe_section->address_scrambling_control, 2, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "LLC_SNAP_flag", pmpe_section->LLC_SNAP_flag, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "current_next_indicator", pmpe_section->current_next_indicator, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "section_number", pmpe_section->section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "last_section_number", pmpe_section->last_section_number, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "MAC_address_4", pmpe_section->MAC_address_4, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "MAC_address_3", pmpe_section->MAC_address_3, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "MAC_address_2", pmpe_section->MAC_address_2, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "MAC_address_1", pmpe_section->MAC_address_1, 8, "uimsbf", NULL);

		if (pmpe_section->LLC_SNAP_flag == 1)
		{
			pxmlDoc->NewElementForByteBuf(pxmlRootNode, "LLC_SNAP[ ]", pmpe_section->LLC_SNAP_data_byte, pmpe_section->LLC_SNAP_data_length, NULL);
		}
		else
		{
			pxmlDoc->NewElementForByteBuf(pxmlRootNode, "IP_datagram_data_byte[ ]", pmpe_section->IP_datagram_data_byte, pmpe_section->IP_datagram_data_length, NULL);
		}

		if (pmpe_section->section_syntax_indicator == 1)
		{
			sprintf_s(pszField, sizeof(pszField), "CRC_32");
			sprintf_s(pszMnemonic, sizeof(pszMnemonic), "rpchof");
		}
		else
		{
			sprintf_s(pszField, sizeof(pszField), "checksum");
			sprintf_s(pszMnemonic, sizeof(pszMnemonic), "uimsbf");
		}

		XMLElement* pxmlCrcNode = pxmlDoc->NewElementForBits(pxmlRootNode, pszField, pmpe_section->encodedCheckValue, 32, pszMnemonic, NULL);
		if (pmpe_section->recalculatedCheckValue != pmpe_section->encodedCheckValue)
		{
			sprintf_s(pszComment, sizeof(pszComment), "Should be 0x%08X", pmpe_section->recalculatedCheckValue);
			pxmlCrcNode->SetAttribute("error", pszComment);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
