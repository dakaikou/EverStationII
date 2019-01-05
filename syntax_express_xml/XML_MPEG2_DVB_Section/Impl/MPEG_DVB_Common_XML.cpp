#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_Common.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "../Include/MPEG_DVB_Common_XML.h"

//int report_descriptor_length_error_to_xml(uint8_t *buf, int length, int max_length, TALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
//{
//	int		rtcode = SECTION_PARSE_NO_ERROR;
//	char   pszTemp[64];
//	BITS_t bs;
//
//	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL))
//	{
//		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "error descriptor()");
//		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + min(length, max_length));
//
//		BITS_map(&bs, buf, length);
//
//		uint8_t descriptor_tag = BITS_get(&bs, 8);
//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", descriptor_tag, 8, "uimsbf", NULL, &bs);
//
//		uint8_t descriptor_length = BITS_get(&bs, 8);
//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length",descriptor_length, 8, "uimsbf", NULL, &bs);
//
//		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", descriptor_tag, length);
//		pxmlDescriptorNode->SetAttribute("comment", pszTemp);
//
//		sprintf_s(pszTemp, sizeof(pszTemp), "Error! descriptor_length field could not exceed: 0x%02X(%d)字节", max_length - 2, max_length - 2);
//		pxmlDescriptorNode->SetAttribute("error", pszTemp);
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
//	}
//
//	return rtcode;
//}

//int present_reserved_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, reserved_descriptor_t* pReservedDescriptor)
//{
//	int		rtcode = SECTION_PARSE_NO_ERROR;
//	char   pszTemp[64];
//	BITS_t bs;
//
//	if ((pxmlDoc != NULL) && (buf != NULL) && (length >= 2))
//	{
//		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unknown descriptor()");
//		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);
//
//		reserved_descriptor_t* preserved_descriptor = (pReservedDescriptor == NULL) ? new reserved_descriptor_t : pReservedDescriptor;
//		memset(preserved_descriptor, 0x00, sizeof(reserved_descriptor_t));
//
//		BITS_map(&bs, buf, length);
//
//		preserved_descriptor->descriptor_tag = BITS_get(&bs, 8);
//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", preserved_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);
//
//		preserved_descriptor->descriptor_length = BITS_get(&bs, 8);
//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", preserved_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);
//
//		if (preserved_descriptor->descriptor_length > 0)
//		{
//			uint8_t* ptemp = bs.p_cur;
//			BITS_byteSkip(&bs, preserved_descriptor->descriptor_length);
//			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_payload_buf[ ]", ptemp, preserved_descriptor->descriptor_length, NULL, &bs);
//		}
//
//		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", preserved_descriptor->descriptor_tag, length);
//		pxmlDescriptorNode->SetAttribute("comment", pszTemp);
//
//		if (pReservedDescriptor == NULL)
//		{
//			delete preserved_descriptor;
//		}
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
//	}
//
//	return rtcode;
//}

int MPEG_DVB_present_reserved_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, reserved_descriptor_t* preserved_descriptor)
{
	int	   rtcode = SECTION_PARSE_NO_ERROR;
	char   pszFiled[64];

	if ((pxmlDoc != NULL) && (preserved_descriptor != NULL))
	{
		uint8_t descriptor_tag = (preserved_descriptor->descriptor_tag & 0x00FF);

		sprintf_s(pszFiled, sizeof(pszFiled), "unknown descriptor(tag: 0x%02X)", descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszFiled, NULL, preserved_descriptor->descriptor_size);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", descriptor_tag, 8, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", preserved_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (preserved_descriptor->descriptor_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "descriptor_payload_buf[ ]", preserved_descriptor->descriptor_buf + 2, preserved_descriptor->descriptor_length, NULL);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}
