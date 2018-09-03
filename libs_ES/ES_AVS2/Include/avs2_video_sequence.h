#ifndef _AVS_VIDEO_SEQUENCE_H_
#define _AVS_VIDEO_SEQUENCE_H_

//#include "HAL\HAL_BitStream\Include\bitstream.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"

#define AVS_VID_DECODER_DEBUG				1

#define AVS_VID_DECODER_I					1
#define AVS_VID_DECODER_P					0
#define AVS_VID_DECODER_B					0
#define AVS_VID_DECODER_IP					0
#define AVS_VID_DECODER_IBP					0

//constant in avs video standard -------------------------------
#define AVS_SLICE_START_CODE_MIN			0X00
#define AVS_SLICE_START_CODE_MAX			0XAF

#define AVS_VIDEO_SEQUENCE_START_CODE		0XB0
#define AVS_VIDEO_SEQUENCE_END_CODE			0XB1
#define AVS_USER_DATA_START_CODE			0XB2
#define AVS_I_PICTURE_START_CODE			0XB3
											//0XB4
#define AVS_EXTENSION_START_CODE			0XB5 
#define AVS_PB_PICTURE_START_CODE			0XB6
#define AVS_VIDEO_EDIT_CODE					0XB7

#define AVS_SEQUENCE_DISPLAY_EXTENSION_ID			0x2
#define AVS_COPYRIGHT_EXTENSION_ID					0x4
#define AVS_PICTURE_DISPLAY_EXTENSION_ID			0x7
#define AVS_CAMERA_PARAMETERS_EXTENSION_ID			0xB

typedef enum
{
	AVS_I_FRAME			= 0,
	AVS_P_FRAME			= 1,
	AVS_B_FRAME			= 2,
	AVS_D_FRAME
} AVS_FRAME_TYPE_e;

typedef enum
{
	AVS_I_8x8,
	AVS_P_Skip,
	AVS_P_16x16,
	AVS_P_16x8,
	AVS_P_8x16,
	AVS_P_8x8,
	AVS_B_Skip,
	AVS_B_Direct_16x16,
	AVS_B_Fwd_16x16,
	AVS_B_Bck_16x16,
	AVS_B_Sym_16x16,
	AVS_B_Fwd_Fwd_16x8,
	AVS_B_Fwd_Fwd_8x16,
	AVS_B_Bck_Bck_16x8,
	AVS_B_Bck_Bck_8x16,
	AVS_B_Fwd_Bck_16x8,
	AVS_B_Fwd_Bck_8x16,
	AVS_B_Bck_Fwd_16x8,
	AVS_B_Bck_Fwd_8x16,
	AVS_B_Fwd_Sym_16x8,
	AVS_B_Fwd_Sym_8x16,
	AVS_B_Bck_Sym_16x8,
	AVS_B_Bck_Sym_8x16,
	AVS_B_Sym_Fwd_16x8,
	AVS_B_Sym_Fwd_8x16,
	AVS_B_Sym_Bck_16x8,
	AVS_B_Sym_Bck_8x16,
	AVS_B_Sym_Sym_16x8,
	AVS_B_Sym_Sym_8x16,
	AVS_B_8x8,
	AVS_SB_Direct_8x8,
	AVS_SB_Fwd_8x8,
	AVS_SB_Bck_8x8,
	AVS_SB_Sym_8x8,
} AVS_MACROBLOCK_e;

typedef enum
{
	AVS_VLC0_Intra		= 0,
	AVS_VLC1_Intra,
	AVS_VLC2_Intra,
	AVS_VLC3_Intra,
	AVS_VLC4_Intra,
	AVS_VLC5_Intra,
	AVS_VLC6_Intra,
	AVS_VLC0_Inter,				//k = 3
	AVS_VLC1_Inter,
	AVS_VLC2_Inter,
	AVS_VLC3_Inter,
	AVS_VLC4_Inter,
	AVS_VLC5_Inter,
	AVS_VLC6_Inter,
	AVS_VLC0_Chroma,
	AVS_VLC1_Chroma,			//k = 0
	AVS_VLC2_Chroma,			//k = 1
	AVS_VLC3_Chroma,			//k = 1	
	AVS_VLC4_Chroma,			//k = 0
} AVS_VLC_TYPE_e;

typedef struct
{
	S32		run;
	S32		level;

} AVS_VLC_Table_t;

typedef struct
{
	//syntax part	
	U32		video_sequence_start_code;						//f(32)

	U8		profile_id;										//u(8)			0x20- GB/T 20090.2, 0x22- GB/T 33475.2, 0x48- GB/T 20090.16, 0x
	U8		level_id;										//u(8)

	U8		progressive_sequence;							//u(1)
	U16		horizontal_size;								//u(14)
	U16		vertical_size;									//u(14)
	U8		chroma_format;									//u(2)
	U8		sample_precision;								//u(3)
	U8		aspect_ratio;									//u(4)
	U8		frame_rate_code;								//u(4)
	
	U32		bit_rate_lower;									//u(18)
	U8		marker_bit0;									//f(1)
	U16		bit_rate_upper;									//u(12)
	U8		low_delay;										//u(1)
	
	U8		marker_bit1;									//f(1)
	U32		bbv_buffer_size;								//u(18)
	U8		reserved_bits;									//u(3)

	//sematic part
	U32		bit_rate;
	double	frame_rate;
	S32		BBS;

} AVS_video_sequence_header_t, *pAVS_video_sequence_header_t;

typedef struct
{
	U32		i_picture_start_code;							//f(32)
	U16		bbv_delay;										//u(16)
	U8		time_code_flag;									//u(1)
//	U32		time_code;										//u(24)
	U8		drop_time_flag;									//u(1)
	U8		time_code_hours;								//u(5)
	U8		time_code_minutes;								//u(6)
	U8		time_code_seconds;								//u(6)
	U8		time_code_pictures;								//u(6)
	U8		picture_distance;								//u(8)
	U32		bbv_check_times;								//ue(v)
	U8		progressive_frame;								//u(1)
	U8		picture_structure;								//u(1)
	U8		top_field_first;								//u(1)
	U8		repeat_first_field;								//u(1)
	U8		fixed_picture_qp;								//u(1)
	U8		picture_qp;										//u(6)
	U8		skip_mode_flag;									//u(1)
	U8		reserved_bits;									//u(4)
	U8		loop_filter_disable;							//u(1)
	U8		loop_filter_parameter_flag;						//u(1)
	U32		alpha_c_offset;									//se(v)
	U32		beta_offset;									//se(v)

	//semantic part

} AVS_i_picture_header_t, *pAVS_i_picture_header_t;

typedef struct
{
	U32		user_data_start_code;							//f(32)
	S32		user_data_length;
	U8		user_data[1024];
	
} AVS_user_data_t;

typedef struct
{
	U32		extension_start_code;							//f(32)
	U8		extension_id;									//f(4)
	U8		video_format;									//u(3)
	U8		sample_range;									//u(1)
	U8		colour_description;								//u(1)
	U8		colour_primaries;								//u(8)
	U8		transfer_characteristics;						//u(8)
	U8		matrix_coefficients;							//u(8)
	U16		display_horizontal_size;						//u(14)
	U8		marker_bit;										//f(1)
	U16		display_vertical_size;							//u(14)
	U8		reserved_bits;									//f(2)
	
} AVS_sequence_display_extension_t;

typedef struct
{
	U32		extension_start_code;							//f(32)
	U8		extension_id;									//f(4)
	U8		copyright_flag;									//u(1)
	U8		copyright_id;									//u(8)
	U8		original_or_copy;								//u(1)
	U8		reserved_bits;									//f(7)
	U8		marker_bit0;									//f(1)
	U32		copyright_number_1;								//u(20)
	U8		marker_bit1;									//f(1)
	U32		copyright_number_2;								//u(22)
	U8		marker_bit2;									//f(2)
	U32		copyright_number_3;								//u(22)
} AVS_copyright_extension_t;

typedef struct
{
	U32		extension_start_code;							//f(32)
	U8		extension_id;									//f(4)

} AVS_camera_parameters_extension_t;

typedef struct
{
	uint32_t		pb_picture_start_code;							//f(32)
	uint16_t		bbv_delay;										//u(16)
	uint8_t			marker_bit;										//f(1)
	uint8_t			picture_coding_type;							//u(2)
	uint8_t			picture_distance;								//u(8)
	uint32_t		bbv_check_times;								//ue(v)
	uint8_t			progressive_frame;								//u(1)
	uint8_t			picture_structure;								//u(1)
	uint8_t			advanced_pred_mode_disable;						//u(1)
	uint8_t			top_field_first;								//u(1)
	uint8_t			repeat_first_field;								//u(1)
	uint8_t			fixed_picture_qp;								//u(1)
	uint8_t			picture_qp;										//u(6)
	uint8_t			picture_reference_flag;							//u(1)
	uint8_t			reserved_bits;									//u(4)
	uint8_t			skip_mode_flag;									//u(1)
	uint8_t			loop_filter_disable;							//u(1)
	uint8_t			loop_filter_parameter_flag;						//u(1)
	int				alpha_c_offset;									//se(v)
	int				beta_offset;									//se(v)
	
} AVS_pb_picture_header_t;

typedef struct
{
	U32		extension_start_code;							//f(32)
	U8		extension_id;									//f(4)
	
} AVS_picture_display_extension_t;

typedef struct
{
	U32		slice_start_code;								//f(32)
	U8		slice_vertical_position;						//u(8)
	U8		slice_vertical_position_extension;				//u(3)
	U8		fixed_slice_qp;									//u(1)
	U8		slice_qp;										//u(6)
	U8		slice_weighting_flag;							//u(1)
	U8		luma_scale[4];									//u(8)
	S8		luma_shift[4];									//i(8)
//	U8		marker_bit0;									//f(1)
	U8		chroma_scale[4];								//u(8)
	S8		chroma_shift[4];								//i(8)
//	U8		marker_bit1;									//f(1)
	U8		mb_weighting_flag;								//u(1)

	S32		mb_skip_run;									//ue(v)

} AVS_slice_t;

typedef struct
{
	U8		mb_type;										//ue(v)
	U8		mb_part_type[4];								//u(2) x 4
	U8		pred_mode_flag[4];								//u(1) x 4
	U8		intra_luma_pred_mode[4];						//u(2) x 4
	U8		intra_chroma_pred_mode;							//ue(v)
	U8		intra_chroma_pred_mode_422;						//ue(v)
	U8		mb_reference_index[4];							//u(1)/u(2)
	S8		mv_diff_x[4];									//se(v)
	S8		mv_diff_y[4];									//se(v)
	U8		weighting_prediction;							//u(1)
	U8		cbp;											//me(v)
	U8		cbp_422;										//me(v)
	U8		mb_qp_delta;									//se(v)

} AVS_macroblock_t;

typedef struct
{
	U8		Type;
	S8		Quant;
	U8		Cbp;
	U8		Cbp422;
	S8		Skipped;

} AVS_MB_INFO_t;


#endif

