#ifndef __MPEG2_TS_PACKET_H__
#define __MPEG2_TS_PACKET_H__

#include "../compile.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

#define TSPACKET_GetPID(buf)		((buf[1] & 0x1f) << 8) | buf[2]

typedef struct _PCR_code_s
{
	U8 base_32_30;
	U16 base_29_15;
	U16 base_14_0;

	U16 extension;

} PCR_code_t;

typedef struct adaptation_field_s
{

	U8		adaptation_field_length;							//8

	U8		adaptation_field_extension_flag;					//1
	U8		transport_private_data_flag;						//1
	U8		splicing_point_flag;								//1
	U8		OPCR_flag;											//1
	U8		PCR_flag;											//1
	U8		elementary_stream_priority_indicator;				//1
	U8		random_access_indicator;							//1
	U8		discontinuity_indicator;							//1

	S8		program_clock_reference_base_32_30;					//3
	S16		program_clock_reference_base_29_15;					//15
	S16		program_clock_reference_base_14_0;					//15
	U8		reserved0;											//6
	S16		program_clock_reference_extension;					//9

	S8		original_program_clock_reference_base_32_30;		//3
	S32		original_program_clock_reference_base_29_15;		//15
	S32		original_program_clock_reference_base_14_0;			//15
	U8		reserved1;											//6
	S16		original_program_clock_reference_extension;			//9

	U8		splice_countdown;

	U8		transport_private_data_length;						//8
	U8*		pprivate_data_byte;

	U8		adaptation_field_extension_length;					//8
	U8		ltw_flag;											//1

	U8		piecewise_rate_flag;								//1
	U8		seamless_splice_flag;								//1

	U8		ltw_valid_flag;										//1
	U16		ltw_offset;											//15

	U32		piecewise_rate;										//22

	U8		splice_type;										//4
	S8		DTS_next_AU_32_30;									//3
	S16		DTS_next_AU_29_15;									//15
	S16		DTS_next_AU_14_0;									//15

	U8*		pstuffing_byte;
	U8		stuffing_byte_length;

} adaptation_field_t, *padaptation_field_t;

typedef struct transport_packet_s 
{
	U8		sync_byte;										//8

	U16		PID;											//13
	S8		transport_priority;								//2
	S8		payload_unit_start_indicator;					//1
	S8		transport_error_indicator;						//1

	S8		continuity_counter;								//4
	S8		adaptation_field_control;						//2
	S8		transport_scrambling_control;					//2

	adaptation_field_t adaptation_field;

	U8		payload_buf[200];
	U8		payload_length;

} transport_packet_t, *ptransport_packet_t;


_CDL_EXPORT int MPEG_decode_TS_adaptation_field(uint8_t *buf, int length, adaptation_field_t* padaptation_field);
_CDL_EXPORT int MPEG_decode_TS_packet(uint8_t *buf, int length, transport_packet_t* ptransport_packet);
_CDL_EXPORT int MPEG_encode_TS_packet(uint8_t *buf, int length, transport_packet_t* ptransport_packet);

_CDL_EXPORT int MPEG_PCR_minus(PCR_code_t* pcr1, PCR_code_t* pcr2);


#endif		//__MPEG2_TS_PACKET_H__