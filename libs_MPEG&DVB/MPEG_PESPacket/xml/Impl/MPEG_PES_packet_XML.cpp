#include <string.h>
#include <assert.h>

#include "../../Include/MPEG_PES_ErrorCode.h"
#include "../../Include/MPEG_PES_packet.h"
#include "../../Include/MPEG_PES_Utilities.h"
#include "../../Include/Mpeg_stream_id.h"

#include "../Include/MPEG_PES_packet_XML.h"

int	MPEG_decode_PES_packet_to_xml(uint8_t *pes_buf, int pes_size, HALForXMLDoc* pxmlDoc, PES_packet_t* pPesPacket)
{
	int	 rtcode = PES_PACKET_NO_ERROR;

	PES_packet_t* ppes_packet = (pPesPacket != NULL) ? pPesPacket : new PES_packet_t;
	rtcode = MPEG_decode_PES_packet(pes_buf, pes_size, ppes_packet);
	rtcode = MPEG_present_PES_packet_to_xml(pxmlDoc, ppes_packet);

	if (pPesPacket == NULL)
	{
		//说明ppat_section指针临时分配，函数返回前需要释放
		delete ppes_packet;
	}

	return rtcode;
}

int	MPEG_present_PES_packet_to_xml(HALForXMLDoc* pxmlDoc, PES_packet_t* pPES_packet)
{
	int rtcode = PES_PACKET_NO_ERROR;

	char pszField[64];
	char pszComment[64];

	if ((pxmlDoc != NULL) && (pPES_packet != NULL))
	{
		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* xmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, xmlDeclaration);

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "PES_packet()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, 6 + pPES_packet->PES_packet_data_length);

		if ((pPES_packet->stream_id != PROGRAM_STREAM_MAP) &&
			(pPES_packet->stream_id != PADDING_STREAM) &&
			(pPES_packet->stream_id != PRIVATE_STREAM_2) &&
			(pPES_packet->stream_id != ECM_STREAM) &&
			(pPES_packet->stream_id != EMM_STREAM) &&
			(pPES_packet->stream_id != PROGRAM_STREAM_DIRECTORY) &&
			(pPES_packet->stream_id != DSMCC_STREAM) &&
			(pPES_packet->stream_id != TYPE_E_STREAM))
		{
			//PES包头
			XMLElement* pxmlHeaderNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "HEADER()", NULL);
			XMLNODE_SetFieldLength(pxmlHeaderNode, 6 + 3 + pPES_packet->PES_header_data_length);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "packet_start_code_prefix", pPES_packet->packet_start_code_prefix, 24, "bslbf", NULL);

			MPEG_PES_NumericCoding2Text_StreamID(pPES_packet->stream_id, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "stream_id", pPES_packet->stream_id, 8, "uimsbf", pszComment);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_packet_length", pPES_packet->PES_packet_length, 16, "uimsbf", NULL);

			if (pPES_packet->mpeg_flag == 0b10)
			{
				strcpy_s(pszComment, sizeof(pszComment), "MPEG-2");
			}
			else if (pPES_packet->mpeg_flag == 0b01)
			{
				strcpy_s(pszComment, sizeof(pszComment), "MPEG-1");
			}
			else
			{
				strcpy_s(pszComment, sizeof(pszComment), "Unknown");
			}
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "mpeg_flag", pPES_packet->mpeg_flag, 2, "bslbf", pszComment);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_scrambling_control", pPES_packet->PES_scrambling_control, 2, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_priority", pPES_packet->PES_priority, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "data_alignment_indicator", pPES_packet->data_alignment_indicator, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "copyright", pPES_packet->copyright, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "original_or_copy", pPES_packet->original_or_copy, 1, "bslbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PTS_DTS_flags", pPES_packet->PTS_DTS_flags, 2, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "ESCR_flag", pPES_packet->ESCR_flag, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "ES_rate_flag", pPES_packet->ES_rate_flag, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "DSM_trick_mode_flag", pPES_packet->DSM_trick_mode_flag, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "additional_copy_info_flag", pPES_packet->additional_copy_info_flag, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_CRC_flag", pPES_packet->PES_CRC_flag, 1, "bslbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_extension_flag", pPES_packet->PES_extension_flag, 1, "bslbf", NULL);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_header_data_length", pPES_packet->PES_header_data_length, 8, "uimsbf", NULL);

			if (pPES_packet->PES_header_data_length > 0)
			{
				if ((pPES_packet->PTS_DTS_flags & 0b10) == 0b10)			//have PTS，5个字节
				{
					XMLElement* pxmlPtsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlHeaderNode, "PTS()", NULL);
					XMLNODE_SetFieldLength(pxmlPtsNode, 5);

					if (pPES_packet->PTS_DTS_flags == 0b10)
					{
						sprintf_s(pszField, sizeof(pszField), "'0010'");
					}
					else if (pPES_packet->PTS_DTS_flags == 0b11)
					{
						sprintf_s(pszField, sizeof(pszField), "'0011'");
					}
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, pszField, pPES_packet->PTS_marker, 4, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, "PTS[32..30]", pPES_packet->PTS_32_30, 3, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, "marker_bit", pPES_packet->PTS_marker_bit1, 1, "bslbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, "PTS[29..15]", pPES_packet->PTS_29_15, 15, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, "marker_bit", pPES_packet->PTS_marker_bit2, 1, "bslbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, "PTS[14..0]", pPES_packet->PTS_14_0, 15, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPtsNode, "marker_bit", pPES_packet->PTS_marker_bit3, 1, "bslbf", NULL);

					if ((pPES_packet->PTS_DTS_flags & 0b01) == 0b01)			// DTS，5个字节
					{
						XMLElement* pxmlDtsNode = XMLDOC_NewElementForString(pxmlDoc, pxmlHeaderNode, "DTS()", NULL);
						XMLNODE_SetFieldLength(pxmlDtsNode, 5);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "'0001'", pPES_packet->DTS_marker, 4, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "DTS[32..30]", pPES_packet->DTS_32_30, 3, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "marker_bit", pPES_packet->DTS_marker_bit1, 1, "bslbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "DTS[29..15]", pPES_packet->DTS_29_15, 15, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "marker_bit", pPES_packet->DTS_marker_bit2, 1, "bslbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "DTS[14..0]", pPES_packet->DTS_14_0, 15, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlDtsNode, "marker_bit", pPES_packet->DTS_marker_bit3, 1, "bslbf", NULL);
					}
				}

				if (pPES_packet->ESCR_flag)
				{
					assert(0);

					XMLElement* pxmlESCRNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "ESCR()", NULL);
					XMLNODE_SetFieldLength(pxmlESCRNode, 6);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "reserved", pPES_packet->ESCR_reserved, 2, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "ESCR_base[32..30]", pPES_packet->ESCR_base_32_30, 3, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "marker_bit", pPES_packet->ESCR_marker_bit0, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "ESCR_base[29..15]", pPES_packet->ESCR_base_29_15, 15, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "marker_bit", pPES_packet->ESCR_marker_bit1, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "ESCR_base[14..0]", pPES_packet->ESCR_base_14_0, 15, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "marker_bit", pPES_packet->ESCR_marker_bit2, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "ESCR_extension", pPES_packet->ESCR_extension, 9, "uimsbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESCRNode, "marker_bit", pPES_packet->ESCR_marker_bit3, 1, "bslbf", NULL);
				}

				if (pPES_packet->ES_rate_flag)
				{
					assert(0);
					XMLElement* pxmlESRateNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "ES_rate()", NULL);
					XMLNODE_SetFieldLength(pxmlESRateNode, 3);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlESRateNode, "marker_bit", pPES_packet->ES_rate_marker_bit0, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESRateNode, "ES_rate", pPES_packet->ES_rate, 22, "uimsbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlESRateNode, "marker_bit", pPES_packet->ES_rate_marker_bit1, 1, "bslbf", NULL);
				}

				if (pPES_packet->DSM_trick_mode_flag)
				{
					assert(0);
					XMLElement* pxmlTrickModeNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "DSM_trick_mode()", NULL);
					XMLNODE_SetFieldLength(pxmlTrickModeNode, 1);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "trick_mode_control", pPES_packet->trick_mode_control, 3, "uimsbf", NULL);

					if (pPES_packet->trick_mode_control == FAST_FORWARD)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "field_id", pPES_packet->trick_mode.fast_forward.field_id, 2, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "intra_slice_refresh", pPES_packet->trick_mode.fast_forward.intra_slice_refresh, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "frequency_truncation", pPES_packet->trick_mode.fast_forward.frequency_truncation, 2, "bslbf", NULL);
					}
					else if (pPES_packet->trick_mode_control == SLOW_MOTION)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "rep_cntrl", pPES_packet->trick_mode.slow_motion.rep_cntrl, 5, "uimsbf", NULL);
					}
					else if (pPES_packet->trick_mode_control == FREEZE_FRAME)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "field_id", pPES_packet->trick_mode.freeze_frame.field_id, 2, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "reversed", pPES_packet->trick_mode.freeze_frame.reserved, 3, "bslbf", NULL);

					}
					else if (pPES_packet->trick_mode_control == FAST_REVERSE)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "field_id", pPES_packet->trick_mode.fast_reverse.field_id, 2, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "intra_slice_refresh", pPES_packet->trick_mode.fast_reverse.intra_slice_refresh, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "frequency_truncation", pPES_packet->trick_mode.fast_reverse.frequency_truncation, 2, "bslbf", NULL);
					}
					else if (pPES_packet->trick_mode_control == SLOW_REVERSE)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "rep_cntrl", pPES_packet->trick_mode.slow_reverse.rep_cntrl, 5, "uimsbf", NULL);
					}
					else
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlTrickModeNode, "reserved", pPES_packet->trick_mode.reserved, 5, "uimsbf", NULL);
					}
				}

				if (pPES_packet->additional_copy_info_flag) 
				{
					XMLElement* pxmlCopyInfoNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "additional_copy_info()", NULL);
					XMLNODE_SetFieldLength(pxmlCopyInfoNode, 1);

					assert(0);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlCopyInfoNode, "marker_bit", pPES_packet->additional_copy_info_marker_bit, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlCopyInfoNode, "additional_copy_info", pPES_packet->additional_copy_info, 7, "bslbf", NULL);
				}

				if (pPES_packet->PES_CRC_flag) 
				{
					assert(0);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "previous_PES_packet_CRC", pPES_packet->previous_PES_packet_CRC, 16, "bslbf", NULL);
				}

				if (pPES_packet->PES_extension_flag) 
				{
					assert(0);

					XMLElement* pxmlPesExtensionNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "PES_extension()", NULL);

					int pes_extension_length = 1;
					if (pPES_packet->PES_private_data_flag == 1) pes_extension_length += 16;
					if (pPES_packet->pack_header_field_flag == 1) pes_extension_length += (1 + pPES_packet->pack_field_length);
					if (pPES_packet->program_packet_sequence_counter_flag == 1) pes_extension_length += 2;
					if (pPES_packet->P_STD_buffer_flag == 1) pes_extension_length += 2;
					if (pPES_packet->PES_extension_flag_2 == 1) pes_extension_length += (1 + pPES_packet->PES_extension_field_length);

					XMLNODE_SetFieldLength(pxmlPesExtensionNode, pes_extension_length);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlPesExtensionNode, "PES_private_data_flag", pPES_packet->PES_private_data_flag, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPesExtensionNode, "pack_header_field_flag", pPES_packet->pack_header_field_flag, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPesExtensionNode, "program_packet_sequence_counter_flag", pPES_packet->program_packet_sequence_counter_flag, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPesExtensionNode, "P-STD_buffer_flag", pPES_packet->P_STD_buffer_flag, 1, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPesExtensionNode, "reserved", pPES_packet->PES_extension_reserved, 3, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlPesExtensionNode, "PES_extension_flag_2", pPES_packet->PES_extension_flag_2, 1, "bslbf", NULL);

					if (pPES_packet->PES_private_data_flag == 1)
					{
						XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlHeaderNode, "PES_private_data[ ]", pPES_packet->PES_private_data_byte, pPES_packet->PES_private_data_length, NULL);
					}
					if (pPES_packet->pack_header_field_flag == 1)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "pack_field_length", pPES_packet->pack_field_length, 8, "uimsbf", NULL);
						if (pPES_packet->pack_field_length > 0)
						{
							XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlHeaderNode, "pack_header()", pPES_packet->pack_header_data_byte, pPES_packet->pack_field_length, NULL);
						}
					}
					if (pPES_packet->program_packet_sequence_counter_flag == 1)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "marker_bit", pPES_packet->program_packet_marker_bit0, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "program_packet_sequence_counter", pPES_packet->program_packet_sequence_counter, 7, "uimsbf", NULL);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "marker_bit", pPES_packet->program_packet_marker_bit1, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "MPEG1_MPEG2_identifier", pPES_packet->MPEG1_MPEG2_identifier, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "original_stuff_length", pPES_packet->original_stuff_length, 6, "uimsbf", NULL);
					}
					if (pPES_packet->P_STD_buffer_flag == 1)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "'01'", pPES_packet->P_STD_marker, 2, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "P-STD_buffer_scale", pPES_packet->P_STD_buffer_scale, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "P-STD_buffer_size", pPES_packet->P_STD_marker, 13, "uimsbf", NULL);
					}
					if (pPES_packet->PES_extension_flag_2 == 1)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "marker_bit", pPES_packet->PES_extension_field_marker_bit, 1, "bslbf", NULL);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlHeaderNode, "PES_extension_field_length", pPES_packet->PES_extension_field_length, 7, "uimsbf", NULL);

						if (pPES_packet->PES_extension_field_length > 0)
						{
							XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlHeaderNode, "reserved[ ]", pPES_packet->PES_extension_field_data_byte, pPES_packet->PES_extension_field_length, NULL);
						}
					}
				}

				if (pPES_packet->stuffing_length > 0)
				{
					XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlHeaderNode, "stuffing_byte[ ]", pPES_packet->stuffing_byte, pPES_packet->stuffing_length, NULL);
				}
			}

			if (pPES_packet->payload_length > 0)
			{
				XMLElement* pxmlPayloadNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "PAYLOAD()", NULL);
				XMLNODE_SetFieldLength(pxmlPayloadNode, pPES_packet->payload_length);

				XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlPayloadNode, "payload_data_byte[ ]", pPES_packet->payload_buf, pPES_packet->payload_length, NULL);
			}
		}
		else
		{
			assert(0);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "packet_start_code_prefix", pPES_packet->packet_start_code_prefix, 24, "bslbf", NULL);

			MPEG_PES_NumericCoding2Text_StreamID(pPES_packet->stream_id, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "stream_id", pPES_packet->stream_id, 8, "uimsbf", pszComment);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "PES_packet_length", pPES_packet->PES_packet_length, 16, "uimsbf", NULL);

			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlRootNode, "PES_packet_data_byte[ ]", pPES_packet->PES_packet_data_byte, pPES_packet->PES_packet_data_length, NULL);
		}
		//else if (pPES_packet->stream_id == PROGRAM_STREAM_MAP ||
		//	pPES_packet->stream_id == PRIVATE_STREAM_2 ||
		//	pPES_packet->stream_id == ECM_STREAM ||
		//	pPES_packet->stream_id == EMM_STREAM ||
		//	pPES_packet->stream_id == PROGRAM_STREAM_DIRECTORY ||
		//	pPES_packet->stream_id == DSMCC_STREAM ||
		//	pPES_packet->stream_id == TYPE_E_STREAM)
		//{
		//	assert(0);
		//	sprintf_s(pszComment, sizeof(pszComment), "stream_id=0x%02X", pPES_packet->stream_id);
		//	XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlRootNode, "PES_packet_data_byte[ ]", pPES_packet->PES_packet_data_byte, pPES_packet->PES_packet_length, pszComment);
		//}
		//else if (pPES_packet->stream_id == PADDING_STREAM)
		//{
		//	assert(0);
		//	sprintf_s(pszComment, sizeof(pszComment), "stream_id=0x%02X", pPES_packet->stream_id);
		//	XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlRootNode, "PES_packet_data_byte[ ]", pPES_packet->PES_packet_data_byte, pPES_packet->PES_packet_length, pszComment);
		//}
		//else
		//{
		//	assert(0);
		//	sprintf_s(pszComment, sizeof(pszComment), "stream_id=0x%02X", pPES_packet->stream_id);
		//	XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlRootNode, "PES_packet_data_byte[ ]", pPES_packet->PES_packet_data_byte, pPES_packet->PES_packet_length, pszComment);
		//}
		//else
		//{
		//	sprintf_s(pszComment, sizeof(pszComment), "stream_id=0x%02X", pPES_packet->stream_id);
		//	XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlRootNode, "PES_packet_data_byte[ ]", pPES_packet->payload_buf, pPES_packet->payload_length, pszComment);
		//}
	}
	else
	{
		rtcode = PES_PACKET_PARAMETER_ERROR;
	}

	return rtcode;
}
