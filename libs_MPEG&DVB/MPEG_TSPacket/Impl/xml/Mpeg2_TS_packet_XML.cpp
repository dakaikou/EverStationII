#include <string.h>
#include <stdio.h>
#include <assert.h>
//#include <math.h>

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

#include "../../Include/Mpeg2_TS_ErrorCode.h"
#include "../../Include/Mpeg2_TS_packet.h"
#include "../../Include/Mpeg2_TS_Utilities.h"
#include "../../Include/xml/Mpeg2_TS_packet_XML.h"

int MPEG_decode_TS_packet_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, transport_packet_t* pTSPacket)
{
	int	 rtcode = TSPACKET_PARSE_NO_ERROR;

	transport_packet_t* ptransport_packet = (pTSPacket == NULL) ? new transport_packet_t : pTSPacket;

	rtcode = MPEG_decode_TS_packet(buf, length, ptransport_packet);

	if (pxmlDoc != NULL)
	{
		char pszComment[64];

		const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

		XMLDeclaration* pxmlDeclaration = XMLDOC_NewDeclaration(pxmlDoc, pszDeclaration);
		XMLDOC_InsertFirstChild(pxmlDoc, pxmlDeclaration);

		//根据ISO/IEC 13818-1:2000 版本组织XML编排

		//根节点
		XMLElement* pxmlRootNode = XMLDOC_NewRootElement(pxmlDoc, "transport_packet()");
		XMLDOC_InsertEndChild(pxmlDoc, pxmlRootNode);
		XMLNODE_SetFieldLength(pxmlRootNode, length);

		sprintf_s(pszComment, sizeof(pszComment), "%d字节", length);
		XMLNODE_SetAttribute(pxmlRootNode, "comment", pszComment);

		if (rtcode != TSPACKET_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			XMLNODE_SetAttribute(pxmlRootNode, "error", pszComment);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "sync_byte", ptransport_packet->sync_byte, 8, "bslbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_error_indicator", ptransport_packet->transport_error_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "payload_unit_start_indicator", ptransport_packet->payload_unit_start_indicator, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_priority", ptransport_packet->transport_priority, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "PID", ptransport_packet->PID, 13, "uimsbf", NULL);

		MPEG2_TS_NumericCoding2Text_transport_scrambling_control(ptransport_packet->transport_scrambling_control, pszComment, sizeof(pszComment));
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_scrambling_control", ptransport_packet->transport_scrambling_control, 2, "bslbf", pszComment);
		MPEG2_TS_NumericCoding2Text_adaptation_field_control(ptransport_packet->adaptation_field_control, pszComment, sizeof(pszComment));
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "adaptation_field_control", ptransport_packet->adaptation_field_control, 2, "bslbf", pszComment);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "continuity_counter", ptransport_packet->continuity_counter, 4, "bslbf", NULL);

		int payload_start_pos = 4;
		if ((ptransport_packet->adaptation_field_control & 0b10) == 0b10)	//判断是不是有adaptation域
		{
			adaptation_field_t* padaptation_field = &(ptransport_packet->adaptation_field);

			int adaptation_field_length = 1 + padaptation_field->adaptation_field_length;

			XMLElement* pxmlAdaptationNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "adaptation_field()");
			XMLNODE_SetFieldLength(pxmlAdaptationNode, adaptation_field_length);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "adaptation_field_length", padaptation_field->adaptation_field_length, 8, "uimsbf", NULL);

			payload_start_pos += adaptation_field_length;

			if (padaptation_field->adaptation_field_length > 0)
			{
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "discontinuity_indicator", padaptation_field->discontinuity_indicator, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "random_access_indicator", padaptation_field->random_access_indicator, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "elementary_stream_priority_indicator", padaptation_field->elementary_stream_priority_indicator, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "PCR_flag", padaptation_field->PCR_flag, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "OPCR_flag", padaptation_field->OPCR_flag, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "splicing_point_flag", padaptation_field->splicing_point_flag, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "transport_private_data_flag", padaptation_field->transport_private_data_flag, 1, "bslbf", NULL);
				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "adaptation_field_extension_flag", padaptation_field->adaptation_field_extension_flag, 1, "bslbf", NULL);

				if (padaptation_field->PCR_flag)
				{
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_32_30", padaptation_field->program_clock_reference_base_32_30, 3, "uimsbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_29_15", padaptation_field->program_clock_reference_base_32_30, 15, "uimsbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_14_0", padaptation_field->program_clock_reference_base_14_0, 15, "uimsbf", NULL);

					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "reserved", padaptation_field->reserved0, 6, "bslbf", NULL);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_extension", padaptation_field->program_clock_reference_extension, 9, "uimsbf", NULL);
				}
			}
		}

		//TS包净荷
		if ((ptransport_packet->adaptation_field_control & 0b01) == 0b01)	//判断是否有净荷
		{
			XMLDOC_SetSyncOffset(pxmlDoc, payload_start_pos);
			XMLDOC_NewElementForBytes(pxmlDoc, pxmlRootNode, "payload_buf[ ]", ptransport_packet->payload_buf, ptransport_packet->payload_length, NULL);
		}
	}

	if (pTSPacket == NULL)
	{
		delete ptransport_packet;
	}

	return rtcode;
}
