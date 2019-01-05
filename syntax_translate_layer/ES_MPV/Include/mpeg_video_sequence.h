#ifndef _KERNEL_MPEG_VIDEO_DECODER_H_
#define _KERNEL_MPEG_VIDEO_DECODER_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPV_EXPORT
#       define MPV_SYNTAX_LIB __declspec(dllexport)
#   elif defined(_MPV_IMPORT)
#       define MPV_SYNTAX_LIB __declspec(dllimport)
#   else
#       define MPV_SYNTAX_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPV_SYNTAX_LIB __attribute__((visibility("default")))
#else
#   define MPV_SYNTAX_LIB
#endif

#include <stdint.h>

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
	int		val;
	int		len;

} VLC_TABLE_t;

typedef struct 
{
	int		run;
	int		level;
	int		len;

} DCT_COEFFICIENT_TABLE_t;

typedef struct
{
	int		run;
	int		signed_level;
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

//define this struct is used to save the original nal segment buffer information
typedef struct
{
	uint8_t* buf;					//volatilable
	int		 length;
} MPGV_nal_snapshot_t;

typedef struct MPGV_sequence_header
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	/*syntax part*/
	uint32_t	sequence_header_code;				//32

	uint16_t	horizontal_size_value;				//12
	uint16_t	vertical_size_value;				//12
	uint8_t		aspect_ratio_information;			//4
	uint8_t		frame_rate_code;					//4

	uint16_t	bit_rate_value;						//18
	uint8_t		marker_bit;							//1
	uint16_t	vbv_buffer_size_value;				//10
	uint8_t		constrained_parameters_flag;		//1
	
	uint8_t		load_intra_quantiser_matrix;		//1
	uint8_t		intra_quantiser_matrix[64];			//8 * 64

	uint8_t		load_non_intra_quantiser_matrix;	//1
	uint8_t		non_intra_quantiser_matrix[64];		//8 * 64

	uint8_t		padding_bits_value;					//8
	int			padding_bits_length;
	uint8_t*    padding_bytes;
	int			padding_length;

} MPGV_sequence_header_t, *pMPGV_sequence_header_t;

typedef struct MPGV_sequence_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;				//32

	uint8_t		extension_start_code_identifier;		//4
	uint8_t		profile_and_level_indication;		//8
	uint8_t		progressive_sequence;		//1
	uint8_t		chroma_format;		//2
	uint8_t		horizontal_size_extension;		//2
	uint8_t		vertical_size_extension;		//2
	uint16_t		bit_rate_extension;	//12
	uint8_t		marker_bit;		//1

	uint8_t		vbv_buffer_size_extension;		//8

	uint8_t		low_delay;		//1
	uint8_t		frame_rate_extension_n;		//2
	uint8_t		frame_rate_extension_d;		//5

	uint8_t		padding_bits_value;								//8
	int			padding_bits_length;
	uint8_t*    padding_bytes;
	int			padding_length;

} MPGV_sequence_extension_t, *pMPGV_sequence_extension_t;

typedef struct MPGV_sequence_display_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4
	uint8_t		video_format;		//3
	uint8_t		colour_description;		//1

	uint8_t		colour_primaries;							//8
	uint8_t		transfer_characteristics;					//8
	uint8_t		matrix_coefficients;						//8

	uint16_t		display_horizontal_size;	//14
	uint8_t		marker_bit;		//1
	uint16_t		display_vertical_size;	//14

} MPGV_sequence_display_extension_t, *pMPGV_sequence_display_extension_t;

typedef struct MPGV_sequence_scalable_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4
	uint8_t		scalable_mode;		//2

	uint8_t		layer_id;									//4

} MPGV_sequence_scalable_extension_t, *pMPGV_sequence_scalable_extension_t;

typedef struct MPGV_quant_matrix_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4

	uint8_t		load_intra_quantiser_matrix;				//1
	uint16_t		intra_quantiser_matrix[64];					//8*64

	uint8_t		load_non_intra_quantiser_matrix;			//1
	uint16_t		non_intra_quantiser_matrix[64];				//8*64

	uint8_t		load_chroma_intra_quantiser_matrix;			//1
	uint16_t		chroma_intra_quantiser_matrix[64];			//8*64

	uint8_t		load_chroma_non_intra_quantiser_matrix;		//1
	uint16_t		chroma_non_intra_quantiser_matrix[64];		//8*64


} MPGV_quant_matrix_extension_t, *pMPGV_quant_matrix_extension_t;

typedef struct MPGV_picture_display_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4

} MPGV_picture_display_extension_t, *pMPGV_picture_display_extension_t;

typedef struct MPGV_picture_temporal_scalable_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4
	uint8_t		reference_select_code;		//2

	uint16_t		forward_temporal_reference;	//10
	uint8_t		marker_bit;		//1
	uint16_t		backward_temporal_reference;	//10

} MPGV_picture_temporal_scalable_extension_t, *pMPGV_picture_temporal_scalable_extension_t;

typedef struct MPGV_picture_spatial_scalable_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4

} MPGV_picture_spatial_scalable_extension_t, *pMPGV_picture_spatial_scalable_extension_t;

typedef struct MPGV_copyright_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;		//4

} MPGV_copyright_extension_t, *pMPGV_copyright_extension_t;

typedef struct MPGV_sequence_end
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		sequence_end_code;						//32

} MPGV_sequence_end_t, *pMPGV_sequence_end_t;

typedef struct MPGV_group_of_pictures_header
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		group_start_code;							//32

	uint8_t		drop_frame_flag;
	uint8_t		time_code_hours;
	uint8_t		time_code_minutes;
	uint8_t		marker_bit;
	uint8_t		time_code_seconds;
	uint8_t		time_code_pictures;
	uint8_t		closed_gop;				//1
	uint8_t		broken_link;			//1

	uint8_t		padding_bits_value;								//8
	int			padding_bits_length;
	uint8_t*    padding_bytes;
	int			padding_length;

} MPGV_group_of_pictures_header_t, *pMPGV_group_of_pictures_header_t;

typedef struct MPGV_picture_header
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t picture_start_code;							//32

	uint16_t		temporal_reference;						//10
	uint8_t		picture_coding_type;					//3
	uint16_t		vbv_delay;								//16

	uint8_t		full_pel_forward_vector;		//1
	uint8_t		forward_f_code;					//3
	uint8_t		full_pel_backward_vector;		//1
	uint8_t		backward_f_code;				//3

	uint8_t extra_bit_picture;				//1
	uint8_t extra_information_picture;		//8

	uint8_t		padding_bits_value;								//8
	int			padding_bits_length;
	uint8_t*    padding_bytes;
	int			padding_length;

} MPGV_picture_header_t, *pMPGV_picture_header_t;

typedef struct MPGV_picture_coding_extension
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t		extension_start_code;						//32

	uint8_t		extension_start_code_identifier;			//4

	uint8_t		f_code[2][2];								//4*4

	uint8_t		intra_dc_precision;							//2
	uint8_t		picture_structure;							//2

	uint8_t		top_field_first;							//1
	uint8_t		frame_pred_frame_dct;						//1
	uint8_t		concealment_motion_vectors;					//1
	uint8_t		q_scale_type;								//1
	uint8_t		intra_vlc_format;							//1
	uint8_t		alternate_scan;								//1
	uint8_t		repeat_first_field;							//1
	uint8_t		chroma_420_type;							//1

	uint8_t		progressive_frame;							//1
	uint8_t		composite_display_flag;						//1
	uint8_t		v_axis;										//1
	uint8_t		field_sequence;								//3
	uint8_t		sub_carrier;								//1
	uint8_t		burst_amplitude;							//7
	uint8_t		sub_carrier_phase;							//8

	uint8_t		padding_bits_value;								//8
	int			padding_bits_length;
	uint8_t*    padding_bytes;
	int			padding_length;

} MPGV_picture_coding_extension_t, *pMPGV_picture_coding_extension_t;

//typedef struct MPGV_unknown_nal
//{
//	uint32_t		start_code;
//
//} MPGV_unknown_nal_t, *pMPGV_unknown_nal_t;

typedef struct MPGV_user_data
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	uint32_t	user_data_start_code;

	uint8_t*	user_data_buf;
	int			user_data_length;

} MPGV_user_data_t, *pMPGV_user_data_t;

typedef struct MPGV_slice
{
	//original buffer snapshot
	MPGV_nal_snapshot_t nal_snapshot;

	//sematic part
	uint8_t		slice_vertical_position;

	//syntax part
	uint32_t	slice_start_code;							//32
	uint8_t		slice_vertical_position_extension;			//3
	uint8_t		priority_breakpoint;						//7
	uint8_t		quantiser_scale_code;						//5
	uint8_t		intra_slice_flag;							//1
	uint8_t		intra_slice;								//1
//	uint8_t		reserved_bits;								//7
	uint8_t		slice_picture_id_enable;					//1
	uint8_t		slice_picture_id;							//6
	uint8_t		extra_bit_slice;							//1
	uint8_t		extra_information_slice;					//8

	uint8_t		padding_bits_value;							//8
	int			padding_bits_length;
	uint8_t*    padding_bytes;
	int			padding_length;

} MPGV_slice_t, *pMPGV_slice_t;

typedef struct MPGV_macroblock_modes
{
	uint8_t		macroblock_quant;
	uint8_t		macroblock_motion_forward;
	uint8_t		macroblock_motion_backward;
	uint8_t		macroblock_pattern;
	uint8_t		macroblock_intra;
	uint8_t		spatial_temporal_weight_code_flag;
	uint8_t		permitted_spatial_temporal_weight_classes;

	uint8_t		spatial_temporal_weight_code;
	uint8_t		motion_type;
	uint8_t		dct_type;

	uint8_t		motion_vector_count;
	uint8_t		mv_format;
	uint8_t		dmv;
	uint8_t		mvscale;
	int		motion_vertical_field_select[2][2];

} MPGV_macroblock_modes_t, *pMPGV_macroblock_modes_t;

typedef struct
{
	uint16_t		previous_macroblock_address;					//
	uint16_t		macroblock_address;								//
	uint16_t		macroblock_address_increment;
	uint16_t		max_mb_address_in_slice;
	uint8_t		quantiser_scale_code;							//5
	uint8_t		marker_bit;										//1

	uint8_t		pattern_code[12];								//
	uint16_t		QFS[64];										//4:2:0 = 6, 4:2:2 = 8, 4:4:4 = 12

	int		dc_dct_pred[3];									//DCÏµÊýÔ¤²âÆ÷
	int		PMV[2][2][2];

} MPGV_macroblock_t, *pMPGV_macroblock_t;

typedef struct
{
	uint8_t		Type;
	uint8_t		Quant;
	uint8_t		DCT;
	uint8_t		Cbp;
	uint8_t		MotionType;
	uint8_t		Skipped;
	uint8_t		MotionVector[2][2][2];

} MPEG_MB_INFO_t;

//int		VLC_macroblock_address_increment(BITS_t* pbs);
//int		VLC_macroblock_type(int picture_coding_type, MPGV_macroblock_modes_t* pmacroblock_modes, BITS_t* pbs);
//int		VLC_dct_dc_size_luminance(BITS_t* pbs);
//int		VLC_dct_dc_size_chrominance(BITS_t* pbs);
//int		VLC_first_dct_coefficient(BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient, int mpeg2_video_stream);
//int		VLC_next_dct_coefficient(int macroblock_intra, int intra_vlc_format, BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient, int mpeg2_video_stream);
//int		VLC_dct_coefficient0(BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient, int mpeg2_video_stream);
//int		VLC_dct_coefficient1(BITS_t* pbs, DCT_COEFFICIENT_t* pdct_coefficient);
//int		VLC_coded_block_pattern_420(BITS_t* pbs);
//int		VLC_motion_code(BITS_t* pbs);
//int		VLC_dmvector(BITS_t* pbs);

MPV_SYNTAX_LIB int mpgv_decode_sequence_header(uint8_t* nal_buf, int nal_length, MPGV_sequence_header_t* psequence_header);
MPV_SYNTAX_LIB int	mpgv_decode_sequence_extension(uint8_t* nal_buf, int nal_length, MPGV_sequence_extension_t* psequence_extension);
MPV_SYNTAX_LIB int mpgv_decode_sequence_display_extension(uint8_t* nal_buf, int nal_length, MPGV_sequence_display_extension_t* psequence_display_extension);
MPV_SYNTAX_LIB int	mpgv_decode_group_of_pictures_header(uint8_t* nal_buf, int nal_length, MPGV_group_of_pictures_header_t* pgroup_of_pictures_header);
MPV_SYNTAX_LIB int mpgv_decode_picture_header(uint8_t* nal_buf, int nal_length, MPGV_picture_header_t* ppicture_header);
MPV_SYNTAX_LIB int	mpgv_decode_picture_coding_extension(uint8_t* nal_buf, int nal_length, MPGV_picture_coding_extension_t* ppicture_coding_extension);
MPV_SYNTAX_LIB int	mpgv_decode_slice(uint8_t* nal_buf, int nal_length, MPGV_slice_t* pslice);
MPV_SYNTAX_LIB int	mpgv_decode_user_data(uint8_t* nal_buf, int nal_length, MPGV_user_data_t* puser_data);


#endif


