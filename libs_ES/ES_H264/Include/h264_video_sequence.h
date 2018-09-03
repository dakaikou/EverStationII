#ifndef _KERNEL_H264_DECODER_H_
#define _KERNEL_H264_DECODER_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "HAL/HAL_BitStream/Include/HALForBitStream.h"
//#include "../../ES_Include/video_common.h"

#include "../compile.h"

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
	S32		aspect_ratio_info_present_flag;				//u(1)
	S32		aspect_ratio_idc;							//u(8)
	S32		sar_width;									//u(16)
	S32		sar_height;									//u(16)
	S32		overscan_info_present_flag;					//u(1)
	S32		overscan_appropriate_flag;					//u(1)
	S32		video_signal_type_present_flag;				//u(1)
	S32		video_format;								//u(3)
	S32		video_full_range_flag;						//u(1)
	S32		colour_description_present_flag;			//u(1)
	S32		colour_primaries;							//u(8)
	S32		transfer_characteristics;					//u(8)
	S32		matrix_coefficients;						//u(8)

	S32		chroma_loc_info_present_flag;				//u(1)
	S32		chroma_sample_loc_type_top_filed;			//ue(v)
	S32		chroma_sample_loc_type_bottom_field;		//ue(v)
	S32		timing_info_present_flag;					//u(1)
	U32		num_units_in_tick;							//u(32)
	U32		time_scale;									//u(32)
	S32		fixed_frame_rate_flag;						//u(1)

	S32		nal_hrd_parameters_present_flag;			//u(1)
	S32		vcl_hrd_parameters_present_flag;			//u(1)
	S32		low_delay_hrd_flag;							//u(1)
	S32		pic_struct_present_flag;					//u(1)
	S32		bitstream_restriction_flag;					//u(1)

	S32		motion_vectors_over_pic_boundaries_flag;	//u(1)
	S32		max_bytes_per_pic_denom;					//ue(v)
	S32		max_bits_per_mb_denom;						//ue(v)
	S32		log2_max_mv_length_horizontal;				//ue(v)
	S32		log2_max_mv_length_vertical;				//ue(v)
	S32		num_reorder_frames;							//ue(v)
	S32		max_dec_frame_buffering;					//ue(v)

} H264_vui_parameters_t, *pH264_vui_parameters_t;

typedef struct
{
	U32		start_code_prefix;					//24~32bits
	S32		forbidden_zero_bit;					//f(1)
	S32		nal_ref_idc;						//u(2)
	S32		nal_unit_type;						//u(5)

	U8		rbsp_byte[16 * 1024];
	S32		NumBytesInRBSP;

} H264_nal_unit_t, *pH264_nal_unit_t;

typedef struct
{
	//syntax part	
	S32		profile_idc;									//u(8)

	S32		constraint_set0_flag;							//u(1)
	S32		constraint_set1_flag;							//u(1)
	S32		constraint_set2_flag;							//u(1)
	S32		constraint_set3_flag;							//u(1)
	S32		reserved_zero_4bits;							//u(4)

	S32		level_idc;										//u(8)

	S32		seq_parameter_set_id;							//ue(v)

	//high 100, high 110 high 122, high 144 profile
	/*{{{{					*/
	S32		chroma_format_idc;								//ue(v)
	S32		residual_colour_transform_flag;					//u(1)
	S32		bit_depth_luma_minus8;							//ue(v)
	S32		bit_depth_chroma_minus8;						//ue(v)
	S32		qpprime_y_zero_transform_bypass_flag;			//u(1)
	S32		seq_scaling_matrix_present_flag;				//u(1)
	S32		seq_scaling_list_present_flag[8];				//u(1) x 8

//	S16		ScalingList4x4[6][16];
//	S16		ScalingList8x8[2][64];
	/*}}}}					*/

	S32		log2_max_frame_num_minus4;						//ue(v)
	S32		pic_order_cnt_type;								//ue(v)

	S32		log2_max_pic_order_cnt_lsb_minus4;				//ue(v)
	S32		delta_pic_order_always_zero_flag;				//u(1)
	S32		offset_for_non_ref_pic;							//se(v)
	S32		offset_for_top_to_bottom_field;					//se(v)
	S32		num_ref_frames_in_pic_order_cnt_cycle;			//ue(v)
	S32		offset_for_ref_frame[256];						//se(v)

	S32		num_ref_frames;									//ue(v)
	S32		gaps_in_frame_num_value_allowed_flag;			//u(1)
	S32		pic_width_in_mbs_minus1;						//ue(v)
	S32		pic_height_in_map_units_minus1;					//ue(v)
	S32		frame_mbs_only_flag;							//u(1)
	S32		mb_adaptive_frame_field_flag;					//u(1)
	S32		direct_8x8_inference_flag;						//u(1)
	S32		frame_cropping_flag;							//u(1)

	S32		frame_crop_left_offset;							//ue(v)
	S32		frame_crop_right_offset;						//ue(v)
	S32		frame_crop_top_offset;							//ue(v)
	S32		frame_crop_bottom_offset;						//ue(v)

	S32		vui_parameters_present_flag;					//u(1)

	H264_vui_parameters_t	vui_parameters;

	//sematic part
	S32		SubWidthC;
	S32		SubHeightC;
	S32		MbWidthC;
	S32		MbHeightC;
	S32		BitDepthY;
	S32		BitDepthC;
	S32		QpBdOffsetY;
	S32		QpBdOffsetC;
	S32		MaxFrameNum;
	S32		MaxPicOrderCntLsb;
	S32		PicWidthInMbs;
	S32		PicHeightInMapUnits;
	S32		PicSizeInMapUnits;
	S32		FrameHeightInMbs;

	S32		PicWidthInSamplesL;
	S32		PicHeightInSamplesL;
	S32		PicWidthInSamplesC;
	S32		PicHeightInSamplesC;

} H264_seq_parameter_set_t, *pH264_seq_parameter_set_t;

typedef struct
{
	S32		pic_parameter_set_id;							//ue(v)
	S32		seq_parameter_set_id;							//ue(v)
	S32		entropy_coding_mode_flag;						//u(1)
	S32		pic_order_present_flag;							//u(1)
	S32		num_slice_groups_minus1;						//ue(v)
	S32		slice_group_map_type;							//ue(v)
	S32		run_length_minus1[8];							//ue(v)				//baseline profile最多就8个,其余profile为0
	S32		top_left[8];									//ue(v)
	S32		bottom_right[8];								//ue(v)
	S32		slice_group_change_direction_flag;				//ue(1)
	S32		slice_group_change_rate_minus1;					//ue(v)
	S32		pic_size_in_map_units_minus1;					//ue(v)
	S32		slice_group_id[16];								//u(v)
	S32		num_ref_idx_l0_active_minus1;					//ue(v)				//0~31,可以被slice_header覆盖
	S32		num_ref_idx_l1_active_minus1;					//ue(v)				//0~31,可以被slice_header覆盖
	S32		weighted_pred_flag;								//u(1)
	S32		weighted_bipred_idc;							//u(2)
	S32		pic_init_qp_minus26;							//se(v)
	S32		pic_init_qs_minus26;							//se(v)
	S32		chroma_qp_index_offset;							//se(v)
	S32		deblocking_filter_control_present_flag;			//u(1)
	S32		constrained_intra_pred_flag;					//u(1)
	S32		redundant_pic_cnt_present_flag;					//u(1)
	
	//14496-10 2005以后的版本有此部分
	S32		transform_8x8_mode_flag;						//u(1)
	S32		pic_scaling_matrix_present_flag;				//u(1)
	S32		pic_scaling_list_present_flag[8];				//u(1) x 8
	S32		second_chroma_qp_index_offset;					//se(v)

	//sematic part

} H264_pic_parameter_set_t, *pH264_pic_parameter_set_t;

typedef struct
{
	S32		ref_pic_list_reordering_flag_l0;				//u(1)
	S32		reordering_of_pic_nums_idc[2];				//ue(v)
	S32		abs_diff_pic_num_minus1[2][32];					//ue(v)
	S32		long_term_pic_num[2][32];						//ue(v)

	S32		ref_pic_list_reordering_flag_l1;				//u(1)

} H264_ref_pic_list_reordering_t, *pH264_ref_pic_list_reordering_t;

typedef struct
{
	S32		luma_log2_weight_denom;								//ue(v)
	S32		chroma_log2_weight_denom;							//ue(v)
	S32		luma_weight_l0_flag[32];							//u(1)
	S32		luma_weight_l0[32];									//se(v)
	S32		luma_offset_l0[32];									//se(v)
	S32		chroma_weight_l0_flag[32];							//u(1)
	S32		chroma_weight_l0[32][2];							//se(v)
	S32		chroma_offset_l0[32][2];							//se(v)
	S32		luma_weight_l1_flag[32];							//u(1)
	S32		luma_weight_l1[32];									//se(v)
	S32		luma_offset_l1[32];									//se(v)
	S32		chroma_weight_l1_flag[32];							//u(1)
	S32		chroma_weight_l1[32][2];							//se(v)
	S32		chroma_offset_l1[32][2];							//se(v)

} H264_pred_weight_table_t, *pH264_pred_weight_table_t;

typedef struct
{
	S32		no_output_of_prior_pics_flag;							//u(1)
	S32		long_term_reference_flag;								//u(1)
	S32		adaptive_ref_pic_marking_mode_flag;						//u(1)
	S32		memory_management_control_operation;					//ue(v)
	S32		difference_of_pic_nums_minus1[7];						//ue(v)
	S32		long_term_pic_num[7];									//ue(v)
	S32		long_term_frame_idx[7];									//ue(v)
	S32		max_long_term_frame_idx_plus1[7];						//ue(v)

} H264_dec_ref_pic_marking_t, *pH264_dec_ref_pic_marking_t;

typedef struct
{
	S32		first_mb_in_slice;					//ue(v)
	S32		slice_type;							//ue(v)
	S32		pic_parameter_set_id;				//ue(v)
	S32		frame_num;							//u(v)
	S32		field_pic_flag;						//u(1)
	S32		bottom_field_flag;					//u(1)
	S32		idr_pic_id;							//ue(v)
	S32		pic_order_cnt_lsb;					//u(v)
	S32		delta_pic_order_cnt_bottom;			//se(v)
	S32		delta_pic_order_cnt[2];				//se(v) x 2
	S32		redundant_pic_cnt;					//ue(v)
	S32		direct_spatial_mv_pred_flag;		//u(1)
	S32		num_ref_idx_active_override_flag;	//u(1)
	S32		num_ref_idx_l0_active_minus1;		//ue(v)
	S32		num_ref_idx_l1_active_minus1;		//ue(v)

	H264_ref_pic_list_reordering_t	ref_pic_list_reordering;
	H264_pred_weight_table_t		pred_weight_table;
	H264_dec_ref_pic_marking_t		dec_ref_pic_marking;

	S32		cabac_init_idc;						//ue(v)
	S32		slice_qp_delta;						//se(v)
	S32		sp_for_switch_flag;					//u(1)
	S32		slice_qs_delta;						//se(v)
	S32		disable_deblocking_filter_idc;		//ue(v)
	S32		slice_alpha_c0_offset_div2;			//se(v)
	S32		slice_beta_offset_div2;				//se(v)

	S32		slice_group_change_cycle;			//u(v)

	//semantic part
	S32		MbaffFrameFlag;
	S32		SliceQPy;
	S32		SliceQSy;
	S32		slice_number;

} H264_slice_header_t, *pH264_slice_header_t;

typedef struct
{
	S32		cabac_alignment_one_bit;			//f(1)
	S32		mb_skip_run;						//ue(v)
	S32		mb_skip_flag;						//ae(v)
	S32		mb_field_decoding_flag;				//u(1)|ae(v)
	S32		end_of_slice_flag;

} H264_slice_data_t, *pH264_slice_data_t;

typedef struct
{
	S32		mb_type;							//ue(v)/ae(v)
	S32		pcm_alignment_zero_bit;				//f(1)
	S32		pcm_sample_luma[256];				//u(v)
	S32		pcm_sample_chroma[256];				//u(v)
	S32		transform_size_8x8_flag;			//u(1)/ae(v)
	S32		coded_block_pattern;				//me(v)/ae(v)
	S32		mb_qp_delta;						//se(v)/ae(v)
	S32		prev_intra4x4_pred_mode_flag[16];
	S32		rem_intra4x4_pred_mode[16];
	S32		prev_intra8x8_pred_mode_flag[4];
	S32		rem_intra8x8_pred_mode[4];
	S32		intra_chroma_pred_mode;
	S32		ref_idx_l0[2];
	S32		ref_idx_l1[2];
	S32		mvd_l0[2][2][2];
	S32		mvd_l1[2][2][2];

	S32		slice_number;

	//semantic part
	S32		CodedBlockPatternLuma;
	S32		CodedBlockPatternChroma;

	S32		TotalCoeff[48];						//keep info
	S32		Intra4x4PredMode[48];

	S32		QPy;
	S32		QPy_prev;
	S32		QPy_;

} H264_macroblock_layer_t, *pH264_macroblock_layer_t;

typedef struct
{
	S32		mb_type;
	S32		cbp;

} H264_MB_INFO_t;

int	h264_video_decode_slice_layer_without_partitioning_rbsp(H264_nal_unit_t* pnal_unit);
int	h264_video_decode_seq_parameter_set(H264_nal_unit_t* pnal_unit);
int	h264_video_decode_pic_parameter_set(H264_nal_unit_t* pnal_unit);

_inline U32 CeilLog2(U32 uiVal)
{
	U32 uiTmp = uiVal - 1;
	U32 uiRet = 0;

	while (uiTmp != 0)
	{
		uiTmp >>= 1;
		uiRet++;
	}

	return uiRet;
}

#endif

