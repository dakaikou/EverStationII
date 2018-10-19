#include <windows.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

#include <mmsystem.h>

#include "../../Include/mpeg_video_sequence.h"
#include "../../Include/mpeg_video_errorcode.h"
#include "../../Include/mpeg_video_sequence.h"
#include "../../Include/mpeg_video_utilities.h"
#include "../Include/mpeg_video_sequence_xml.h"

int mpgv_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MPV_NO_ERROR;
	char			pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unaligned nal[ ]", nal_buf, nal_length);
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MPV_NO_ERROR;
	char			pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unknown nal[ ]", nal_buf, nal_length);
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_header_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_header_t* pParam)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	int		i;
	char	pszTemp[64];
	//S32		v, u;
	BITS_t	bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlSequenceNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "sequence_header()");
		pxmlDoc->UpdateBufMark(pxmlSequenceNode, nal_buf, nal_buf + nal_length);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlSequenceNode->SetAttribute("comment", pszTemp);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_sequence_header_t* psequence_header = (pParam == NULL) ? new MPGV_sequence_header_t : pParam;

			memset(psequence_header, 0x00, sizeof(MPGV_sequence_header_t));
			BITS_map(&bs, nal_buf, nal_length);

			/*syntax part parse*/
			psequence_header->sequence_header_code = BITS_get(&bs, 32);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "sequence_header_code", psequence_header->sequence_header_code, 32, "bslbf", NULL, &bs);

			psequence_header->horizontal_size_value = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "horizontal_size_value", psequence_header->horizontal_size_value, 12, "uimsbf", NULL, &bs);

			psequence_header->vertical_size_value = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "vertical_size_value", psequence_header->vertical_size_value, 12, "uimsbf", NULL, &bs);

			psequence_header->aspect_ratio_information = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "aspect_ratio_information", psequence_header->aspect_ratio_information, 4, "uimsbf", NULL, &bs);

			psequence_header->frame_rate_code = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "frame_rate_code", psequence_header->frame_rate_code, 4, "uimsbf", NULL, &bs);

			psequence_header->bit_rate_value = BITS_get(&bs, 18);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "bit_rate_value", psequence_header->bit_rate_value, 18, "uimsbf", NULL, &bs);

			psequence_header->marker_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "marker_bit", psequence_header->marker_bit, 1, "bslbf", NULL, &bs);

			psequence_header->vbv_buffer_size_value = BITS_get(&bs, 10);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "vbv_buffer_size_value", psequence_header->vbv_buffer_size_value, 10, "uimsbf", NULL, &bs);

			psequence_header->constrained_parameters_flag = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "constrained_parameters_flag", psequence_header->constrained_parameters_flag, 1, "bslbf", NULL, &bs);

			psequence_header->load_intra_quantiser_matrix = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "load_intra_quantiser_matrix", psequence_header->load_intra_quantiser_matrix, 1, "bslbf", NULL, &bs);

			if (psequence_header->load_intra_quantiser_matrix)
			{
				for (i = 0; i < 64; i++)
				{
					psequence_header->intra_quantiser_matrix[i] = BITS_get(&bs, 8);
				}

				//for (v = 0; v < 8; v++)
				//{
				//	for (u = 0; u < 8; u++)
				//	{
				//		//W[0][v][u] = psequence_header->intra_quantiser_matrix[scan[0][v][u]];
				//	}
				//}
			}
			else		//use default quant matrix 
			{
				//memcpy(W[0], DW[0], 64 * sizeof(S16));
			}

			psequence_header->load_non_intra_quantiser_matrix = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "load_non_intra_quantiser_matrix", psequence_header->load_non_intra_quantiser_matrix, 1, "bslbf", NULL, &bs);

			if (psequence_header->load_non_intra_quantiser_matrix)
			{
				for (i = 0; i < 64; i++)
				{
					psequence_header->non_intra_quantiser_matrix[i] = BITS_get(&bs, 8);
				}

				//for (v = 0; v < 8; v++)
				//{
				//	for (u = 0; u < 8; u++)
				//	{
				//		W[1][v][u] = psequence_header->non_intra_quantiser_matrix[scan[0][v][u]];
				//	}
				//}
			}
			else
			{
				//memcpy(W[1], DW[1], 64 * sizeof(S16));
			}

			//	mpeg_video_next_start_code(pbs);

			//added by chendelin		20060730
			//memcpy(W[2], W[0], 64 * sizeof(S16));
			//memcpy(W[3], W[1], 64 * sizeof(S16));

			if (pParam == NULL)
			{
				delete psequence_header;
			}
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlSequenceNode, "mpgv sequence header buffer parameter error!");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_decode_group_of_pictures_header_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_group_of_pictures_header_t* pGOPHeader)
{
	S32		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;
	char pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlGOPNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "group_of_pictures_header()");
		pxmlDoc->UpdateBufMark(pxmlGOPNode, nal_buf, nal_buf + nal_length);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_group_of_pictures_header_t* pgroup_of_pictures_header = (pGOPHeader == NULL) ? new MPGV_group_of_pictures_header_t : pGOPHeader;
			memset(pgroup_of_pictures_header, 0x00, sizeof(MPGV_group_of_pictures_header_t));

			BITS_map(&bs, nal_buf, nal_length);

			pgroup_of_pictures_header->group_start_code = BITS_get(&bs, 32);			//bytes 0~3
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "group_start_code", pgroup_of_pictures_header->group_start_code, 32, "bslbf", NULL, &bs);

			pgroup_of_pictures_header->drop_frame_flag = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "drop_frame_flag", pgroup_of_pictures_header->drop_frame_flag, 1, "bslbf", NULL, &bs);
			pgroup_of_pictures_header->time_code_hours = BITS_get(&bs, 5);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "time_code_hours", pgroup_of_pictures_header->time_code_hours, 5, "bslbf", NULL, &bs);
			pgroup_of_pictures_header->time_code_minutes = BITS_get(&bs, 6);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "time_code_minutes", pgroup_of_pictures_header->time_code_minutes, 6, "bslbf", NULL, &bs);
			pgroup_of_pictures_header->marker_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "marker_bit", pgroup_of_pictures_header->marker_bit, 1, "bslbf", NULL, &bs);
			pgroup_of_pictures_header->time_code_seconds = BITS_get(&bs, 6);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "time_code_seconds", pgroup_of_pictures_header->time_code_seconds, 6, "bslbf", NULL, &bs);
			pgroup_of_pictures_header->time_code_pictures = BITS_get(&bs, 6);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "time_code_pictures", pgroup_of_pictures_header->time_code_pictures, 6, "bslbf", NULL, &bs);

			pgroup_of_pictures_header->closed_gop = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "closed_gop", pgroup_of_pictures_header->closed_gop, 1, "bslbf", NULL, &bs);
			pgroup_of_pictures_header->broken_link = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "broken_link", pgroup_of_pictures_header->broken_link, 1, "bslbf", NULL, &bs);

			sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
			pxmlGOPNode->SetAttribute("comment", pszTemp);
			if (pGOPHeader == NULL)
			{
				delete pgroup_of_pictures_header;
			}

			rtcode = MPV_NO_ERROR;
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlGOPNode, "mpgv group picture header parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_picture_header_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_header_t* pPictureHeader)
{
	int		rtcode = MPV_NO_ERROR;
	BITS_t		bs;
	char pszTemp[64];
	char pszFrameType[16];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlPictureNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "picture_header()");
		pxmlDoc->UpdateBufMark(pxmlPictureNode, nal_buf, nal_buf + nal_length);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_picture_header_t* ppicture_header = (pPictureHeader == NULL) ? new MPGV_picture_header_t : pPictureHeader;

			memset(ppicture_header, 0x00, sizeof(MPGV_picture_header_t));
			BITS_map(&bs, nal_buf, nal_length);

			ppicture_header->picture_start_code = BITS_get(&bs, 32);			//bytes 0~3
			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_start_code", ppicture_header->picture_start_code, 32, "bslbf", NULL, &bs);

			ppicture_header->temporal_reference = BITS_get(&bs, 10);
			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "temporal_reference", ppicture_header->temporal_reference, 10, "uimsbf", NULL, &bs);

			ppicture_header->picture_coding_type = BITS_get(&bs, 3);			//000	-- forbidden
																				//001	-- I frame
																				//010	-- P frame
																				//011	-- B frame
																				//100	-- DC intra-coded(D)
																				//101	-- reserved
																				//110	-- reserved
																				//111	-- reserved

			MPGV_DecodePictureCodingTypetoText(ppicture_header->picture_coding_type, pszFrameType, sizeof(pszFrameType));
			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "picture_coding_type", ppicture_header->picture_coding_type, 3, "uimsbf", pszFrameType, &bs);

			ppicture_header->vbv_delay = BITS_get(&bs, 16);
			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "vbv_delay", ppicture_header->vbv_delay, 16, "uimsbf", NULL, &bs);

			if (ppicture_header->picture_coding_type == MPEG_P_FRAME || ppicture_header->picture_coding_type == MPEG_B_FRAME)
			{
				ppicture_header->full_pel_forward_vector = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "full_pel_forward_vector", ppicture_header->full_pel_forward_vector, 1, "bslbf", NULL, &bs);

				ppicture_header->forward_f_code = BITS_get(&bs, 3);
				pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "forward_f_code", ppicture_header->forward_f_code, 3, "bslbf", NULL, &bs);
			}

			if (ppicture_header->picture_coding_type == MPEG_B_FRAME)
			{
				ppicture_header->full_pel_backward_vector = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "full_pel_backward_vector", ppicture_header->full_pel_backward_vector, 1, "bslbf", NULL, &bs);

				ppicture_header->backward_f_code = BITS_get(&bs, 3);
				pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "backward_f_code", ppicture_header->backward_f_code, 3, "bslbf", NULL, &bs);
			}

			ppicture_header->extra_bit_picture = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "extra_bit_picture", ppicture_header->extra_bit_picture, 1, "bslbf", NULL, &bs);

			if (ppicture_header->extra_bit_picture == 1)
			{
				ppicture_header->extra_information_picture = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "extra_information_picture", ppicture_header->extra_information_picture, 8, "uimsbf", NULL, &bs);

			}
			//如果是mpeg-1视频，则在图像头后就进行解码帧重排序，否则要等到解picture_coding_extension之后
			//if (m_mpeg2_video_stream == 0)
			//{
			//	mpgv_decode_reorder_decode_frame(m_picture_header.picture_coding_type, m_picture_coding_extension.picture_structure);
			//}

			sprintf_s(pszTemp, sizeof(pszTemp), "%d字节 - %s", nal_length, pszFrameType);
			pxmlPictureNode->SetAttribute("comment", pszTemp);

			if (pPictureHeader == NULL)
			{
				delete ppicture_header;
			}
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlPictureNode, "mpgv picture header parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_extension_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_extension_t* pSequenceExtension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;
	char  pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlExtensionNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "sequence_extension()");
		pxmlDoc->UpdateBufMark(pxmlExtensionNode, nal_buf, nal_buf + nal_length);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlExtensionNode->SetAttribute("comment", pszTemp);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_sequence_extension_t* psequence_extension = (pSequenceExtension == NULL) ? new MPGV_sequence_extension_t : pSequenceExtension;
			memset(psequence_extension, 0x00, sizeof(MPGV_sequence_extension_t));
			BITS_map(&bs, nal_buf, nal_length);

			psequence_extension->extension_start_code = BITS_get(&bs, 32);			//bytes 0~3
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "extension_start_code", psequence_extension->extension_start_code, 32, "bslbf", NULL, &bs);

			psequence_extension->extension_start_code_identifier = BITS_get(&bs, 4);		//byte 4
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "extension_start_code_identifier", psequence_extension->extension_start_code_identifier, 4, "uimsbf", NULL, &bs);

			psequence_extension->profile_and_level_indication = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "profile_and_level_indication", psequence_extension->profile_and_level_indication, 8, "uimsbf", NULL, &bs);
			psequence_extension->progressive_sequence = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "progressive_sequence", psequence_extension->progressive_sequence, 1, "uimsbf", NULL, &bs);
			psequence_extension->chroma_format = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "chroma_format", psequence_extension->chroma_format, 2, "uimsbf", NULL, &bs);
			psequence_extension->horizontal_size_extension = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "horizontal_size_extension", psequence_extension->horizontal_size_extension, 2, "uimsbf", NULL, &bs);
			psequence_extension->vertical_size_extension = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "vertical_size_extension", psequence_extension->vertical_size_extension, 2, "uimsbf", NULL, &bs);
			psequence_extension->bit_rate_extension = BITS_get(&bs, 12);					//byte8
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "bit_rate_extension", psequence_extension->bit_rate_extension, 12, "uimsbf", NULL, &bs);
			psequence_extension->marker_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "marker_bit", psequence_extension->marker_bit, 1, "bslbf", NULL, &bs);

			psequence_extension->vbv_buffer_size_extension = BITS_get(&bs, 8);	//byte 10
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "vbv_buffer_size_extension", psequence_extension->vbv_buffer_size_extension, 8, "uimsbf", NULL, &bs);

			psequence_extension->low_delay = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "low_delay", psequence_extension->low_delay, 1, "uimsbf", NULL, &bs);
			psequence_extension->frame_rate_extension_n = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "frame_rate_extension_n", psequence_extension->frame_rate_extension_n, 2, "uimsbf", NULL, &bs);
			psequence_extension->frame_rate_extension_d = BITS_get(&bs, 5);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "frame_rate_extension_d", psequence_extension->frame_rate_extension_d, 5, "uimsbf", NULL, &bs);

			if (pSequenceExtension == NULL)
			{
				delete psequence_extension;
			}

			rtcode = MPV_NO_ERROR;
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "mpgv sequence extension parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_sequence_display_extension_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_sequence_display_extension_t* pSequenceDisplayExtension)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;
	char  pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlExtensionNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "sequence_display_extension()");
		pxmlDoc->UpdateBufMark(pxmlExtensionNode, nal_buf, nal_buf + nal_length);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlExtensionNode->SetAttribute("comment", pszTemp);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_sequence_display_extension_t* psequence_display_extension = (pSequenceDisplayExtension == NULL) ? new MPGV_sequence_display_extension_t : pSequenceDisplayExtension;
			memset(psequence_display_extension, 0x00, sizeof(MPGV_sequence_display_extension_t));
			BITS_map(&bs, nal_buf, nal_length);

			psequence_display_extension->extension_start_code = BITS_get(&bs, 32);			//bytes 0~3
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "extension_start_code", psequence_display_extension->extension_start_code, 32, "bslbf", NULL, &bs);

			psequence_display_extension->extension_start_code_identifier = BITS_get(&bs, 4);		//byte 4
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "extension_start_code_identifier", psequence_display_extension->extension_start_code_identifier, 4, "uimsbf", NULL, &bs);

			psequence_display_extension->video_format = BITS_get(&bs, 3);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "video_format", psequence_display_extension->video_format, 3, "uimsbf", NULL, &bs);

			psequence_display_extension->colour_description = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "colour_description", psequence_display_extension->colour_description, 1, "uimsbf", NULL, &bs);

			if (psequence_display_extension->colour_description)
			{
				psequence_display_extension->colour_primaries = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "colour_primaries", psequence_display_extension->colour_primaries, 8, "uimsbf", NULL, &bs);

				psequence_display_extension->transfer_characteristics = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "transfer_characteristics", psequence_display_extension->transfer_characteristics, 8, "uimsbf", NULL, &bs);

				psequence_display_extension->matrix_coefficients = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "matrix_coefficients", psequence_display_extension->matrix_coefficients, 8, "uimsbf", NULL, &bs);
			}

			psequence_display_extension->display_horizontal_size = BITS_get(&bs, 14);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "display_horizontal_size", psequence_display_extension->display_horizontal_size, 14, "uimsbf", NULL, &bs);

			psequence_display_extension->marker_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "marker_bit", psequence_display_extension->marker_bit, 1, "uimsbf", NULL, &bs);

			psequence_display_extension->display_vertical_size = BITS_get(&bs, 14);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "display_vertical_size", psequence_display_extension->display_vertical_size, 14, "uimsbf", NULL, &bs);

			if (pSequenceDisplayExtension == NULL)
			{
				delete psequence_display_extension;
			}

			rtcode = MPV_NO_ERROR;
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "mpgv sequence display extension parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_picture_coding_extension_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_picture_coding_extension_t* pPictureCodingExtension)
{
	S32		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t		bs;
	char  pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlExtensionNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "picture_coding_extension()");
		pxmlDoc->UpdateBufMark(pxmlExtensionNode, nal_buf, nal_buf + nal_length);

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlExtensionNode->SetAttribute("comment", pszTemp);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_picture_coding_extension_t* ppicture_coding_extension = (pPictureCodingExtension == NULL) ? new MPGV_picture_coding_extension_t : pPictureCodingExtension;

			memset(ppicture_coding_extension, 0x00, sizeof(MPGV_picture_coding_extension_t));
			BITS_map(&bs, nal_buf, nal_length);

			ppicture_coding_extension->extension_start_code = BITS_get(&bs, 32);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "extension_start_code", ppicture_coding_extension->extension_start_code, 32, "bslbf", NULL, &bs);

			ppicture_coding_extension->extension_start_code_identifier = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "extension_start_code_identifier", ppicture_coding_extension->extension_start_code_identifier, 4, "uimsbf", NULL, &bs);

			ppicture_coding_extension->f_code[0][0] = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "f_code[0][0]", ppicture_coding_extension->f_code[0][0], 4, "uimsbf", NULL, &bs);
			ppicture_coding_extension->f_code[0][1] = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "f_code[0][1]", ppicture_coding_extension->f_code[0][1], 4, "uimsbf", NULL, &bs);
			ppicture_coding_extension->f_code[1][0] = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "f_code[1][0]", ppicture_coding_extension->f_code[1][0], 4, "uimsbf", NULL, &bs);
			ppicture_coding_extension->f_code[1][1] = BITS_get(&bs, 4);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "f_code[1][1]", ppicture_coding_extension->f_code[1][1], 4, "uimsbf", NULL, &bs);

			ppicture_coding_extension->intra_dc_precision = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "intra_dc_precision", ppicture_coding_extension->intra_dc_precision, 2, "uimsbf", NULL, &bs);
			ppicture_coding_extension->picture_structure = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "picture_structure", ppicture_coding_extension->picture_structure, 2, "uimsbf", NULL, &bs);

			ppicture_coding_extension->top_field_first = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "top_field_first", ppicture_coding_extension->top_field_first, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->frame_pred_frame_dct = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "frame_pred_frame_dct", ppicture_coding_extension->frame_pred_frame_dct, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->concealment_motion_vectors = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "concealment_motion_vectors", ppicture_coding_extension->concealment_motion_vectors, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->q_scale_type = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "q_scale_type", ppicture_coding_extension->q_scale_type, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->intra_vlc_format = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "intra_vlc_format", ppicture_coding_extension->intra_vlc_format, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->alternate_scan = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "alternate_scan", ppicture_coding_extension->alternate_scan, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->repeat_first_field = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "repeat_first_field", ppicture_coding_extension->repeat_first_field, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->chroma_420_type = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "chroma_420_type", ppicture_coding_extension->chroma_420_type, 1, "uimsbf", NULL, &bs);

			ppicture_coding_extension->progressive_frame = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "progressive_frame", ppicture_coding_extension->progressive_frame, 1, "uimsbf", NULL, &bs);
			ppicture_coding_extension->composite_display_flag = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "composite_display_flag", ppicture_coding_extension->composite_display_flag, 1, "uimsbf", NULL, &bs);

			if (ppicture_coding_extension->composite_display_flag)
			{
				ppicture_coding_extension->v_axis = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "v_axis", ppicture_coding_extension->v_axis, 1, "uimsbf", NULL, &bs);
				ppicture_coding_extension->field_sequence = BITS_get(&bs, 3);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "field_sequence", ppicture_coding_extension->field_sequence, 3, "uimsbf", NULL, &bs);
				ppicture_coding_extension->sub_carrier = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "sub_carrier", ppicture_coding_extension->sub_carrier, 1, "uimsbf", NULL, &bs);

				ppicture_coding_extension->burst_amplitude = BITS_get(&bs, 7);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "burst_amplitude", ppicture_coding_extension->burst_amplitude, 7, "uimsbf", NULL, &bs);
				ppicture_coding_extension->sub_carrier_phase = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "sub_carrier_phase", ppicture_coding_extension->sub_carrier_phase, 8, "uimsbf", NULL, &bs);
			}

			//如果是mpeg-2视频，在此重排序解码帧缓存，否则需要在解picture_header时就要重排序
			//if (m_mpeg2_video_stream == 1)
			//{
			//	mpgv_decode_reorder_decode_frame(m_picture_header.picture_coding_type, m_picture_coding_extension.picture_structure);
			//}

			if (pPictureCodingExtension == NULL)
			{
				delete ppicture_coding_extension;
			}

			rtcode = MPV_NO_ERROR;
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlExtensionNode, "mpgv picture coding extension parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int	mpgv_decode_user_data_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_user_data_t* pUserData)
{
	int rtcode = MPV_NO_ERROR;
	char			pszTemp[64];
	BITS_t			bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUserDataNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "user_data()");
		pxmlDoc->UpdateBufMark(pxmlUserDataNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlUserDataNode->SetAttribute("comment", pszTemp);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			MPGV_user_data_t* puser_data = (pUserData == NULL) ? new MPGV_user_data_t : pUserData;
			memset(puser_data, 0x00, sizeof(MPGV_user_data_t));

			BITS_map(&bs, nal_buf, nal_length);

			puser_data->user_data_start_code = BITS_get(&bs, 32);
			pxmlDoc->NewKeyValuePairElement(pxmlUserDataNode, "user_data_start_code", puser_data->user_data_start_code, 32, "bslbf", NULL, &bs);

			int user_data_length = nal_length - 4;
			uint8_t* user_data_buf = bs.p_cur;
			BITS_byteSkip(&bs, user_data_length);
			tinyxml2::XMLElement* pxmlTempNode = pxmlDoc->NewKeyValuePairElement(pxmlUserDataNode, "user_data[ ]", user_data_buf, user_data_length, NULL, &bs);

			if (pUserData == NULL)
			{
				delete puser_data;
			}
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlUserDataNode, "mpgv user data parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpgv_decode_slice_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, XMLElement* pxmlParentNode, MPGV_slice_t* pSliceParam)
{
	int		rtcode = MPV_UNKNOWN_ERROR;
	BITS_t	 bs;
	char  pszVerPos[32];
	char  pszTemp[64];
	//S32		 quantiser_scale_code;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlSliceNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "slice()");
		pxmlDoc->UpdateBufMark(pxmlSliceNode, nal_buf, nal_buf + nal_length);

		//sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		//pxmlSliceNode->SetAttribute("comment", pszTemp);

		if ((nal_buf != NULL) && (nal_length >= 4))
		{
			BITS_map(&bs, nal_buf, nal_length);

			MPGV_slice_t* pslice = (pSliceParam == NULL) ? new MPGV_slice_t : pSliceParam;
			memset(pslice, 0x00, sizeof(MPGV_slice_t));

			pslice->slice_start_code = BITS_get(&bs, 32);
			pslice->slice_vertical_position = (pslice->slice_start_code & 0x000000ff);
			sprintf_s(pszVerPos, sizeof(pszVerPos), "vertical_position=0x%02X(%d)", pslice->slice_vertical_position, pslice->slice_vertical_position);
			pxmlDoc->NewKeyValuePairElement(pxmlSliceNode, "slice_start_code", pslice->slice_start_code, 32, "bslbf", pszVerPos, &bs);

			sprintf_s(pszTemp, sizeof(pszTemp), "%d字节, %s", nal_length, pszVerPos);
			pxmlSliceNode->SetAttribute("comment", pszTemp);


			//		if (gbInitDecoder
			//#if MPEG_VID_DECODER_I
			//			&& (m_picture_header.picture_coding_type == MPEG_I_FRAME))
			//#elif MPEG_VID_DECODER_P
			//			&& (m_picture_header.picture_coding_type == MPEG_P_FRAME))
			//#elif MPEG_VID_DECODER_B 
			//			&& (m_picture_header.picture_coding_type == MPEG_B_FRAME))
			//#elif MPEG_VID_DECODER_IP 
			//			&& ((m_picture_header.picture_coding_type == MPEG_I_FRAME) ||
			//				(m_picture_header.picture_coding_type == MPEG_P_FRAME)))
			//#else
			//			)
			//#endif
			//		{
			//			//		if (m_slice.slice_vertical_position < 2)
			//			{
			//				if (m_sequence_header.vertical_size_value > 2800)			//vertical_size is a member variable, derived from m_sequence_header and m_sequence_extension
			//				{
			//					m_slice.slice_vertical_position = (BITS_get(&bs, 3) << 7) + m_slice.slice_vertical_position;
			//				}
			//
			//				if (m_find_sequence_scalable_extension)
			//				{
			//					if (m_sequence_scalable_extension.scalable_mode == MPEG_DATA_PARTITIONING)		//data partitioning
			//					{
			//						//priority breakpoint
			//						BITS_skip(&bs, 7);
			//					}
			//				}
			//
			//				quantiser_scale_code = BITS_get(&bs, 5);
			//				m_quantiser_scale = LUT_quantiser_scale[m_picture_coding_extension.q_scale_type][quantiser_scale_code];
			//
			//#if MPEG_VID_DECODER_DEBUG
			//				if (m_hDebugFile)
			//				{
			//					fprintf(m_hDebugFile, " [SLICE]quantiser_scale_code = %d\n", quantiser_scale_code);
			//				}
			//#endif
			//
			//				if (BITS_show(&bs, 1))
			//				{
			//					//intra_slice_flag
			//					BITS_skip(&bs, 1);
			//					//intra_slice
			//					BITS_skip(&bs, 1);
			//
			//					// slice_id introduced in March 1995 as part of the video corridendum
			//					// (after the IS was drafted in November 1994) 
			//					BITS_skip(&bs, 1);
			//					BITS_skip(&bs, 6);
			//
			//					while (BITS_show(&bs, 1))
			//					{
			//						//extra_bit_slice
			//						BITS_skip(&bs, 1);
			//						//extra_information_slice
			//						BITS_skip(&bs, 8);
			//					}
			//				}
			//
			//				//extra_bit_slice
			//				BITS_skip(&bs, 1);
			//
			//				m_macroblock.previous_macroblock_address = (m_slice.slice_vertical_position - 1) * m_VidDecodeInfo.mb_width - 1;		//at the start of a slice
			//				m_macroblock.max_mb_address_in_slice = (m_slice.slice_vertical_position * m_VidDecodeInfo.mb_width);
			//
			//				//片的开始处, 初始化预测器
			//
			//				/* ISO/IEC 13818-2 section 7.2.1 */
			//				m_macroblock.dc_dct_pred[CC_Y] = m_macroblock.dc_dct_pred[CC_Cb] = m_macroblock.dc_dct_pred[CC_Cr] = LUT_dc_dct_pred[m_picture_coding_extension.intra_dc_precision];
			//
			//				/* ISO/IEC 13818-2 section 7.6.3.4 */
			//				//forward
			//				m_macroblock.PMV[0][0][0] = m_macroblock.PMV[0][0][1] = m_macroblock.PMV[1][0][0] = m_macroblock.PMV[1][0][1] = 0;
			//				//backward
			//				m_macroblock.PMV[0][1][0] = m_macroblock.PMV[0][1][1] = m_macroblock.PMV[1][1][0] = m_macroblock.PMV[1][1][1] = 0;
			//
			//				//		U32		nextbits;
			//				do
			//				{
			//					rtcode = mpgv_decode_macroblock(&bs);
			//
			//					//			nextbits = BITS_show(pbs, 23);
			//					//			if (nextbits == 0)
			//					//			{
			//					//				break;
			//					//			}
			//				} while ((rtcode >= 0) && (BITS_show(&bs, 23) != 0));
			//				//			} while ((rtcode >= 0) && (bs.p < bs.p_end));
			//				//		} while (rtcode >= 0);
			//			}
			//
			//			//		mpeg_video_next_start_code(pbs);
			//
			//			if (m_macroblock.macroblock_address >= (m_VidDecodeInfo.mb_count - 1))		//reach the last macroblock
			//			{
			//				mpgv_decode_reorder_display_frame(m_picture_header.picture_coding_type);
			//
			//#if MPEG_VID_FRAMERATE_DEBUG
			//				if (m_hFrameRateFile != NULL)
			//				{
			//					newtime = timeGetTime();
			//					fprintf(m_hFrameRateFile, "frame = %d, picture_coding_type = %d, picture_structure = %d, decoding_time = %d(ms)\n",
			//						m_nDebugFrameCount,
			//						m_picture_header.picture_coding_type,
			//						m_picture_coding_extension.picture_structure,
			//						newtime - oldtime);
			//				}
			//#endif
			//			}
			//		}

			if (pSliceParam == NULL)
			{
				delete pslice;
			}

			rtcode = MPV_NO_ERROR;
		}
		else
		{
			pxmlDoc->NewKeyValuePairElement(pxmlSliceNode, "mpgv slice parameter error !");
			rtcode = MPV_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPV_PARAMETER_ERROR;
	}

	return rtcode;
}

