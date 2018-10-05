#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

#include "../../Include/mpeg_audio_sequence.h"
#include "../../Include/mpeg_audio_errorcode.h"
#include "../../Include/mpeg_audio_utilities.h"

#include "../Include/mpeg_audio_sequence_xml.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int mpga_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = MPA_NO_ERROR;
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
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}

int mpga_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = MPA_NO_ERROR;
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
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


/*********************************************************************************
	函数：Decode_header()
	描述：解析MPEG音频数据流，要求数据必须帧对齐
	输入：pbs				-- 音频数据流
		  pheader			-- 参考的信息头数据，并将解析后的数据返回
	返回：0					-- 信息头未发生变化
		  1					-- 信息头发生变化
*********************************************************************************/
int	mpga_decode_frame_header_to_xml(uint8_t* es_header_buf, int es_header_length, XMLDocForMpegSyntax* pxmlDoc, MPA_header_t* pmpa_header)
{
	BITS_t	bs;
	int		rtcode = MPA_UNKNOWN_ERROR;
	int     header_size = 4;

	if ((es_header_buf != NULL) && (es_header_length >= 4) && (pmpa_header != NULL))
	{
		memset(pmpa_header, 0x00, sizeof(MPA_header_t));
		BITS_map(&bs, es_header_buf, es_header_length);

		pmpa_header->syncword = BITS_get(&bs, 12);
		pmpa_header->ID = BITS_get(&bs, 1);
		pmpa_header->layer = BITS_get(&bs, 2);
		pmpa_header->protection_bit = BITS_get(&bs, 1);

		pmpa_header->bitrate_index = BITS_get(&bs, 4);
		pmpa_header->sampling_frequency = BITS_get(&bs, 2);
		pmpa_header->padding_bit = BITS_get(&bs, 1);
		pmpa_header->private_bit = BITS_get(&bs, 1);

		pmpa_header->mode = BITS_get(&bs, 2);
		pmpa_header->mode_extension = BITS_get(&bs, 2);
		pmpa_header->copyright = BITS_get(&bs, 1);
		pmpa_header->original_or_copy = BITS_get(&bs, 1);
		pmpa_header->emphasis = BITS_get(&bs, 2);

		/*sematic part*/
		//pmpa_header->layer_index = 3 - pmpa_header->layer;
		//pmpa_header->bit_rate = MPA_bit_rate_table[pmpa_header->ID][pmpa_header->layer_index][pmpa_header->bitrate_index];
		//pmpa_header->sampling_rate = MPA_sampling_rate_table[pmpa_header->ID][pmpa_header->sampling_frequency];

		if (pmpa_header->protection_bit == 0)
		{
			pmpa_header->crc_check = BITS_get(&bs, 16);

			pmpa_header->crc_length = 2;
			header_size += 2;
		}
		else
		{
			pmpa_header->crc_length = 0;
		}

		pmpa_header->num_of_slots = (S32)(144 * pmpa_header->bit_rate / pmpa_header->sampling_rate) + pmpa_header->padding_bit;
		pmpa_header->data_length = pmpa_header->num_of_slots - header_size;

		pmpa_header->bitspersample = MPEG_AUDIO_BITS_PER_SAMPLE;

		if (pmpa_header->mode == MPEG_AUDIO_SINGLE_CHANNEL)				//singla channel
		{
			pmpa_header->nch = 1;
		}
		else
		{
			pmpa_header->nch = 2;
		}

		rtcode = MPA_NO_ERROR;
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


int	mpga_decode_frame_to_xml(uint8_t* es_frame_buf, int es_frame_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, MPA_frame_t* pMpaFrame)
{
	BITS_t	bs;
	int		rtcode = MPA_UNKNOWN_ERROR;
	int     header_size = 4;
	char    pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlFrameNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "frame()");
		pxmlDoc->UpdateBufMark(pxmlFrameNode, es_frame_buf, es_frame_buf + es_frame_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", es_frame_length);
		pxmlFrameNode->SetAttribute("comment", pszTemp);

		if ((es_frame_buf != NULL) && (es_frame_length >= 4))
		{
			MPA_frame_t* pmpa_frame = (pMpaFrame == NULL) ? new MPA_frame_t : pMpaFrame;
			memset(pmpa_frame, 0x00, sizeof(MPA_frame_t));
			BITS_map(&bs, es_frame_buf, es_frame_length);

			uint8_t* old_header_ptr = bs.p_cur;
			tinyxml2::XMLElement* pxmlHeaderNode = pxmlDoc->NewKeyValuePairElement(pxmlFrameNode, "header()");

			MPA_header_t* pmpa_header = &(pmpa_frame->header);

			pmpa_header->syncword = BITS_get(&bs, 12);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "syncword", pmpa_header->syncword, 12, "bslbf", NULL, &bs);

			pmpa_header->ID = BITS_get(&bs, 1);
			M2A_DecodeID(pmpa_header->ID, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "ID", pmpa_header->ID, 1, "bslbf", pszTemp, &bs);

			pmpa_header->layer = BITS_get(&bs, 2);
			M2A_DecodeLayer(pmpa_header->layer, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "layer", pmpa_header->layer, 2, "bslbf", pszTemp, &bs);

			pmpa_header->protection_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "protection_bit", pmpa_header->protection_bit, 1, "bslbf", NULL, &bs);

			pmpa_header->bitrate_index = BITS_get(&bs, 4);
			M2A_DecodeBitrate(pmpa_header->ID, pmpa_header->layer, pmpa_header->bitrate_index, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "bitrate_index", pmpa_header->bitrate_index, 4, "bslbf", pszTemp, &bs);

			pmpa_header->sampling_frequency = BITS_get(&bs, 2);
			M2A_DecodeSamplingFrequency(pmpa_header->ID, pmpa_header->sampling_frequency, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "sampling_frequency", pmpa_header->sampling_frequency, 2, "bslbf", pszTemp, &bs);

			pmpa_header->padding_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "padding_bit", pmpa_header->padding_bit, 1, "bslbf", NULL, &bs);
			pmpa_header->private_bit = BITS_get(&bs, 1);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "private_bit", pmpa_header->private_bit, 1, "bslbf", NULL, &bs);

			pmpa_header->mode = BITS_get(&bs, 2);
			M2A_DecodeMode(pmpa_header->mode, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "mode", pmpa_header->mode, 2, "bslbf", pszTemp, &bs);

			pmpa_header->mode_extension = BITS_get(&bs, 2);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "mode_extension", pmpa_header->mode_extension, 2, "bslbf", NULL, &bs);

			pmpa_header->copyright = BITS_get(&bs, 1);
			M2A_DecodeCopyright(pmpa_header->copyright, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "copyright", pmpa_header->copyright, 1, "bslbf", pszTemp, &bs);

			pmpa_header->original_or_copy = BITS_get(&bs, 1);
			M2A_DecodeOriginalOrCopy(pmpa_header->original_or_copy, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "original_or_copy", pmpa_header->original_or_copy, 1, "bslbf", pszTemp, &bs);

			pmpa_header->emphasis = BITS_get(&bs, 2);
			M2A_DecodeEmphasis(pmpa_header->emphasis, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "emphasis", pmpa_header->emphasis, 2, "bslbf", pszTemp, &bs);

			/*sematic part*/
			//pmpa_header->layer_index = 3 - pmpa_header->layer;
			//pmpa_header->bit_rate = MPA_bit_rate_table[pmpa_header->ID][pmpa_header->layer_index][pmpa_header->bitrate_index];
			//pmpa_header->sampling_rate = MPA_sampling_rate_table[pmpa_header->ID][pmpa_header->sampling_frequency];

			if (pmpa_header->protection_bit == 0)
			{
				pmpa_header->crc_check = BITS_get(&bs, 16);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "crc_check", pmpa_header->crc_check, 16, "bslbf", NULL, &bs);

				pmpa_header->crc_length = 2;
				header_size += 2;
			}
			else
			{
				pmpa_header->crc_length = 0;
			}

			//sematic part
			pmpa_header->num_of_slots = (int)(144 * pmpa_header->bit_rate / pmpa_header->sampling_rate) + pmpa_header->padding_bit;
			pmpa_header->data_length = pmpa_header->num_of_slots - header_size;

			pmpa_header->bitspersample = MPEG_AUDIO_BITS_PER_SAMPLE;

			if (pmpa_header->mode == MPEG_AUDIO_SINGLE_CHANNEL)				//singla channel
			{
				pmpa_header->nch = 1;
			}
			else
			{
				pmpa_header->nch = 2;
			}

			pxmlDoc->UpdateBufMark(pxmlHeaderNode, old_header_ptr, bs.p_cur);

			if (pMpaFrame == NULL)
			{
				delete pmpa_frame;
			}

			rtcode = MPA_NO_ERROR;
		}
		else
		{
			pxmlFrameNode->SetAttribute("error", "parameter error !");
			rtcode = MPA_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = MPA_PARAMETER_ERROR;
	}

	return rtcode;
}


