#ifndef __MPEG2_TS_PACKET_H__
#define __MPEG2_TS_PACKET_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_TSPACKET_EXPORT
#       define MPEG2_TS_PACKET_LIB __declspec(dllexport)
#   elif defined(_MPEG2_TSPACKET_IMPORT)
#       define MPEG2_TS_PACKET_LIB __declspec(dllimport)
#   else
#       define MPEG2_TS_PACKET_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_TS_PACKET_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_TS_PACKET_LIB
#endif

#include <stdint.h>

//#define TSPACKET_GetPID(buf)		((buf[1] & 0x1f) << 8) | buf[2]

typedef struct _PCR_code_s
{
	uint8_t base_32_30;
	uint16_t base_29_15;
	uint16_t base_14_0;

	uint16_t extension;

} PCR_code_t;

typedef struct adaptation_field_s
{
	uint8_t		adaptation_field_length;							//8

	uint8_t		adaptation_field_extension_flag;					//1
	uint8_t		transport_private_data_flag;						//1
	uint8_t		splicing_point_flag;								//1
	uint8_t		OPCR_flag;											//1
	uint8_t		PCR_flag;											//1
	uint8_t		elementary_stream_priority_indicator;				//1
	uint8_t		random_access_indicator;							//1
	uint8_t		discontinuity_indicator;							//1

	uint8_t		program_clock_reference_base_32_30;					//3
	uint16_t	program_clock_reference_base_29_15;					//15
	uint16_t	program_clock_reference_base_14_0;					//15
	uint8_t		reserved0;											//6
	uint8_t		program_clock_reference_extension;					//9

	uint8_t		original_program_clock_reference_base_32_30;		//3
	uint16_t	original_program_clock_reference_base_29_15;		//15
	uint16_t	original_program_clock_reference_base_14_0;			//15
	uint8_t		reserved1;											//6
	uint16_t	original_program_clock_reference_extension;			//9

	uint8_t		splice_countdown;

	uint8_t		transport_private_data_length;						//8
	uint8_t*	pprivate_data_byte;

	uint8_t		adaptation_field_extension_length;					//8

	uint8_t		ltw_flag;											//1

	uint8_t		piecewise_rate_flag;								//1
	uint8_t		seamless_splice_flag;								//1

	uint8_t		ltw_valid_flag;										//1
	uint16_t	ltw_offset;											//15

	uint32_t	piecewise_rate;										//22

	uint8_t		splice_type;										//4
	uint8_t		DTS_next_AU_32_30;									//3
	uint16_t	DTS_next_AU_29_15;									//15
	uint16_t	DTS_next_AU_14_0;									//15

	uint8_t*	pstuffing_byte;
	uint8_t		stuffing_byte_length;

} adaptation_field_t, *padaptation_field_t;

typedef struct transport_packet_s 
{
	uint8_t		sync_byte;										//8

	uint16_t	PID;											//13
	uint8_t		transport_priority;								//2
	uint8_t		payload_unit_start_indicator;					//1
	uint8_t		transport_error_indicator;						//1

	uint8_t		continuity_counter;								//4
	uint8_t		adaptation_field_control;						//2
	uint8_t		transport_scrambling_control;					//2

	adaptation_field_t adaptation_field;

	uint8_t*	payload_buf;				//voltile member
	uint8_t		payload_length;

} transport_packet_t, *ptransport_packet_t;


MPEG2_TS_PACKET_LIB int MPEG_decode_TS_adaptation_field(uint8_t *buf, int length, adaptation_field_t* padaptation_field);
MPEG2_TS_PACKET_LIB int MPEG_decode_TS_packet(uint8_t *buf, int length, transport_packet_t* ptransport_packet);

MPEG2_TS_PACKET_LIB int MPEG_encode_TS_packet(uint8_t *buf, int length, transport_packet_t* ptransport_packet);

MPEG2_TS_PACKET_LIB int MPEG_PCR_minus(PCR_code_t* pcr1, PCR_code_t* pcr2);


#endif		//__MPEG2_TS_PACKET_H__