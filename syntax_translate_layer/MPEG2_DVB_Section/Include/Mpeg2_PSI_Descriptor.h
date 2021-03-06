#ifndef __MPEG2_PSI_DESCRIPTOR_H__
#define __MPEG2_PSI_DESCRIPTOR_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define MPEG2_PSI_DESCRIPTOR_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define MPEG2_PSI_DESCRIPTOR_LIB __declspec(dllimport)
#   else
#       define MPEG2_PSI_DESCRIPTOR_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_PSI_DESCRIPTOR_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_PSI_DESCRIPTOR_LIB
#endif

#include <stdint.h>

#include "MPEG2_DVB_SysCapability.h"

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
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint8_t		multiple_frame_rate_flag;			//1
	uint8_t		frame_rate_code;					//4
	uint8_t		MPEG_1_only_flag;					//1
	uint8_t		constrained_parameter_flag;			//1
	uint8_t		still_picture_flag;					//1

	uint8_t		profile_and_level_indication;		//8
	uint8_t		chroma_format;						//2
	uint8_t		frame_rate_extension_flag;			//1
	uint8_t		reserved;							//5

} video_stream_descriptor_t, *pvideo_stream_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_video_stream_descriptor(uint8_t *buf, int length, video_stream_descriptor_t* pvideo_stream_descriptor);

/*TAG = MPEG2_PSI_AUDIO_STREAM_DESCRIPTOR			0x03*/
typedef struct _audio_stream_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint8_t		free_format_flag;					//1
	uint8_t		ID;									//1
	uint8_t		layer;								//2
	uint8_t		variable_rate_audio_indicator;		//1
	uint8_t		reserved;							//3

} audio_stream_descriptor_t, *paudio_stream_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_audio_stream_descriptor(uint8_t *buf, int length, audio_stream_descriptor_t* paudio_stream_descriptor);

/*TAG = MPEG2_PSI_HIERARCHY_DESCRIPTOR		0x04*/
typedef struct _hierarchy_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint8_t	reserved0;							//4
	uint8_t hierarchy_type;						//4

	uint8_t	reserved1;							//2
	uint8_t hierarchy_layer_index;				//6

	uint8_t	reserved2;							//2
	uint8_t hierarchy_embedded_layer_index;		//6

	uint8_t	reserved3;							//2
	uint8_t hierarchy_channel;					//6

} hierarchy_descriptor_t, *phierarchy_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_hierarchy_descriptor(uint8_t *buf, int length, hierarchy_descriptor_t* phierarchy_descriptor);

/*TAG = MPEG2_PSI_REGISTRATION_DESCRIPTOR		0x05*/
typedef struct _registration_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	//uint32_t format_identifier;						//32
	char	 format_identifier_char[5];

	int		 info_length;
	char	 additional_identification_info[128];		//are defined by the assignee of that format_identifier

} registration_descriptor_t, *pregistration_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_registration_descriptor(uint8_t* buf, int length, registration_descriptor_t* pregistration_descriptor);

/*TAG = MPEG2_PSI_DATA_STREAM_ALIGNMENT_DESCRIPTOR		0x06*/
typedef struct _data_stream_alignment_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint16_t			descriptor_length;					//8

	uint16_t alignment_type;					//8

} data_stream_alignment_descriptor_t, *pdata_stream_alignment_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_data_stream_alignment_descriptor(uint8_t* buf, int length, pdata_stream_alignment_descriptor_t pdata_stream_alignment_descriptor);

/*TAG = MPEG2_PSI_TARGET_BACKGROUND_GRID_DESCRIPTOR		0x07*/
typedef struct _target_background_grid_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint16_t	descriptor_length;					//8

	uint16_t  horizontal_size;						//14
	uint16_t  vertical_size;							//14
	uint16_t  aspect_ratio_information;				//4

} target_background_grid_descriptor_t, *ptarget_background_grid_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_target_background_grid_descriptor(uint8_t* buf, int length, target_background_grid_descriptor_t* ptarget_background_grid_descriptor);

/*TAG = MPEG2_PSI_VIDEO_WINDOW_DESCRIPTOR				0x08*/
typedef struct _video_window_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint16_t horizontal_offset;							//14
	uint16_t vertical_offset;							//14
	uint8_t  window_priority;							//4

}video_window_descriptor_t, *pvideo_window_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_video_window_descriptor(uint8_t* buf, int length, pvideo_window_descriptor_t pvideo_window_descriptor);

/*TAG = MPEG2_PSI_CA_DESCRIPTOR				0x09*/
typedef struct _CA_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint16_t	CA_system_ID;								//16
	
	uint8_t		reserved;									//3
	uint16_t	CA_PID;										//13

	int		private_data_length;
	//uint8_t	private_data_byte[MAX_PRIVATE_DATA_LENGTH];
	uint8_t* private_data_byte;					//volatilable pointer, only valid when the corresponding descriptor's buf is exists.

} CA_descriptor_t, *pCA_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_CA_descriptor(uint8_t* buf, int length, CA_descriptor_t* pCA_descriptor);

/*TAG = MPEG2_PSI_ISO_639_LANGUAGE_DESCRIPTOR				0x0A*/
typedef struct _ISO_639_language_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	int					N;
	struct
	{
		char			ISO_639_language_code_char[4];			//24
		uint8_t			audio_type;								//8
	}st[MAX_LANGUAGES];

} ISO_639_language_descriptor_t, *pISO_639_language_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_ISO_639_language_descriptor(uint8_t* buf, int length, ISO_639_language_descriptor_t* pISO_639_language_descriptor);

/*TAG = MPEG2_PSI_SYSTEM_CLOCK_DESCRIPTOR			0x0B*/
typedef struct _system_clock_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint8_t		external_clock_reference_indicator;			//1
	uint8_t		reserved0;									//1
	uint8_t		clock_accuracy_integer;						//6

	uint8_t		clock_accuracy_exponent;					//3
	uint8_t		reserved1;									//5

} system_clock_descriptor_t, *psystem_clock_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_system_clock_descriptor(uint8_t *buf, int length, system_clock_descriptor_t* psystem_clock_descriptor);

/*TAG = MPEG2_PSI_MULTIPLEX_BUFFER_UTILIZATION_DESCRIPTOR		0x0C*/
typedef struct _multiplex_buffer_utilization_descriptor
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint8_t		bound_valid_flag;						//1
	uint16_t	LTW_offset_lower_bound;					//15
	uint8_t		reserved;								//1
	uint16_t	LTW_offset_upper_bound;					//15

} multiplex_buffer_utilization_descriptor_t, *pmultiplex_buffer_utilization_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(uint8_t *buf, int length, multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor);

/*TAG = MPEG2_PSI_COPYRIGHT_DESCRIPTOR				0x0D*/
typedef struct _copyright_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	  descriptor_tag;						//8
	uint8_t		  descriptor_length;					//8

	uint32_t	  copyright_identifier;					//32
	char		  additional_copyright_info[MAX_COPYRIGHT_INFO_LENGTH + 1];

} copyright_descriptor_t, *pcopyright_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_copyright_descriptor(uint8_t* buf, int length, copyright_descriptor_t* pcopyright_descriptor);

/*TAB = MPEG2_PSI_MAXIMUM_BITRATE_DESCRIPTOR				0x0E*/
typedef struct _maximum_bitrate_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	 descriptor_tag;						//8
	uint8_t		 descriptor_length;						//8

	uint8_t		 reserved;								//2
	uint32_t	 maximum_bitrate;						//22

} maximum_bitrate_descriptor_t, *pmaximum_bitrate_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_maximum_bitrate_descriptor(uint8_t *buf, int length, maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor);

/*TAG = MPEG2_PSI_PRIVATE_DATA_INDICATOR_DESCRIPTOR			0x0F*/
typedef struct _private_data_indicator_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint32_t		private_data_indicator;				//32

} private_data_indicator_descriptor_t, *pprivate_data_indicator_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_private_data_indicator_descriptor(uint8_t* buf, int length, private_data_indicator_descriptor_t* pprivate_data_indicator_descriptor);

/*TAG = MPEG2_PSI_SMOOTHING_BUFFER_DESCRIPTOR					0x10*/
typedef struct _smoothing_buffer_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t		  reserved0;									//2
	uint32_t	  sb_leak_rate;									//22

	uint8_t		  reserved1;									//2
	uint32_t	  sb_size;										//22
} smoothing_buffer_descriptor_t, *psmoothing_buffer_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_smoothing_buffer_descriptor(uint8_t* buf, int length, smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor);

/*TAG = MPEG2_PSI_STD_DESCRIPTOR						0x11*/
typedef struct _STD_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t descriptor_tag;						//8
	uint8_t	 descriptor_length;					//8

	uint8_t	 reserved;							 //7
	uint8_t	 leak_valid_flag;						//1

} STD_descriptor_t, *pSTD_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_STD_descriptor(uint8_t* buf, int length, pSTD_descriptor_t pSTD_descriptor);

/*TAG = MPEG2_PSI_IBP_DESCRIPTOR					0x12*/
typedef struct _IBP_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t 	descriptor_length;					//8

	uint8_t  closed_gop_flag;				//1
	uint8_t  identical_gop_flag;				//1
	uint16_t max_gop_length;					//14

} IBP_descriptor_t, *pIBP_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_IBP_descriptor(uint8_t* buf, int length, pIBP_descriptor_t pIBP_descriptor);

/*TAG = MPEG2_DSMCC_CAROUSEL_IDENTIFIER_DESCRIPTOR						0x13*/
typedef struct _carousel_identifier_descriptor_s
{
	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint32_t		carousel_id;					//32

	uint8_t			N;
	uint8_t			private_data_byte[64];

} carousel_identifier_descriptor_t, *pcarousel_identifier_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_DSMCC_decode_carousel_identifier_descriptor(uint8_t* buf, int length, carousel_identifier_descriptor_t* pcarousel_identifier_descriptor);

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

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_DSMCC_decode_association_tag_descriptor(uint8_t* buf, int length, association_tag_descriptor_t* passociation_tag_descriptor);

/*TAG = MPEG2_DSMCC_DEFERRED_ASSOCIATION_TAGS_DESCRIPTOR					0x15*/
typedef struct _deferred_association_tags_descriptor_s
{
	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t	association_tags_loop_length;	//8
	uint8_t	N1;
	uint16_t association_tag[16];			//16 x N1
	uint16_t	transport_stream_id;			//16
	uint16_t program_number;					//16
	uint8_t  N2;
	uint8_t  private_data_byte[64];			//8 x N2

} deferred_association_tags_descriptor_t, *pdeferred_association_tags_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int DSMCC_decode_deferred_association_tags_descriptor(uint8_t* buf, int length, pdeferred_association_tags_descriptor_t pdeferred_association_tags_descriptor);

/*TAG = MPEG2_PSI_MPEG4_VIDEO_DESCRIPTOR				0x1B	*/
typedef struct _MPEG4_video_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t	MPEG4_visual_profile_and_level;		//8

} MPEG4_video_descriptor_t, *pMPEG4_video_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB	int MPEG2_PSI_decode_MPEG4_video_descriptor(uint8_t* buf, int length, pMPEG4_video_descriptor_t pMPEG4_video_descriptor);

/*TAG = MPEG2_MPEG4_AUDIO_DESCRIPTOR				0x1C	*/
typedef struct _MPEG4_audio_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t	MPEG4_audio_profile_and_level;		//8

} MPEG4_audio_descriptor_t, *pMPEG4_audio_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB	int MPEG2_PSI_decode_MPEG4_audio_descriptor(uint8_t* buf, int length, pMPEG4_audio_descriptor_t pMPEG4_audio_descriptor);

/*TAG = MPEG2_IOD_DESCRIPTOR						0x1D	*/
typedef struct _IOD_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8


	uint8_t	Scope_of_IOD_lable;					//8
	uint8_t	IOD_label;							//8
	uint8_t	InitialObjectDescriptor;			//8

} IOD_descriptor_t, *pIOD_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_IOD_descriptor(uint8_t* buf, int length, pIOD_descriptor_t pIOD_descriptor);

/*TAG = MPEG2_SL_DESCRIPTOR						0x1E	*/
typedef struct _SL_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint16_t	ES_ID;								//16

} SL_descriptor_t, *pSL_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_SL_descriptor(uint8_t* buf, int length, pSL_descriptor_t pSL_descriptor);

/*TAG = MPEG2_FMC_DESCRIPTOR						0x1F	*/
typedef struct _FMC_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	int	N;

	uint16_t	ES_ID[1];							//16
	uint8_t	FlexMuxChannel[1];					//8
} FMC_descriptor_t, *pFMC_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_FMC_descriptor(uint8_t* buf, int length, pFMC_descriptor_t pFMC_descriptor);

/*TAG = MPEG2_EXTERNAL_ES_ID_DESCRIPTOR		0x20	*/
typedef struct _External_ES_ID_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint16_t	External_ES_ID;								//16
} External_ES_ID_descriptor_t, *pExternal_ES_ID_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_External_ES_ID_descriptor(uint8_t* buf, int length, pExternal_ES_ID_descriptor_t pExternal_ES_ID_descriptor);

/*TAG = MPEG2_MUXCODE_DESCRIPTOR						0x21	*/
typedef struct _Muxcode_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8


	//	MuxCodeTableEntry()

} Muxcode_descriptor_t, *pMuxcode_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_Muxcode_descriptor(uint8_t* buf, int length, pMuxcode_descriptor_t pMuxcode_descriptor);

/*TAG = FMXBUFFERSIZE_DESCRIPTOR				0x22	*/
typedef struct _FmxBufferSize_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8


	//DefaultFlexMuxBufferDescriptor()

} FmxBufferSize_descriptor_t, *pFmxBufferSize_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_FmxBufferSize_descriptor(uint8_t* buf, int length, pFmxBufferSize_descriptor_t pFmxBufferSize_descriptor);

/*TAG = MULTIPLEXBUFFER_DESCRIPTOR				0x23	*/
typedef struct _MultiplexBuffer_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint32_t	MB_buffer_size;
	uint32_t TB_leak_rate;

} MultiplexBuffer_descriptor_t, *pMultiplexBuffer_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_MultiplexBuffer_descriptor(uint8_t* buf, int length, pMultiplexBuffer_descriptor_t pMultiplexBuffer_descriptor);

/*TAG = FLEXMUXTIMING_DESCRIPTOR				0x24	*/
/*TAG = METADATA_POINTER_DESCRIPTOR				0x25	*/
/*TAG = METADATA_DESCRIPTOR						0x26	*/
/*TAG = METADATA_STD_DESCRIPTOR					0x27	*/

/*TAG = AVC_VIDEO_DESCRIPTOR					0x28	*/
typedef struct _AVC_video_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8


	uint8_t	profile_idc;				//8

	uint8_t	constraint_set0_flag;		//1
	uint8_t	constraint_set1_flag;		//1
	uint8_t	constraint_set2_flag;		//1
	uint8_t	AVC_compatible_flags;		//5

	uint8_t	level_idc;					//8
	
	uint8_t	AVC_still_present;			//1
	uint8_t	AVC_24_hour_picture_flag;	//1
	uint8_t	reserved;					//6

} AVC_video_descriptor_t, *pAVC_video_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_AVC_video_descriptor(uint8_t* buf, int length, pAVC_video_descriptor_t pAVC_video_descriptor);

/*TAG = IPMP_DESCRIPTOR							0x29	*/

/*TAG = AVC_TIMING_AND_HRD_DESCRIPTOR	0x2A	*/
typedef struct _AVC_timing_and_HRD_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t	hrd_management_valid_flag;					//1
	uint8_t	reserved0;									//6
	uint8_t	picture_and_timing_info_present;			//1

	uint8_t	flag_90kHz;									//1
	uint8_t	reserved1;									//7

	int	N;											//32
	int	K;											//32
	int	num_units_in_tick;							//32

	uint8_t	fixed_frame_rate_flag;						//1
	uint8_t	temporal_poc_flag;							//1
	uint8_t	picture_to_display_conversion_flag;			//1
	uint8_t	reserved2;									//5

} AVC_timing_and_HRD_descriptor_t, *pAVC_timing_and_HRD_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_AVC_timing_and_HRD_descriptor(uint8_t* buf, int length, pAVC_timing_and_HRD_descriptor_t pAVC_timing_and_HRD_descriptor);

/*TAG = MPEG2_AAC_AUDIO_DESCRIPTOR						0x2B	*/
typedef struct _MPEG2_AAC_audio_descriptor_s
{
	//uint8_t*			descriptor_buf;						//8
	//uint8_t			descriptor_size;					//8

	uint16_t			descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t	MPEG2_AAC_profile;			//8
	uint8_t	MPEG2_AAC_channel_configuration;		//8
	uint8_t	MPEG2_AAC_additional_information;		//8

} MPEG2_AAC_audio_descriptor_t, *pMPEG2_AAC_audio_descriptor_t;

MPEG2_PSI_DESCRIPTOR_LIB int MPEG2_PSI_decode_MPEG2_AAC_audio_descriptor(uint8_t* buf, int length, pMPEG2_AAC_audio_descriptor_t pMPEG2_AAC_audio_descriptor);

/*TAG = FLEXMUX_TIMING_DESCRIPTOR						0x2C	*/
/*TAG = MPEG4_TEXT_DESCRIPTOR							0x2D	*/
/*TAG = MPEG4_TEXT_EXTENSION_DESCRIPTOR					0x2E	*/


#endif