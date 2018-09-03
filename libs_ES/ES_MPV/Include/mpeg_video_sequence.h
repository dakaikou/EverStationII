#ifndef _KERNEL_MPEG_VIDEO_DECODER_H_
#define _KERNEL_MPEG_VIDEO_DECODER_H_

#include "../compile.h"

#include "HAL\HAL_Sys\Include\IntTypes.h"

#define MPEG_VID_DECODER_DEBUG				0
#define MPEG_VID_FRAMERATE_DEBUG			0

#define MPEG_VID_DECODER_I					0
#define MPEG_VID_DECODER_P					0
#define MPEG_VID_DECODER_B					0
#define MPEG_VID_DECODER_IP					0
#define MPEG_VID_DECODER_IBP				1

#define MPEG_VID_DECODER_Luma				1
#define MPEG_VID_DECODER_Chroma				1

#define MPEG_VID_DECODER_FIELD				1
#define MPEG_VID_DECODER_CLEAR				1

#define MPEG_VID_DECODER_FastAlgorithm		1

#define MPEG_VID_DECODER_IDCT_Fast			0
#define MPEG_VID_DECODER_IDCT_Ref			0
#define MPEG_VID_DECODER_IDCT_Fpu			1
#define MPEG_VID_DECODER_IDCT_Mmx			0

#define Abs(x)								((x >= 0) ? x : -x)
#define Sign(x)								((x > 0) ? 1 : ((x == 0) ? 0 : -1))

typedef enum
{
	MPEG_I_FRAME			= 1,
	MPEG_P_FRAME,
	MPEG_B_FRAME,
	MPEG_D_FRAME
} MPEG_FRAME_TYPE_e;

typedef enum
{
	MPEG_TOP_FIELD			= 1,
	MPEG_BOTTOM_FIELD,
	MPEG_FRAME_PICTURE
} MPEG_FRAME_FIELD_TYPE_e;

typedef enum
{
	MPEG_DATA_PARTITIONING	= 0,
	MPEG_SPATIAL_SCALABILITY,
	MPEG_SNR_SCALABILITY,
	MPEG_TEMPORAL_SCALABILITY
} MPEG_FRAME_COMPRESS_TYPE_e;

typedef enum
{
	FIELD_PRED_FIELD_BASED	= 1,
	FIELD_PRED_16X8MC,
	FIELD_PRED_DUAL_PRIME
} FIELD_PRED_TYPE_e;

typedef enum
{
	FRAME_PRED_FIELD_BASED	= 1,
	FRAME_PRED_FRAME_BASED,
	FRAME_PRED_DUAL_PRIME
} FRAME_PRED_TYPE_e;

typedef enum
{
	MV_FORMAT_FIELD = 0,
	MV_FORMAT_FRAME
} MV_FORMAT_TYPE_e;

typedef struct 
{
	S32		val;
	S32		len;

} VLC_TABLE_t;

typedef struct 
{
	S32		run;
	S32		level;
	S32		len;

} DCT_COEFFICIENT_TABLE_t;

typedef struct
{
	S32		run;
	S32		signed_level;
} DCT_COEFFICIENT_t;

#define MPGV_PICTURE_START_CODE			0x00
#define MPGV_SLICE_START_CODE_MIN		0x01
#define MPGV_SLICE_START_CODE_MAX		0xAF
//reserved							0xB0
//reserved							0xB1
#define MPGV_USER_DATA_START_CODE		0xB2
#define MPGV_SEQUENCE_HEADER_CODE		0xB3
#define MPGV_SEQUENCE_ERROR_CODE		0xB4
#define MPGV_EXTENSION_START_CODE		0xB5
//reserved							0xB6
#define MPGV_SEQUENCE_END_CODE			0xB7
#define MPGV_GROUP_START_CODE			0xB8

//	0x0		reserved	
#define MPGV_SEQUENCE_EXTENSION_ID					0x1
#define MPGV_SEQUENCE_DISPLAY_EXTENSION_ID			0x2
#define MPGV_QUANT_MATRIX_EXTENSION_ID				0x3
#define MPGV_COPYRIGHT_EXTENSION_ID					0x4
#define MPGV_SEQUENCE_SCALABLE_EXTENSION_ID			0x5
//  0x6	reserved
#define MPGV_PICTURE_DISPLAY_EXTENSION_ID			0x7
#define MPGV_PICTURE_CODING_EXTENSION_ID			0x8
#define MPGV_PICTURE_SPATIAL_SCALABLE_EXTENSION_ID	0x9
#define MPGV_PICTURE_TEMPORAL_SCALABLE_EXTENSION_ID	0xA
//  0xB ~ 0xF reserved

typedef struct MPGV_sequence_header
{
	/*syntax part*/
	U32		sequence_header_code;				//32

	S16		horizontal_size_value;				//12
	S16		vertical_size_value;				//12
	S8		aspect_ratio_information;			//4
	S8		frame_rate_code;					//4

	S32		bit_rate_value;						//18
	S8		marker_bit;							//1
	S16		vbv_buffer_size_value;				//10
	S8		constrained_parameters_flag;		//1
	
	S8		load_intra_quantiser_matrix;		//1
	S16		intra_quantiser_matrix[64];			//8 * 64

	S8		load_non_intra_quantiser_matrix;	//1
	S16		non_intra_quantiser_matrix[64];		//8 * 64

} MPGV_sequence_header_t, *pMPGV_sequence_header_t;

typedef struct MPGV_sequence_extension
{
	U32		extension_start_code;				//32

	S8		extension_start_code_identifier;		//4
	S8		profile_and_level_indication;		//8
	S8		progressive_sequence;		//1
	S8		chroma_format;		//2
	S8		horizontal_size_extension;		//2
	S8		vertical_size_extension;		//2
	S16		bit_rate_extension;	//12
	S8		marker_bit;		//1

	U8		vbv_buffer_size_extension;		//8

	S8		low_delay;		//1
	S8		frame_rate_extension_n;		//2
	S8		frame_rate_extension_d;		//5

} MPGV_sequence_extension_t, *pMPGV_sequence_extension_t;

typedef struct MPGV_sequence_display_extension
{
	U32		extension_start_code;						//32

	U8		extension_start_code_identifier;		//4
	U8		video_format;		//3
	U8		colour_description;		//1

	U8		colour_primaries;							//8
	U8		transfer_characteristics;					//8
	U8		matrix_coefficients;						//8

	S16		display_horizontal_size;	//14
	S16		marker_bit;		//1
	S16		display_vertical_size;	//14

} MPGV_sequence_display_extension_t, *pMPGV_sequence_display_extension_t;

typedef struct MPGV_sequence_scalable_extension
{
	U32		extension_start_code;						//32

	S8		extension_start_code_identifier;		//4
	S8		scalable_mode;		//2

	U8		layer_id;									//4

} MPGV_sequence_scalable_extension_t, *pMPGV_sequence_scalable_extension_t;

typedef struct MPGV_quant_matrix_extension
{
	U32		extension_start_code;						//32

	S8		extension_start_code_identifier;		//4

	U8		load_intra_quantiser_matrix;				//1
	S16		intra_quantiser_matrix[64];					//8*64

	U8		load_non_intra_quantiser_matrix;			//1
	S16		non_intra_quantiser_matrix[64];				//8*64

	U8		load_chroma_intra_quantiser_matrix;			//1
	S16		chroma_intra_quantiser_matrix[64];			//8*64

	U8		load_chroma_non_intra_quantiser_matrix;		//1
	S16		chroma_non_intra_quantiser_matrix[64];		//8*64


} MPGV_quant_matrix_extension_t, *pMPGV_quant_matrix_extension_t;

typedef struct MPGV_picture_display_extension
{
	U32		extension_start_code;						//32

	S8		extension_start_code_identifier;		//4

} MPGV_picture_display_extension_t, *pMPGV_picture_display_extension_t;

typedef struct MPGV_picture_temporal_scalable_extension
{
	U32		extension_start_code;						//32

	U8		extension_start_code_identifier;		//4
	U8		reference_select_code;		//2

	S16		forward_temporal_reference;	//10
	S8		marker_bit;		//1
	S16		backward_temporal_reference;	//10

} MPGV_picture_temporal_scalable_extension_t, *pMPGV_picture_temporal_scalable_extension_t;

typedef struct MPGV_picture_spatial_scalable_extension
{
	U32		extension_start_code;						//32

	U8		extension_start_code_identifier;		//4

} MPGV_picture_spatial_scalable_extension_t, *pMPGV_picture_spatial_scalable_extension_t;

typedef struct MPGV_copyright_extension
{
	U32		extension_start_code;						//32

	U8		extension_start_code_identifier;		//4

} MPGV_copyright_extension_t, *pMPGV_copyright_extension_t;

typedef struct MPGV_sequence_end
{
	U32		sequence_end_code;						//32

} MPGV_sequence_end_t, *pMPGV_sequence_end_t;

typedef struct MPGV_group_of_pictures_header
{
	U32		group_start_code;							//32

	U8		drop_frame_flag;
	U8		time_code_hours;
	U8		time_code_minutes;
	U8		marker_bit;
	U8		time_code_seconds;
	U8		time_code_pictures;
	U8		closed_gop;				//1
	U8		broken_link;			//1

} MPGV_group_of_pictures_header_t, *pMPGV_group_of_pictures_header_t;

typedef struct MPGV_picture_header
{
	U32		picture_start_code;							//32

	S16		temporal_reference;						//10
	S8		picture_coding_type;					//3
	U16		vbv_delay;								//16

	U8		full_pel_forward_vector;		//1
	U8		forward_f_code;					//3
	U8		full_pel_backward_vector;		//1
	U8		backward_f_code;				//3

	uint8_t extra_bit_picture;				//1
	uint8_t extra_information_picture;		//8

} MPGV_picture_header_t, *pMPGV_picture_header_t;

typedef struct MPGV_picture_coding_extension
{
	U32		extension_start_code;						//32

	U8		extension_start_code_identifier;			//4

	S8		f_code[2][2];								//4*4

	U8		intra_dc_precision;							//2
	U8		picture_structure;							//2

	U8		top_field_first;							//1
	U8		frame_pred_frame_dct;						//1
	U8		concealment_motion_vectors;					//1
	U8		q_scale_type;								//1
	U8		intra_vlc_format;							//1
	U8		alternate_scan;								//1
	U8		repeat_first_field;							//1
	U8		chroma_420_type;							//1

	U8		progressive_frame;							//1
	U8		composite_display_flag;						//1
	U8		v_axis;										//1
	U8		field_sequence;								//3
	U8		sub_carrier;								//1
	U8		burst_amplitude;							//7
	U8		sub_carrier_phase;							//8

} MPGV_picture_coding_extension_t, *pMPGV_picture_coding_extension_t;

typedef struct MPGV_unknown_nal
{
	U32		start_code;

} MPGV_unknown_nal_t, *pMPGV_unknown_nal_t;

typedef struct MPGV_user_data
{
	U32		user_data_start_code;

} MPGV_user_data_t, *pMPGV_user_data_t;

typedef struct MPGV_slice
{
	U32		slice_start_code;							//32
	U8		slice_vertical_position;
	U8		slice_vertical_position_extension;			//3
	U8		priority_breakpoint;						//7
	U8		quantiser_scale_code;						//5
	U8		intra_slice_flag;							//1
	U8		intra_slice;								//1
//	U8		reserved_bits;								//7
	U8		slice_picture_id_enable;					//1
	U8		slice_picture_id;							//6
	U8		extra_bit_slice;							//1
	U8		extra_information_slice;					//8

} MPGV_slice_t, *pMPGV_slice_t;

typedef struct MPGV_macroblock_modes
{
	U8		macroblock_quant;
	U8		macroblock_motion_forward;
	U8		macroblock_motion_backward;
	U8		macroblock_pattern;
	U8		macroblock_intra;
	U8		spatial_temporal_weight_code_flag;
	U8		permitted_spatial_temporal_weight_classes;

	U8		spatial_temporal_weight_code;
	U8		motion_type;
	S8		dct_type;

	U8		motion_vector_count;
	U8		mv_format;
	U8		dmv;
	U8		mvscale;
	S32		motion_vertical_field_select[2][2];

} MPGV_macroblock_modes_t, *pMPGV_macroblock_modes_t;

typedef struct
{
	S16		previous_macroblock_address;					//
	S16		macroblock_address;								//
	S16		macroblock_address_increment;
	S16		max_mb_address_in_slice;	
	U8		quantiser_scale_code;							//5
	U8		marker_bit;										//1

	U8		pattern_code[12];								//
	S16		QFS[64];										//4:2:0 = 6, 4:2:2 = 8, 4:4:4 = 12

	S32		dc_dct_pred[3];									//DCÏµÊýÔ¤²âÆ÷
	S32		PMV[2][2][2];

} MPGV_macroblock_t, *pMPGV_macroblock_t;

typedef struct
{
	U8		Type;
	S8		Quant;
	S8		DCT;
	U8		Cbp;
	U8		MotionType;
	S8		Skipped;
	S8		MotionVector[2][2][2];

} MPEG_MB_INFO_t;

//S32		VLC_macroblock_address_increment(BITS_t* pbs);
//S32		VLC_macroblock_type(S32 picture_coding_type, MPGV_macroblock_modes_t* pmacroblock_modes, BITS_t* pbs);
//S32		VLC_dct_dc_size_luminance(BITS_t* pbs);
//S32		VLC_dct_dc_size_chrominance(BITS_t* pbs);
//S32		VLC_first_dct_coefficient(BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient, S32 mpeg2_video_stream);
//S32		VLC_next_dct_coefficient(S32 macroblock_intra, S32 intra_vlc_format, BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient, S32 mpeg2_video_stream);
//S32		VLC_dct_coefficient0(BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient, S32 mpeg2_video_stream);
//S32		VLC_dct_coefficient1(BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient);
//S32		VLC_coded_block_pattern_420(BITS_t* pbs);
//S32		VLC_motion_code(BITS_t* pbs);
//S32		VLC_dmvector(BITS_t* pbs);

_CDL_EXPORT int mpgv_decode_sequence_header(uint8_t* nal_buf, int nal_length, MPGV_sequence_header_t* psequence_header);

_CDL_EXPORT int	mpgv_decode_sequence_extension(uint8_t* nal_buf, int nal_length, MPGV_sequence_extension_t* psequence_extension);

_CDL_EXPORT int	mpgv_decode_group_of_pictures_header(uint8_t* nal_buf, int nal_length, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header);

_CDL_EXPORT int mpgv_decode_picture_header(uint8_t* nal_buf, int nal_length, MPGV_picture_header_t* ppicture_header);

_CDL_EXPORT int	mpgv_decode_picture_coding_extension(uint8_t* nal_buf, int nal_length, MPGV_picture_coding_extension_t* ppicture_coding_extension);

_CDL_EXPORT int	mpgv_decode_slice(uint8_t* nal_buf, int nal_length, MPGV_slice_t* pslice);

_CDL_EXPORT int	mpgv_picture_type_lookup(uint8_t code, char* pszTemp, int strSize);

#endif


