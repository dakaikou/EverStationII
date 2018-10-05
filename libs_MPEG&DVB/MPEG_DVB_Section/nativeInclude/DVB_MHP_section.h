#ifndef _DVB_MHP_SECTION_H_
#define _DVB_MHP_SECTION_H_

#include "../compile.h"

#include "DVB_SI_Descriptor.h"
#include "MPEG_DVB_SysCapability.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

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
	U32		organisation_id;					//32
	U16		application_id;						//16
} application_identifier_t, *papplication_identifier_t;

/*------------------------------------------------------------
Application Information
------------------------------------------------------------*/
typedef struct application_s
{
	application_identifier_t	application_identifier;						//48
	U8							application_control_code;					//8
	U8							reserved_future_use;						//4
	U16							application_descriptors_loop_length;		//12

																			//	application_descriptor_t		application_descriptor;
																			//	application_name_descriptor_t	application_name_descriptor;
																			//	transport_protocol_descriptor_t	transport_protocol_descriptor;
																			//	DVB_HTML_application_location_descriptor_t	DVB_HTML_application_location_descriptor;

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} application_t, *papplication_t;

/*------------------------------------------------------------
AIT definition
ETSI TS 102 812
-------------------------------------------------------------*/
typedef struct application_information_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		reserved_future_use0;						//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U8		test_application_flag;						//1
	U16		application_type;							//15

	U8		reserved1;									//2
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	U16		reserved_future_use1;						//4
	U16		common_descriptors_length;					//12

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

	U16		reserved_future_use2;						//4
	U16		application_loop_length;					//12

	application_t	applications[MAX_APPLICATIONS];
	S8				N;

	U32				CRC_32;										//32
	U32				CRC_32_verify;								//32

} application_information_section_t, *papplication_information_section_t;


_CDL_EXPORT	int	DVB_MHP_AIT_DecodeSection(uint8_t *buf, int length, application_information_section_t* pait_section);


#endif
