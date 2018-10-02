#ifndef _DVB_SI_SECTION_H_
#define _DVB_SI_SECTION_H_

#include "../compile.h"

#include "DVB_SI_Descriptor.h"
#include "MPEG_DVB_SysCapability.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

#define MIN(x, y)						(x < y? x : y)
//#define min(x, y)						(x < y? x : y)

/*------------------------------------------------------------
Section size definition
-------------------------------------------------------------*/
#define DVB_SI_NIT_SECTION_MAX_SIZE			1024
#define DVB_SI_BAT_SECTION_MAX_SIZE			1024
#define DVB_SI_SDT_SECTION_MAX_SIZE			1024
#define DVB_SI_EIT_SECTION_MAX_SIZE			4096
#define DVB_SI_TDT_SECTION_MAX_SIZE			8
#define DVB_SI_TOT_SECTION_MAX_SIZE			1024
#define DVB_SI_RST_SECTION_MAX_SIZE			1024

#define DVB_SI_NIT_SECTION_MIN_SIZE			16
#define DVB_SI_BAT_SECTION_MIN_SIZE			16
#define DVB_SI_SDT_SECTION_MIN_SIZE			15
#define DVB_SI_EIT_SECTION_MIN_SIZE			18
#define DVB_SI_TDT_SECTION_MIN_SIZE			8
#define DVB_SI_TOT_SECTION_MIN_SIZE			14
#define DVB_SI_RST_SECTION_MIN_SIZE			3
#define DVB_SI_CMT_SECTION_MIN_SIZE			3

/*------------------------------------------------------------
		NIT definition 
-------------------------------------------------------------*/
typedef struct _STREAM_DESCRIPTION_s
{
	uint16_t									transport_stream_id;
	uint16_t									original_network_id;

	uint8_t										reserved;
	uint16_t									transport_descriptors_length;

	int											transport_descriptor_count;
	reserved_descriptor_t						transport_descriptors[MAX_RESERVED_DESCRIPTORS];

} STREAM_DESCRIPTION_t, *PSTREAM_DESCRIPTION_t;

typedef struct network_information_section_s
{
	U8			table_id;									//8

	U8			section_syntax_indicator;					//1
	U8			reserved_future_use0;						//1
	U8			reserved0;									//2
	U16			section_length;								//12
	U16			network_id;									//16
	U8			reserved1;									//2
	U8			version_number;								//5
	U8			current_next_indicator;						//1
	U8			section_number;								//8
	U8			last_section_number;						//8

	U8			reserved_future_use1;						//4
	U16			network_descriptors_length;					//12

	int										network_descriptor_count;
	reserved_descriptor_t					network_descriptors[MAX_RESERVED_DESCRIPTORS];

	U8						reserved_future_use2;						//4
	U16						transport_stream_loop_length;				//12

	int						stream_count;
	STREAM_DESCRIPTION_t	astStreams[MAX_STREAMS_PER_NIT_SECTION];

	U32						CRC_32;										//32
	U32						CRC_32_recalculated;						//32

} network_information_section_t, *pnetwork_information_section_t;

_CDL_EXPORT	int	DVB_SI_NIT_DecodeSection(uint8_t* buf, int length, network_information_section_t* pNITSection);

/*------------------------------------------------------------
		BAT definition 
-------------------------------------------------------------*/
typedef struct bouquet_association_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use0;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U16		bouquet_id;									//16

	U8		reserved1;									//2
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	U8						reserved_future_use1;						//4
	U16						bouquet_descriptors_length;					//12

	int						bouquet_descriptor_count;
	reserved_descriptor_t	bouquet_descriptors[MAX_RESERVED_DESCRIPTORS];

	U8		reserved_future_use2;						//4
	U16		transport_stream_loop_length;				//12

	int						stream_count;
	STREAM_DESCRIPTION_t	astStreams[MAX_STREAMS_PER_BAT_SECTION];

	uint32_t				CRC_32;										//32
	uint32_t				CRC_32_recalculated;						//32
	
} bouquet_association_section_t, *pbouquet_association_section_t;

_CDL_EXPORT int	DVB_SI_BAT_DecodeSection(uint8_t *buf, int length, bouquet_association_section_t* pBATSection);

/*------------------------------------------------------------
		SDT definition 
-------------------------------------------------------------*/
typedef struct _SERVICE_DESCRIPTION_s
{
	uint16_t								service_id;						//16

	U8										reserved_future_use;			//6
	U8										EIT_schedule_flag;				//1
	U8										EIT_present_following_flag;		//1

	U8										running_status;					//3
	U8										free_CA_mode;					//1
	U16										descriptors_loop_length;		//12

	int										service_descriptor_count;
	reserved_descriptor_t					service_descriptors[MAX_RESERVED_DESCRIPTORS];

} SERVICE_DESCRIPTION_t, *PSERVICE_DESCRIPTION_t;

typedef struct service_description_section_s
{
	U8		table_id;									//8

	U16		section_length;								//12
	U8		reserved0;									//2
	U8		reserved_future_use0;						//1
	U8		section_syntax_indicator;					//1

	U16		transport_stream_id;						//16

	U8		current_next_indicator;						//1
	U8		version_number;								//5
	U8		reserved1;									//2

	U8		section_number;								//8
	U8		last_section_number;						//8
	U16		original_network_id;						//16

	U8		reserved_future_use1;						// 8

	int						service_count;
	SERVICE_DESCRIPTION_t	astServiceDescriptions[MAX_SERVICES_PER_STREAM];		

	U32						CRC_32;										//32
	U32						CRC_32_recalculated;						//32

} service_description_section_t, *pservice_description_section_t;

_CDL_EXPORT	int	DVB_SI_SDT_DecodeSection(uint8_t *buf, int length, service_description_section_t* pSDTSection);

/*------------------------------------------------------------
		EIT definition 
-------------------------------------------------------------*/
typedef struct _EVENT_DESCRIPTION_s
{
	uint16_t	event_id;									//16

	uint64_t	start_time;								//40
	uint32_t	duration;								//24

	uint8_t		running_status;								//3
	uint8_t		free_CA_mode;								//1
	uint16_t	descriptors_loop_length;					//12

	int										event_descriptor_count;
	reserved_descriptor_t					event_descriptors[MAX_RESERVED_DESCRIPTORS];

} EVENT_DESCRIPTION_t, *PEVENT_DESCRIPTION_t;

typedef struct event_information_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U16		service_id;									//16

	U8		reserved1;									//2
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8
	U16		transport_stream_id;						//16
	U16		original_network_id;						//16
	U8		segment_last_section_number;				//8
	U8		last_table_id;								//8

	int						event_count;
	EVENT_DESCRIPTION_t		astEvents[MAX_EVENTS_IN_EIT_SECTION];

	U32						CRC_32;										//32
	U32						CRC_32_recalculated;						//32

} event_information_section_t, *pevent_information_section_t;

_CDL_EXPORT	int	DVB_SI_EIT_DecodeSection(uint8_t* buf, int length, event_information_section_t* pEITSection);

/*------------------------------------------------------------
		RST definition 
-------------------------------------------------------------*/
typedef struct _RUNSTATE_DESCRIPTION_s
{
	U16		transport_stream_id;
	U16		original_network_id;
	U16		service_id;
	U16		event_id;

	U8		reserved_future_use;
	U8		running_status;

} RUNSTATUS_DESCRIPTION_t, *PRUNSTATUS_DESCRIPTION_t;

typedef struct running_status_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved;									//2
	U16		section_length;								//12

	S32							N;										
	RUNSTATUS_DESCRIPTION_t		astRunStatus[MAX_STREAMS_PER_NETWORK];

} running_status_section_t, *prunning_status_section_t;

_CDL_EXPORT	int	DVB_SI_RST_DecodeSection(uint8_t* buf, int length, running_status_section_t* pRSTSection);

/*------------------------------------------------------------
		TDT definition 
-------------------------------------------------------------*/
typedef struct time_date_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved;									//2

	U16			section_length;								//12
	uint64_t	UTC_time;									//40

} time_date_section_t, *ptime_date_section_t;

_CDL_EXPORT	int	DVB_SI_TDT_DecodeSection(uint8_t* buf, int length, time_date_section_t* pTDTSection);

/*------------------------------------------------------------
		TOT definition 
-------------------------------------------------------------*/
typedef struct time_offset_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	uint64_t UTC_time;									//40
	
	U8		reserved1;									// 4
	U16		descriptors_loop_length;					//12

	S32										descriptor_count;
	reserved_descriptor_t					descriptors[MAX_RESERVED_DESCRIPTORS];

	U32		CRC_32;										//32
	U32		CRC_32_recalculated;						//32

} time_offset_section_t, *ptime_offset_section_t;

_CDL_EXPORT	int	DVB_SI_TOT_DecodeSection(uint8_t* buf, int length, time_offset_section_t* pTOTSection);

/*------------------------------------------------------------
CMT definition
-------------------------------------------------------------*/
typedef struct CA_message_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		DVB_reserved;								//1
	uint8_t		ISO_reserved;								//2
	uint16_t	CA_section_length;							//12

	uint8_t		CA_data_byte[64];							//

} CA_message_section_t, *pCA_message_section_t;

_CDL_EXPORT	int	DVB_SI_CMT_DecodeSection(uint8_t* buf, int length, CA_message_section_t* pcmt_ection);

#endif
