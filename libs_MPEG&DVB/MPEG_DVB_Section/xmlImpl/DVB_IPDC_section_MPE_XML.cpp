#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../nativeInclude/DVB_IPDC_section.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"
#include "../xmlInclude/DVB_IPDC_section_XML.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_IPDC_MPE_DecodeSection_to_XML(uint8_t *section_buf, int section_size, XMLDocForMpegSyntax* pxmlDoc, datagram_section_t* pMPESection)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;
	int  loop_length;
	char pszTemp[64];
	BITS_t bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(section_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("MPE_datagram_section()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, section_buf, section_buf + section_size);

		if ((section_buf != NULL) && (section_size >= DVB_IPDC_MPE_SECTION_MIN_SIZE) && (section_size <= DVB_IPDC_MPE_SECTION_MAX_SIZE))
		{
			unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
			unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

			datagram_section_t* pmpe_section = (pMPESection != NULL) ? pMPESection : new datagram_section_t;
			memset(pmpe_section, 0x00, sizeof(datagram_section_t));

			BITS_map(&bs, section_buf, section_size);

			pmpe_section->table_id = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "table_id", pmpe_section->table_id, 8, "uimsbf", NULL, &bs);

			pmpe_section->section_syntax_indicator = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_syntax_indicator", pmpe_section->section_syntax_indicator, 1, "bslbf", NULL, &bs);

			pmpe_section->private_indicator = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "private_indicator", pmpe_section->private_indicator, 1, "bslbf", NULL, &bs);

			pmpe_section->reserved0 = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved0", pmpe_section->reserved0, 2, "bslbf", NULL, &bs);

			pmpe_section->section_length = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_length", pmpe_section->section_length, 12, "uimsbf", NULL, &bs);

			pmpe_section->MAC_address_6 = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "MAC_address_6", pmpe_section->MAC_address_6, 8, "uimsbf", NULL, &bs);

			pmpe_section->MAC_address_5 = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "MAC_address_5", pmpe_section->MAC_address_5, 8, "uimsbf", NULL, &bs);

			pmpe_section->reserved1 = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "reserved1", pmpe_section->reserved1, 2, "bslbf", NULL, &bs);

			pmpe_section->payload_scrambling_control = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "payload_scrambling_control", pmpe_section->payload_scrambling_control, 2, "bslbf", NULL, &bs);

			pmpe_section->address_scrambling_control = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "address_scrambling_control", pmpe_section->address_scrambling_control, 2, "bslbf", NULL, &bs);

			pmpe_section->LLC_SNAP_flag = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "LLC_SNAP_flag", pmpe_section->LLC_SNAP_flag, 1, "bslbf", NULL, &bs);

			pmpe_section->current_next_indicator = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "current_next_indicator", pmpe_section->current_next_indicator, 1, "bslbf", NULL, &bs);

			pmpe_section->section_number = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "section_number", pmpe_section->section_number, 8, "uimsbf", NULL, &bs);

			pmpe_section->last_section_number = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "last_section_number", pmpe_section->last_section_number, 8, "uimsbf", NULL, &bs);

			pmpe_section->MAC_address_4 = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "MAC_address_4", pmpe_section->MAC_address_4, 8, "uimsbf", NULL, &bs);

			pmpe_section->MAC_address_3 = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "MAC_address_3", pmpe_section->MAC_address_3, 8, "uimsbf", NULL, &bs);

			pmpe_section->MAC_address_2 = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "MAC_address_2", pmpe_section->MAC_address_2, 8, "uimsbf", NULL, &bs);

			pmpe_section->MAC_address_1 = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "MAC_address_1", pmpe_section->MAC_address_1, 8, "uimsbf", NULL, &bs);

			loop_length = pmpe_section->section_length - 9 - 4;			//exclude CRC32
			BITS_byteSkip(&bs, loop_length);

			if (pmpe_section->LLC_SNAP_flag == 1)
			{
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "LLC_SNAP()", -1, -1, NULL, NULL, &bs);
			}
			else
			{
				pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "IP_datagram_data_byte()", -1, -1, NULL, NULL, &bs);
				//get the IP data
				//FILE* fp = NULL;
				//fopen_s(&fp, "e:\\ip_data.dat", "wb");
				//if (fp != NULL)
				//{
				//	fwrite(ptemp, 1, loop_length, fp);
				//	fclose(fp);
				//}
			}

			pmpe_section->checksum = 0x00000000;
			pmpe_section->CRC_32 = 0x00000000;

			if (pmpe_section->section_syntax_indicator == 0)
			{
				pmpe_section->checksum = BITS_get(&bs, 32);
			}
			else
			{
				pmpe_section->CRC_32 = BITS_get(&bs, 32);
			}

			if (pMPESection == NULL)
			{
				//说明ppat_section指针临时分配，函数返回前需要释放
				delete pmpe_section;
			}
		}
		else
		{
			sprintf_s(pszTemp, sizeof(pszTemp), "section buffer parameters error! section_size = %d", section_size);
			pxmlRootNode->SetAttribute("error", pszTemp);
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
