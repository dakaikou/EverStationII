#include <string.h>

#include "../Include/Mpeg2_TS_ErrorCode.h"
#include "../Include/Mpeg2_TS_packet.h"

#define USE_BIT_STREAM_INLINE_PARSER			0

#if USE_BIT_STREAM_INLINE_PARSER

#include "../../Stream_AL/BitStream.h"

//
//  notes: use BITS_t inline function to parse syntax is much slower than native method.   chendelin 2018.11.25
//

int MPEG_decode_TS_adaptation_field(uint8_t *buf, int length, adaptation_field_t* padaptation_field)
{
	int		rtcode = TSPACKET_PARSE_NO_ERROR;

	if ((buf != NULL) && (length <= 204) && (padaptation_field != NULL))
	{
		BITS_t bs;
		BITS_map(&bs, buf, length);

		padaptation_field->adaptation_field_length = BITS_get(&bs, 8);

		if ((padaptation_field->adaptation_field_length >= 0) && (padaptation_field->adaptation_field_length <= 183))
		{
			if (padaptation_field->adaptation_field_length > 0)
			{
				padaptation_field->discontinuity_indicator = BITS_get(&bs, 1);
				padaptation_field->random_access_indicator = BITS_get(&bs, 1);
				padaptation_field->elementary_stream_priority_indicator = BITS_get(&bs, 1);
				padaptation_field->PCR_flag	 = BITS_get(&bs, 1);
				padaptation_field->OPCR_flag = BITS_get(&bs, 1);
				padaptation_field->splicing_point_flag = BITS_get(&bs, 1);
				padaptation_field->transport_private_data_flag = BITS_get(&bs, 1);
				padaptation_field->adaptation_field_extension_flag = BITS_get(&bs, 1);

				if( padaptation_field->PCR_flag )
				{
					padaptation_field->program_clock_reference_base_32_30  = BITS_get(&bs, 3);
					padaptation_field->program_clock_reference_base_29_15  = BITS_get(&bs, 15);
					padaptation_field->program_clock_reference_base_14_0 = BITS_get(&bs, 15);

					padaptation_field->reserved0 = BITS_get(&bs, 6);
					padaptation_field->program_clock_reference_extension  = BITS_get(&bs, 9);
				}
		/*
				if( padaptation_field->OPCR_flag ){

					padaptation_field->original_program_clock_reference_base  = *buf++;
					padaptation_field->original_program_clock_reference_base <<= 25;
					padaptation_field->original_program_clock_reference_base |= *buf++ << 17;
					padaptation_field->original_program_clock_reference_base |= *buf++ << 9;
					padaptation_field->original_program_clock_reference_base |= *buf++ << 1;
					padaptation_field->original_program_clock_reference_base |= ( *buf & 0x80 ) >> 7;

					padaptation_field->original_program_clock_reference_extension  = ( *buf++ & 0x01 ) << 8;
					padaptation_field->original_program_clock_reference_extension |=  *buf++;
				}

				if( padaptation_field->splicing_point_flag ){
					padaptation_field->splice_countdown = *buf++;

				}

				if( padaptation_field->transport_private_data_flag ){
					padaptation_field->transport_private_data_length = *buf++;
					padaptation_field->pprivate_data_byte = buf;

					buf += padaptation_field->transport_private_data_length;
				}

				if( padaptation_field->adaptation_field_extension_flag ){
					padaptation_field->adaptation_field_extension_length = *buf++;

					padaptation_field->ltw_flag = ( *buf & 0x80 ) >> 7;
					padaptation_field->piecewise_rate_flag = ( *buf & 0x40 ) >> 6;
					padaptation_field->seamless_splice_flag = ( *buf++ & 0x20 ) >> 5;

					if( padaptation_field->ltw_flag ){
						padaptation_field->ltw_valid_flag = ( *buf & 0x80 ) >> 7;
						padaptation_field->ltw_offset = ( ( *buf++ & 0x7F ) << 8 ) + *buf++;
					}

					if( padaptation_field->piecewise_rate_flag ){
						padaptation_field->piecewise_rate = ( ( *buf++ & 0x3F ) << 16 ) + ( *buf++ << 8 ) + *buf++;
					}

					if( padaptation_field->seamless_splice_flag ){
						padaptation_field->splice_type = (*buf & 0xF0 ) >> 4;
						padaptation_field->DTS_next_AU = ( ( *buf++ & 0x0E ) << 30 ) + ( *buf++ << 22 ) + ( ( *buf++ & 0xFE ) << 15 ) + ( *buf++ << 7 ) + ( (*buf++ & 0xFE ) >> 1 );
					}
				}
		*/
			}
		}
		else
		{
			rtcode = TSPACKET_PARSE_SYNTAX_ERROR;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


//
//  notes: use BITS_t inline function to parse syntax is much slower than native method.   chendelin 2018.11.25
//


int MPEG_decode_TS_packet(uint8_t *buf, int length, transport_packet_t* ptransport_packet)
{
	int	 rtcode = TSPACKET_PARSE_NO_ERROR;

	if ((buf != NULL) &&
		((length == 188) || (length == 204)) &&
		(ptransport_packet != NULL))
	{
		memset(ptransport_packet, 0x00, sizeof(transport_packet_t));

		if ((buf[0] == 0x47) || (buf[0] == 0xB8))
		{
			BITS_t bs;
			BITS_map(&bs, buf, length);

			ptransport_packet->sync_byte = BITS_get(&bs, 8);

			ptransport_packet->transport_error_indicator = BITS_get(&bs, 1);
			ptransport_packet->payload_unit_start_indicator = BITS_get(&bs, 1);
			ptransport_packet->transport_priority = BITS_get(&bs, 1);
			ptransport_packet->PID = BITS_get(&bs, 13);

			ptransport_packet->transport_scrambling_control = BITS_get(&bs, 2);
			ptransport_packet->adaptation_field_control = BITS_get(&bs, 2);
			ptransport_packet->continuity_counter = BITS_get(&bs, 4);

			if ((ptransport_packet->adaptation_field_control & 0b10) == 0b10)	//have adaptation field
			{
				rtcode = MPEG_decode_TS_adaptation_field(bs.p_cur, length - 4, &(ptransport_packet->adaptation_field));
				if (rtcode == TSPACKET_PARSE_NO_ERROR)
				{
					BITS_byteSkip(&bs, ptransport_packet->adaptation_field.adaptation_field_length + 1);
				}
			}

			if (rtcode == TSPACKET_PARSE_NO_ERROR)
			{
				if ((ptransport_packet->adaptation_field_control & 0b01) == 0b01)	//have payload 
				{
					ptransport_packet->payload_length = (uint8_t)(188 - (bs.p_cur - bs.p_start));
					if (ptransport_packet->payload_length > 0)
					{
						ptransport_packet->payload_buf = bs.p_cur;
						BITS_byteSkip(&bs, ptransport_packet->payload_length);
						//BITS_byteCopy(ptransport_packet->payload_buf, sizeof(ptransport_packet->payload_buf), &bs, ptransport_packet->payload_length);
					}
				}
				else
				{
					ptransport_packet->payload_length = 0;
				}
			}
		}
		else
		{
			rtcode = TSPACKET_PARSE_SYNC_ERROR;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

#else 

int MPEG_decode_TS_adaptation_field(uint8_t *buf, int length, adaptation_field_t* padaptation_field)
{
	int		rtcode = TSPACKET_PARSE_NO_ERROR;

	if ((buf != NULL) && (length <= 204) && (padaptation_field != NULL))
	{
		padaptation_field->adaptation_field_length = *buf++;

		if ((padaptation_field->adaptation_field_length >= 0) && (padaptation_field->adaptation_field_length <= 183))
		{
			if (padaptation_field->adaptation_field_length > 0)
			{
				padaptation_field->discontinuity_indicator = (*buf & 0x80) >> 7;
				padaptation_field->random_access_indicator = (*buf & 0x40) >> 6;
				padaptation_field->elementary_stream_priority_indicator = (*buf & 0x20) >> 5;
				padaptation_field->PCR_flag = (*buf & 0x10) >> 4;
				padaptation_field->OPCR_flag = (*buf & 0x08) >> 3;
				padaptation_field->splicing_point_flag = (*buf & 0x04) >> 2;
				padaptation_field->transport_private_data_flag = (*buf & 0x02) >> 1;
				padaptation_field->adaptation_field_extension_flag = *buf++ & 0x01;

				if (padaptation_field->PCR_flag)
				{
					padaptation_field->program_clock_reference_base_32_30 = (*buf & 0xe0) >> 5;
					padaptation_field->program_clock_reference_base_29_15 = (*buf++ & 0x1f);
					padaptation_field->program_clock_reference_base_29_15 <<= 8;
					padaptation_field->program_clock_reference_base_29_15 |= *buf++;
					padaptation_field->program_clock_reference_base_29_15 <<= 2;
					padaptation_field->program_clock_reference_base_29_15 |= (*buf & 0xc0) >> 6;
					padaptation_field->program_clock_reference_base_14_0 = (*buf++ & 0x3f);
					padaptation_field->program_clock_reference_base_14_0 <<= 8;
					padaptation_field->program_clock_reference_base_14_0 |= *buf++;
					padaptation_field->program_clock_reference_base_14_0 <<= 1;
					padaptation_field->program_clock_reference_base_14_0 |= (*buf & 0x80) >> 7;

					padaptation_field->program_clock_reference_extension = (*buf++ & 0x01) << 8;
					padaptation_field->program_clock_reference_extension |= *buf++;
				}
				/*
				if( padaptation_field->OPCR_flag ){

				padaptation_field->original_program_clock_reference_base  = *buf++;
				padaptation_field->original_program_clock_reference_base <<= 25;
				padaptation_field->original_program_clock_reference_base |= *buf++ << 17;
				padaptation_field->original_program_clock_reference_base |= *buf++ << 9;
				padaptation_field->original_program_clock_reference_base |= *buf++ << 1;
				padaptation_field->original_program_clock_reference_base |= ( *buf & 0x80 ) >> 7;

				padaptation_field->original_program_clock_reference_extension  = ( *buf++ & 0x01 ) << 8;
				padaptation_field->original_program_clock_reference_extension |=  *buf++;
				}

				if( padaptation_field->splicing_point_flag ){
				padaptation_field->splice_countdown = *buf++;

				}

				if( padaptation_field->transport_private_data_flag ){
				padaptation_field->transport_private_data_length = *buf++;
				padaptation_field->pprivate_data_byte = buf;

				buf += padaptation_field->transport_private_data_length;
				}

				if( padaptation_field->adaptation_field_extension_flag ){
				padaptation_field->adaptation_field_extension_length = *buf++;

				padaptation_field->ltw_flag = ( *buf & 0x80 ) >> 7;
				padaptation_field->piecewise_rate_flag = ( *buf & 0x40 ) >> 6;
				padaptation_field->seamless_splice_flag = ( *buf++ & 0x20 ) >> 5;

				if( padaptation_field->ltw_flag ){
				padaptation_field->ltw_valid_flag = ( *buf & 0x80 ) >> 7;
				padaptation_field->ltw_offset = ( ( *buf++ & 0x7F ) << 8 ) + *buf++;
				}

				if( padaptation_field->piecewise_rate_flag ){
				padaptation_field->piecewise_rate = ( ( *buf++ & 0x3F ) << 16 ) + ( *buf++ << 8 ) + *buf++;
				}

				if( padaptation_field->seamless_splice_flag ){
				padaptation_field->splice_type = (*buf & 0xF0 ) >> 4;
				padaptation_field->DTS_next_AU = ( ( *buf++ & 0x0E ) << 30 ) + ( *buf++ << 22 ) + ( ( *buf++ & 0xFE ) << 15 ) + ( *buf++ << 7 ) + ( (*buf++ & 0xFE ) >> 1 );
				}
				}
				*/
			}
		}
		else
		{
			rtcode = TSPACKET_PARSE_SYNTAX_ERROR;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG_decode_TS_packet(uint8_t *buf, int length, transport_packet_t* ptransport_packet)
{
	int	 rtcode = TSPACKET_PARSE_NO_ERROR;

	uint8_t*  porigin = buf;
	int		  offset = 0;

	if ((buf != NULL) &&
		((length == 188) || (length == 204)) &&
		(ptransport_packet != NULL))
	{
		memset(ptransport_packet, 0x00, sizeof(transport_packet_t));

		if ((buf[0] == 0x47) || (buf[0] == 0xB8))
		{
			ptransport_packet->sync_byte = *buf++;

			ptransport_packet->transport_error_indicator = (*buf & 0x80) >> 7;
			ptransport_packet->payload_unit_start_indicator = (*buf & 0x40) >> 6;
			ptransport_packet->transport_priority = (*buf & 0x20) >> 5;

			ptransport_packet->PID = (*buf++ & 0x1F);
			ptransport_packet->PID <<= 8;
			ptransport_packet->PID |= *buf++;

			ptransport_packet->transport_scrambling_control = (*buf & 0xC0) >> 6;
			ptransport_packet->adaptation_field_control = (*buf & 0x30) >> 4;
			ptransport_packet->continuity_counter = (*buf++) & 0x0F;

			if ((ptransport_packet->adaptation_field_control & 0b10) == 0b10)
			{
				rtcode = MPEG_decode_TS_adaptation_field(buf, length - 4, &(ptransport_packet->adaptation_field));

				if (rtcode == TSPACKET_PARSE_NO_ERROR)
				{
					buf += (ptransport_packet->adaptation_field.adaptation_field_length + 1);
				}
			}

			if (rtcode == TSPACKET_PARSE_NO_ERROR)
			{
				offset = (int)(buf - porigin);
				if (offset > 188)			//failure
				{
					rtcode = TSPACKET_PARSE_SYNTAX_ERROR;
				}
				else								//success
				{
					if ((ptransport_packet->adaptation_field_control & 0b01) == 0b01)
					{
						ptransport_packet->payload_length = (uint8_t)(188 - offset);
						if (ptransport_packet->payload_length > 0)
						{
							ptransport_packet->payload_buf = buf;
							buf += ptransport_packet->payload_length;
							//memcpy(ptransport_packet->payload_buf, buf, ptransport_packet->payload_length);
						}
					}
					else
					{
						ptransport_packet->payload_length = 0;
					}
				}
			}
		}
		else
		{
			rtcode = TSPACKET_PARSE_SYNC_ERROR;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

#endif

int MPEG_encode_TS_packet(uint8_t *buf, int length, transport_packet_t* pTS_packet)
{
	int			rtcode = TSPACKET_PARSE_NO_ERROR;
	uint16_t	usTemp;
	uint8_t		ucTemp;

	if ((buf != NULL) && (pTS_packet != NULL))
	{
		buf[0] = pTS_packet->sync_byte;

		usTemp = 0x0000;
		if (pTS_packet->transport_error_indicator)
		{
			usTemp |= 0x8000;
		}
		if (pTS_packet->payload_unit_start_indicator)
		{
			usTemp |= 0x4000;
		}
		if (pTS_packet->transport_priority)
		{
			usTemp |= 0x2000;
		}
		usTemp |= (pTS_packet->PID & 0x1fff);

		buf[1] = (usTemp >> 8);
		buf[2] = (usTemp & 0x00ff);

		ucTemp = 0x00;
		ucTemp |= (pTS_packet->transport_scrambling_control & 3) << 6;
		ucTemp |= (pTS_packet->adaptation_field_control & 3) << 4;
		ucTemp |= (pTS_packet->continuity_counter & 0xf);
		buf[3] = ucTemp;

		memset(buf + 4, 0xff, length - 4);
		memcpy(buf + 4, pTS_packet->payload_buf, pTS_packet->payload_length);
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


//int TSPacketHasPcrField(uint8_t *buf, int length)
//{
//	int haspcr = 0;
//
//	int condition1 = (buf[3] & 0x20) > 0 ? 1 : 0;				//adaptation_field_control = 2 or 3
//	int	condition2 = buf[4]	> 0 ? 1 : 0;
//	int	condition3 = (buf[5] & 0x10) > 0 ? 1 : 0;
//
//	haspcr = condition1 & condition2 & condition3;
//
//	return haspcr;
//}


