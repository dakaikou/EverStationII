#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Include/avs2_video_sequence.h"
#include "../../Include/avs2_video_errorcode.h"
#include "../Include/avs2_video_sequence_xml.h"
#include "HAL/HAL_BitStream/Include/HALForBitStream.h"


#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int avs_decode_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = AVS_NO_ERROR;
	char			pszTemp[64];
	int				i;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unaligned nal()");
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d×Ö½Ú", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);

		for (i = 0; i < min(16, nal_length); i++)
		{
			sprintf_s(pszTemp + 3 * i, 4, "%02X ", nal_buf[i]);
		}
		if (nal_length > 16)
		{
			sprintf_s(pszTemp + 3 * i, 5, "...\n");
		}
		tinyxml2::XMLElement* pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, pszTemp);
		pxmlDoc->UpdateBufMark(pxmlTempNode, nal_buf, nal_buf + nal_length);
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}

int avs_decode_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = AVS_NO_ERROR;
	char			pszTemp[64];
	int				i;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unknown nal()");
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d×Ö½Ú", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);

		for (i = 0; i < min(16, nal_length); i++)
		{
			sprintf_s(pszTemp + 3 * i, 4, "%02X ", nal_buf[i]);
		}
		if (nal_length > 16)
		{
			sprintf_s(pszTemp + 3 * i, 5, "...\n");
		}
		tinyxml2::XMLElement* pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, pszTemp);
		pxmlDoc->UpdateBufMark(pxmlTempNode, nal_buf, nal_buf + nal_length);
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}
