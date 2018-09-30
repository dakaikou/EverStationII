#ifndef __DVB_SI_DESCRIPTOR_H__
#define __DVB_SI_DESCRIPTOR_H__

#include "../compile.h"

#include "MPEG_DVB_Common.h"
#include "MPEG_DVB_SysCapability.h"
#include "DVB_Databroadcast.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

/*descriptor tag define*/

/* 0x40 - 0x7F: ETSI EN 300468	 */
#define DVB_SI_NETWORK_NAME_DESCRIPTOR								0x0040
#define DVB_SI_SERVICE_LIST_DESCRIPTOR								0x0041
#define DVB_SI_STUFFING_DESCRIPTOR									0x0042
#define DVB_SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR					0x0043
#define DVB_SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR						0x0044
#define DVB_SI_VBI_DATA_DESCRIPTOR									0x0045
#define DVB_SI_VBI_TELETEXT_DESCRIPTOR								0x0046
#define DVB_SI_BOUQUET_NAME_DESCRIPTOR								0x0047
#define DVB_SI_SERVICE_DESCRIPTOR									0x0048
#define DVB_SI_COUNTRY_AVAILABILITY_DESCRIPTOR						0x0049
#define DVB_SI_LINKAGE_DESCRIPTOR									0x004A
#define DVB_SI_NVOD_REFERENCE_DESCRIPTOR							0x004B
#define DVB_SI_TIME_SHIFTED_SERVICE_DESCRIPTOR						0x004C
#define DVB_SI_SHORT_EVENT_DESCRIPTOR								0x004D
#define DVB_SI_EXTENDED_EVENT_DESCRIPTOR							0x004E
#define DVB_SI_TIME_SHIFTED_EVENT_DESCRIPTOR						0x004F
#define DVB_SI_COMPONENT_DESCRIPTOR									0x0050
#define DVB_SI_MOSAIC_DESCRIPTOR									0x0051
#define DVB_SI_STREAM_IDENTIFIER_DESCRIPTOR							0x0052
#define DVB_SI_CA_IDENTIFIER_DESCRIPTOR								0x0053
#define DVB_SI_CONTENT_DESCRIPTOR									0x0054
#define DVB_SI_PARENTAL_RATING_DESCRIPTOR							0x0055
#define DVB_SI_TELETEXT_DESCRIPTOR									0x0056
#define DVB_SI_TELEPHONE_DESCRIPTOR									0x0057
#define DVB_SI_LOCAL_TIME_OFFSET_DESCRIPTOR							0x0058
#define DVB_SI_SUBTITLING_DESCRIPTOR								0x0059
#define DVB_SI_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR				0x005A
#define DVB_SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR					0x005B
#define DVB_SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR					0x005C
#define DVB_SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR					0x005D
#define DVB_SI_MULTILINGUAL_COMPONENT_DESCRIPTOR					0x005E
#define DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR					0x005F
#define DVB_SI_SERVICE_MOVE_DESCRIPTOR								0x0060
#define DVB_SI_SHORT_SMOOTHING_BUFFER_DESCRIPTOR					0x0061
#define DVB_SI_FREQUENCY_LIST_DESCRIPTOR							0x0062
#define DVB_SI_PARTIAL_TRANSPORT_STREAM_DESCRIPTOR					0x0063
#define DVB_SI_DATA_BROADCAST_DESCRIPTOR							0x0064
#define DVB_SI_SCRAMBLING_DESCRIPTOR								0x0065
#define DVB_SI_DATA_BROADCAST_ID_DESCRIPTOR							0x0066
#define DVB_SI_TRANSPORT_STREAM_DESCRIPTOR							0x0067
#define DVB_SI_DSNG_DESCRIPTOR										0x0068
#define DVB_SI_PDC_DESCRIPTOR										0x0069
#define DVB_SI_AC3_DESCRIPTOR										0x006A
#define DVB_SI_ANCILLARY_DATA_DESCRIPTOR							0x006B
#define DVB_SI_CELL_LIST_DESCRIPTOR									0x006C
#define DVB_SI_CELL_FREQUENCY_LINK_DESCRIPTOR						0x006D
#define DVB_SI_ANNOUNCEMENT_SUPPORT_DESCRIPTOR						0x006E
#define DVB_SI_APPLICATION_SIGNALLING_DESCRIPTOR					0x006F					//defined by MHP
#define DVB_SI_ADAPTATION_FIELD_DATA_DESCRIPTOR						0x0070
#define DVB_SI_SERVICE_IDENTIFIER_DESCRIPTOR						0x0071
#define DVB_SI_SERVICE_AVAILABILITY_DESCRIPTOR						0x0072
#define DVB_SI_DEFAULT_AUTHORITY_DESCRIPTOR							0x0073
#define DVB_SI_RELATED_CONTENT_DESCRIPTOR							0x0074
#define DVB_SI_TVA_ID_DESCRIPTOR									0x0075
#define DVB_SI_CONTENT_IDENTIFIER_DESCRIPTOR						0x0076
#define DVB_SI_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR					0x0077
#define DVB_SI_ECM_REPETITION_RATE_DESCRIPTOR						0x0078
#define DVB_SI_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR				0x0079
#define DVB_SI_ENHANCED_AC3_DESCRIPTOR								0x007A
#define DVB_SI_DTS_DESCRIPTOR										0x007B
#define DVB_SI_AAC_DESCRIPTOR										0x007C
#define DVB_SI_XAIT_LOCATION_DESCRIPTOR								0x007D
#define DVB_SI_FTA_CONTENT_MANAGEMENT_DESCRIPTOR					0x007E
#define DVB_SI_EXTENSION_DESCRIPTOR									0x007F

//user defined											0x0080
#define DVB_SI_AC3_AUDIO_DESCRIPTOR									0x0081
//#define SI_CHANNEL_ORDER_DESCRIPTOR								0x0082
//#define SI_CHANNEL_VOLUME_COMPENSATING_DESCRIPTOR				0x0083
//#define SI_CHANNEL_CLASSIFY_DESCRIPTOR							0x0089
//user defined												0x0082~0x00FE
//Forbidden													0x00FF

/*TAG = SI_NETWORK_NAME_DESCRIPTOR						0x40	*/
typedef struct _network_name_descriptor_s
{
	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	char			network_name[MAX_NETWORK_NAME_LENGTH + 1];						//max support for 8 Chinese charactors or 16 English charactors
	char*			trimmed_network_name;

} network_name_descriptor_t, *pnetwork_name_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_network_name_descriptor(uint8_t* buf, int length, network_name_descriptor_t* pnetwork_name_descriptor);

/*TAG = SI_SERVICE_LIST_DESCRIPTOR						0x41	*/
typedef struct _service_list_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S32 N;

	U16 service_id[MAX_SERVICES_PER_STREAM];								//16 * N
	U8  service_type[MAX_SERVICES_PER_STREAM];							//8 * N

} service_list_descriptor_t, *pservice_list_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_service_list_descriptor(uint8_t* buf, int length, service_list_descriptor_t* pservice_list_descriptor);

/*TAG = STUFFING_DESCRIPTOR								0x42	*/
typedef struct _stuffing_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	S8  stuffing_byte[MAX_STUFFING_LENGTH + 1];

} stuffing_descriptor_t, *pstuffing_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_stuffing_descriptor(U8* buf, S32 length, pstuffing_descriptor_t pstuffing_descriptor);

/*TAG = SI_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR			0x43	*/
typedef struct _satellite_delivery_system_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32 frequency;							//32
	U16 orbital_position;					//16

	U8  west_east_flag;						//1
	U8  polarization;						//2
//	U8  modulation;							//5
	U8	roll_off;							//2
	U8	modulation_system;					//1
	U8	modulation_type;					//2
	
	U32 symbol_rate;						//28
	U8  FEC_inner;							//4

} satellite_delivery_system_descriptor_t, *psatellite_delivery_system_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_satellite_delivery_system_descriptor(U8* buf, S32 length, psatellite_delivery_system_descriptor_t psatellite_delivery_system_descriptor);

/*TAG = SI_CABLE_DELIVERY_SYSTEM_DESCRIPTOR				0x44	*/
typedef struct _cable_delivery_system_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32 frequency;								//32
	U16 reserved_future_use;					//12
	U8  FEC_outer;								//4
	U8  modulation;							//8
	U32 symbol_rate;							//28
	U8  FEC_inner;								//4

} cable_delivery_system_descriptor_t, *pcable_delivery_system_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_cable_delivery_system_descriptor(U8* buf, S32 length, pcable_delivery_system_descriptor_t pcable_delivery_system_descriptor);

/* TAG = SI_VBI_DATA_DESCRIPTOR			0x45	*/
typedef struct _VBI_data_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S8	N;

	U8	data_service_id[MAX_VBI_DATA_SERVICES];
	U8	data_service_descriptor_length[MAX_VBI_DATA_SERVICES];

} VBI_data_descriptor_t, *pVBI_data_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_VBI_data_descriptor(U8* buf, S32 length, pVBI_data_descriptor_t pVBI_data_descriptor);


/* TAG = SI_VBI_TELETEXT_DESCRIPTOR			0x46		*/


/*TAG = SI_BOUQUET_NAME_DESCRIPTOR				0x47		*/
typedef struct _bouquet_name_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	char		bouquet_name[MAX_BOUQUET_NAME_LENGTH + 1];
	char*		trimmed_bouquet_name;

} bouquet_name_descriptor_t, *pbouquet_name_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_bouquet_name_descriptor(U8* buf, S32 length, pbouquet_name_descriptor_t pbouquet_name_descriptor);

/*TAG = SI_SERVICE_DESCRIPTOR				0x48	*/
typedef struct _service_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint8_t service_type;							//8
	uint8_t service_provider_name_length;			//8
	char	service_provider_name[MAX_SERVICE_PROVIDER_NAME_LENGTH + 1];
	char*	trimmed_service_provider_name;

	uint8_t service_name_length;					//8
	char	service_name[MAX_SERVICE_NAME_LENGTH + 1];
	char*	trimmed_service_name;

} service_descriptor_t, *pservice_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_service_descriptor(uint8_t* buf, int length, service_descriptor_t* pservice_descriptor);

/*TAG = SI_COUNTRY_AVAILABILITY_DESCRIPTOR			0x49		*/
typedef struct _country_availability_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 country_availability_flag;				//1
	U8 reserved_future_use;						//7
	U8	N;

	U32	country_code[MAX_COUNTRIES];
	S8  country_code_char[MAX_COUNTRIES][4];

} country_availability_descriptor_t, *pcountry_availability_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_country_availability_descriptor(U8* buf, S32 length, pcountry_availability_descriptor_t pcountry_availability_descriptor);

/*TAG = SI_LINKAGE_DESCRIPTOR					0x4A	*/
typedef struct _linkage_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 transport_stream_id;					//16
	U16 original_network_id;					//16
	U16 service_id;								//16
	U8 linkage_type;							//8

	//linkage_type == 0x0B		ref: EN301192 clause 8.2.1 Linkage descriptor for an IP/MAC Notification Table
	U8	platform_id_data_length;				//8
	S8  platform_N;
	U32 platform_id[MAX_IP_PLATFORMS];			//24 x
	U8	platform_name_loop_length[MAX_IP_PLATFORMS];		//8 x

	S8	platform_name_N[MAX_LANGUAGES];
	U32	ISO_639_language_code[MAX_IP_PLATFORMS][MAX_LANGUAGES];
	S8	ISO_639_language_code_char[MAX_IP_PLATFORMS][MAX_LANGUAGES][4];
	U8  platform_name_length[MAX_IP_PLATFORMS][MAX_LANGUAGES];				//8 x
	S8	platform_name_text_char[MAX_IP_PLATFORMS][MAX_LANGUAGES][MAX_IP_PLATFORM_NAME_LENGTH];

	//linkage_type == 0x0C

	//stuffing bytes
	S32 private_data_length;
	U8	private_data_byte[MAX_PRIVATE_DATA_LENGTH];

} linkage_descriptor_t, *plinkage_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_linkage_descriptor(U8* buf, S32 length, plinkage_descriptor_t plinkage_descriptor);

/*TAG = SI_NOVD_REFERENCE_DESCRIPTOR					0x4B		*/
typedef struct _NVOD_reference_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S32  N;
	U16 transport_stream_id[32];				//16		
    U16 original_network_id[32];				//16
	U16 service_id[32];							//16

} NVOD_reference_descriptor_t, *pNVOD_reference_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_NVOD_reference_descriptor(U8* buf, S32 length, pNVOD_reference_descriptor_t pNVOD_reference_descriptor);

/*TAG = TIME_SHIFTED_SERVICE_DESCRIPTOR				0x4C	*/
typedef struct _time_shifted_service_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16	reference_service_id;

} time_shifted_service_descriptor_t, *ptime_shifted_service_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_time_shifted_service_descriptor(U8* buf, S32 length, ptime_shifted_service_descriptor_t ptime_shifted_service_descriptor);

/*TAG = SI_SHORT_EVENT_DESCRIPTOR					0x4D	*/
typedef struct _short_event_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	uint32_t	ISO_639_language_code;
	char		ISO_639_language_code_char[4];				//24

	uint8_t		event_name_length;							//8
	char		event_name_char[MAX_EVENT_NAME_LENGTH + 1];
	char*		trimmed_event_name_char;
	
	uint8_t		text_length;									//8
	char		text_char[MAX_EVENT_TEXT_LENGTH + 1];
	char*		trimmed_text_char;

} short_event_descriptor_t, *pshort_event_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_short_event_descriptor(U8* buf, S32 length, pshort_event_descriptor_t pshort_event_descriptor);

/*TAG = EXTENDED_EVENT_DESCRIPTOR				0x4E	*/
typedef struct _extended_event_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 descriptor_number;					//4
	U8 last_descriptor_number;				//4
	
	U32 ISO_639_language_code;
	S8	ISO_639_language_code_char[4];		//24
	U8  length_of_items;					//8

	S32	N;

	U8  item_description_length[MAX_EXTENDED_EVENTS];		//8
	S8	item_description_char[MAX_EXTENDED_EVENTS][64];
	U8  item_length[MAX_EXTENDED_EVENTS];					//8
	S8  item_char[MAX_EXTENDED_EVENTS][64];

	U8  text_length;						//8
	S8	text_char[MAX_EVENT_TEXT_LENGTH + 1];

} extended_event_descriptor_t, *pextended_event_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_extended_event_descriptor(U8* buf, S32 length, pextended_event_descriptor_t pextended_event_descriptor);

/*TAG = TIME_SHIFTED_EVENT_DESCRIPTOR			0x4F	*/
typedef struct _time_shifted_event_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 reference_service_id;				//16
	U16 reference_event_id;				//16

} time_shifted_event_descriptor_t, *ptime_shifted_event_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_time_shifted_event_descriptor(U8* buf, S32 length, ptime_shifted_event_descriptor_t ptime_shifted_event_descriptor);

/*TAG = SI_COMPONENT_DESCRIPTOR				0x50		*/
typedef struct _component_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	reserved_future_use;					//4
	U8 stream_content;							//4
	U8 component_type;							//8
	U8 component_tag;							//8

	U32	ISO_639_language_code;
	S8	ISO_639_language_code_char[4];			//24

	S8	text_char[MAX_COMPONENT_TEXT_LENGTH + 1];

} component_descriptor_t, *pcomponent_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_component_descriptor(U8* buf, S32 length, pcomponent_descriptor_t pcomponent_descriptor);

/*TAG = MOSAIC_DESCRIPTOR				0x51	*/
typedef struct _mosaic_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8 mosaic_entry_point;							//1
	U8 number_of_horizontal_elementary_cells;		//3
	U8 reserved_future_use0;							//1
	U8 number_of_vertical_elementary_cells;			//3

	S32		N;

	U8		logical_cell_id[32];					//6
	U8		reserved_future_use1[32];				//7
	U8	    logical_cell_presentation_info[32];		//3

	U8     elementary_cell_field_length[32];		//8
	U8     elementary_cell_id[32][32];
	
	U8     cell_linkage_info[32];					//8

	U16	bouquet_id[32];							//16
	U16	original_network_id[32];				//16
	U16	transport_stream_id[32];				//16
	U16	service_id[32];							//16
	U16	event_id[32];							//16

} mosaic_descriptor_t, *pmosaic_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_mosaic_descriptor(U8* buf, S32 length, pmosaic_descriptor_t pmosaic_descriptor);

/*TAG = SI_STREAM_IDENTIFIER_DESCRIPTOR				0x52	*/
typedef struct _stream_identifier_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 component_tag;						//8
} stream_identifier_descriptor_t, *pstream_identifier_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_stream_identifier_descriptor(U8* buf, S32 length, pstream_identifier_descriptor_t pstream_identifier_descriptor);

/*TAG = SI_CA_IDENTIFIER_DESCRIPTOR					0x53	*/
typedef struct _CA_identifier_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S32  N;
	U16 CA_system_id[MAX_CAS];					//16x

} CA_identifier_descriptor_t, *pCA_identifier_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_CA_identifier_descriptor(U8* buf, S32 length, pCA_identifier_descriptor_t pCA_identifier_descriptor);

/*TAG = SI_CONTENT_DESCRIPTOR				0x54	*/
typedef struct _content_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	S32		N;

	U8 content_nibble_level_1[MAX_NIBBLES];				//4
	U8 content_nibble_level_2[MAX_NIBBLES];				//4
	U8 user_nibble_1[MAX_NIBBLES];						//4
	U8 user_nibble_2[MAX_NIBBLES];						//4

}content_descriptor_t, *pcontent_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_content_descriptor(U8* buf, S32 length, pcontent_descriptor_t pcontent_descriptor);

/*TAG = SI_PARENTAL_RATING_DESCRIPTOR			0x55	*/
typedef struct _parental_rating_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	S32		N;
	U32		country_code[MAX_COUNTRIES];
	S8		country_code_char[MAX_COUNTRIES][4];			//24
	U8		rating[MAX_COUNTRIES];					//8

} parental_rating_descriptor_t, *pparental_rating_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_parental_rating_descriptor(U8* buf, S32 length, pparental_rating_descriptor_t pparental_rating_descriptor);

/*TAG = DVB_SI_TELETEXT_DESCRIPTOR				0x56		*/
typedef struct _teletext_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	int		N;

	struct {
		uint32_t ISO_639_language_code;
		char     ISO_639_language_code_char[4];
		uint8_t	 teletext_type;
		uint8_t  teletext_magazine_number;
		uint8_t  teletext_page_number;

	} description_loop[MAX_LANGUAGES];

} teletext_descriptor_t, *pteletext_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_teletext_descriptor(uint8_t* buf, int length, teletext_descriptor_t* pteletext_descriptor);

/*TAG = TELEPHONE_DESCRIPTOR			0x57		*/
typedef struct _telephone_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8 reserved_future_use0;						//2
	U8 foreign_availability;						//1
	U8 connection_type ;							//5
	
	U8 reserved_future_use1;						//1
	U8 country_prefix_length;						//2
	U8 international_area_code_length;				//3
	U8 operator_code_length;						//2
	
	U8 reserved_future_use2;						//1
	U8 national_area_code_length;					//3
	U8 core_number_length;							//4

	S8	country_prefix_char[5];
	S8	international_area_code_char[9];
	S8	operator_code_char[5];
	S8	national_area_code_char[9];
	S8	core_number_char[17];

} telephone_descriptor_t, *ptelephone_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_telephone_descriptor(U8* buf, S32 length, ptelephone_descriptor_t ptelephone_descriptor);


typedef struct
{
	uint32_t country_code;										//24
	uint8_t	 country_region_id;									//6
	uint8_t  reserved;											//1
	uint8_t  local_time_offset_polarity;						//1
	uint8_t  local_time_offset;									//16
	int64_t  time_of_change;									//40
	uint16_t next_time_offset;									//16

} local_time_offset_t;

/*TAG = SI_LOCAL_TIME_OFFSET_DESCRIPTOR   0x58*/							 
typedef struct _local_time_offset_descriptor_s
{
	uint16_t			descriptor_tag;						//8
	uint8_t				descriptor_length;					//8

	int					N;
	local_time_offset_t local_time_offset[MAX_LOCAL_TIME_AREA];

} local_time_offset_descriptor_t, *plocal_time_offset_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_local_time_offset_descriptor(uint8_t* buf, int length, local_time_offset_descriptor_t* plocal_time_offset_descriptor);

/*TAG = SI_SUBTITLING_DESCRIPTOR		0x59*/
typedef struct _subtitling_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	int		 	N;

	struct {
		uint32_t	ISO_639_language_code;
		char		ISO_639_language_code_char[4];					//24
		uint8_t		subtitling_type;								//8
		uint16_t	composition_page_id;							//16
		uint16_t	ancillary_page_id;								//16
	} description_loop[MAX_LANGUAGES];

} subtitling_descriptor_t, *psubtitling_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_subtitling_descriptor(uint8_t* buf, int length, subtitling_descriptor_t* psubtitling_descriptor);

/*TAG = SI_TERRESTRIAL_DELIVERY_SYSTEM_DESCRIPTOR		0x5A	*/
typedef struct _terrestrial_delivery_system_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32 centre_frequency;							//32

	U8  bandwidth;									//3
	U8  priority;									//1
	U8  time_slicing_indicator;						//1
	U8  MPE_FEC_indecator;							//1
	U8  reserved_future_use0;						//2

	U8  constellation;								//2
	U8  hierarchy_information;						//3
	U8  code_rate_HP_stream;						//3
	U8  code_rate_LP_stream;						//3
	U8  guard_interval;								//2
	U8  transmission_mode;							//2
	U8  other_frequency_flag;						//1

	U32 reserved_future_use1;						//32

} terrestrial_delivery_system_descriptor_t, *pterrestrial_delivery_system_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_terrestrial_delivery_system_descriptor(U8* buf, S32 length, pterrestrial_delivery_system_descriptor_t pterrestrial_delivery_system_descriptor);

/*TAG = SI_MULTILINGUAL_NETWORK_NAME_DESCRIPTOR				0x5B*/
typedef struct _multilingual_network_name_descriptor
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	int N;
	struct
	{
		uint32_t ISO_639_language_code;
		char	 ISO_639_language_code_char[4];				//24
		
		uint8_t	 network_name_length;							//8
		char	 network_name_char[MAX_NETWORK_NAME_LENGTH + 1];
		char*	trimmed_network_name_char;

	} LANGUAGE[MAX_LANGUAGES];

} multilingual_network_name_descriptor_t, *pmultilingual_network_name_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_multilingual_network_name_descriptor(U8* buf, S32 length, pmultilingual_network_name_descriptor_t pmultilingual_network_name_descriptor);

/*TAG = SI_MULTILINGUAL_BOUQUET_NAME_DESCRIPTOR				0x5C*/
typedef struct _multilingual_bouquet_name_descriptor
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	int N;
	struct
	{
		uint32_t ISO_639_language_code;
		char	 ISO_639_language_code_char[4];				//24
		uint8_t	 bouquet_name_length;							//8
		char	 bouquet_name_char[MAX_BOUQUET_NAME_LENGTH + 1];
		char*	 trimmed_bouquet_name_char;
	} LANGUAGE[MAX_LANGUAGES];

} multilingual_bouquet_name_descriptor_t, *pmultilingual_bouquet_name_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_multilingual_bouquet_name_descriptor(U8* buf, S32 length, pmultilingual_bouquet_name_descriptor_t pmultilingual_bouquet_name_descriptor);

/*TAG = SI_MULTILINGUAL_SERVICE_NAME_DESCRIPTOR			0x5D*/
typedef struct _multilingual_service_name_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	int		 N;
	struct
	{
		uint32_t ISO_639_language_code;
		char	 ISO_639_language_code_char[4];				//8
		uint8_t	 service_provider_name_length;				//8
		char	 service_provider_name_char[MAX_SERVICE_PROVIDER_NAME_LENGTH + 1];
		char*	 trimmed_service_provider_name_char;

		uint8_t	 service_name_length;						//8
		char	 service_name_char[MAX_SERVICE_NAME_LENGTH + 1];
		char*	 trimmed_service_name_char;
	} LANGUAGE[MAX_LANGUAGES];

} multilingual_service_name_descriptor_t, *pmultilingual_service_name_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_multilingual_service_name_descriptor(U8* buf, S32 length, pmultilingual_service_name_descriptor_t pmultilingual_service_name_descriptor);

/*TAG = SI_MULTILINGUAL_COMPONENT_DESCRIPTOR			0x5E	*/
typedef struct _multilingual_component_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8	 component_tag;						//8

	S32	 N;
	U32	 ISO_639_language_code[MAX_LANGUAGES];
	S8	 ISO_639_language_code_char[MAX_LANGUAGES][4];		//24
	U8	 text_description_length[MAX_LANGUAGES];			//8
	S8	 text_char[MAX_LANGUAGES][MAX_COMPONENT_TEXT_LENGTH + 1];

} multilingual_component_descriptor_t, *pmultilingual_component_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_multilingual_component_descriptor(U8* buf, S32 length, pmultilingual_component_descriptor_t pmultilingual_component_descriptor);

/*TAG = DVB_SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR			0x5F	*/
typedef struct _private_data_specifier_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32 private_data_specifier;						//32
} private_data_specifier_descriptor_t, *pprivate_data_specifier_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_private_data_specifier_descriptor(uint8_t* buf, int length, private_data_specifier_descriptor_t* pprivate_data_specifier_descriptor);

/*TAG = SERVICE_MOVE_DESCRIPTOR						0x60	*/
typedef struct _service_move_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 new_original_network_id;					//16
	U16 new_transport_stream_id;					//16
	U16 new_service_id;							//16

} service_move_descriptor_t, *pservice_move_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_service_move_descriptor(U8* buf, S32 length, pservice_move_descriptor_t pservice_move_descriptor);

/*TAG = SHORT_SMOOTHING_BUFFER_DESCRIPTOR			0x61	*/
typedef struct _short_smoothing_buffer_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 sb_size;									//2
	U8 sb_leak_rate;								//6
	//DVB_reserved

} short_smoothing_buffer_descriptor_t, *pshort_smoothing_buffer_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_short_smoothing_buffer_descriptor(U8* buf, S32 length, pshort_smoothing_buffer_descriptor_t pshort_smoothing_buffer_descriptor);

/*TAG = FREQUENCY_LIST_DESCRIPTOR				0x62	*/
typedef struct _frequency_list_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	S8	reserved_future_use;						//6
	S8  coding_type;								//2

	S32	N;
	U32 centre_frequency[32];						//32

} frequency_list_descriptor_t, *pfrequency_list_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_frequency_list_descriptor(U8* buf, S32 length, pfrequency_list_descriptor_t pfrequency_list_descriptor);

/*TAG = PARTIAL_TRANSPORT_STREAM_DESCRIPTOR			0x63	*/
typedef struct _partial_transport_stream_descriptor
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8 DVB_reserved_future_use0;						//2
	U32 peak_rate;									//22
	
	U8 DVB_reserved_future_use1;				//2
	U32 minimum_overall_smoothing_rate;			//22
	
	U8 DVB_reserved_future_use2;					//2
	U16 maximum_overall_smoothing_buffer;			//14

} partial_transport_stream_descriptor_t, *ppartial_transport_stream_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_partial_transport_stream_descriptor(U8* buf, S32 length, ppartial_transport_stream_descriptor_t ppartial_transport_stream_descriptor);

/*TAG = SI_DATA_BROADCAST_DESCRIPTOR		0x64		*/
typedef struct _data_broadcast_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 data_broadcast_id;					//16	
	U8  component_tag;						//8
	U8  selector_length;					//8

	union
	{
		U8										buf[MAX_DATABROADCAST_SELECT_LENGTH];
		multiprotocol_encapsulation_info_t		multiprotocol_encapsulation_info;
		data_carousel_info_t					data_carousel_info;
		object_carousel_info_t					object_carousel_info;
		IP_MAC_notification_info_t				IP_MAC_notification_info;

	} selector_byte;

	U32 ISO_639_language_code;				
	S8 ISO_639_language_code_char[4];		//24
	U8 text_length;							//8
	S8 text_char[MAX_DATABROADCAST_TEXT_LENGTH + 1];

} data_broadcast_descriptor_t, *pdata_broadcast_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_data_broadcast_descriptor(uint8_t* buf, int length, data_broadcast_descriptor_t* pdata_broadcast_descriptor);

/*TAG = CA_SYSTEM_DESCRIPTOR				0x65	*/
/*TAG = SI_SCRAMBLING_DESCRIPTOR				0x65	*/
//typedef struct _CA_system_descriptor_s
typedef struct _scrambling_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8 scrambling_mode;						//8

} scrambling_descriptor_t, *pscrambling_descriptor_t;

//_CDL_EXPORT S32 SI_decode_CA_system_descriptor(U8* buf, S32 length, pCA_system_descriptor_t pCA_system_descriptor);
_CDL_EXPORT int DVB_SI_decode_scrambling_descriptor(U8* buf, S32 length, pscrambling_descriptor_t pscrambling_descriptor);

/*TAG = SI_DATA_BROADCAST_ID_DESCRIPTOR		0x66*/
typedef struct _data_broadcast_id_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U16 data_broadcast_id;					//16

	S8 id_selector_length;
//	U8 id_selector_byte[MAX_DATABROADCAST_SELECT_LENGTH];
	union
	{
		U8										buf[MAX_DATABROADCAST_SELECT_LENGTH];
		multiprotocol_encapsulation_info_t		multiprotocol_encapsulation_info;
		data_carousel_info_t					data_carousel_info;
		object_carousel_info_t					object_carousel_info;
		IP_MAC_notification_info_t				IP_MAC_notification_info;

	} id_selector_byte;

} data_broadcast_id_descriptor_t, *pdata_broadcast_id_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_data_broadcast_id_descriptor(uint8_t* buf, int length, data_broadcast_id_descriptor_t* pdata_broadcast_id_descriptor);


/*TAG = SI_TRANSPORT_STREAM_DESCRIPTOR		0x67*/
typedef struct _transport_stream_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	char		byte_char[4];						//

} transport_stream_descriptor_t, *ptransport_stream_descriptor_t;

_CDL_EXPORT int DVB_SI_decode_transport_stream_descriptor(uint8_t* buf, int length, transport_stream_descriptor_t* ptransport_stream_descriptor);

/*TAG = SI_DSNG_DESCRIPTOR			0x68*/

/*TAG = SI_PDC_DESCRIPTOR			0x69*/


/*TAG = SI_AC3_DESCRIPTOR			0x6A*/
//在TS 102 366中定义
typedef struct _ac3_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		component_type_flag;				//1
	U8		bsid_flag;							//1
	U8		mainid_flag;						//1
	U8		asvc_flag;							//1
	U8		reserved_flags;						//4

	U8		component_type;						//8
	U8		bsid;								//8
	U8		mainid;								//8
	U8		asvc;								//8

	U8		N;
	U8		additional_info[16];				//8xN

} ac3_descriptor_t, *pac3_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_ac3_descriptor(U8* buf, S32 length, pac3_descriptor_t pac3_descriptor);

/*TAG = SI_ANCILLARY_DATA_DESCRIPTOR				0x6B*/

/*TAG = SI_CELL_LIST_DESCRIPTOR				0x6C*/
typedef struct _cell_list_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	S8		cell_N;								//
	U16		cell_id[MAX_CELLS];					//16
	U16		cell_latitude[MAX_CELLS];			//16
	U16		cell_longitude[MAX_CELLS];			//16
	U16		cell_extent_of_latitude[MAX_CELLS];			//12
	U16		cell_extent_of_longitude[MAX_CELLS];		//12

	S8		subcell_info_loop_length[MAX_CELLS];					//8
	U8		subcell_N[MAX_CELLS];

	U8		cell_id_extension[MAX_CELLS][MAX_SUB_CELLS];
	U16		subcell_latitude[MAX_CELLS][MAX_SUB_CELLS];					//16
	U16		subcell_longitude[MAX_CELLS][MAX_SUB_CELLS];				//16
	U16		subcell_extent_of_latitude[MAX_CELLS][MAX_SUB_CELLS];		//12
	U16		subcell_extent_of_longitude[MAX_CELLS][MAX_SUB_CELLS];		//12

} cell_list_descriptor_t, *pcell_list_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_cell_list_descriptor(U8* buf, S32 length, pcell_list_descriptor_t pcell_list_descriptor);

/*TAG = SI_CELL_FREQUENCY_LINK_DESCRIPTOR				0x6D*/
typedef struct _cell_frequency_link_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	S8		cell_N;								//

	U16		cell_id[MAX_CELLS];					//16
	U32		frequency[MAX_CELLS];				//32
	U8		subcell_info_loop_length[MAX_CELLS];//8
	U8		subcell_N[MAX_CELLS];

	U8		cell_id_extension[MAX_CELLS][MAX_SUB_CELLS];
	U32		transposer_frequency[MAX_CELLS][MAX_SUB_CELLS];					//16

} cell_frequency_link_descriptor_t, *pcell_frequency_link_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_cell_frequency_link_descriptor(U8* buf, S32 length, pcell_frequency_link_descriptor_t pcell_frequency_link_descriptor);

/*TAG = SI_ANNOUNCEMENT_SUPPORT_DESCRIPTOR			0x6E*/

/*TAG = SI_APPLICATION_SIGNALLING_DESCRIPTOR			0x6F*/
typedef struct _application_signalling_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	S32	N;
	U8  reserved_future_use0[MAX_APPLICATIONS];
	U16 application_type[MAX_APPLICATIONS];
	U8  reserved_future_use1[MAX_APPLICATIONS];
	U8	AIT_version_number[MAX_APPLICATIONS];
	
} application_signalling_descriptor_t, *papplication_signalling_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_application_signalling_descriptor(U8* buf, S32 length, papplication_signalling_descriptor_t papplication_signalling_descriptor);

/*TAG = SI_ADAPTATION_FIELD_DATA_DESCRIPTOR			0x70*/
/*TAG = SI_SERVICE_IDENTIFIER_DESCRIPTOR			0x71*/
/*TAG = SI_SERVICE_AVAILABILITY_DESCRIPTOR			0x72*/
/*TAG = SI_DEFAULT_AUTHORITY_DESCRIPTOR				0x73*/
/*TAG = SI_RELATED_CONTENT_DESCRIPTOR				0x74*/
/*TAG = SI_TVA_ID_DESCRIPTOR						0x75*/
/*TAG = SI_CONTENT_IDENTIFIER_DESCRIPTOR			0x76*/

/*TAG = SI_TIME_SLICE_FEC_IDENTIFIER_DESCRIPTOR			0x77*/
typedef struct _time_slice_fec_identifier_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		time_slicing;							//1
	U8		mpe_fec;								//2
	U8		reserved_for_future_use;				//2
	U8		frame_size;								//3
	U8		max_burst_duration;						//8
	U8		max_average_rate;						//4
	U8		time_slice_fec_id;						//4

	S8		N;
	U8		id_selector_byte[16];					//8 x N

} time_slice_fec_identifier_descriptor_t, *ptime_slice_fec_identifier_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_time_slice_fec_identifier_descriptor(U8* buf, S32 length, ptime_slice_fec_identifier_descriptor_t ptime_slice_fec_identifier_descriptor);

/*TAG = SI_ECM_REPETITION_RATE_DESCRIPTOR					0x78*/
/*TAG = SI_S2_SATELLITE_DELIVERY_SYSTEM_DESCRIPTOR			0x79*/
/*TAG = SI_ENHANCED_AC3_DESCRIPTOR							0x7A*/
/*TAG = SI_DTS_DESCRIPTOR									0x7B*/
/*TAG = SI_AAC_DESCRIPTOR									0x7C*/
/*TAG = SI_XAIT_LOCATION_DESCRIPTOR							0x7D*/
/*TAG = SI_FTA_CONTENT_MANAGEMENT_DESCRIPTOR				0x7E*/

/*TAG = SI_EXTENSION_DESCRIPTOR			0x7F*/
typedef struct _extension_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		descriptor_tag_extension;				//8

} extension_descriptor_t, *pextension_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_extension_descriptor(U8* buf, S32 length, pextension_descriptor_t pextension_descriptor);


/*TAG = AC3_AUDIO_STREAM_DESCRIPTOR  0x81*/
//ATSC A52标准定义
typedef struct _AC3_audio_stream_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		sample_rate_code		:3;			//3
	U8		bsid					:5;			//5

	U8		bit_rate_code			:6;			//6
	U8		surround_mode			:2;			//2

	U8		bsmod					:3;			//3
	U8		num_channels			:4;			//4
	U8		full_svc				:1;			//1

	U8		langcod;							//8

	U8		langcod2;							//8

	U8		mainid					:3;			//3
	U8		reserved				:5;			//5

	U8		asvcflags;							//8

	U8		textlen					:7;			//7
	U8		text_code				:1;			//1

	S8		text[128];
	S8		additional_info[128];

} AC3_audio_stream_descriptor_t, *pAC3_audio_stream_descriptor_t;

_CDL_EXPORT S32 DVB_SI_decode_AC3_audio_stream_descriptor(U8* buf, S32 length, pAC3_audio_stream_descriptor_t pAC3_audio_stream_descriptor);

/*TAG = CHANNEL_ORDER_DESCRIPTOR			0x82*/
typedef struct _channel_order_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		N;
	U16		service_id[MAX_SERVICES_PER_STREAM];				
    U16		channel_index_no[MAX_SERVICES_PER_STREAM];

} channel_order_descriptor_t, *pchannel_order_descriptor_t;

_CDL_EXPORT S32 SI_decode_channel_order_descriptor(U8* buf, S32 length, pchannel_order_descriptor_t pchannel_order_descriptor);

/*TAG = CHANNEL_VOLUME_COMPENSATING_DESCRIPTOR		0x83	*/
typedef struct _channel_volume_compensating_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		N;
	U16		service_id[MAX_SERVICES_PER_STREAM];				
    U8		channel_volume_compensat[MAX_SERVICES_PER_STREAM];

} channel_volume_compensating_descriptor_t, *pchannel_volume_compensating_descriptor_t;

_CDL_EXPORT S32 SI_decode_channel_volume_compensating_descriptor(U8* buf, S32 length, pchannel_volume_compensating_descriptor_t pchannel_volume_compensating_descriptor);

/*TAG = CHANNEL_CLASSIFY_DESCRIPTOR				0x89*/
typedef struct _channel_classify_descriptor_s
{
	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		N;
	U16		service_id[MAX_SERVICES_PER_STREAM];				
    U8		channel_class[MAX_SERVICES_PER_STREAM];

} channel_classify_descriptor_t, *pchannel_classify_descriptor_t;

_CDL_EXPORT S32 SI_decode_channel_classify_descriptor(U8* buf, S32 length, pchannel_classify_descriptor_t pchannel_classify_descriptor);


#endif