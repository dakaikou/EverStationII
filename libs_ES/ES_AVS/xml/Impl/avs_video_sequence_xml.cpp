#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Include/avs_video_sequence.h"
#include "../../Include/avs_video_utilities.h"
#include "../../Include/avs_video_errorcode.h"
#include "../Include/avs_video_sequence_xml.h"

int avs_present_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AVS_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[ ]", nal_buf, nal_length);
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}

int avs_present_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AVS_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[ ]", nal_buf, nal_length);
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}

int	avs_decode_pb_picture_header_to_xml(uint8_t* nal_buf, int nal_length, AVS_global_param_t* pparam, HALForXMLDoc* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, AVS_pb_picture_header_t* pPBPictureHeader)
{
	int		rtcode = AVS_NO_ERROR;
	char pszTemp[64];
	char pszFrameType[16];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		//tinyxml2::XMLElement* pxmlPictureNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "pb_picture_header()");
		//pxmlDoc->UpdateBufMark(pxmlPictureNode, nal_buf, nal_buf + nal_length);

		//if ((nal_buf != NULL) && (nal_length >= 4) && (pparam != NULL))
		//{
		//	AVS_pb_picture_header_t* ppb_picture_header = (pPBPictureHeader == NULL) ? new AVS_pb_picture_header_t : pPBPictureHeader;

		//	memset(ppb_picture_header, 0x00, sizeof(AVS_pb_picture_header_t));
		//	BITS_map(&bs, nal_buf, nal_length);

		//	ppb_picture_header->pb_picture_start_code = BITS_get(&bs, 32);			
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "pb_picture_start_code", ppb_picture_header->pb_picture_start_code, 32, "f(32)", NULL, &bs);

		//	ppb_picture_header->bbv_delay = BITS_get(&bs, 16);			
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "bbv_delay", ppb_picture_header->bbv_delay, 16, "u(16)", NULL, &bs);

		//	if (pparam->profile_id == 0x48)
		//	{
		//		ppb_picture_header->marker_bit = BITS_get(&bs, 1);
		//		pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "marker_bit", ppb_picture_header->marker_bit, 1, "f(1)", NULL, &bs);

		//		ppb_picture_header->bbv_delay_extension = BITS_get(&bs, 7);
		//		pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "bbv_delay_extension", ppb_picture_header->bbv_delay_extension, 7, "u(7)", NULL, &bs);
		//	}

		//	ppb_picture_header->picture_coding_type = BITS_get(&bs, 2);
		//	AVS_DecodePictureCodingTypetoText(ppb_picture_header->picture_coding_type, pszFrameType, sizeof(pszFrameType));
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_coding_type", ppb_picture_header->picture_coding_type, 2, "u(2)", pszFrameType, &bs);

		//	ppb_picture_header->picture_distance = BITS_get(&bs, 8);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_distance", ppb_picture_header->picture_distance, 8, "u(8)", NULL, &bs);

		//	if (pparam->low_delay == 1)
		//	{
		//		ppb_picture_header->bbv_check_times = 0;
		//	}

		//	ppb_picture_header->progressive_frame = BITS_get(&bs, 1);
		//	AVS_DecodeProgressiveFrameCodetoText(ppb_picture_header->progressive_frame, pszTemp, sizeof(pszTemp));
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "progressive_frame", ppb_picture_header->progressive_frame, 1, "u(1)", pszTemp, &bs);

		//	if (ppb_picture_header->progressive_frame == 0)
		//	{
		//		ppb_picture_header->picture_structure = BITS_get(&bs, 1);
		//		AVS_DecodePictureStructureCodetoText(ppb_picture_header->picture_structure, pszTemp, sizeof(pszTemp));
		//		pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_structure", ppb_picture_header->picture_structure, 1, "u(1)", pszTemp, &bs);

		//		if (ppb_picture_header->picture_structure == 0)
		//		{
		//			ppb_picture_header->advanced_pred_mode_disable = BITS_get(&bs, 1);
		//			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "advanced_pred_mode_disable", ppb_picture_header->advanced_pred_mode_disable, 1, "u(1)", NULL, &bs);
		//		}
		//	}

		//	ppb_picture_header->top_field_first = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "top_field_first", ppb_picture_header->top_field_first, 1, "u(1)", NULL, &bs);

		//	ppb_picture_header->repeat_first_field = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "repeat_first_field", ppb_picture_header->repeat_first_field, 1, "u(1)", NULL, &bs);

		//	ppb_picture_header->fixed_picture_qp = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "fixed_picture_qp", ppb_picture_header->fixed_picture_qp, 1, "u(1)", NULL, &bs);

		//	pparam->fixed_picture_qp = ppb_picture_header->fixed_picture_qp;		//*****

		//	ppb_picture_header->picture_qp = BITS_get(&bs, 6);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_qp", ppb_picture_header->picture_qp, 6, "u(6)", NULL, &bs);

		//	if (!((ppb_picture_header->picture_coding_type == 0b10) && (ppb_picture_header->picture_structure == 1)))
		//	{
		//		ppb_picture_header->picture_reference_flag = BITS_get(&bs, 1);
		//		pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_reference_flag", ppb_picture_header->picture_reference_flag, 1, "u(1)", NULL, &bs);
		//	}

		//	ppb_picture_header->no_forward_reference_flag = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "no_forward_reference_flag", ppb_picture_header->no_forward_reference_flag, 1, "u(1)", NULL, &bs);

		//	ppb_picture_header->pb_filed_enhanced_flag = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "pb_filed_enhanced_flag", ppb_picture_header->pb_filed_enhanced_flag, 1, "u(1)", NULL, &bs);

		//	ppb_picture_header->reserved_bits = BITS_get(&bs, 2);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "reserved_bits", ppb_picture_header->reserved_bits, 2, "r(2)", NULL, &bs);

		//	ppb_picture_header->skip_mode_flag = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "skip_mode_flag", ppb_picture_header->skip_mode_flag, 1, "u(1)", NULL, &bs);

		//	ppb_picture_header->loop_filter_disable = BITS_get(&bs, 1);
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "loop_filter_disable", ppb_picture_header->loop_filter_disable, 1, "u(1)", NULL, &bs);

		//	if (!ppb_picture_header->loop_filter_disable)
		//	{
		//		ppb_picture_header->loop_filter_parameter_flag = BITS_get(&bs, 1);
		//		pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "loop_filter_parameter_flag", ppb_picture_header->loop_filter_parameter_flag, 1, "u(1)", NULL, &bs);

		//		if (ppb_picture_header->loop_filter_parameter_flag)
		//		{

		//		}
		//	}

		//	sprintf_s(pszTemp, sizeof(pszTemp), "%d×Ö½Ú - %s", nal_length, pszFrameType);
		//	pxmlPictureNode->SetAttribute("comment", pszTemp);

		//	if (pPBPictureHeader == NULL)
		//	{
		//		delete ppb_picture_header;
		//	}
		//}
		//else
		//{
		//	pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "avs pb picture header parameter error !");
		//	rtcode = AVS_PARAMETER_ERROR;
		//}
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}

int avs_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, AVS_global_param_t* pparam, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AVS_slice_t* pSliceParam)
{
	int		rtcode = AVS_UNKNOWN_ERROR;
	BITS_t	 bs;
	char  pszVerPos[32];
	char  pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		//tinyxml2::XMLElement* pxmlSliceNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "slice()");
		//pxmlDoc->UpdateBufMark(pxmlSliceNode, nal_buf, nal_buf + nal_length);

		////pxmlSliceNode->SetAttribute("comment", pszTemp);

		//if ((nal_buf != NULL) && (nal_length >= 4) && (pparam != NULL))
		//{
		//	BITS_map(&bs, nal_buf, nal_length);

		//	AVS_slice_t* pslice = (pSliceParam == NULL) ? new AVS_slice_t : pSliceParam;
		//	memset(pslice, 0x00, sizeof(AVS_slice_t));

		//	pslice->slice_start_code = BITS_get(&bs, 32);
		//	pslice->slice_vertical_position = (pslice->slice_start_code & 0x000000ff);
		//	sprintf_s(pszVerPos, sizeof(pszVerPos), "vertical_position=0x%02X(%d)", pslice->slice_vertical_position, pslice->slice_vertical_position);
		//	pxmlDoc->NewKeyValuePairElement(pxmlSliceNode, "slice_start_code", pslice->slice_start_code, 32, "f(32)", pszVerPos, &bs);

		//	sprintf_s(pszTemp, sizeof(pszTemp), "%d×Ö½Ú, %s", nal_length, pszVerPos);
		//	pxmlSliceNode->SetAttribute("comment", pszTemp);

		//	if (pparam->vertical_size > 2800)
		//	{
		//		pslice->slice_vertical_position_extension = BITS_get(&bs, 3);
		//		pxmlDoc->NewKeyValuePairElement(pxmlSliceNode, "slice_vertical_position_extension", pslice->slice_vertical_position_extension, 3, "u(3)", NULL, &bs);
		//	}

		//	if (pSliceParam == NULL)
		//	{
		//		delete pslice;
		//	}

		//	rtcode = AVS_NO_ERROR;
		//}
		//else
		//{
		//	pxmlDoc->NewKeyValuePairElement(pxmlSliceNode, "avs slice parameter error !");
		//	rtcode = AVS_PARAMETER_ERROR;
		//}
	}
	else
	{
		rtcode = AVS_PARAMETER_ERROR;
	}

	return rtcode;
}
