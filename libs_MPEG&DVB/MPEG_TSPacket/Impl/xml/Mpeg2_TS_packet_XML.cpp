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

//int MPEG_decode_TS_adaptation_field_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, adaptation_field_t* pAdaptFieldParam)
//{
//	int	 rtcode = TSPACKET_PARSE_NO_ERROR;
//	uint8_t*  old_buf = NULL;
//	uint8_t*  adaptation_start_ptr = NULL;
//	uint8_t*  adaptation_end_ptr = NULL;
//	XMLElement* pxmlAdaptationNode = NULL;
//
//	BITS_t bs;
//
//	if ((buf != NULL) && (length < 188))
//	{
//		BITS_map(&bs, buf, length);
//
//		adaptation_field_t* padaptation_field = (pAdaptFieldParam == NULL) ? new adaptation_field_t : pAdaptFieldParam;
//		memset(padaptation_field, 0x00, sizeof(adaptation_field_t));
//
//		adaptation_start_ptr = buf;
//		pxmlAdaptationNode = XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "adaptation_field()");
//
//		padaptation_field->adaptation_field_length = BITS_get(&bs, 8);
//		XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "adaptation_field_length", padaptation_field->adaptation_field_length, 8, "uimsbf", NULL, &bs);
//
//		if ((padaptation_field->adaptation_field_length >= 0) && (padaptation_field->adaptation_field_length <= 183))
//		{
//			if (padaptation_field->adaptation_field_length > 0)
//			{
//				padaptation_field->discontinuity_indicator = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "discontinuity_indicator", padaptation_field->discontinuity_indicator, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->random_access_indicator = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "random_access_indicator", padaptation_field->random_access_indicator, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->elementary_stream_priority_indicator = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "elementary_stream_priority_indicator", padaptation_field->elementary_stream_priority_indicator, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->PCR_flag = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "PCR_flag", padaptation_field->PCR_flag, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->OPCR_flag = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "OPCR_flag", padaptation_field->OPCR_flag, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->splicing_point_flag = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "splicing_point_flag", padaptation_field->splicing_point_flag, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->transport_private_data_flag = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "transport_private_data_flag", padaptation_field->transport_private_data_flag, 1, "bslbf", NULL, &bs);
//
//				padaptation_field->adaptation_field_extension_flag = BITS_get(&bs, 1);
//				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "adaptation_field_extension_flag", padaptation_field->adaptation_field_extension_flag, 1, "bslbf", NULL, &bs);
//
//				if (padaptation_field->PCR_flag)
//				{
//					padaptation_field->program_clock_reference_base_32_30 = BITS_get(&bs, 3);
//					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_32_30", padaptation_field->program_clock_reference_base_32_30, 3, "uimsbf", NULL, &bs);
//					padaptation_field->program_clock_reference_base_29_15 = BITS_get(&bs, 15);
//					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_29_15", padaptation_field->program_clock_reference_base_32_30, 15, "uimsbf", NULL, &bs);
//					padaptation_field->program_clock_reference_base_14_0 = BITS_get(&bs, 15);
//					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_14_0", padaptation_field->program_clock_reference_base_14_0, 15, "uimsbf", NULL, &bs);
//
//					padaptation_field->reserved0 = BITS_get(&bs, 6);
//					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "reserved", padaptation_field->reserved0, 6, "bslbf", NULL, &bs);
//
//					padaptation_field->program_clock_reference_extension = BITS_get(&bs, 9);
//					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_extension", padaptation_field->program_clock_reference_extension, 9, "uimsbf", NULL, &bs);
//				}
//				/*
//				if( padaptation_field->OPCR_flag ){
//
//				padaptation_field->original_program_clock_reference_base  = *buf++;
//				padaptation_field->original_program_clock_reference_base <<= 25;
//				padaptation_field->original_program_clock_reference_base |= *buf++ << 17;
//				padaptation_field->original_program_clock_reference_base |= *buf++ << 9;
//				padaptation_field->original_program_clock_reference_base |= *buf++ << 1;
//				padaptation_field->original_program_clock_reference_base |= ( *buf & 0x80 ) >> 7;
//
//				padaptation_field->original_program_clock_reference_extension  = ( *buf++ & 0x01 ) << 8;
//				padaptation_field->original_program_clock_reference_extension |=  *buf++;
//				}
//
//				if( padaptation_field->splicing_point_flag ){
//				padaptation_field->splice_countdown = *buf++;
//
//				}
//
//				if( padaptation_field->transport_private_data_flag ){
//				padaptation_field->transport_private_data_length = *buf++;
//				padaptation_field->pprivate_data_byte = buf;
//
//				buf += padaptation_field->transport_private_data_length;
//				}
//
//				if( padaptation_field->adaptation_field_extension_flag ){
//				padaptation_field->adaptation_field_extension_length = *buf++;
//
//				padaptation_field->ltw_flag = ( *buf & 0x80 ) >> 7;
//				padaptation_field->piecewise_rate_flag = ( *buf & 0x40 ) >> 6;
//				padaptation_field->seamless_splice_flag = ( *buf++ & 0x20 ) >> 5;
//
//				if( padaptation_field->ltw_flag ){
//				padaptation_field->ltw_valid_flag = ( *buf & 0x80 ) >> 7;
//				padaptation_field->ltw_offset = ( ( *buf++ & 0x7F ) << 8 ) + *buf++;
//				}
//
//				if( padaptation_field->piecewise_rate_flag ){
//				padaptation_field->piecewise_rate = ( ( *buf++ & 0x3F ) << 16 ) + ( *buf++ << 8 ) + *buf++;
//				}
//
//				if( padaptation_field->seamless_splice_flag ){
//				padaptation_field->splice_type = (*buf & 0xF0 ) >> 4;
//				padaptation_field->DTS_next_AU = ( ( *buf++ & 0x0E ) << 30 ) + ( *buf++ << 22 ) + ( ( *buf++ & 0xFE ) << 15 ) + ( *buf++ << 7 ) + ( (*buf++ & 0xFE ) >> 1 );
//				}
//				}
//				*/
//			}
//
//			adaptation_end_ptr = adaptation_start_ptr + padaptation_field->adaptation_field_length + 1;
//			XMLNODE_SetStartEndPtr(pxmlAdaptationNode, adaptation_start_ptr, adaptation_end_ptr);
//
//			if (pAdaptFieldParam == NULL)
//			{
//				delete padaptation_field;
//			}
//		}
//		else
//		{
//			XMLDOC_NewElementForString(pxmlDoc, pxmlParentNode, "TS packet adaptation field syntax error !");
//			rtcode = TSPACKET_PARSE_SYNTAX_ERROR;
//		}
//	}
//	else
//	{
//		XMLNODE_SetAttribute(pxmlParentNode, "error", "TS packet adaptation field parameter error !");
//		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
//	}
//
//	return rtcode;
//}

int MPEG_decode_TS_packet_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, transport_packet_t* pTSPacket)
{
	int	 rtcode = TSPACKET_PARSE_NO_ERROR;

	transport_packet_t* ptransport_packet = (pTSPacket == NULL) ? new transport_packet_t : pTSPacket;

	rtcode = MPEG_decode_TS_packet(buf, length, ptransport_packet);
	if (rtcode == TSPACKET_PARSE_NO_ERROR)
	{
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
			XMLNODE_SetStartEndPtr(pxmlRootNode, 0, length);

			BITS_TRACER_t bs_tracer;
			bs_tracer.offset = 0;
			bs_tracer.i_left = 8;

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "sync_byte", ptransport_packet->sync_byte, 8, "bslbf", NULL, &bs_tracer);

			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_error_indicator", ptransport_packet->transport_error_indicator, 1, "bslbf", NULL, &bs_tracer);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "payload_unit_start_indicator", ptransport_packet->payload_unit_start_indicator, 1, "bslbf", NULL, &bs_tracer);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_priority", ptransport_packet->transport_priority, 1, "bslbf", NULL, &bs_tracer);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "PID", ptransport_packet->PID, 13, "uimsbf", NULL, &bs_tracer);

			MPEG2_TS_NumericCoding2Text_transport_scrambling_control(ptransport_packet->transport_scrambling_control, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "transport_scrambling_control", ptransport_packet->transport_scrambling_control, 2, "bslbf", pszComment, &bs_tracer);
			MPEG2_TS_NumericCoding2Text_adaptation_field_control(ptransport_packet->adaptation_field_control, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "adaptation_field_control", ptransport_packet->adaptation_field_control, 2, "bslbf", pszComment, &bs_tracer);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlRootNode, "continuity_counter", ptransport_packet->continuity_counter, 4, "bslbf", NULL, &bs_tracer);

			if ((ptransport_packet->adaptation_field_control & 0b10) == 0b10)	//判断是不是有adaptation域
			{
				adaptation_field_t* padaptation_field = &(ptransport_packet->adaptation_field);
				XMLElement* pxmlAdaptationNode = XMLDOC_NewElementForString(pxmlDoc, pxmlRootNode, "adaptation_field()");
				pxmlAdaptationNode->SetStartEndPtr(bs_tracer.offset, 1 + padaptation_field->adaptation_field_length);

				XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "adaptation_field_length", padaptation_field->adaptation_field_length, 8, "uimsbf", NULL, &bs_tracer);

				if (padaptation_field->adaptation_field_length > 0)
				{
					BITS_TRACER_t adaptation_bs_tracer;
					adaptation_bs_tracer.offset = bs_tracer.offset;
					adaptation_bs_tracer.i_left = bs_tracer.i_left;

					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "discontinuity_indicator", padaptation_field->discontinuity_indicator, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "random_access_indicator", padaptation_field->random_access_indicator, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "elementary_stream_priority_indicator", padaptation_field->elementary_stream_priority_indicator, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "PCR_flag", padaptation_field->PCR_flag, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "OPCR_flag", padaptation_field->OPCR_flag, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "splicing_point_flag", padaptation_field->splicing_point_flag, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "transport_private_data_flag", padaptation_field->transport_private_data_flag, 1, "bslbf", NULL, &adaptation_bs_tracer);
					XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "adaptation_field_extension_flag", padaptation_field->adaptation_field_extension_flag, 1, "bslbf", NULL, &adaptation_bs_tracer);

					if (padaptation_field->PCR_flag)
					{
						XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_32_30", padaptation_field->program_clock_reference_base_32_30, 3, "uimsbf", NULL, &adaptation_bs_tracer);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_29_15", padaptation_field->program_clock_reference_base_32_30, 15, "uimsbf", NULL, &adaptation_bs_tracer);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_base_14_0", padaptation_field->program_clock_reference_base_14_0, 15, "uimsbf", NULL, &adaptation_bs_tracer);

						XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "reserved", padaptation_field->reserved0, 6, "bslbf", NULL, &adaptation_bs_tracer);
						XMLDOC_NewElementForBits(pxmlDoc, pxmlAdaptationNode, "program_clock_reference_extension", padaptation_field->program_clock_reference_extension, 9, "uimsbf", NULL, &adaptation_bs_tracer);
					}

					bs_tracer.offset += padaptation_field->adaptation_field_length;
				}
			}

			//TS包净荷
			if ((ptransport_packet->adaptation_field_control & 0b01) == 0b01)	//判断是否有净荷
			{
				XMLDOC_NewElementForBytes(pxmlDoc, pxmlRootNode, "payload_buf[ ]", ptransport_packet->payload_buf, ptransport_packet->payload_length, NULL, &bs_tracer);
			}
		}
	}

	if (pTSPacket == NULL)
	{
		delete ptransport_packet;
	}

	return rtcode;
}
