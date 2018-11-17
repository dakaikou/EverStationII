#ifndef _AVS2_VIDEO_SEQUENCE_H_
#define _AVS2_VIDEO_SEQUENCE_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _AVS2_EXPORT
#       define AVS2_SYNTAX_LIB __declspec(dllexport)
#   elif defined(_AVS2_IMPORT)
#       define AVS2_SYNTAX_LIB __declspec(dllimport)
#   else
#       define AVS2_SYNTAX_LIB
#   endif
#elif __GNUC__ >= 4
#   define AVS2_SYNTAX_LIB __attribute__((visibility("default")))
#else
#   define AVS2_SYNTAX_LIB
#endif

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
	int		run;
	int		level;

} AVS_VLC_Table_t;

typedef struct
{
	//syntax part	
	uint32_t		video_sequence_start_code;						//f(32)

	uint8_t		profile_id;										//u(8)			0x20- GB/T 20090.2, 0x22- GB/T 33475.2, 0x48- GB/T 20090.16, 0x
	uint8_t		level_id;										//u(8)

	uint8_t		progressive_sequence;							//u(1)
	uint16_t		horizontal_size;								//u(14)
	uint16_t		vertical_size;									//u(14)
	uint8_t		chroma_format;									//u(2)
	uint8_t		sample_precision;								//u(3)
	uint8_t		aspect_ratio;									//u(4)
	uint8_t		frame_rate_code;								//u(4)
	
	uint32_t		bit_rate_lower;									//u(18)
	uint8_t		marker_bit0;									//f(1)
	uint16_t		bit_rate_upper;									//u(12)
	uint8_t		low_delay;										//u(1)
	
	uint8_t		marker_bit1;									//f(1)
	uint32_t		bbv_buffer_size;								//u(18)
	uint8_t		reserved_bits;									//u(3)

	//sematic part
	uint32_t		bit_rate;
	double	frame_rate;
	int		BBS;

} AVS_video_sequence_header_t, *pAVS_video_sequence_header_t;

typedef struct
{
	uint32_t		i_picture_start_code;							//f(32)
	uint16_t		bbv_delay;										//u(16)
	uint8_t		time_code_flag;									//u(1)
//	uint32_t		time_code;										//u(24)
	uint8_t		drop_time_flag;									//u(1)
	uint8_t		time_code_hours;								//u(5)
	uint8_t		time_code_minutes;								//u(6)
	uint8_t		time_code_seconds;								//u(6)
	uint8_t		time_code_pictures;								//u(6)
	uint8_t		picture_distance;								//u(8)
	uint32_t		bbv_check_times;								//ue(v)
	uint8_t		progressive_frame;								//u(1)
	uint8_t		picture_structure;								//u(1)
	uint8_t		top_field_first;								//u(1)
	uint8_t		repeat_first_field;								//u(1)
	uint8_t		fixed_picture_qp;								//u(1)
	uint8_t		picture_qp;										//u(6)
	uint8_t		skip_mode_flag;									//u(1)
	uint8_t		reserved_bits;									//u(4)
	uint8_t		loop_filter_disable;							//u(1)
	uint8_t		loop_filter_parameter_flag;						//u(1)
	uint32_t		alpha_c_offset;									//se(v)
	uint32_t		beta_offset;									//se(v)

	//semantic part

} AVS_i_picture_header_t, *pAVS_i_picture_header_t;

typedef struct
{
	uint32_t		user_data_start_code;							//f(32)
	int		user_data_length;
	uint8_t		user_data[1024];
	
} AVS_user_data_t;

typedef struct
{
	uint32_t		extension_start_code;							//f(32)
	uint8_t		extension_id;									//f(4)
	uint8_t		video_format;									//u(3)
	uint8_t		sample_range;									//u(1)
	uint8_t		colour_description;								//u(1)
	uint8_t		colour_primaries;								//u(8)
	uint8_t		transfer_characteristics;						//u(8)
	uint8_t		matrix_coefficients;							//u(8)
	uint16_t		display_horizontal_size;						//u(14)
	uint8_t		marker_bit;										//f(1)
	uint16_t		display_vertical_size;							//u(14)
	uint8_t		reserved_bits;									//f(2)
	
} AVS_sequence_display_extension_t;

typedef struct
{
	uint32_t		extension_start_code;							//f(32)
	uint8_t		extension_id;									//f(4)
	uint8_t		copyright_flag;									//u(1)
	uint8_t		copyright_id;									//u(8)
	uint8_t		original_or_copy;								//u(1)
	uint8_t		reserved_bits;									//f(7)
	uint8_t		marker_bit0;									//f(1)
	uint32_t		copyright_number_1;								//u(20)
	uint8_t		marker_bit1;									//f(1)
	uint32_t		copyright_number_2;								//u(22)
	uint8_t		marker_bit2;									//f(2)
	uint32_t		copyright_number_3;								//u(22)
} AVS_copyright_extension_t;

typedef struct
{
	uint32_t		extension_start_code;							//f(32)
	uint8_t		extension_id;									//f(4)

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
	uint32_t		extension_start_code;							//f(32)
	uint8_t		extension_id;									//f(4)
	
} AVS_picture_display_extension_t;

typedef struct
{
	uint32_t		slice_start_code;								//f(32)
	uint8_t		slice_vertical_position;						//u(8)
	uint8_t		slice_vertical_position_extension;				//u(3)
	uint8_t		fixed_slice_qp;									//u(1)
	uint8_t		slice_qp;										//u(6)
	uint8_t		slice_weighting_flag;							//u(1)
	uint8_t		luma_scale[4];									//u(8)
	int8_t		luma_shift[4];									//i(8)
//	uint8_t		marker_bit0;									//f(1)
	uint8_t		chroma_scale[4];								//u(8)
	int8_t		chroma_shift[4];								//i(8)
//	uint8_t		marker_bit1;									//f(1)
	uint8_t		mb_weighting_flag;								//u(1)

	int		mb_skip_run;									//ue(v)

} AVS_slice_t;

typedef struct
{
	uint8_t		mb_type;										//ue(v)
	uint8_t		mb_part_type[4];								//u(2) x 4
	uint8_t		pred_mode_flag[4];								//u(1) x 4
	uint8_t		intra_luma_pred_mode[4];						//u(2) x 4
	uint8_t		intra_chroma_pred_mode;							//ue(v)
	uint8_t		intra_chroma_pred_mode_422;						//ue(v)
	uint8_t		mb_reference_index[4];							//u(1)/u(2)
	int8_t		mv_diff_x[4];									//se(v)
	int8_t		mv_diff_y[4];									//se(v)
	uint8_t		weighting_prediction;							//u(1)
	uint8_t		cbp;											//me(v)
	uint8_t		cbp_422;										//me(v)
	uint8_t		mb_qp_delta;									//se(v)

} AVS_macroblock_t;

typedef struct
{
	uint8_t		Type;
	int8_t		Quant;
	uint8_t		Cbp;
	uint8_t		Cbp422;
	int8_t		Skipped;

} AVS_MB_INFO_t;


#endif

