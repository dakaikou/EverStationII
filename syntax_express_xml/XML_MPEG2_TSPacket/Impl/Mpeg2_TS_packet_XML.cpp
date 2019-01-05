#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_packet.h"
#include "syntax_translate_layer/MPEG2_TSPacket/Include/Mpeg2_TS_Utilities.h"

#include "../Include/Mpeg2_TS_packet_XML.h"

int MPEG_decode_TS_packet_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, transport_packet_t* pTSPacket)
{
	int	 rtcode = TSPACKET_PARSE_NO_ERROR;

	if ((buf != NULL) && ((length == 188) || (length == 204)) && (pxmlDoc != NULL))
	{
		transport_packet_t* ptransport_packet = (pTSPacket == NULL) ? new transport_packet_t : pTSPacket;

		rtcode = MPEG_decode_TS_packet(buf, length, ptransport_packet);
		rtcode = MPEG_present_TS_packet_to_XML(pxmlDoc, ptransport_packet);

		if (pTSPacket == NULL)
		{
			delete ptransport_packet;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG_present_TS_packet_to_XML(TALForXMLDoc* pxmlDoc, transport_packet_t* ptransport_packet)
{
	int	 rtcode = TSPACKET_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (ptransport_packet != NULL))
	{
		char pszComment[64];

		//根据ISO/IEC 13818-1:2000 版本组织XML编排

		//根节点
		int ts_packet_length = 4;
		if ((ptransport_packet->adaptation_field_control & 0b10) == 0b10) ts_packet_length += 1 + ptransport_packet->adaptation_field.adaptation_field_length;
		if ((ptransport_packet->adaptation_field_control & 0b01) == 0b01) ts_packet_length += ptransport_packet->payload_length;

		XMLElement* pxmlRootNode = pxmlDoc->NewRootElement("transport_packet()", NULL, ts_packet_length);

		if (rtcode != TSPACKET_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			pxmlRootNode->SetAttribute("error", pszComment);
		}

		pxmlDoc->NewElementForBits(pxmlRootNode, "sync_byte", ptransport_packet->sync_byte, 8, "bslbf", NULL);

		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_error_indicator", ptransport_packet->transport_error_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "payload_unit_start_indicator", ptransport_packet->payload_unit_start_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_priority", ptransport_packet->transport_priority, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlRootNode, "PID", ptransport_packet->PID, 13, "uimsbf", NULL);

		MPEG2_TS_NumericCoding2Text_transport_scrambling_control(ptransport_packet->transport_scrambling_control, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlRootNode, "transport_scrambling_control", ptransport_packet->transport_scrambling_control, 2, "bslbf", pszComment);
		MPEG2_TS_NumericCoding2Text_adaptation_field_control(ptransport_packet->adaptation_field_control, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlRootNode, "adaptation_field_control", ptransport_packet->adaptation_field_control, 2, "bslbf", pszComment);
		pxmlDoc->NewElementForBits(pxmlRootNode, "continuity_counter", ptransport_packet->continuity_counter, 4, "bslbf", NULL);

		int payload_start_pos = 4;
		if ((ptransport_packet->adaptation_field_control & 0b10) == 0b10)	//判断是不是有adaptation域
		{
			adaptation_field_t* padaptation_field = &(ptransport_packet->adaptation_field);

			int adaptation_field_length = 1 + padaptation_field->adaptation_field_length;

			XMLElement* pxmlAdaptationNode = pxmlDoc->NewBranchElement(pxmlRootNode, "adaptation_field()", NULL, adaptation_field_length);

			pxmlDoc->NewElementForBits(pxmlAdaptationNode, "adaptation_field_length", padaptation_field->adaptation_field_length, 8, "uimsbf", NULL);

			payload_start_pos += adaptation_field_length;

			if (padaptation_field->adaptation_field_length > 0)
			{
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "discontinuity_indicator", padaptation_field->discontinuity_indicator, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "random_access_indicator", padaptation_field->random_access_indicator, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "elementary_stream_priority_indicator", padaptation_field->elementary_stream_priority_indicator, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "PCR_flag", padaptation_field->PCR_flag, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "OPCR_flag", padaptation_field->OPCR_flag, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "splicing_point_flag", padaptation_field->splicing_point_flag, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "transport_private_data_flag", padaptation_field->transport_private_data_flag, 1, "bslbf", NULL);
				pxmlDoc->NewElementForBits(pxmlAdaptationNode, "adaptation_field_extension_flag", padaptation_field->adaptation_field_extension_flag, 1, "bslbf", NULL);

				if (padaptation_field->PCR_flag)
				{
					pxmlDoc->NewElementForBits(pxmlAdaptationNode, "program_clock_reference_base_32_30", padaptation_field->program_clock_reference_base_32_30, 3, "uimsbf", NULL);
					pxmlDoc->NewElementForBits(pxmlAdaptationNode, "program_clock_reference_base_29_15", padaptation_field->program_clock_reference_base_32_30, 15, "uimsbf", NULL);
					pxmlDoc->NewElementForBits(pxmlAdaptationNode, "program_clock_reference_base_14_0", padaptation_field->program_clock_reference_base_14_0, 15, "uimsbf", NULL);

					pxmlDoc->NewElementForBits(pxmlAdaptationNode, "reserved", padaptation_field->reserved0, 6, "bslbf", NULL);
					pxmlDoc->NewElementForBits(pxmlAdaptationNode, "program_clock_reference_extension", padaptation_field->program_clock_reference_extension, 9, "uimsbf", NULL);
				}
			}
		}

		//TS包净荷
		if ((ptransport_packet->adaptation_field_control & 0b01) == 0b01)	//判断是否有净荷
		{
			pxmlDoc->SetSyncOffset(payload_start_pos);
			pxmlDoc->NewElementForByteBuf(pxmlRootNode, "payload_buf[ ]", ptransport_packet->payload_buf, ptransport_packet->payload_length, NULL);
		}
	}

	return rtcode;
}
