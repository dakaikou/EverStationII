#ifndef __MPEG2_PACKET_ELEMENTARY_STREAM_H__
#define __MPEG2_PACKET_ELEMENTARY_STREAM_H__

#include "../compile.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"


typedef struct PES_packet_s 
{
	U32  packet_start_code_prefix;							// 24 bit		
	U8   stream_id;											// 8 bit
	
	U16  PES_packet_length;									// 16 bit
	
	U8	 mpeg_flag;											// 2 bit
	U8   PES_scrambling_control;							// 2 bit
	U8   PES_priority;										// 1 bit
	U8   data_alignment_indicator;							// 1 bit
	U8   copyright;											// 1 bit
	U8   original_or_copy;									// 1 bit

	U8   PTS_DTS_flags;										// 2 bit
	U8   ESCR_flag;											// 1 bit
	U8   ES_rate_flag;										// 1 bit
	U8   DSM_trick_mode_flag;								// 1 bit
	U8   additional_copy_info_flag;							// 1 bit
	U8   PES_CRC_flag;										// 1 bit
	U8   PES_extension_flag;								// 1 bit

	U8	 PES_header_data_length;							// 8 bit
	U8	 PES_header_data[256];

	U8	 PTS_marker;						// 4 bit
	S8	 PTS_32_30;							// 3 bit
	U8	 PTS_marker_bit1;					// 1 bit
	S16	 PTS_29_15;							// 15 bit
	U8	 PTS_marker_bit2;					// 1 bit
	S16	 PTS_14_0;							// 15 bit
	U8	 PTS_marker_bit3;					// 1 bit

	U8	 DTS_marker;						// 4 bit
	S8	 DTS_32_30;							// 3 bit
	U8	 DTS_marker_bit1;					// 1 bit
	S16	 DTS_29_15;							// 15 bit
	U8	 DTS_marker_bit2;					// 1 bit
	S16	 DTS_14_0;							// 15 bit
	U8	 DTS_marker_bit3;					// 1 bit

	S8   ESCR_base_32_30;					// 3 bit
	S8   ESCR_base_29_15;					// 15 bit
	S8   ESCR_base_14_0;					// 15 bit

	S16  ESCR_extension;					// 9 bit
	
	S32  ES_rate;							// 24 bit

	U8   trick_mode_control;				// 3 bit

	U8   field_id;							// 2 bit
	U8   intra_slice_refresh;				// 1 bit
	U8   frequency_truncation;				// 2 bit
	U8   rep_cntrl;							// 5 bit

	U8   additional_copy_info;				// 7 bit
	U16  previous_PES_packet_CRC;			// 16 bit

	U8   PES_private_data_flag;					// 1 bit
	U8   pack_header_field_flag;				// 1 bit
	U8   program_packet_sequence_counter_flag;	// 1 bit
	U8   P_STD_buffer_flag;						// 1 bit
	U8   PES_extension_flag_2;					// 1 bit

	U8   PES_private_data[16];					//128
	U8	 pack_field_length;						//8
	U8*  ppack_header;

	U8	 program_packet_sequence_counter;		//7
	U8   MPEG1_MPEG2_identifier;				//1
	U8   original_stuff_length;					//6

	U8   P_STD_buffer_scale;					//1
	U16  P_STD_buffer_size;						//13

	U8   PES_extension_field_length;			//7

	unsigned char*  es_payload_buf;
	int			    es_payload_length;

} PES_packet_t, *pPES_packet_t;

_CDL_EXPORT	int	MPEG_decode_PES_packet(uint8_t *buf, int length, PES_packet_t* pPES_packet);

#endif

