#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "../../Include/mpeg_video_sequence.h"
#include "../../Include/mpeg_video_errorcode.h"
#include "../../Include/mpeg_video_sequence.h"
#include "../../Include/mpeg_video_utilities.h"
#include "../Include/mpeg_video_sequence_xml.h"

int mpgv_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MPV_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[ ]", nal_buf, nal_length, NULL);
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MPV_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[ ]", nal_buf, nal_length, NULL);
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_header_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* pSequenceHeader)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_sequence_header_t* psequence_header = (pSequenceHeader == NULL) ? new MPGV_sequence_header_t : pSequenceHeader;
		rtcode = mpgv_decode_sequence_header(nal_buf, nal_length, psequence_header);
		rtcode = mpgv_present_sequence_header_to_xml(pxmlDoc, pxmlParentNode, psequence_header);

		if (pSequenceHeader == NULL)
		{
			delete psequence_header;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_present_sequence_header_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* psequence_header)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	char	pszFrameRate[32];
	char	pszAspecRatio[32];
	char	pszBitrate[32];
	char	pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (psequence_header != NULL))
	{
		MPGV_DecodeFrameRateCodetoText(psequence_header->frame_rate_code, pszFrameRate, sizeof(pszFrameRate));
		MPGV_DecodeAspectRatioCodetoText(psequence_header->aspect_ratio_information, pszAspecRatio, sizeof(pszAspecRatio));
		sprintf_s(pszBitrate, sizeof(pszBitrate), "%d bps", (int)ceil(psequence_header->bit_rate_value * 400));

		sprintf_s(pszComment, sizeof(pszComment), "%d x %d, %s, %s, %s", psequence_header->horizontal_size_value, psequence_header->vertical_size_value, pszAspecRatio, pszFrameRate, pszBitrate);

		int sequence_header_length = 12;
		if (psequence_header->load_intra_quantiser_matrix) sequence_header_length += 64;
		if (psequence_header->load_non_intra_quantiser_matrix) sequence_header_length += 64;
		assert(sequence_header_length == psequence_header->nal_snapshot.length);

		XMLElement* pxmlSequenceNode = pxmlDoc->NewBranchElement(pxmlParentNode, "sequence_header()", pszComment, sequence_header_length);

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "sequence_header_code", psequence_header->sequence_header_code, 32, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "horizontal_size_value", psequence_header->horizontal_size_value, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "vertical_size_value", psequence_header->vertical_size_value, 12, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "aspect_ratio_information", psequence_header->aspect_ratio_information, 4, "uimsbf", pszAspecRatio);
		pxmlDoc->NewElementForBits(pxmlSequenceNode, "frame_rate_code", psequence_header->frame_rate_code, 4, "uimsbf", pszFrameRate);

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "bit_rate_value", psequence_header->bit_rate_value, 18, "uimsbf", pszBitrate);
		pxmlDoc->NewElementForBits(pxmlSequenceNode, "marker_bit", psequence_header->marker_bit, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlSequenceNode, "vbv_buffer_size_value", psequence_header->vbv_buffer_size_value, 10, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlSequenceNode, "constrained_parameters_flag", psequence_header->constrained_parameters_flag, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "load_intra_quantiser_matrix", psequence_header->load_intra_quantiser_matrix, 1, "bslbf", NULL);
		if (psequence_header->load_intra_quantiser_matrix)
		{
			pxmlDoc->NewElementForByteBuf(pxmlSequenceNode, "intra_quantiser_matrix[64]", psequence_header->intra_quantiser_matrix, 64);
		}

		pxmlDoc->NewElementForBits(pxmlSequenceNode, "load_non_intra_quantiser_matrix", psequence_header->load_non_intra_quantiser_matrix, 1, "bslbf", NULL);
		if (psequence_header->load_non_intra_quantiser_matrix)
		{
			pxmlDoc->NewElementForByteBuf(pxmlSequenceNode, "non_intra_quantiser_matrix[64]", psequence_header->non_intra_quantiser_matrix, 64);
		}

		if (psequence_header->padding_bits_length > 0)
		{
			pxmlDoc->NewElementForBits(pxmlSequenceNode, "padding_bits", psequence_header->padding_bits_value, psequence_header->padding_bits_length, "bslbf");
		}

		if (psequence_header->padding_length > 0)
		{
			XMLElement* pxmlNode = pxmlDoc->NewElementForByteBuf(pxmlSequenceNode, "unrecognized bytes[ ]", psequence_header->padding_bytes, psequence_header->padding_length);
			pxmlNode->SetAttribute("error", "to be parsed");
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_decode_group_of_pictures_header_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pGOPHeader)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_group_of_pictures_header_t* pgroup_of_pictures_header = (pGOPHeader == NULL) ? new MPGV_group_of_pictures_header_t : pGOPHeader;
		rtcode = mpgv_decode_group_of_pictures_header(nal_buf, nal_length, pgroup_of_pictures_header);
		rtcode = mpgv_present_group_of_pictures_header_to_xml(pxmlDoc, pxmlParentNode, pgroup_of_pictures_header);

		if (pGOPHeader == NULL)
		{
			delete pgroup_of_pictures_header;
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_present_group_of_pictures_header_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	char	pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pgroup_of_pictures_header != NULL))
	{
		sprintf_s(pszComment, "%02d:%02d:%02d-%02d", pgroup_of_pictures_header->time_code_hours, pgroup_of_pictures_header->time_code_minutes, pgroup_of_pictures_header->time_code_seconds, pgroup_of_pictures_header->time_code_pictures);
		XMLElement* pxmlGOPNode = pxmlDoc->NewBranchElement(pxmlParentNode, "group_of_pictures_header()", pszComment, pgroup_of_pictures_header->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlGOPNode, "group_start_code", pgroup_of_pictures_header->group_start_code, 32, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlGOPNode, "drop_frame_flag", pgroup_of_pictures_header->drop_frame_flag, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlGOPNode, "time_code_hours", pgroup_of_pictures_header->time_code_hours, 5, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlGOPNode, "time_code_minutes", pgroup_of_pictures_header->time_code_minutes, 6, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlGOPNode, "marker_bit", pgroup_of_pictures_header->marker_bit, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlGOPNode, "time_code_seconds", pgroup_of_pictures_header->time_code_seconds, 6, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlGOPNode, "time_code_pictures", pgroup_of_pictures_header->time_code_pictures, 6, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlGOPNode, "closed_gop", pgroup_of_pictures_header->closed_gop, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlGOPNode, "broken_link", pgroup_of_pictures_header->broken_link, 1, "bslbf", NULL);

		if (pgroup_of_pictures_header->padding_bits_length > 0)
		{
			pxmlDoc->NewElementForBits(pxmlGOPNode, "padding_bits", pgroup_of_pictures_header->padding_bits_value, pgroup_of_pictures_header->padding_bits_length, "bslbf");
		}

		if (pgroup_of_pictures_header->padding_length > 0)
		{
			XMLElement* pxmlNode = pxmlDoc->NewElementForByteBuf(pxmlGOPNode, "unrecognized bytes[ ]", pgroup_of_pictures_header->padding_bytes, pgroup_of_pictures_header->padding_length);
			pxmlNode->SetAttribute("error", "to be parsed");
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_picture_header_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* pPictureHeader)
{
	int		rtcode = MPV_NO_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_picture_header_t* ppicture_header = (pPictureHeader == NULL) ? new MPGV_picture_header_t : pPictureHeader;
		rtcode = mpgv_decode_picture_header(nal_buf, nal_length, ppicture_header);
		rtcode = mpgv_present_picture_header_to_xml(pxmlDoc, pxmlParentNode, ppicture_header);

		if (pPictureHeader == NULL)
		{
			delete ppicture_header;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_present_picture_header_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* ppicture_header)
{
	int		rtcode = MPV_NO_ERROR;
	char pszFrameType[16];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (ppicture_header != NULL))
	{
		MPGV_DecodePictureCodingTypetoText(ppicture_header->picture_coding_type, pszFrameType, sizeof(pszFrameType));
		XMLElement* pxmlPictureNode = pxmlDoc->NewBranchElement(pxmlParentNode, "picture_header()", pszFrameType, ppicture_header->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlPictureNode, "picture_start_code", ppicture_header->picture_start_code, 32, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlPictureNode, "temporal_reference", ppicture_header->temporal_reference, 10, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlPictureNode, "picture_coding_type", ppicture_header->picture_coding_type, 3, "uimsbf", pszFrameType);

		pxmlDoc->NewElementForBits(pxmlPictureNode, "vbv_delay", ppicture_header->vbv_delay, 16, "uimsbf", NULL);

		if (ppicture_header->picture_coding_type == MPEG_P_FRAME || ppicture_header->picture_coding_type == MPEG_B_FRAME)
		{
			pxmlDoc->NewElementForBits(pxmlPictureNode, "full_pel_forward_vector", ppicture_header->full_pel_forward_vector, 1, "bslbf", NULL);

			pxmlDoc->NewElementForBits(pxmlPictureNode, "forward_f_code", ppicture_header->forward_f_code, 3, "bslbf", NULL);
		}

		if (ppicture_header->picture_coding_type == MPEG_B_FRAME)
		{
			pxmlDoc->NewElementForBits(pxmlPictureNode, "full_pel_backward_vector", ppicture_header->full_pel_backward_vector, 1, "bslbf", NULL);

			pxmlDoc->NewElementForBits(pxmlPictureNode, "backward_f_code", ppicture_header->backward_f_code, 3, "bslbf", NULL);
		}

		if (ppicture_header->padding_bits_length > 0)
		{
			pxmlDoc->NewElementForBits(pxmlPictureNode, "padding_bits", ppicture_header->padding_bits_value, ppicture_header->padding_bits_length, "bslbf");
		}

		if (ppicture_header->padding_length > 0)
		{
			XMLElement* pxmlNode = pxmlDoc->NewElementForByteBuf(pxmlPictureNode, "unrecognized bytes[ ]", ppicture_header->padding_bytes, ppicture_header->padding_length);
			pxmlNode->SetAttribute("error", "to be parsed");
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_extension_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* pSequenceExtension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_sequence_extension_t* psequence_extension = (pSequenceExtension == NULL) ? new MPGV_sequence_extension_t : pSequenceExtension;
		rtcode = mpgv_decode_sequence_extension(nal_buf, nal_length, psequence_extension);
		rtcode = mpgv_present_sequence_extension_to_xml(pxmlDoc, pxmlParentNode, psequence_extension);

		if (pSequenceExtension == NULL)
		{
			delete psequence_extension;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_present_sequence_extension_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* psequence_extension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	char	pszProfile[16];
	char	pszLevel[16];
	char	pszProfileAndLevel[32];
	char	pszChroma[16];
	char	pszSequence[64];
	char	pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (psequence_extension != NULL))
	{
		int profile = (psequence_extension->profile_and_level_indication & 0x70) >> 4;
		int level = (psequence_extension->profile_and_level_indication & 0x0f);

		MPGV_DecodeProfileCodetoText(profile, pszProfile, sizeof(pszProfile));
		MPGV_DecodeLevelCodetoText(level, pszLevel, sizeof(pszLevel));
		sprintf_s(pszProfileAndLevel, sizeof(pszProfileAndLevel), "%s Profile @ %s Level", pszProfile, pszLevel);

		MPGV_DecodeChromaFormatCodetoText(psequence_extension->chroma_format, pszChroma, sizeof(pszChroma));
		MPGV_DecodeProgressiveSequenceCodetoText(psequence_extension->progressive_sequence, pszSequence, sizeof(pszSequence));

		sprintf_s(pszComment, sizeof(pszComment), "%s Profile @ %s Level, %s, %s", pszProfile, pszLevel, pszChroma, pszSequence);

		//assert(psequence_extension->nal_snapshot.length == 10);
		XMLElement* pxmlExtensionNode = pxmlDoc->NewBranchElement(pxmlParentNode, "sequence_extension()", pszComment, psequence_extension->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "extension_start_code", psequence_extension->extension_start_code, 32, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "extension_start_code_identifier", psequence_extension->extension_start_code_identifier, 4, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "profile_and_level_indication", psequence_extension->profile_and_level_indication, 8, "uimsbf", pszProfileAndLevel);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "progressive_sequence", psequence_extension->progressive_sequence, 1, "uimsbf", pszSequence);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "chroma_format", psequence_extension->chroma_format, 2, "uimsbf", pszChroma);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "horizontal_size_extension", psequence_extension->horizontal_size_extension, 2, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "vertical_size_extension", psequence_extension->vertical_size_extension, 2, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "bit_rate_extension", psequence_extension->bit_rate_extension, 12, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "marker_bit", psequence_extension->marker_bit, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "vbv_buffer_size_extension", psequence_extension->vbv_buffer_size_extension, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "low_delay", psequence_extension->low_delay, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "frame_rate_extension_n", psequence_extension->frame_rate_extension_n, 2, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "frame_rate_extension_d", psequence_extension->frame_rate_extension_d, 5, "uimsbf", NULL);

		if (psequence_extension->padding_bits_length > 0)
		{
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "padding_bits", psequence_extension->padding_bits_value, psequence_extension->padding_bits_length, "bslbf");
		}

		if (psequence_extension->padding_length > 0)
		{
			XMLElement* pxmlNode = pxmlDoc->NewElementForByteBuf(pxmlExtensionNode, "unrecognized bytes[ ]", psequence_extension->padding_bytes, psequence_extension->padding_length);
			pxmlNode->SetAttribute("error", "to be parsed");
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_display_extension_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* pSequenceDisplayExtension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_sequence_display_extension_t* psequence_display_extension = (pSequenceDisplayExtension == NULL) ? new MPGV_sequence_display_extension_t : pSequenceDisplayExtension;
		rtcode = mpgv_decode_sequence_display_extension(nal_buf, nal_length, psequence_display_extension);
		rtcode = mpgv_present_sequence_display_extension_to_xml(pxmlDoc, pxmlParentNode, psequence_display_extension);

		if (pSequenceDisplayExtension == NULL)
		{
			delete psequence_display_extension;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_present_sequence_display_extension_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* psequence_display_extension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (psequence_display_extension != NULL))
	{
		XMLElement* pxmlExtensionNode = pxmlDoc->NewBranchElement(pxmlParentNode, "sequence_display_extension()", NULL, psequence_display_extension->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "extension_start_code", psequence_display_extension->extension_start_code, 32, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "extension_start_code_identifier", psequence_display_extension->extension_start_code_identifier, 4, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "video_format", psequence_display_extension->video_format, 3, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "colour_description", psequence_display_extension->colour_description, 1, "uimsbf", NULL);

		if (psequence_display_extension->colour_description)
		{
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "colour_primaries", psequence_display_extension->colour_primaries, 8, "uimsbf", NULL);

			pxmlDoc->NewElementForBits(pxmlExtensionNode, "transfer_characteristics", psequence_display_extension->transfer_characteristics, 8, "uimsbf", NULL);

			pxmlDoc->NewElementForBits(pxmlExtensionNode, "matrix_coefficients", psequence_display_extension->matrix_coefficients, 8, "uimsbf", NULL);
		}

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "display_horizontal_size", psequence_display_extension->display_horizontal_size, 14, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "marker_bit", psequence_display_extension->marker_bit, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "display_vertical_size", psequence_display_extension->display_vertical_size, 14, "uimsbf", NULL);


		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_picture_coding_extension_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* pPictureCodingExtension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_picture_coding_extension_t* ppicture_coding_extension = (pPictureCodingExtension == NULL) ? new MPGV_picture_coding_extension_t : pPictureCodingExtension;

		rtcode = mpgv_decode_picture_coding_extension(nal_buf, nal_length, ppicture_coding_extension);
		rtcode = mpgv_present_picture_coding_extension_to_xml(pxmlDoc, pxmlParentNode, ppicture_coding_extension);

		if (pPictureCodingExtension == NULL)
		{
			delete ppicture_coding_extension;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_present_picture_coding_extension_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* ppicture_coding_extension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (ppicture_coding_extension != NULL))
	{
		XMLElement* pxmlExtensionNode = pxmlDoc->NewBranchElement(pxmlParentNode, "picture_coding_extension()", NULL, ppicture_coding_extension->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "extension_start_code", ppicture_coding_extension->extension_start_code, 32, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "extension_start_code_identifier", ppicture_coding_extension->extension_start_code_identifier, 4, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "f_code[0][0]", ppicture_coding_extension->f_code[0][0], 4, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "f_code[0][1]", ppicture_coding_extension->f_code[0][1], 4, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "f_code[1][0]", ppicture_coding_extension->f_code[1][0], 4, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "f_code[1][1]", ppicture_coding_extension->f_code[1][1], 4, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "intra_dc_precision", ppicture_coding_extension->intra_dc_precision, 2, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "picture_structure", ppicture_coding_extension->picture_structure, 2, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "top_field_first", ppicture_coding_extension->top_field_first, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "frame_pred_frame_dct", ppicture_coding_extension->frame_pred_frame_dct, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "concealment_motion_vectors", ppicture_coding_extension->concealment_motion_vectors, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "q_scale_type", ppicture_coding_extension->q_scale_type, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "intra_vlc_format", ppicture_coding_extension->intra_vlc_format, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "alternate_scan", ppicture_coding_extension->alternate_scan, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "repeat_first_field", ppicture_coding_extension->repeat_first_field, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "chroma_420_type", ppicture_coding_extension->chroma_420_type, 1, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlExtensionNode, "progressive_frame", ppicture_coding_extension->progressive_frame, 1, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlExtensionNode, "composite_display_flag", ppicture_coding_extension->composite_display_flag, 1, "uimsbf", NULL);

		if (ppicture_coding_extension->composite_display_flag)
		{
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "v_axis", ppicture_coding_extension->v_axis, 1, "uimsbf", NULL);
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "field_sequence", ppicture_coding_extension->field_sequence, 3, "uimsbf", NULL);
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "sub_carrier", ppicture_coding_extension->sub_carrier, 1, "uimsbf", NULL);

			pxmlDoc->NewElementForBits(pxmlExtensionNode, "burst_amplitude", ppicture_coding_extension->burst_amplitude, 7, "uimsbf", NULL);
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "sub_carrier_phase", ppicture_coding_extension->sub_carrier_phase, 8, "uimsbf", NULL);
		}

		if (ppicture_coding_extension->padding_bits_length > 0)
		{
			pxmlDoc->NewElementForBits(pxmlExtensionNode, "padding_bits", ppicture_coding_extension->padding_bits_value, ppicture_coding_extension->padding_bits_length, "bslbf");
		}

		if (ppicture_coding_extension->padding_length > 0)
		{
			XMLElement* pxmlNode = pxmlDoc->NewElementForByteBuf(pxmlExtensionNode, "unrecognized bytes[ ]", ppicture_coding_extension->padding_bytes, ppicture_coding_extension->padding_length);
			pxmlNode->SetAttribute("error", "to be parsed");
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_decode_user_data_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* pUserData)
{
	int rtcode = MPV_NO_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_user_data_t* puser_data = (pUserData == NULL) ? new MPGV_user_data_t : pUserData;
		rtcode = mpgv_decode_user_data(nal_buf, nal_length, puser_data);
		rtcode = mpgv_present_user_data_to_xml(pxmlDoc, pxmlParentNode, puser_data);

		if (pUserData == NULL)
		{
			delete puser_data;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_present_user_data_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* puser_data)
{
	int rtcode = MPV_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (puser_data != NULL))
	{
		XMLElement* pxmlUserDataNode = pxmlDoc->NewBranchElement(pxmlParentNode, "user_data()", NULL, puser_data->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlUserDataNode, "user_data_start_code", puser_data->user_data_start_code, 32, "bslbf", NULL);

		XMLElement* pxmlTempNode = pxmlDoc->NewElementForByteBuf(pxmlUserDataNode, "user_data[ ]", puser_data->user_data_buf, puser_data->user_data_length, NULL);
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pSliceParam)
{
	int		rtcode = MPV_UNKNOWN_ERROR;

	if ((nal_buf != NULL) && (nal_length >= 4) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPGV_slice_t* pslice = (pSliceParam == NULL) ? new MPGV_slice_t : pSliceParam;
		rtcode = mpgv_decode_slice(nal_buf, nal_length, pslice);
		rtcode = mpgv_present_slice_to_xml(pxmlDoc, pxmlParentNode, pslice);

		if (pSliceParam == NULL)
		{
			delete pslice;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_present_slice_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pslice)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	char  pszField[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pslice != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "slice(vertical_position=0x%02X(%d))", pslice->slice_vertical_position, pslice->slice_vertical_position);
		XMLElement* pxmlSliceNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pslice->nal_snapshot.length);

		pxmlDoc->NewElementForBits(pxmlSliceNode, "slice_start_code", pslice->slice_start_code, 32, "bslbf");

		if (pslice->padding_bits_length > 0)
		{
			pxmlDoc->NewElementForBits(pxmlSliceNode, "padding_bits", pslice->padding_bits_value, pslice->padding_bits_length, "bslbf");
		}

		if (pslice->padding_length > 0)
		{
			XMLElement* pxmlNode = pxmlDoc->NewElementForByteBuf(pxmlSliceNode, "unrecognized bytes[ ]", pslice->padding_bytes, pslice->padding_length);
			pxmlNode->SetAttribute("error", "to be parsed");
		}

		rtcode = MPV_NO_ERROR;
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}
