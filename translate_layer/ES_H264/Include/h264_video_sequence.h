#ifndef _KERNEL_H264_DECODER_H_
#define _KERNEL_H264_DECODER_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _H264_EXPORT
#       define H264_SYNTAX_LIB __declspec(dllexport)
#   elif defined(_H264_IMPORT)
#       define H264_SYNTAX_LIB __declspec(dllimport)
#   else
#       define H264_SYNTAX_LIB
#   endif
#elif __GNUC__ >= 4
#   define H264_SYNTAX_LIB __attribute__((visibility("default")))
#else
#   define H264_SYNTAX_LIB
#endif

#include <stdint.h>

#define H264_VID_DECODER_DEBUG					0

#define H264_VID_DECODER_I						1
//#define H264_VID_DECODER_P					0
//#define H264_VID_DECODER_B					0
#define H264_VID_DECODER_IP						0
#define H264_VID_DECODER_IBP					0

#define H264_VID_DECODER_CLEAR						1

#define H264_VID_DECODER_Chroma						0
#define H264_VID_DECODER_FastAlgorithm				0

#define H264_CODED_SLICE_OF_A_NON_IDR_PICTURE		0x01
#define H264_CODED_SLICE_DATA_PARTITION_A			0x02
#define H264_CODED_SLICE_DATA_PARTITION_B			0x03
#define H264_CODED_SLICE_DATA_PARTITION_C			0x04
#define H264_CODED_SLICE_OF_AN_IDR_PICTURE			0x05
#define H264_SUPPLEMENTAL_ENHANCEMENT_INFORMATION	0x06
#define H264_SEQUENCE_PARAMETER_SET					0x07
#define H264_PICTURE_PARAMETER_SET					0x08
#define H264_ACCESS_UNIT_DELIMITER					0x09
#define H264_END_OF_SEQUENCE						0x0a
#define H264_END_OF_STREAM							0x0b
#define H264_FILLER_DATA							0x0c

#define H264_PROFILE_BASELINE			66
#define H264_PROFILE_MAIN				77
#define H264_PROFILE_EXTENDED			88
#define H264_PROFILE_FREXT_HIGH	        100      //!< YUV 4:2:0/8 bits "High"
#define H264_PROFILE_FREXT_HIGH_110     110      //!< YUV 4:2:0/10 bits "High 10"
#define H264_PROFILE_FREXT_HIGH_122     122      //!< YUV 4:2:2/10 bits "High 4:2:2"
#define H264_PROFILE_FREXT_HIGH_144     144      //!< YUV 4:4:4/12 bits "High 4:4:4"

typedef enum
{
	H264_EXTENDED_SAR				= 255,
} H264_EXTENDED_e;

typedef enum
{
	H264_SLICE_P0	= 0,		
	H264_SLICE_B0,		
	H264_SLICE_I0,		
	H264_SLICE_SP0,		
	H264_SLICE_SI0,		
	H264_SLICE_P1,		
	H264_SLICE_B1,		
	H264_SLICE_I1,		
	H264_SLICE_SP1,		
	H264_SLICE_SI1,		
} H264_SLICE_TYPE_e;

typedef enum	//Name of mb_type for I slices
{
	H264_I_NxN		= 0,
	H264_I_16x16_0_0_0,
	H264_I_16x16_1_0_0,
	H264_I_16x16_2_0_0,
	H264_I_16x16_3_0_0,
	H264_I_16x16_0_1_0,
	H264_I_16x16_1_1_0,
	H264_I_16x16_2_1_0,
	H264_I_16x16_3_1_0,
	H264_I_16x16_0_2_0,
	H264_I_16x16_1_2_0,
	H264_I_16x16_2_2_0,
	H264_I_16x16_3_2_0,
	H264_I_16x16_0_0_1,
	H264_I_16x16_1_0_1,
	H264_I_16x16_2_0_1,
	H264_I_16x16_3_0_1,
	H264_I_16x16_0_1_1,
	H264_I_16x16_1_1_1,
	H264_I_16x16_2_1_1,
	H264_I_16x16_3_1_1,
	H264_I_16x16_0_2_1,
	H264_I_16x16_1_2_1,
	H264_I_16x16_2_2_1,
	H264_I_16x16_3_2_1,
	H264_I_PCM,
} H264_I_TYPE_e;

typedef enum	//Name of mb_type for SI slices
{
	H264_SI		= 0,
} H264_SI_TYPE_e;

typedef enum	//Name of mb_type for P and SP slices
{
	H264_P_L0_16x16		= 0,
	H264_P_L0_L0_16x8,
	H264_P_L0_L0_8x16,
	H264_P_8x8,
	H264_P_8x8ref0,
	H264_P_Skip,
} H264_P_TYPE_4;

typedef enum	//Name of mb_type for B slices
{
	H264_B_Direct_16x16		= 0,
	H264_B_L0_16x16,
	H264_B_L1_16x16,
	H264_B_Bi_16x16,
	H264_B_L0_L0_16x8,
	H264_B_L0_L0_8x16,
	H264_B_L1_L1_16x8,
	H264_B_L1_L1_8x16,
	H264_B_L0_L1_16x8,
	H264_B_L0_L1_8x16,
	H264_B_L1_L0_16x8,
	H264_B_L1_L0_8x16,
	H264_B_L0_Bi_16x8,
	H264_B_L0_Bi_8x16,
	H264_B_L1_Bi_16x8,
	H264_B_L1_Bi_8x16,
	H264_B_Bi_L0_16x8,
	H264_B_Bi_L0_8x16,
	H264_B_Bi_L1_16x8,
	H264_B_Bi_L1_8x16,
	H264_B_Bi_Bi_16x8,
	H264_B_Bi_Bi_8x16,
	H264_B_8x8,
	H264_B_Skip,
} H264_B_TYPE_e;

#define MAXSPS					  32
#define MAXPPS					  256

typedef struct
{
	int		aspect_ratio_info_present_flag;				//u(1)
	int		aspect_ratio_idc;							//u(8)
	int		sar_width;									//u(16)
	int		sar_height;									//u(16)
	int		overscan_info_present_flag;					//u(1)
	int		overscan_appropriate_flag;					//u(1)
	int		video_signal_type_present_flag;				//u(1)
	int		video_format;								//u(3)
	int		video_full_range_flag;						//u(1)
	int		colour_description_present_flag;			//u(1)
	int		colour_primaries;							//u(8)
	int		transfer_characteristics;					//u(8)
	int		matrix_coefficients;						//u(8)

	int		chroma_loc_info_present_flag;				//u(1)
	int		chroma_sample_loc_type_top_filed;			//ue(v)
	int		chroma_sample_loc_type_bottom_field;		//ue(v)
	int		timing_info_present_flag;					//u(1)
	uint32_t		num_units_in_tick;							//u(32)
	uint32_t		time_scale;									//u(32)
	int		fixed_frame_rate_flag;						//u(1)

	int		nal_hrd_parameters_present_flag;			//u(1)
	int		vcl_hrd_parameters_present_flag;			//u(1)
	int		low_delay_hrd_flag;							//u(1)
	int		pic_struct_present_flag;					//u(1)
	int		bitstream_restriction_flag;					//u(1)

	int		motion_vectors_over_pic_boundaries_flag;	//u(1)
	int		max_bytes_per_pic_denom;					//ue(v)
	int		max_bits_per_mb_denom;						//ue(v)
	int		log2_max_mv_length_horizontal;				//ue(v)
	int		log2_max_mv_length_vertical;				//ue(v)
	int		num_reorder_frames;							//ue(v)
	int		max_dec_frame_buffering;					//ue(v)

} H264_vui_parameters_t, *pH264_vui_parameters_t;

typedef struct
{
	uint32_t		start_code_prefix;					//24~32bits
	int		forbidden_zero_bit;					//f(1)
	int		nal_ref_idc;						//u(2)
	int		nal_unit_type;						//u(5)

	uint8_t		rbsp_byte[16 * 1024];
	int		NumBytesInRBSP;

} H264_nal_unit_t, *pH264_nal_unit_t;

typedef struct
{
	//syntax part	
	int		profile_idc;									//u(8)

	int		constraint_set0_flag;							//u(1)
	int		constraint_set1_flag;							//u(1)
	int		constraint_set2_flag;							//u(1)
	int		constraint_set3_flag;							//u(1)
	int		reserved_zero_4bits;							//u(4)

	int		level_idc;										//u(8)

	int		seq_parameter_set_id;							//ue(v)

	//high 100, high 110 high 122, high 144 profile
	/*{{{{					*/
	int		chroma_format_idc;								//ue(v)
	int		residual_colour_transform_flag;					//u(1)
	int		bit_depth_luma_minus8;							//ue(v)
	int		bit_depth_chroma_minus8;						//ue(v)
	int		qpprime_y_zero_transform_bypass_flag;			//u(1)
	int		seq_scaling_matrix_present_flag;				//u(1)
	int		seq_scaling_list_present_flag[8];				//u(1) x 8

//	S16		ScalingList4x4[6][16];
//	S16		ScalingList8x8[2][64];
	/*}}}}					*/

	int		log2_max_frame_num_minus4;						//ue(v)
	int		pic_order_cnt_type;								//ue(v)

	int		log2_max_pic_order_cnt_lsb_minus4;				//ue(v)
	int		delta_pic_order_always_zero_flag;				//u(1)
	int		offset_for_non_ref_pic;							//se(v)
	int		offset_for_top_to_bottom_field;					//se(v)
	int		num_ref_frames_in_pic_order_cnt_cycle;			//ue(v)
	int		offset_for_ref_frame[256];						//se(v)

	int		num_ref_frames;									//ue(v)
	int		gaps_in_frame_num_value_allowed_flag;			//u(1)
	int		pic_width_in_mbs_minus1;						//ue(v)
	int		pic_height_in_map_units_minus1;					//ue(v)
	int		frame_mbs_only_flag;							//u(1)
	int		mb_adaptive_frame_field_flag;					//u(1)
	int		direct_8x8_inference_flag;						//u(1)
	int		frame_cropping_flag;							//u(1)

	int		frame_crop_left_offset;							//ue(v)
	int		frame_crop_right_offset;						//ue(v)
	int		frame_crop_top_offset;							//ue(v)
	int		frame_crop_bottom_offset;						//ue(v)

	int		vui_parameters_present_flag;					//u(1)

	H264_vui_parameters_t	vui_parameters;

	//sematic part
	int		SubWidthC;
	int		SubHeightC;
	int		MbWidthC;
	int		MbHeightC;
	int		BitDepthY;
	int		BitDepthC;
	int		QpBdOffsetY;
	int		QpBdOffsetC;
	int		MaxFrameNum;
	int		MaxPicOrderCntLsb;
	int		PicWidthInMbs;
	int		PicHeightInMapUnits;
	int		PicSizeInMapUnits;
	int		FrameHeightInMbs;

	int		PicWidthInSamplesL;
	int		PicHeightInSamplesL;
	int		PicWidthInSamplesC;
	int		PicHeightInSamplesC;

} H264_seq_parameter_set_t, *pH264_seq_parameter_set_t;

typedef struct
{
	int		pic_parameter_set_id;							//ue(v)
	int		seq_parameter_set_id;							//ue(v)
	int		entropy_coding_mode_flag;						//u(1)
	int		pic_order_present_flag;							//u(1)
	int		num_slice_groups_minus1;						//ue(v)
	int		slice_group_map_type;							//ue(v)
	int		run_length_minus1[8];							//ue(v)				//baseline profile最多就8个,其余profile为0
	int		top_left[8];									//ue(v)
	int		bottom_right[8];								//ue(v)
	int		slice_group_change_direction_flag;				//ue(1)
	int		slice_group_change_rate_minus1;					//ue(v)
	int		pic_size_in_map_units_minus1;					//ue(v)
	int		slice_group_id[16];								//u(v)
	int		num_ref_idx_l0_active_minus1;					//ue(v)				//0~31,可以被slice_header覆盖
	int		num_ref_idx_l1_active_minus1;					//ue(v)				//0~31,可以被slice_header覆盖
	int		weighted_pred_flag;								//u(1)
	int		weighted_bipred_idc;							//u(2)
	int		pic_init_qp_minus26;							//se(v)
	int		pic_init_qs_minus26;							//se(v)
	int		chroma_qp_index_offset;							//se(v)
	int		deblocking_filter_control_present_flag;			//u(1)
	int		constrained_intra_pred_flag;					//u(1)
	int		redundant_pic_cnt_present_flag;					//u(1)
	
	//14496-10 2005以后的版本有此部分
	int		transform_8x8_mode_flag;						//u(1)
	int		pic_scaling_matrix_present_flag;				//u(1)
	int		pic_scaling_list_present_flag[8];				//u(1) x 8
	int		second_chroma_qp_index_offset;					//se(v)

	//sematic part

} H264_pic_parameter_set_t, *pH264_pic_parameter_set_t;

typedef struct
{
	int		ref_pic_list_reordering_flag_l0;				//u(1)
	int		reordering_of_pic_nums_idc[2];				//ue(v)
	int		abs_diff_pic_num_minus1[2][32];					//ue(v)
	int		long_term_pic_num[2][32];						//ue(v)

	int		ref_pic_list_reordering_flag_l1;				//u(1)

} H264_ref_pic_list_reordering_t, *pH264_ref_pic_list_reordering_t;

typedef struct
{
	int		luma_log2_weight_denom;								//ue(v)
	int		chroma_log2_weight_denom;							//ue(v)
	int		luma_weight_l0_flag[32];							//u(1)
	int		luma_weight_l0[32];									//se(v)
	int		luma_offset_l0[32];									//se(v)
	int		chroma_weight_l0_flag[32];							//u(1)
	int		chroma_weight_l0[32][2];							//se(v)
	int		chroma_offset_l0[32][2];							//se(v)
	int		luma_weight_l1_flag[32];							//u(1)
	int		luma_weight_l1[32];									//se(v)
	int		luma_offset_l1[32];									//se(v)
	int		chroma_weight_l1_flag[32];							//u(1)
	int		chroma_weight_l1[32][2];							//se(v)
	int		chroma_offset_l1[32][2];							//se(v)

} H264_pred_weight_table_t, *pH264_pred_weight_table_t;

typedef struct
{
	int		no_output_of_prior_pics_flag;							//u(1)
	int		long_term_reference_flag;								//u(1)
	int		adaptive_ref_pic_marking_mode_flag;						//u(1)
	int		memory_management_control_operation;					//ue(v)
	int		difference_of_pic_nums_minus1[7];						//ue(v)
	int		long_term_pic_num[7];									//ue(v)
	int		long_term_frame_idx[7];									//ue(v)
	int		max_long_term_frame_idx_plus1[7];						//ue(v)

} H264_dec_ref_pic_marking_t, *pH264_dec_ref_pic_marking_t;

typedef struct
{
	int		first_mb_in_slice;					//ue(v)
	int		slice_type;							//ue(v)
	int		pic_parameter_set_id;				//ue(v)
	int		frame_num;							//u(v)
	int		field_pic_flag;						//u(1)
	int		bottom_field_flag;					//u(1)
	int		idr_pic_id;							//ue(v)
	int		pic_order_cnt_lsb;					//u(v)
	int		delta_pic_order_cnt_bottom;			//se(v)
	int		delta_pic_order_cnt[2];				//se(v) x 2
	int		redundant_pic_cnt;					//ue(v)
	int		direct_spatial_mv_pred_flag;		//u(1)
	int		num_ref_idx_active_override_flag;	//u(1)
	int		num_ref_idx_l0_active_minus1;		//ue(v)
	int		num_ref_idx_l1_active_minus1;		//ue(v)

	H264_ref_pic_list_reordering_t	ref_pic_list_reordering;
	H264_pred_weight_table_t		pred_weight_table;
	H264_dec_ref_pic_marking_t		dec_ref_pic_marking;

	int		cabac_init_idc;						//ue(v)
	int		slice_qp_delta;						//se(v)
	int		sp_for_switch_flag;					//u(1)
	int		slice_qs_delta;						//se(v)
	int		disable_deblocking_filter_idc;		//ue(v)
	int		slice_alpha_c0_offset_div2;			//se(v)
	int		slice_beta_offset_div2;				//se(v)

	int		slice_group_change_cycle;			//u(v)

	//semantic part
	int		MbaffFrameFlag;
	int		SliceQPy;
	int		SliceQSy;
	int		slice_number;

} H264_slice_header_t, *pH264_slice_header_t;

typedef struct
{
	int		cabac_alignment_one_bit;			//f(1)
	int		mb_skip_run;						//ue(v)
	int		mb_skip_flag;						//ae(v)
	int		mb_field_decoding_flag;				//u(1)|ae(v)
	int		end_of_slice_flag;

} H264_slice_data_t, *pH264_slice_data_t;

typedef struct
{
	int		mb_type;							//ue(v)/ae(v)
	int		pcm_alignment_zero_bit;				//f(1)
	int		pcm_sample_luma[256];				//u(v)
	int		pcm_sample_chroma[256];				//u(v)
	int		transform_size_8x8_flag;			//u(1)/ae(v)
	int		coded_block_pattern;				//me(v)/ae(v)
	int		mb_qp_delta;						//se(v)/ae(v)
	int		prev_intra4x4_pred_mode_flag[16];
	int		rem_intra4x4_pred_mode[16];
	int		prev_intra8x8_pred_mode_flag[4];
	int		rem_intra8x8_pred_mode[4];
	int		intra_chroma_pred_mode;
	int		ref_idx_l0[2];
	int		ref_idx_l1[2];
	int		mvd_l0[2][2][2];
	int		mvd_l1[2][2][2];

	int		slice_number;

	//semantic part
	int		CodedBlockPatternLuma;
	int		CodedBlockPatternChroma;

	int		TotalCoeff[48];						//keep info
	int		Intra4x4PredMode[48];

	int		QPy;
	int		QPy_prev;
	int		QPy_;

} H264_macroblock_layer_t, *pH264_macroblock_layer_t;

typedef struct
{
	int		mb_type;
	int		cbp;

} H264_MB_INFO_t;

int	h264_video_decode_slice_layer_without_partitioning_rbsp(H264_nal_unit_t* pnal_unit);
int	h264_video_decode_seq_parameter_set(H264_nal_unit_t* pnal_unit);
int	h264_video_decode_pic_parameter_set(H264_nal_unit_t* pnal_unit);

_inline uint32_t CeilLog2(uint32_t uiVal)
{
	uint32_t uiTmp = uiVal - 1;
	uint32_t uiRet = 0;

	while (uiTmp != 0)
	{
		uiTmp >>= 1;
		uiRet++;
	}

	return uiRet;
}

#endif

