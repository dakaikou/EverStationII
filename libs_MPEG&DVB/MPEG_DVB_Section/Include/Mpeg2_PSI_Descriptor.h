#ifndef __MPEG2_PSI_DESCRIPTOR_H__
#define __MPEG2_PSI_DESCRIPTOR_H__

#include <stdint.h>

#include "../compile.h"
#include "MPEG_DVB_SysCapability.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

/*descriptor tag define*/

/* 0x00 - 0x3F: ITU-T Rec. H.222.0 | ISO/IEC 13818-1 */
//Reserved												0x0000
//Reserved												0x0001
#define MPEG2_PSI_VIDEO_STREAM_DESCRIPTOR							0x0002
#define MPEG2_PSI_AUDIO_STREAM_DESCRIPTOR							0x0003
#define MPEG2_PSI_HIERARCHY_DESCRIPTOR								0x0004
#define MPEG2_PSI_REGISTRATION_DESCRIPTOR							0x0005
#define MPEG2_PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR					0x0006
#define MPEG2_PSI_TARGET_BACKGROUND_GRID_DESCRIPTOR					0x0007
#define MPEG2_PSI_VIDEO_WINDOW_DESCRIPTOR							0x0008
#define MPEG2_PSI_CA_DESCRIPTOR										0x0009
#define MPEG2_PSI_ISO_639_LANGUAGE_DESCRIPTOR						0x000A
#define MPEG2_PSI_SYSTEM_CLOCK_DESCRIPTOR							0x000B
#define MPEG2_PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR			0x000C
#define MPEG2_PSI_COPYRIGHT_DESCRIPTOR								0x000D
#define MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR						0x000E
#define MPEG2_PSI_PRIVATE_DATA_INDICATOR_DESCRIPTOR					0x000F
#define MPEG2_PSI_SMOOTHING_BUFFER_DESCRIPTOR						0x0010
#define MPEG2_PSI_STD_DESCRIPTOR									0x0011
#define MPEG2_PSI_IBP_DESCRIPTOR									0x0012

//ISO?IEC 13818-1 Reserved								0x0013~0x001A			Defined in ISO/IEC 13818-6
#define MPEG2_DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR					0x0013
#define MPEG2_DSMCC_ASSOCIATION_TAG_DESCRIPTOR						0x0014
#define MPEG2_DSMCC_DEFERRED_ASSOCIATION_TAGS_DESCRIPTOR			0x0015

//ISO?IEC 13818-1 Reserved								0x0016~0x001A			Defined in ISO/IEC 13818-6
#define	MPEG2_PSI_MPEG4_VIDEO_DESCRIPTOR							0x001B
#define MPEG2_PSI_MPEG4_AUDIO_DESCRIPTOR							0x001C
#define MPEG2_PSI_IOD_DESCRIPTOR									0x001D
#define MPEG2_PSI_SL_DESCRIPTOR										0x001E
#define MPEG2_PSI_FMC_DESCRIPTOR									0x001F
#define MPEG2_PSI_EXTERNAL_ES_ID_DESCRIPTOR							0x0020
#define MPEG2_PSI_MUXCODE_DESCRIPTOR								0x0021
#define MPEG2_PSI_FMXBUFFERSIZE_DESCRIPTOR							0x0022
#define MPEG2_PSI_MULTIPLEXBUFFER_DESCRIPTOR						0x0023
#define MPEG2_PSI_FLEXMUXTIMING_DESCRIPTOR							0x0024
#define MPEG2_PSI_METADATA_POINTER_DESCRIPTOR						0x0025
#define MPEG2_PSI_METADATA_DESCRIPTOR								0x0026
#define MPEG2_PSI_METADATA_STD_DESCRIPTOR							0x0027
#define MPEG2_PSI_AVC_VIDEO_DESCRIPTOR								0x0028
#define MPEG2_PSI_IPMP_DESCRIPTOR									0x0029
#define MPEG2_PSI_AVC_TIMING_AND_HRD_DESCRIPTOR						0x002A
#define MPEG2_PSI_MPEG2_AAC_AUDIO_DESCRIPTOR						0x002B
#define MPEG2_PSI_FLEXMUX_TIMING_DESCRIPTOR							0x002C
#define MPEG2_PSI_MPEG4_TEXT_DESCRIPTOR								0x002D
#define MPEG2_PSI_MPEG4_TEXT_EXTENSION_DESCRIPTOR					0x002E
//														0x002F~0x003F			ISO/IEC 13818-1 Reserved		

/*TAG = MPEG2_PSI_VIDEO_STREAM_DESCRIPTOR			0x02*/
typedef struct _video_stream_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8			multiple_frame_rate_flag;			//1
    U8			frame_rate_code;					//4
    U8			MPEG_1_only_flag;					//1
	U8			constrained_parameter_flag;			//1
	U8			still_picture_flag;					//1

	U8			profile_and_level_indication;		//8
	U8			chroma_format;						//2
	U8			frame_rate_extension_flag;			//1
	U8			reserved;							//5

} video_stream_descriptor_t, *pvideo_stream_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_video_stream_descriptor(U8 *buf, S32 length, pvideo_stream_descriptor_t pvideo_stream_descriptor);

/*TAG = MPEG2_PSI_AUDIO_STREAM_DESCRIPTOR			0x03*/
typedef struct _audio_stream_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 free_format_flag;					//1
	U8 ID;									//1
    U8 layer;								//2
    U8 variable_rate_audio_indicator;		//1
	U8 reserved;							//3

} audio_stream_descriptor_t, *paudio_stream_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_audio_stream_descriptor(U8 *buf, S32 length, paudio_stream_descriptor_t paudio_stream_descriptor);

/*TAG = MPEG2_PSI_HIERARCHY_DESCRIPTOR		0x04*/
typedef struct _hierarchy_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	reserved0;							//4
	U8 hierarchy_type;						//4

	U8	reserved1;							//2
    U8 hierarchy_layer_index;				//6

	U8	reserved2;							//2
    U8 hierarchy_embedded_layer_index;		//6

	U8	reserved3;							//2
    U8 hierarchy_channel;					//6

} hierarchy_descriptor_t, *phierarchy_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_hierarchy_descriptor(U8 *buf, S32 length, phierarchy_descriptor_t phierarchy_descriptor);

/*TAG = MPEG2_PSI_REGISTRATION_DESCRIPTOR		0x05*/
typedef struct _registration_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32 format_identifier;						//32
	S8  format_identifier_char[5];
	S8	additional_identification_info[128];

} registration_descriptor_t, *pregistration_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_registration_descriptor(U8* buf, S32 length, pregistration_descriptor_t pregistration_descriptor);

/*TAG = MPEG2_PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR		0x06*/
typedef struct _data_stream_alignment_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 alignment_type;					//8

} data_stream_alignment_descriptor_t, *pdata_stream_alignment_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_data_stream_alignment_descriptor(U8* buf, S32 length, pdata_stream_alignment_descriptor_t pdata_stream_alignment_descriptor);

/*TAG = MPEG2_PSI_TARGET_BACKGROUND_GRID_DESCRIPTOR		0x07*/
typedef struct _target_background_grid_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 horizontal_size;						//14
	U16 vertical_size;							//14
	U8  aspect_ratio_information;				//4

} target_background_grid_descriptor_t, *ptarget_background_grid_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_target_background_grid_descriptor(U8* buf, S32 length, ptarget_background_grid_descriptor_t ptarget_background_grid_descriptor);

/*TAG = MPEG2_PSI_VIDEO_WINDOW_DESCRIPTOR				0x08*/
typedef struct _video_window_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 horizontal_offset;							//14
	U16 vertical_offset;							//14
	U8  window_priority;							//4

}video_window_descriptor_t, *pvideo_window_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_video_window_descriptor(U8* buf, S32 length, pvideo_window_descriptor_t pvideo_window_descriptor);

/*TAG = MPEG2_PSI_CA_DESCRIPTOR				0x09*/
typedef struct _CA_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	uint8_t		descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint16_t	CA_system_ID;								//16
	
	uint8_t		reserved;									//3
	uint16_t	CA_PID;										//13

	int		private_data_length;
	uint8_t	private_data_byte[MAX_PRIVATE_DATA_LENGTH];

} CA_descriptor_t, *pCA_descriptor_t;

_CDL_EXPORT int MPEG2_PSI_decode_CA_descriptor(uint8_t* buf, int length, CA_descriptor_t* pCA_descriptor);

typedef struct _ISO_639_language_s
{
	U32		ISO_639_language_code;
	S8		ISO_639_language_code_char[4];			//24
	U8		audio_type;								//8

} ISO_639_language_t, *pISO_639_language_t;

/*TAG = MPEG2_PSI_ISO_639_LANGUAGE_DESCRIPTOR				0x0A*/
typedef struct _ISO_639_language_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S32 N;
    ISO_639_language_t  ISO_639_language[MAX_LANGUAGES];

} ISO_639_language_descriptor_t, *pISO_639_language_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_ISO_639_language_descriptor(U8* buf, S32 length, pISO_639_language_descriptor_t pISO_639_language_descriptor);

/*TAG = MPEG2_PSI_SYSTEM_CLOCK_DESCRIPTOR			0x0B*/
typedef struct _system_clock_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 external_clock_reference_indicator;			//1
	U8 reserved0;									//1
	U8 clock_accuracy_integer;						//6

	U8 clock_accuracy_exponent;						//3
	U8 reserved1;									//5

} system_clock_descriptor_t, *psystem_clock_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_system_clock_descriptor(U8 *buf, S32 length, psystem_clock_descriptor_t psystem_clock_descriptor);

/*TAG = MPEG2_PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR		0x0C*/
typedef struct _multiplex_buffer_utilization_descriptor
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 bound_valid_flag;							//1
	U16 LTW_offset_lower_bound;					//15
	U8	reserved;								//1
	U16 LTW_offset_upper_bound;					//15

} multiplex_buffer_utilization_descriptor_t, *pmultiplex_buffer_utilization_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(U8 *buf, S32 length, pmultiplex_buffer_utilization_descriptor_t pmultiplex_buffer_utilization_descriptor);

/*TAG = MPEG2_PSI_COPYRIGHT_DESCRIPTOR				0x0D*/
typedef struct _copyright_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32  copyright_identifier;					//32
	S8	 additional_copyright_info[MAX_COPYRIGHT_INFO_LENGTH + 1];

} copyright_descriptor_t, *pcopyright_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_copyright_descriptor(U8* buf, S32 length, pcopyright_descriptor_t pcopyright_descriptor);

/*TAB = MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR				0x0E*/
typedef struct _maximum_bitrate_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 reserved;								//2
	U32 maximum_bitrate;						//22

} maximum_bitrate_descriptor_t, *pmaximum_bitrate_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_maximum_bitrate_descriptor(U8 *buf, S32 length, pmaximum_bitrate_descriptor_t pmaximum_bitrate_descriptor);

/*TAG = MPEG2_PSI_PRIVATE_DATA_INDICATOR_DESCRIPTOR			0x0F*/
typedef struct _private_data_indicator_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32  private_data_indicator;				//32

} private_data_indicator_descriptor_t, *pprivate_data_indicator_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_private_data_indicator_descriptor(U8* buf, S32 length, pprivate_data_indicator_descriptor_t pprivate_data_indicator_descriptor);

/*TAG = MPEG2_PSI_SMOOTHING_BUFFER_DESCRIPTOR					0x10*/
typedef struct _smoothing_buffer_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	   reserved0;										//2
	U32    sb_leak_rate;									//22
	U8	   reserved1;										//2
	U32    sb_size;											//22
} smoothing_buffer_descriptor_t, *psmoothing_buffer_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_smoothing_buffer_descriptor(U8* buf, S32 length, psmoothing_buffer_descriptor_t psmoothing_buffer_descriptor);

/*TAG = MPEG2_PSI_STD_DESCRIPTOR						0x11*/
typedef struct _STD_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 reserved;							 //7
	U8 leak_valid_flag;						//1

} STD_descriptor_t, *pSTD_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_STD_descriptor(U8* buf, S32 length, pSTD_descriptor_t pSTD_descriptor);

/*TAG = MPEG2_PSI_IBP_DESCRIPTOR					0x12*/
typedef struct _IBP_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8  closed_gop_flag;				//1
	U8  identical_gop_flag;				//1
	U16 max_gop_length;					//14

} IBP_descriptor_t, *pIBP_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_IBP_descriptor(U8* buf, S32 length, pIBP_descriptor_t pIBP_descriptor);

/*TAG = MPEG2_DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR						0x13*/
typedef struct _carousel_identifier_descriptor_s
{
	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint32_t		carousel_id;					//32

	uint8_t			N;
	uint8_t			private_data_byte[64];

} carousel_identifier_descriptor_t, *pcarousel_identifier_descriptor_t;

_CDL_EXPORT int MPEG2_DSMCC_decode_carousel_identifier_descriptor(uint8_t* buf, int length, carousel_identifier_descriptor_t* pcarousel_identifier_descriptor);

/*TAG = MPEG2_DSMCC_ASSOCIATION_TAG_DESCRIPTOR					0x14*/
typedef struct _association_tag_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint16_t	association_tag;				//16
	uint16_t	use;							//16

	uint8_t		selector_byte_length;			//8
	union
	{
		struct {
			uint32_t	transaction_id;					//32
			uint32_t	timeout;						//32
		} use_0x0000;

		struct {
			uint8_t		N1;
			uint8_t selector_byte[64];
		} use_other;
	} u;

	uint8_t		N2;
	uint8_t		private_data_byte[64];			//8 x N2

} association_tag_descriptor_t, *passociation_tag_descriptor_t;

_CDL_EXPORT int MPEG2_DSMCC_decode_association_tag_descriptor(uint8_t* buf, int length, association_tag_descriptor_t* passociation_tag_descriptor);

/*TAG = MPEG2_DSMCC_DEFERRED_ASSOCIATION_TAGS_DESCRIPTOR					0x15*/
typedef struct _deferred_association_tags_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	association_tags_loop_length;	//8
	U8	N1;
	U16 association_tag[16];			//16 x N1
	U16	transport_stream_id;			//16
	U16 program_number;					//16
	U8  N2;
	U8  private_data_byte[64];			//8 x N2

} deferred_association_tags_descriptor_t, *pdeferred_association_tags_descriptor_t;

_CDL_EXPORT S32 DSMCC_decode_deferred_association_tags_descriptor(U8* buf, S32 length, pdeferred_association_tags_descriptor_t pdeferred_association_tags_descriptor);

/*TAG = MPEG2_PSI_MPEG4_VIDEO_DESCRIPTOR				0x1B	*/
typedef struct _MPEG4_video_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	MPEG4_visual_profile_and_level;		//8

} MPEG4_video_descriptor_t, *pMPEG4_video_descriptor_t;

_CDL_EXPORT	S32 MPEG2_PSI_decode_MPEG4_video_descriptor(U8* buf, S32 length, pMPEG4_video_descriptor_t pMPEG4_video_descriptor);

/*TAG = MPEG2_MPEG4_AUDIO_DESCRIPTOR				0x1C	*/
typedef struct _MPEG4_audio_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	MPEG4_audio_profile_and_level;		//8

} MPEG4_audio_descriptor_t, *pMPEG4_audio_descriptor_t;

_CDL_EXPORT	S32 MPEG2_PSI_decode_MPEG4_audio_descriptor(U8* buf, S32 length, pMPEG4_audio_descriptor_t pMPEG4_audio_descriptor);

/*TAG = MPEG2_IOD_DESCRIPTOR						0x1D	*/
typedef struct _IOD_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8	Scope_of_IOD_lable;					//8
	U8	IOD_label;							//8
	U8	InitialObjectDescriptor;			//8

} IOD_descriptor_t, *pIOD_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_IOD_descriptor(U8* buf, S32 length, pIOD_descriptor_t pIOD_descriptor);

/*TAG = MPEG2_SL_DESCRIPTOR						0x1E	*/
typedef struct _SL_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16	ES_ID;								//16

} SL_descriptor_t, *pSL_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_SL_descriptor(U8* buf, S32 length, pSL_descriptor_t pSL_descriptor);

/*TAG = MPEG2_FMC_DESCRIPTOR						0x1F	*/
typedef struct _FMC_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S8	N;

	U16	ES_ID[1];							//16
	U8	FlexMuxChannel[1];					//8
} FMC_descriptor_t, *pFMC_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_FMC_descriptor(U8* buf, S32 length, pFMC_descriptor_t pFMC_descriptor);

/*TAG = MPEG2_EXTERNAL_ES_ID_DESCRIPTOR		0x20	*/
typedef struct _External_ES_ID_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16	External_ES_ID;								//16
} External_ES_ID_descriptor_t, *pExternal_ES_ID_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_External_ES_ID_descriptor(U8* buf, S32 length, pExternal_ES_ID_descriptor_t pExternal_ES_ID_descriptor);

/*TAG = MPEG2_MUXCODE_DESCRIPTOR						0x21	*/
typedef struct _Muxcode_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	//	MuxCodeTableEntry()

} Muxcode_descriptor_t, *pMuxcode_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_Muxcode_descriptor(U8* buf, S32 length, pMuxcode_descriptor_t pMuxcode_descriptor);

/*TAG = FMXBUFFERSIZE_DESCRIPTOR				0x22	*/
typedef struct _FmxBufferSize_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	//DefaultFlexMuxBufferDescriptor()

} FmxBufferSize_descriptor_t, *pFmxBufferSize_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_FmxBufferSize_descriptor(U8* buf, S32 length, pFmxBufferSize_descriptor_t pFmxBufferSize_descriptor);

/*TAG = MULTIPLEXBUFFER_DESCRIPTOR				0x23	*/
typedef struct _MultiplexBuffer_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32	MB_buffer_size;
	U32 TB_leak_rate;

} MultiplexBuffer_descriptor_t, *pMultiplexBuffer_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_MultiplexBuffer_descriptor(U8* buf, S32 length, pMultiplexBuffer_descriptor_t pMultiplexBuffer_descriptor);

/*TAG = FLEXMUXTIMING_DESCRIPTOR				0x24	*/
/*TAG = METADATA_POINTER_DESCRIPTOR				0x25	*/
/*TAG = METADATA_DESCRIPTOR						0x26	*/
/*TAG = METADATA_STD_DESCRIPTOR					0x27	*/

/*TAG = AVC_VIDEO_DESCRIPTOR					0x28	*/
typedef struct _AVC_video_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8	profile_idc;				//8

	U8	constraint_set0_flag;		//1
	U8	constraint_set1_flag;		//1
	U8	constraint_set2_flag;		//1
	U8	AVC_compatible_flags;		//5

	U8	level_idc;					//8
	
	U8	AVC_still_present;			//1
	U8	AVC_24_hour_picture_flag;	//1
	U8	reserved;					//6

} AVC_video_descriptor_t, *pAVC_video_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_AVC_video_descriptor(U8* buf, S32 length, pAVC_video_descriptor_t pAVC_video_descriptor);

/*TAG = IPMP_DESCRIPTOR							0x29	*/

/*TAG = AVC_TIMING_AND_HRD_DESCRIPTOR	0x2A	*/
typedef struct _AVC_timing_and_HRD_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	hrd_management_valid_flag;					//1
	U8	reserved0;									//6
	U8	picture_and_timing_info_present;			//1

	U8	flag_90kHz;									//1
	U8	reserved1;									//7

	S32	N;											//32
	S32	K;											//32
	S32	num_units_in_tick;							//32

	U8	fixed_frame_rate_flag;						//1
	U8	temporal_poc_flag;							//1
	U8	picture_to_display_conversion_flag;			//1
	U8	reserved2;									//5

} AVC_timing_and_HRD_descriptor_t, *pAVC_timing_and_HRD_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_AVC_timing_and_HRD_descriptor(U8* buf, S32 length, pAVC_timing_and_HRD_descriptor_t pAVC_timing_and_HRD_descriptor);

/*TAG = MPEG2_AAC_AUDIO_DESCRIPTOR						0x2B	*/
typedef struct _MPEG2_AAC_audio_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	MPEG2_AAC_profile;			//8
	U8	MPEG2_AAC_channel_configuration;		//8
	U8	MPEG2_AAC_additional_information;		//8

} MPEG2_AAC_audio_descriptor_t, *pMPEG2_AAC_audio_descriptor_t;

_CDL_EXPORT S32 MPEG2_PSI_decode_MPEG2_AAC_audio_descriptor(U8* buf, S32 length, pMPEG2_AAC_audio_descriptor_t pMPEG2_AAC_audio_descriptor);

/*TAG = FLEXMUX_TIMING_DESCRIPTOR						0x2C	*/
/*TAG = MPEG4_TEXT_DESCRIPTOR							0x2D	*/
/*TAG = MPEG4_TEXT_EXTENSION_DESCRIPTOR					0x2E	*/


#endif