#include <string.h>
#include <stdlib.h>
#include <assert.h>

//#include "../../Include/Mpeg2_DSMCC_Utilities.h"
#include "../../Include/Mpeg2_DSMCC_Descriptor.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//DSM-CC  0x02
int MPEG2_DSMCC_decode_name_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, name_descriptor_t* pNameDescriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	char		pszTemp[64];
	BITS_t		bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		if ((buf != NULL) && (length >= 2))
		{
			tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "name_descriptor()");
			pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

			name_descriptor_t* pname_descriptor = (pNameDescriptor == NULL) ? new name_descriptor_t : pNameDescriptor;
			memset(pname_descriptor, 0x00, sizeof(name_descriptor_t));

			BITS_map(&bs, buf, length);

			pname_descriptor->descriptor_tag = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pname_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

			pname_descriptor->descriptor_length = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pname_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

			if (pname_descriptor->descriptor_length > 0)
			{
				int copy_length = min(sizeof(pname_descriptor->text_char) -1, pname_descriptor->descriptor_length);
				memcpy(pname_descriptor->text_char, bs.p_cur, copy_length);
				BITS_byteSkip(&bs, copy_length);

				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "text_char[ ]", (uint8_t*)pname_descriptor->text_char, copy_length, pname_descriptor->text_char, &bs);
			}

			sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d×Ö½Ú", pname_descriptor->descriptor_tag, length);
			pxmlDescriptorNode->SetAttribute("comment", pszTemp);

			if (pNameDescriptor == NULL)
			{
				delete pname_descriptor;
			}
		}
		else
		{
			pxmlParentNode->SetAttribute("error", "parameter error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//DSM-CC  0x06
int MPEG2_DSMCC_decode_location_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, location_descriptor_t* pLocationDescriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	char		pszTemp[64];
	BITS_t		bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		if ((buf != NULL) && (length > 2))
		{
			tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "location_descriptor()");
			pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

			location_descriptor_t* plocation_descriptor = (pLocationDescriptor == NULL) ? new location_descriptor_t : pLocationDescriptor;
			memset(plocation_descriptor, 0x00, sizeof(location_descriptor_t));

			BITS_map(&bs, buf, length);

			plocation_descriptor->descriptor_tag = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", plocation_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

			plocation_descriptor->descriptor_length = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", plocation_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

			if (plocation_descriptor->descriptor_length == 1)
			{
				plocation_descriptor->location_tag = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "location_tag", plocation_descriptor->location_tag, 8, "uimsbf", NULL, &bs);
			}
			else
			{
				uint8_t* error_ptr = bs.p_cur;
				BITS_byteSkip(&bs, plocation_descriptor->descriptor_length - 2);

				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "byte_buff[ ]", (uint8_t*)error_ptr, plocation_descriptor->descriptor_length - 2, NULL, &bs);
				pxmlDescriptorNode->SetAttribute("error", "syntax error!");
			}

			sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d×Ö½Ú", plocation_descriptor->descriptor_tag, length);
			pxmlDescriptorNode->SetAttribute("comment", pszTemp);

			if (pLocationDescriptor == NULL)
			{
				delete plocation_descriptor;
			}
		}
		else
		{
			pxmlParentNode->SetAttribute("error", "parameter error!");
			rtcode = SECTION_PARSE_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




