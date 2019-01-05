#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax_translate_layer/ES_H264/Include/h264_video_errorcode.h"
#include "../Include/h264_video_sequence_xml.h"

int h264_present_unaligned_nal_to_xml(uint8_t* segment_buf, int prefix_length, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = H264_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[ ]", segment_buf, prefix_length + nal_length);
	}
	else
	{
		rtcode = H264_PARAMETER_ERROR;
	}

	return rtcode;
}

int h264_present_unknown_nal_to_xml(uint8_t* segment_buf, int prefix_length, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = H264_NO_ERROR;

	uint8_t* prefix_buf = segment_buf;
	uint8_t* nal_buf = segment_buf + prefix_length;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[ ]", segment_buf, prefix_length + nal_length);
		//pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, segment_buf, segment_buf + prefix_length + nal_length);

		//BITS_map(&bs, segment_buf, prefix_length + nal_length);

		//uint32_t prefix_start_code = BITS_get(&bs, prefix_length * 8);
		//pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, "prefix_start_code", prefix_start_code, prefix_length * 8, "bslbf", NULL, &bs);

		//uint8_t forbidden_zero_bit = BITS_get(&bs, 1);
		//pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, "forbidden_zero_bit", forbidden_zero_bit, 1, "f(1)", NULL, &bs);

		//uint8_t nal_ref_idx = BITS_get(&bs, 2);
		//pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, "nal_ref_idx", nal_ref_idx, 2, "u(2)", NULL, &bs);

		//uint8_t nal_unit_type = BITS_get(&bs, 5);
		//pxmlDoc->NewKeyValuePairElement(pxmlUnknownNalNode, "nal_unit_type", nal_unit_type, 5, "u(5)", NULL, &bs);

		//sprintf_s(pszTemp, sizeof(pszTemp), "%d�ֽ�, nal_unit_type=%d", prefix_length + nal_length, nal_unit_type);
		//pxmlUnknownNalNode->SetAttribute("comment", pszTemp);
	}
	else
	{
		rtcode = H264_PARAMETER_ERROR;
	}

	return rtcode;
}
