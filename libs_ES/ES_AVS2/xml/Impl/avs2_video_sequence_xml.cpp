#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Include/avs2_video_sequence.h"
#include "../../Include/avs2_video_errorcode.h"
#include "../Include/avs2_video_sequence_xml.h"


int avs2_present_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AVS_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[]", NULL, nal_length);
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}

int avs2_present_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AVS_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[]", NULL, nal_length);
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}
