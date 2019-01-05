#ifndef _DVB_MHP_SECTION_H_
#define _DVB_MHP_SECTION_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DVB_MHP_SECTION_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DVB_MHP_SECTION_LIB __declspec(dllimport)
#   else
#       define DVB_MHP_SECTION_LIB
#   endif
#elif __GNUC__ >= 4
#   define DVB_MHP_SECTION_LIB __attribute__((visibility("default")))
#else
#   define DVB_MHP_SECTION_LIB
#endif

#include "DVB_SI_Descriptor.h"
#include "MPEG2_DVB_SysCapability.h"
/*------------------------------------------------------------
Section size definition
-------------------------------------------------------------*/
#define DVB_MHP_AIT_SECTION_MAX_SIZE			1024
#define DVB_MHP_AIT_SECTION_MIN_SIZE			16

/*------------------------------------------------------------
Application Identifier
------------------------------------------------------------*/
typedef struct application_identifier_s
{
	uint32_t	organisation_id;					//32
	uint16_t	application_id;						//16
} application_identifier_t, *papplication_identifier_t;

/*------------------------------------------------------------
Application Information
------------------------------------------------------------*/
typedef struct application_s
{
	application_identifier_t	application_identifier;						//48
	uint8_t						application_control_code;					//8
	uint8_t						reserved_future_use;						//4
	uint16_t					application_descriptors_loop_length;		//12

																			//	application_descriptor_t		application_descriptor;
																			//	application_name_descriptor_t	application_name_descriptor;
																			//	transport_protocol_descriptor_t	transport_protocol_descriptor;
																			//	DVB_HTML_application_location_descriptor_t	DVB_HTML_application_location_descriptor;

	int							descriptor_count;
	reserved_descriptor_t		descriptors[MAX_RESERVED_DESCRIPTORS];

} application_t, *papplication_t;

/*------------------------------------------------------------
AIT definition
ETSI TS 102 812
-------------------------------------------------------------*/
typedef struct application_information_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		reserved_future_use0;						//1
	uint8_t		reserved0;									//2
	uint16_t		section_length;								//12

	uint8_t		test_application_flag;						//1
	uint16_t		application_type;							//15

	uint8_t		reserved1;									//2
	uint8_t		version_number;								//5
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	uint16_t		reserved_future_use1;						//4
	uint16_t		common_descriptors_length;					//12

	int							common_descriptor_count;
	reserved_descriptor_t		common_descriptors[MAX_RESERVED_DESCRIPTORS];

	uint16_t		reserved_future_use2;						//4
	uint16_t		application_loop_length;					//12

	application_t	applications[MAX_APPLICATIONS];
	int				application_count;

	uint32_t				CRC_32;										//32
	uint32_t				CRC_32_recalculated;						//32

} application_information_section_t, *papplication_information_section_t;


DVB_MHP_SECTION_LIB	int	DVB_MHP_AIT_DecodeSection(uint8_t *buf, int length, application_information_section_t* pait_section);


#endif
