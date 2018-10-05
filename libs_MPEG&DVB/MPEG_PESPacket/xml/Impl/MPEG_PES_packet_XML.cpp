#include <string.h>
#include <assert.h>

#include "../../Include/MPEG_PES_ErrorCode.h"
#include "../../Include/MPEG_PES_packet.h"
#include "../../Include/MPEG_PES_Utilities.h"
#include "../../Include/Mpeg_stream_id.h"

#include "../Include/MPEG_PES_packet_XML.h"

#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int	MPEG_decode_PES_packet_to_xml(uint8_t *pes_buf, int pes_length, XMLDocForMpegSyntax* pxmlDoc, PES_packet_t* pParam)
{
	int rtcode = PES_PACKET_NO_ERROR;

	uint64_t	dts_pos = 0;
	uint64_t	pts_pos = 0;
	char pszTemp[64];
	unsigned char* packet_start_ptr;
	unsigned char* packet_end_ptr;
	BITS_t		   bs;

	if (pxmlDoc != NULL)
	{
		pxmlDoc->SetOrigin(pes_buf);

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		tinyxml2::XMLDeclaration* xmlDeclaration = pxmlDoc->NewDeclaration(pszDeclaration);
		pxmlDoc->InsertFirstChild(xmlDeclaration);

		packet_start_ptr = pes_buf;
		packet_end_ptr = pes_buf + pes_length;

		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", pes_length);
		//根节点
		tinyxml2::XMLElement* pxmlRootNode = ((tinyxml2::XMLDocument*)pxmlDoc)->NewElement("PES_packet()");
		pxmlDoc->InsertEndChild(pxmlRootNode);
		pxmlDoc->UpdateBufMark(pxmlRootNode, packet_start_ptr, packet_end_ptr);
		pxmlRootNode->SetAttribute("comment", pszTemp);

		if ((pes_buf != NULL) && (pes_length > 4))
		{
			PES_packet_t* pPES_packet = (pParam == NULL) ? new PES_packet_t : pParam;
			memset(pPES_packet, 0x00, sizeof(PES_packet_t));

			BITS_map(&bs, pes_buf, pes_length);

			uint8_t* old_header_ptr = bs.p_cur;
			//TS包头
			tinyxml2::XMLElement* pxmlHeaderNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "HEADER()");

			pPES_packet->packet_start_code_prefix = BITS_get(&bs, 24);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "packet_start_code_prefix", pPES_packet->packet_start_code_prefix, 24, "bslbf", NULL, &bs);

			pPES_packet->stream_id = BITS_get(&bs, 8);
			MPEG_PES_NumericCoding2Text_StreamID(pPES_packet->stream_id, pszTemp, sizeof(pszTemp));
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "stream_id", pPES_packet->stream_id, 8, "uimsbf", pszTemp, &bs);

			pPES_packet->PES_packet_length = BITS_get(&bs, 16);
			pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PES_packet_length", pPES_packet->PES_packet_length, 16, "uimsbf", NULL, &bs);

			if ((pPES_packet->stream_id != PROGRAM_STREAM_MAP) &&
				(pPES_packet->stream_id != PADDING_STREAM) &&
				(pPES_packet->stream_id != PRIVATE_STREAM_2) &&
				(pPES_packet->stream_id != ECM_STREAM) &&
				(pPES_packet->stream_id != EMM_STREAM) &&
				(pPES_packet->stream_id != PROGRAM_STREAM_DIRECTORY) &&
				(pPES_packet->stream_id != DSMCC_STREAM) &&
				(pPES_packet->stream_id != TYPE_E_STREAM))
			{
				pPES_packet->mpeg_flag = BITS_get(&bs, 2);
				if (pPES_packet->mpeg_flag == 0b10)
				{
					strcpy_s(pszTemp, sizeof(pszTemp), "MPEG-2");
				}
				else if (pPES_packet->mpeg_flag == 0b01)
				{
					strcpy_s(pszTemp, sizeof(pszTemp), "MPEG-1");
				}
				else
				{
					strcpy_s(pszTemp, sizeof(pszTemp), "Unknown");
				}
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "mpeg_flag", pPES_packet->mpeg_flag, 2, "bslbf", pszTemp, &bs);

				pPES_packet->PES_scrambling_control = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PES_scrambling_control", pPES_packet->PES_scrambling_control, 2, "bslbf", NULL, &bs);

				pPES_packet->PES_priority = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PES_priority", pPES_packet->PES_priority, 1, "bslbf", NULL, &bs);

				pPES_packet->data_alignment_indicator = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "data_alignment_indicator", pPES_packet->data_alignment_indicator, 1, "bslbf", NULL, &bs);

				pPES_packet->copyright = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "copyright", pPES_packet->copyright, 1, "bslbf", NULL, &bs);

				pPES_packet->original_or_copy = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "original_or_copy", pPES_packet->original_or_copy, 1, "bslbf", NULL, &bs);

				pPES_packet->PTS_DTS_flags = BITS_get(&bs, 2);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PTS_DTS_flags", pPES_packet->PTS_DTS_flags, 2, "bslbf", NULL, &bs);

				pPES_packet->ESCR_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "ESCR_flag", pPES_packet->ESCR_flag, 1, "bslbf", NULL, &bs);

				pPES_packet->ES_rate_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "ES_rate_flag", pPES_packet->ES_rate_flag, 1, "bslbf", NULL, &bs);

				pPES_packet->DSM_trick_mode_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "DSM_trick_mode_flag", pPES_packet->DSM_trick_mode_flag, 1, "bslbf", NULL, &bs);

				pPES_packet->additional_copy_info_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "additional_copy_info_flag", pPES_packet->additional_copy_info_flag, 1, "bslbf", NULL, &bs);

				pPES_packet->PES_CRC_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PES_CRC_flag", pPES_packet->PES_CRC_flag, 1, "bslbf", NULL, &bs);

				pPES_packet->PES_extension_flag = BITS_get(&bs, 1);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PES_extension_flag", pPES_packet->PES_extension_flag, 1, "bslbf", NULL, &bs);

				pPES_packet->PES_header_data_length = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PES_header_data_length", pPES_packet->PES_header_data_length, 8, "uimsbf", NULL, &bs);

				if (pPES_packet->PES_header_data_length > 0)
				{
					BITS_t pts_dts_bs;
					BITS_map(&pts_dts_bs, bs.p_cur, pPES_packet->PES_header_data_length);
					BITS_byteSkip(&bs, pPES_packet->PES_header_data_length);

					if ((pPES_packet->PTS_DTS_flags & 0b10) == 0b10)			//have PTS，5个字节
					{
						pPES_packet->PTS_marker = BITS_get(&pts_dts_bs, 4);
						//assert((pPES_packet->PTS_marker == 0b0010) || (pPES_packet->PTS_marker == 0b0011));

						if (pPES_packet->PTS_DTS_flags == 0b10)
						{
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "'0010'", pPES_packet->PTS_marker, 4, "bslbf", NULL, &pts_dts_bs);
						}
						else if (pPES_packet->PTS_DTS_flags == 0b11)
						{
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "'0011'", pPES_packet->PTS_marker, 4, "bslbf", NULL, &pts_dts_bs);
						}

						pPES_packet->PTS_32_30 = BITS_get(&pts_dts_bs, 3);
						pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PTS[32..30]", pPES_packet->PTS_32_30, 3, "bslbf", NULL, &pts_dts_bs);

						pPES_packet->PTS_marker_bit1 = BITS_get(&pts_dts_bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "marker_bit", pPES_packet->PTS_marker_bit1, 1, "bslbf", NULL, &pts_dts_bs);

						pPES_packet->PTS_29_15 = BITS_get(&pts_dts_bs, 15);
						pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PTS[29..15]", pPES_packet->PTS_29_15, 15, "bslbf", NULL, &pts_dts_bs);

						pPES_packet->PTS_marker_bit2 = BITS_get(&pts_dts_bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "marker_bit", pPES_packet->PTS_marker_bit2, 1, "bslbf", NULL, &pts_dts_bs);

						pPES_packet->PTS_14_0 = BITS_get(&pts_dts_bs, 15);
						pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "PTS[14..0]", pPES_packet->PTS_14_0, 15, "bslbf", NULL, &pts_dts_bs);

						pPES_packet->PTS_marker_bit3 = BITS_get(&pts_dts_bs, 1);
						pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "marker_bit", pPES_packet->PTS_marker_bit3, 1, "bslbf", NULL, &pts_dts_bs);

						if ((pPES_packet->PTS_DTS_flags & 0b01) == 0b01)			// DTS，5个字节
						{
							pPES_packet->DTS_marker = BITS_get(&pts_dts_bs, 4);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "'0001'", pPES_packet->DTS_marker, 4, "bslbf", NULL, &pts_dts_bs);

							pPES_packet->DTS_32_30 = BITS_get(&pts_dts_bs, 3);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "DTS[32..30]", pPES_packet->DTS_32_30, 3, "bslbf", NULL, &pts_dts_bs);

							pPES_packet->DTS_marker_bit1 = BITS_get(&pts_dts_bs, 1);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "marker_bit", pPES_packet->DTS_marker_bit1, 1, "bslbf", NULL, &pts_dts_bs);

							pPES_packet->DTS_29_15 = BITS_get(&pts_dts_bs, 15);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "DTS[29..15]", pPES_packet->DTS_29_15, 15, "bslbf", NULL, &pts_dts_bs);

							pPES_packet->DTS_marker_bit2 = BITS_get(&pts_dts_bs, 1);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "marker_bit", pPES_packet->DTS_marker_bit2, 1, "bslbf", NULL, &pts_dts_bs);

							pPES_packet->DTS_14_0 = BITS_get(&pts_dts_bs, 15);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "DTS[14..0]", pPES_packet->DTS_14_0, 15, "bslbf", NULL, &pts_dts_bs);

							pPES_packet->DTS_marker_bit3 = BITS_get(&pts_dts_bs, 1);
							pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "marker_bit", pPES_packet->DTS_marker_bit3, 1, "bslbf", NULL, &pts_dts_bs);
						}
					}

					if (pPES_packet->ESCR_flag)
					{
						//pPES_packet->ESCR_base_32_30 = BITS_get(&pts_dts_bs, 3);

						//pPES_packet->ESCR_base_29_15 = BITS_get(&pts_dts_bs, 15);

						//pPES_packet->ESCR_base_14_0 = BITS_get(&pts_dts_bs, 15);

						//pPES_packet->ESCR_extension = BITS_get(&pts_dts_bs, 2);
						//pPES_packet->ESCR_extension <<= 7;
						//pPES_packet->ESCR_extension |= (*ptemp++ & 0xFE) >> 1;
					}

					if (pPES_packet->ES_rate_flag)
					{
						//pPES_packet->ES_rate = *ptemp++ & 0x7F;
						//pPES_packet->ES_rate <<= 8;
						//pPES_packet->ES_rate |= *ptemp++;
						//pPES_packet->ES_rate <<= 7;
						//pPES_packet->ES_rate |= (*ptemp++ & 0xFE) >> 1;
					}

					if (pPES_packet->DSM_trick_mode_flag)
					{
						//pPES_packet->trick_mode_control = (*ptemp & 0xE0) >> 5;

						//if (pPES_packet->trick_mode_control == FAST_FORWARD) {

						//	pPES_packet->field_id = (*ptemp & 0x18) >> 3;
						//	pPES_packet->intra_slice_refresh = (*ptemp & 0x04) >> 2;
						//	pPES_packet->frequency_truncation = *ptemp++ & 0x03;
						//}
						//else if (pPES_packet->trick_mode_control == SLOW_MOTION) {

						//	pPES_packet->rep_cntrl = *ptemp++ & 0x1F;
						//}
						//else if (pPES_packet->trick_mode_control == FREEZE_FRAME) {

						//	pPES_packet->field_id = (*ptemp++ & 0x18) >> 3;
						//}
						//else if (pPES_packet->trick_mode_control == FAST_REVERSE) {

						//	pPES_packet->field_id = (*ptemp & 0x18) >> 3;
						//	pPES_packet->intra_slice_refresh = (*ptemp & 0x04) >> 2;
						//	pPES_packet->frequency_truncation = *ptemp++ & 0x03;
						//}
						//else if (pPES_packet->trick_mode_control == SLOW_REVERSE) {

						//	pPES_packet->rep_cntrl = *ptemp++ & 0x1F;
						//}
						//else {
						//	ptemp++;
						//}
					}

					if (pPES_packet->additional_copy_info_flag) {

						//pPES_packet->additional_copy_info = *ptemp++ & 0x7F;
					}

					if (pPES_packet->PES_CRC_flag) {

						//pPES_packet->previous_PES_packet_CRC = *ptemp++;
						//pPES_packet->previous_PES_packet_CRC <<= 8;
						//pPES_packet->previous_PES_packet_CRC |= *ptemp++;
					}

					if (pPES_packet->PES_extension_flag) {

						//pPES_packet->PES_private_data_flag = (*ptemp & 0x80) >> 7;
						//pPES_packet->pack_header_field_flag = (*ptemp & 0x40) >> 6;
						//pPES_packet->program_packet_sequence_counter_flag = (*ptemp & 0x20) >> 5;
						//pPES_packet->P_STD_buffer_flag = (*ptemp & 0x10) >> 4;
						//pPES_packet->PES_extension_flag_2 = *ptemp++ & 0x01;

						//if (pPES_packet->PES_private_data_flag) {

						//	memcpy(pPES_packet->PES_private_data, ptemp, 16);
						//	ptemp += 16;
						//}
						//if (pPES_packet->pack_header_field_flag) {

						//	pPES_packet->pack_field_length = *ptemp++;
						//	pPES_packet->ppack_header = ptemp;

						//	ptemp += pPES_packet->pack_field_length;
						//}
						//if (pPES_packet->program_packet_sequence_counter_flag) {

						//	pPES_packet->program_packet_sequence_counter = *ptemp++ & 0x7F;
						//	pPES_packet->MPEG1_MPEG2_identifier = (*ptemp & 0x40) >> 6;
						//	pPES_packet->original_stuff_length = *ptemp++ & 0x3F;
						//}
						//if (pPES_packet->P_STD_buffer_flag) {

						//	pPES_packet->P_STD_buffer_scale = (*ptemp & 20) >> 5;
						//	pPES_packet->P_STD_buffer_size = *ptemp++ & 0x1F;
						//	pPES_packet->P_STD_buffer_size <<= 8;
						//	pPES_packet->P_STD_buffer_size |= *ptemp++;
						//}
						//if (pPES_packet->PES_extension_flag_2) {

						//	pPES_packet->PES_extension_field_length = *ptemp++ & 0x7F;
						//	ptemp += pPES_packet->PES_extension_field_length;
						//}

					}
				}

				pxmlDoc->UpdateBufMark(pxmlHeaderNode, packet_start_ptr, bs.p_cur);

				sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", (int)(bs.p_cur - packet_start_ptr));
				pxmlHeaderNode->SetAttribute("comment", pszTemp);

				pPES_packet->es_payload_buf = bs.p_cur;
				pPES_packet->es_payload_length = (int)(packet_end_ptr - bs.p_cur);

				if (pPES_packet->es_payload_length > 0)
				{
					tinyxml2::XMLElement* pxmlPayloadNode = pxmlDoc->NewKeyValuePairElement(pxmlRootNode, "PAYLOAD()");
					pxmlDoc->UpdateBufMark(pxmlPayloadNode, bs.p_cur, packet_end_ptr);

					sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", (int)(packet_end_ptr - bs.p_cur));
					pxmlPayloadNode->SetAttribute("comment", pszTemp);

					//int i;
					//for (i = 0; i < min(16, pPES_packet->es_payload_length); i++)
					//{
					//	sprintf_s(pszTemp + i * 3, 4, "%02X ", buf[i]);
					//}
					//if (pPES_packet->es_payload_length > 16)
					//{
					//	sprintf_s(pszTemp + i * 3, 5, "... ");
					//}
					//else
					//{
					//	pszTemp[i * 3] = '\0';
					//}

					//pxmlDoc->NewElement(pxmlPayload, pszTemp, -1, NULL, -1, buf, packet_end_ptr);


					//if (pPES_packet->PES_packet_length == 0x00) {// video

					//											 //			pPES_packet->pPES_packet_data_byte = NULL;
					//}
					//else {//audio, video or other stream

					//	  //			pPES_packet->pPES_packet_data_byte = buf;
					//	//buf += pPES_packet->PES_packet_length - pPES_packet->PES_header_data_length - 3;
					//}
				}
			}
			else if (pPES_packet->stream_id == PROGRAM_STREAM_MAP ||
				pPES_packet->stream_id == PRIVATE_STREAM_2 ||
				pPES_packet->stream_id == ECM_STREAM ||
				pPES_packet->stream_id == EMM_STREAM ||
				pPES_packet->stream_id == PROGRAM_STREAM_DIRECTORY ||
				pPES_packet->stream_id == DSMCC_STREAM ||
				pPES_packet->stream_id == TYPE_E_STREAM)
			{

				//		pPES_packet->pPES_packet_data_byte = buf;
				//buf += pPES_packet->PES_packet_length;
				BITS_byteSkip(&bs, pPES_packet->PES_packet_length);
			}
			else if (pPES_packet->stream_id == PADDING_STREAM)
			{

				//		pPES_packet->pPES_packet_data_byte = NULL;
				//buf += pPES_packet->PES_packet_length;
				BITS_byteSkip(&bs, pPES_packet->PES_packet_length);
			}
			else
			{
				//		pPES_packet->pPES_packet_data_byte = NULL;
				//buf += pPES_packet->PES_packet_length;
				BITS_byteSkip(&bs, pPES_packet->PES_packet_length);
			}

			//	PES_header.pts_pos = pts_pos;
			//	PES_header.dts_pos = dts_pos;
			if (pParam == NULL)
			{
				delete pPES_packet;
			}
		}
		else
		{
			pxmlRootNode->SetAttribute("error", "parameter error!");
			rtcode = PES_PACKET_PARAMETER_ERROR;
		}
	}
	else
	{
		rtcode = PES_PACKET_PARAMETER_ERROR;
	}

	return rtcode;
}
