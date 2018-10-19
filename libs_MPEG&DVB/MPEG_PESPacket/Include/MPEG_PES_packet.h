#ifndef __MPEG2_PACKET_ELEMENTARY_STREAM_H__
#define __MPEG2_PACKET_ELEMENTARY_STREAM_H__

#include "../compile.h"

#include <stdint.h>

//#include "HAL/HAL_Sys/Include/INTTYPES.H"

typedef struct PES_packet_s 
{
	uint32_t  packet_start_code_prefix;							// 24 bit		
	uint8_t   stream_id;											// 8 bit
	
	uint32_t  PES_packet_length;							// 16 bit

	int		  PES_packet_data_length;
	uint8_t*  PES_packet_data_byte;
	
	uint8_t	  mpeg_flag;										// 2 bit
	uint8_t   PES_scrambling_control;							// 2 bit
	uint8_t   PES_priority;										// 1 bit
	uint8_t   data_alignment_indicator;							// 1 bit
	uint8_t   copyright;										// 1 bit
	uint8_t   original_or_copy;									// 1 bit

	uint8_t   PTS_DTS_flags;									// 2 bit
	uint8_t   ESCR_flag;										// 1 bit
	uint8_t   ES_rate_flag;										// 1 bit
	uint8_t   DSM_trick_mode_flag;								// 1 bit
	uint8_t   additional_copy_info_flag;						// 1 bit
	uint8_t   PES_CRC_flag;										// 1 bit
	uint8_t   PES_extension_flag;								// 1 bit

	uint8_t		 PES_header_data_length;					// 8 bit
	uint8_t*	 PES_header_data_byte;

	uint8_t	 PTS_marker;						// 4 bit
	int		 PTS_32_30;							// 3 bit
	uint8_t	 PTS_marker_bit1;					// 1 bit
	int		 PTS_29_15;							// 15 bit
	uint8_t	 PTS_marker_bit2;					// 1 bit
	int		 PTS_14_0;							// 15 bit
	uint8_t	 PTS_marker_bit3;					// 1 bit

	uint8_t	 DTS_marker;						// 4 bit
	int		 DTS_32_30;							// 3 bit
	uint8_t	 DTS_marker_bit1;					// 1 bit
	int		 DTS_29_15;							// 15 bit
	uint8_t	 DTS_marker_bit2;					// 1 bit
	int		 DTS_14_0;							// 15 bit
	uint8_t	 DTS_marker_bit3;					// 1 bit

	uint8_t   ESCR_reserved;				//2 bits
	uint8_t   ESCR_base_32_30;				//3 bits
	uint8_t   ESCR_marker_bit0;				//1 bit
	uint16_t  ESCR_base_29_15;				//15 bits
	uint8_t	  ESCR_marker_bit1;				//1 bit
	uint16_t  ESCR_base_14_0;				//15 bits
	uint8_t   ESCR_marker_bit2;				//1 bit
	uint16_t  ESCR_extension;				//9 bits
	uint8_t   ESCR_marker_bit3;				//1 bit

	uint8_t   ES_rate_marker_bit0;				//1 bit
	uint32_t  ES_rate;							//22 bit
	uint8_t   ES_rate_marker_bit1;				//1 bit

	uint8_t   trick_mode_control;				// 3 bit
	union
	{
		struct
		{
			uint8_t   field_id;							// 2 bit
			uint8_t   intra_slice_refresh;				// 1 bit
			uint8_t   frequency_truncation;				// 2 bit
		} fast_forward;

		struct
		{
			uint8_t   rep_cntrl;						// 5 bit
		} slow_motion;

		struct
		{
			uint8_t   field_id;							// 2 bits
			uint8_t	  reserved;							// 3 bits
		} freeze_frame;

		struct
		{
			uint8_t   field_id;							// 2 bits
			uint8_t   intra_slice_refresh;				// 1 bit
			uint8_t   frequency_truncation;				// 2 bit
		} fast_reverse;

		struct
		{
			uint8_t   rep_cntrl;						// 5 bit
		} slow_reverse;

		uint8_t		reserved;							//5 bits

	} trick_mode;

	uint8_t	  additional_copy_info_marker_bit;		//1 bit
	uint8_t   additional_copy_info;				//7 bit

	uint16_t  previous_PES_packet_CRC;			// 16 bit

	uint8_t   PES_private_data_flag;				// 1 bit
	uint8_t   pack_header_field_flag;				// 1 bit
	uint8_t   program_packet_sequence_counter_flag;	// 1 bit
	uint8_t   P_STD_buffer_flag;					// 1 bit
	uint8_t	  PES_extension_reserved;				// 3 bits
	uint8_t   PES_extension_flag_2;					// 1 bit

	uint8_t*  PES_private_data_byte;				//128
	int		  PES_private_data_length;

	uint8_t	  pack_field_length;						//8
	uint8_t*  pack_header_data_byte;

	uint8_t   program_packet_marker_bit0;			//1
	uint8_t	  program_packet_sequence_counter;		//7

	uint8_t   program_packet_marker_bit1;			//1
	uint8_t   MPEG1_MPEG2_identifier;				//1
	uint8_t   original_stuff_length;					//6

	uint8_t   P_STD_marker;								//2
	uint8_t   P_STD_buffer_scale;						//1
	uint16_t  P_STD_buffer_size;						//13

	uint8_t   PES_extension_field_marker_bit;		//1
	uint8_t   PES_extension_field_length;			//7
	uint8_t*  PES_extension_field_data_byte;

	uint8_t*  stuffing_byte;
	int		  stuffing_length;

	uint8_t*  payload_buf;
	int	      payload_length;

} PES_packet_t, *pPES_packet_t;

_CDL_EXPORT	int	MPEG_decode_PES_packet(uint8_t *buf, int length, PES_packet_t* pPES_packet);

#endif

