#ifndef _KERNEL_MPEG_PS_DECODER_H_
#define _KERNEL_MPEG_PS_DECODER_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../compile.h"

typedef enum
{
	PS_UNKNOWN				= 0,
	PS_MPEG_11172,
	PS_MPEG_13818
} PS_STREAM_STANDARD_e;

typedef struct
{
	U8		reserved0;											//4
	S8		system_clock_reference_base_32_30;					//3
	S8		marker_bit0;										//1

	S16		system_clock_reference_base_29_15;					//15
	S8		marker_bit1;										//1

	S16		system_clock_reference_base_14_0;					//15
	S8		marker_bit2;										//1

	S8		marker_bit3;										//1
	U32		program_mux_rate;									//22
	S8		marker_bit4;										//1

} mpeg1_ps_t;

typedef struct
{
	U8		reserved0;											//2
	S8		system_clock_reference_base_32_30;					//3
	S8		marker_bit0;										//1
	S16		system_clock_reference_base_29_15;					//15
	S8		marker_bit1;										//1
	S16		system_clock_reference_base_14_0;					//15
	S8		marker_bit2;										//1
	S16		system_clock_reference_extension;					//9
	S8		marker_bit3;										//1

	U32		program_mux_rate;									//22
	S8		marker_bit4;										//1
	S8		marker_bit5;										//1

	U8		reserved1;											//5
	U8		pack_stuffing_length;								//3

	U8		stuffing_byte[8];
} mpeg2_ps_t;

typedef struct pack_header_s 
{
	U32		pack_start_code;									//32

	union
	{
		mpeg1_ps_t		mpeg1_ps;
		mpeg2_ps_t		mpeg2_ps;
	} u;

	//semantic part

	U8		nextbits;

} MPEG_pack_header_t, *pMPEG_pack_header_t;

#define MAX_STREAMS_PER_PROGRAM					5

typedef struct system_header_s
{
	U32		system_header_start_code;				//32
	
	U16		header_length;							//16

	S8		marker_bit0;							//1
	U32		rate_bound;								//22
	S8		marker_bit1;							//1

	U8		audio_bound;							//6
	U8		fixed_flag;								//1
	U8		CSPS_flag;								//1

	U8		system_audio_lock_flag;					//1
	U8		system_video_lock_flag;					//1
	U8		marker_bit2;							//1
	U8		video_bound;							//5

	U8		packet_rate_restriction_flag;			//1
	U8		reserved_bits;							//7

	S32		stream_count;
	U8		stream_id[MAX_STREAMS_PER_PROGRAM];		//8x
	U8		reserved[MAX_STREAMS_PER_PROGRAM];
	U8		P_STD_buffer_bound_scale[MAX_STREAMS_PER_PROGRAM];
	U16		P_STD_buffer_size_bound[MAX_STREAMS_PER_PROGRAM];

} MPEG_system_header_t, *pMPEG_system_header_t;

_CDL_EXPORT S32 mpeg_ps_decode_pack_header(U8 *buf, S32* plength, pMPEG_pack_header_t ppack_header);
_CDL_EXPORT S32 mpeg_ps_decode_system_header(U8* buf, S32* plength, pMPEG_system_header_t psystem_header);


#endif
