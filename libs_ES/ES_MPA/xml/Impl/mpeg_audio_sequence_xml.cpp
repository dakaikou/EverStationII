#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Include/mpeg_audio_sequence.h"
#include "../../Include/mpeg_audio_errorcode.h"
#include "../../Include/mpeg_audio_utilities.h"

#include "../Include/mpeg_audio_sequence_xml.h"

int mpga_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MPA_NO_ERROR;

	if ((nal_buf != NULL) && (nal_size > 0) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[ ]", nal_buf, nal_size);
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpga_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = MPA_NO_ERROR;

	if ((nal_buf != NULL) && (nal_size > 0) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[ ]", nal_buf, nal_size);
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


int	mpga_decode_frame_to_xml(uint8_t* es_frame_buf, int es_frame_size, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPA_frame_t* pMpaFrame)
{
	int		rtcode = MPA_UNKNOWN_ERROR;

	if ((es_frame_buf != NULL) && (es_frame_size > 0) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPA_frame_t* pmpa_frame = (pMpaFrame == NULL) ? new MPA_frame_t : pMpaFrame;
		rtcode = mpga_decode_frame(es_frame_buf, es_frame_size, pmpa_frame);
		mpga_present_frame_to_xml(pxmlDoc, pxmlParentNode, pmpa_frame);

		if (pMpaFrame == NULL)
		{
			delete pmpa_frame;
		}
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


int	mpga_present_frame_to_xml(HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, MPA_frame_t* pmpa_frame)
{
	int		rtcode = MPA_UNKNOWN_ERROR;
	char    pszID[32];
	char	pszLayer[32];
	char	pszSampling[32];
	char	pszBitrate[32];
	char	pszMode[32];
	char    pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmpa_frame != NULL))
	{
		MPA_header_t* pmpa_header = &(pmpa_frame->header);
		M2A_DecodeID(pmpa_header->ID, pszID, sizeof(pszID));
		M2A_DecodeLayer(pmpa_header->layer, pszLayer, sizeof(pszLayer));
		M2A_DecodeBitrate(pmpa_header->ID, pmpa_header->layer, pmpa_header->bitrate_index, pszBitrate, sizeof(pszBitrate));
		M2A_DecodeSamplingFrequency(pmpa_header->ID, pmpa_header->sampling_frequency, pszSampling, sizeof(pszSampling));
		M2A_DecodeMode(pmpa_header->mode, pszMode, sizeof(pszMode));

		sprintf_s(pszComment, sizeof(pszComment), "%s, %s, %s, %s, %s", pszID, pszLayer, pszBitrate, pszSampling, pszMode);

		XMLElement* pxmlFrameNode = pxmlDoc->NewBranchElement(pxmlParentNode, "frame()", pszComment, pmpa_frame->snapshot.length);

		XMLElement* pxmlHeaderNode = pxmlDoc->NewBranchElement(pxmlFrameNode, "header()", NULL, 4 + pmpa_header->crc_length);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "syncword", pmpa_header->syncword, 12, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "ID", pmpa_header->ID, 1, "bslbf", pszID);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "layer", pmpa_header->layer, 2, "bslbf", pszLayer);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "protection_bit", pmpa_header->protection_bit, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "bitrate_index", pmpa_header->bitrate_index, 4, "bslbf", pszBitrate);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "sampling_frequency", pmpa_header->sampling_frequency, 2, "bslbf", pszSampling);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "padding_bit", pmpa_header->padding_bit, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlHeaderNode, "private_bit", pmpa_header->private_bit, 1, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "mode", pmpa_header->mode, 2, "bslbf", pszMode);

		pxmlDoc->NewElementForBits(pxmlHeaderNode, "mode_extension", pmpa_header->mode_extension, 2, "bslbf", NULL);

		M2A_DecodeCopyright(pmpa_header->copyright, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlHeaderNode, "copyright", pmpa_header->copyright, 1, "bslbf", pszComment);

		M2A_DecodeOriginalOrCopy(pmpa_header->original_or_copy, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlHeaderNode, "original_or_copy", pmpa_header->original_or_copy, 1, "bslbf", pszComment);

		M2A_DecodeEmphasis(pmpa_header->emphasis, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlHeaderNode, "emphasis", pmpa_header->emphasis, 2, "bslbf", pszComment);

		if (pmpa_header->protection_bit == 0)
		{
			pxmlDoc->NewElementForBits(pxmlHeaderNode, "crc_check", pmpa_header->crc_check, 16, "bslbf", NULL);
		}

		pxmlDoc->NewElementForByteBuf(pxmlFrameNode, "audio_data[ ]", pmpa_frame->audio_data_buf, pmpa_frame->audio_data_length);

		rtcode = MPA_NO_ERROR;
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}
