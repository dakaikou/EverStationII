#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../../Include/MPEG_DVB_Common.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int report_descriptor_length_error_to_xml(uint8_t *buf, int length, int max_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszTemp[64];
	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "error descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + min(length, max_length));

		BITS_map(&bs, buf, length);

		uint8_t descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", descriptor_tag, 8, "uimsbf", NULL, &bs);

		uint8_t descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length",descriptor_length, 8, "uimsbf", NULL, &bs);

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		sprintf_s(pszTemp, sizeof(pszTemp), "Error! descriptor_length field could not exceed: 0x%02X(%d)字节", max_length - 2, max_length - 2);
		pxmlDescriptorNode->SetAttribute("error", pszTemp);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

int decode_reserved_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, reserved_descriptor_t* pReservedDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszTemp[64];
	BITS_t bs;

	if ((pxmlDoc != NULL) && (buf != NULL) && (length >= 2))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unknown descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		reserved_descriptor_t* preserved_descriptor = (pReservedDescriptor == NULL) ? new reserved_descriptor_t : pReservedDescriptor;
		memset(preserved_descriptor, 0x00, sizeof(reserved_descriptor_t));

		BITS_map(&bs, buf, length);

		preserved_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", preserved_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		preserved_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", preserved_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		if (preserved_descriptor->descriptor_length > 0)
		{
			uint8_t* ptemp = bs.p_cur;
			BITS_byteSkip(&bs, preserved_descriptor->descriptor_length);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_payload_buf[ ]", ptemp, preserved_descriptor->descriptor_length, NULL, &bs);
		}

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", preserved_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (pReservedDescriptor == NULL)
		{
			delete preserved_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

