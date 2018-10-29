#ifndef __DVB_MHP_DESCRIPTOR_H__
#define __DVB_MHP_DESCRIPTOR_H__

#include "../compile.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "MPEG_DVB_SysCapability.h"
#include "DVB_MHP_section.h"

//MHP 定义的描述子		ETSI TS 102 812
#define DVB_MHP_APPLICATION_DESCRIPTOR								0x2000
#define DVB_MHP_APPLICATION_NAME_DESCRIPTOR							0x2001
#define DVB_MHP_TRANSPORT_PROTOCOL_DESCRIPTOR						0x2002
#define DVB_MHP_DVB_J_APPLICATION_DESCRIPTOR						0x2003
#define DVB_MHP_DVB_J_APPLICATION_LOCATION_DESCRIPTOR				0x2004
#define DVB_MHP_EXTERNAL_APPLICATION_AUTHORIZATION_DESCRIPTOR		0x2005
#define DVB_MHP_DVB_HTML_APPLICATION_DESCRIPTOR						0x2008
#define DVB_MHP_DVB_HTML_APPLICATION_LOCATION_DESCRIPTOR			0x2009
#define DVB_MHP_DVB_HTML_APPLICATION_BOUNDARY_DESCRIPTOR			0x200A
#define DVB_MHP_APPLICATION_ICONS_DESCRIPTOR						0x200B
#define DVB_MHP_PREFETCH_DESCRIPTOR									0x200C
#define DVB_MHP_DII_LOCATION_DESCRIPTOR								0x200D
#define DVB_MHP_APPLICATION_STORAGE_DESCRIPTOR						0x2010
#define DVB_MHP_IP_SIGNALLING_DESCRIPTOR							0x2011
#define DVB_MHP_GRAPHICS_CONSTRAINTS_DESCRIPTOR						0x2014
#define DVB_MHP_APPLICATION_USAGE_DESCRIPTOR						0x2016

/*------------------------------MHP------------------------------------------------------*/

//MHP 定义的描述子		ETSI TS 102 812
/* MHP_APPLICATION_DESCRIPTOR								0x00				*/
typedef struct _application_descriptor_s
{
	U16		descriptor_tag;						//8
	U8		descriptor_length;					//8

	U8		application_profiles_length;			//8

	U8		application_profiles_N;
	U16		application_profile[MAX_APPLICATION_PROFILES];		//16
	U8		version_major[MAX_APPLICATION_PROFILES];			//8
	U8		version_minor[MAX_APPLICATION_PROFILES];			//8
	U8		version_micro[MAX_APPLICATION_PROFILES];			//8

	U8		service_bound_flag;						//1
	U8		visibility;								//2
	U8		reserved_future_use;					//5

	U8		application_priority;					//8

	U8		transport_protocols_N;
	U8		transport_protocol_lable[MAX_APPLICATION_TRANSPORT_PROTOCOLS + 1];				//8

} application_descriptor_t, *papplication_descriptor_t;

_CDL_EXPORT int DVB_MHP_decode_application_descriptor(uint8_t* buf, int length, application_descriptor_t* papplication_descriptor);

/* MHP_APPLICATION_NAME_DESCRIPTOR							0x01				*/
typedef struct _application_name_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8


	int			N;
	struct
	{
		//U32		ISO_639_language_code;
		char		ISO_639_language_char[4];			//24
		uint8_t		application_name_length;			//8
		uint8_t*	application_name_char;

		int			trimmed_application_name_length;
		uint8_t*	trimmed_application_name_char;

	}st[MAX_LANGUAGES];

} application_name_descriptor_t, *papplication_name_descriptor_t;

_CDL_EXPORT int DVB_MHP_decode_application_name_descriptor(uint8_t* buf, int length, application_name_descriptor_t* papplication_name_descriptor);

/* MHP_TRANSPORT_PROTOCOL_DESCRIPTOR							0x02				*/
typedef struct _transport_protocol_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8


	uint16_t	protocol_id;							//16
	uint8_t		transport_protocol_label;				//8
	int			selector_length;
	uint8_t*	selector_byte;							//8 x N

} transport_protocol_descriptor_t, *ptransport_protocol_descriptor_t;

_CDL_EXPORT int DVB_MHP_decode_transport_protocol_descriptor(uint8_t* buf, int length, transport_protocol_descriptor_t* ptransport_protocol_descriptor);

/*	MHP_DVB_J_APPLICATION_DESCRIPTOR						0x03				*/
typedef struct _dvb_j_application_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		N;

	U8		parameter_length[4];					//8
	U8		parameter_byte[4][16];					//8

} dvb_j_application_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_dvb_j_application_descriptor(U8* buf, S32 length, dvb_j_application_descriptor_t* pdvb_j_application_descriptor);

/* MHP_DVB_J_APPLICATION_LOCATION_DESCRIPTOR							0x04				*/
typedef struct _dvb_j_application_location_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t			base_directory_length;					//8
	uint8_t*		base_directory_byte;				//8 x N1

	uint8_t			classpath_extension_length;
	uint8_t*		classpath_extension_byte;			//8 x N2

	int				initial_class_char_length;
	uint8_t*		initial_class_byte;					//8 x N3

} dvb_j_application_location_descriptor_t;

_CDL_EXPORT int DVB_MHP_decode_dvb_j_application_location_descriptor(uint8_t* buf, int length, dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor);

//typedef struct application_identifier_s
//{
//	U32		organisation_id;					//32
//	U16		application_id;						//16
//} application_identifier_t, *papplication_identifier_t;

/* AIT_EXTERNAL_APPLICATION_AUTHORIZATION_DESCRIPTOR		0x05				*/
typedef struct _external_application_authorization_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8							N;
	application_identifier_t	application_identifier[16];
	U8							application_priority[16];			//8

} external_application_authorization_descriptor_t, *pexternal_application_authorization_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_external_application_authorization_descriptor(U8* buf, S32 length, pexternal_application_authorization_descriptor_t pexternal_application_authorization_descriptor);

/*	MHP_DVB_HTML_APPLICATION_DESCRIPTOR						0x08				*/
typedef struct _dvb_html_application_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		appid_set_length;						//8

	U8		N1;
	U16		application_id[4];						//16

	U8		N;
	U8		parameter_bytes[64];					//8

} dvb_html_application_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_dvb_html_application_descriptor(U8* buf, S32 length, dvb_html_application_descriptor_t* pdvb_html_application_descriptor);

/* MHP_DVB_HTML_APPLICATION_LOCATION_DESCRIPTOR							0x09				*/
typedef struct _dvb_html_application_location_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		physical_root_length;					//8
	U8		N1;
	S8		physical_root_bytes[128];				//8 x N1
	U8		N;
	U8		initial_path_bytes[128];				//8 x N

} dvb_html_application_location_descriptor_t, *pdvb_html_application_location_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_dvb_html_application_location_descriptor(U8* buf, S32 length, pdvb_html_application_location_descriptor_t pdvb_html_application_location_descriptor);


/* MHP_DVB_HTML_APPLICATION_BOUNDARY_DESCRIPTOR			0x0A				*/
typedef struct _dvb_html_application_boundary_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		label_length;							//8,  N1
	U8		lable_bytes[128];						//8 x N1

	U8		N2;
	U8		regular_expression_bytes[128];			//8 x N2

} dvb_html_application_boundary_descriptor_t, *pdvb_html_application_boundary_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_dvb_html_application_boundary_descriptor(U8* buf, S32 length, pdvb_html_application_boundary_descriptor_t dvb_html_application_boundary_descriptor);

/* MHP_APPLICATION_ICONS_DESCRIPTOR							0x0B				*/
typedef struct _application_icons_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		icon_locator_length;					//8
	U8		icon_locator_byte[64];					//8 x N1

	U16		icon_flags;								//16
	U8		N2;
	U8		reserved_future_use[64];				//8 x N2

} application_icons_descriptor_t, *papplication_icons_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_application_icons_descriptor(U8* buf, S32 length, papplication_icons_descriptor_t papplication_icons_descriptor);

/* MHP_PREFETCH_DESCRIPTOR							0x0C			*/
typedef struct _prefetch_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		transport_protocol_label;				//8

} prefetch_descriptor_t, *pprefetch_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_prefetch_descriptor(U8* buf, S32 length, pprefetch_descriptor_t pprefetch_descriptor);

/* MHP_APPLICATION_STORAGE_DESCRIPTOR				0x10					*/
typedef struct _application_storage_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		storage_property;						//8

	U8		not_launchable_from_broadcast;			//1
	U8		launchable_completely_from_cache;		//1
	U8		is_launchable_with_older_version;		//1
	U8		reserved0;								//5

	U8		reserved1;								//1
	U32		version;								//31

	U8		priority;								//8

} application_storage_descriptor_t, *papplication_storage_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_application_storage_descriptor(U8* buf, S32 length, papplication_storage_descriptor_t papplication_storage_descriptor);

/* MHP_IP_SIGNALLING_DESCRIPTOR							0x11		*/
typedef struct _ip_signalling_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U32		platform_id;							//24

} ip_signalling_descriptor_t, *pip_signalling_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_ip_signalling_descriptor(U8* buf, S32 length, pip_signalling_descriptor_t pip_signalling_descriptor);

/* MHP_GRAPHICS_CONSTRAINTS_DESCRIPTOR						0x14		*/
typedef struct _graphics_constraints_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8


	U8		reserved_future_use;					//5
	U8		can_run_without_visible_ui;				//1
	U8		handles_configuration_changed;			//1
	U8		handles_externally_controlled_video;	//1

	U8		N;
	U8		graphics_configuration_byte[16];		//8

} graphics_constraints_descriptor_t, *pgraphics_constraints_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_graphics_constraints_descriptor(U8* buf, S32 length, pgraphics_constraints_descriptor_t pgraphics_constraints_descriptor);


/* MHP_APPLICATION_USAGE_DESCRIPTOR							0x16				*/

typedef struct _application_usage_descriptor_s
{
	//U8*			descriptor_buf;						//8
	//U8			descriptor_size;					//8

	U16			descriptor_tag;						//8
	U8			descriptor_length;					//8

	U8		usage_byte;								//8

} application_usage_descriptor_t, *papplication_usage_descriptor_t;

_CDL_EXPORT S32 DVB_MHP_decode_application_usage_descriptor(U8* buf, S32 length, papplication_usage_descriptor_t papplication_usage_descriptor);


#endif