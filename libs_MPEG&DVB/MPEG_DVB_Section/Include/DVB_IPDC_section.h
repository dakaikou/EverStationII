#ifndef _DVB_IPDC_SECTION_H_
#define _DVB_IPDC_SECTION_H_

#include "../compile.h"

#include "DVB_SI_Descriptor.h"
#include "MPEG_DVB_SysCapability.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"

#define DVB_IPDC_MPE_SECTION_MAX_SIZE			4096
#define DVB_IPDC_MPE_SECTION_MIN_SIZE			16

#define DVB_IPDC_MPE_FEC_SECTION_MAX_SIZE		4096
#define DVB_IPDC_MPE_FEC_SECTION_MIN_SIZE		16

#define DVB_IPDC_INT_SECTION_MAX_SIZE			4096
#define DVB_IPDC_INT_SECTION_MIN_SIZE			16


//MPE datagram
typedef struct datagram_section_s
{
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		private_indicator;							//1
	U8		reserved0;									//2
	U16		section_length;								//12

	U8		MAC_address_6;								//8
	U8		MAC_address_5;								//8

	U8		reserved1;									//2
	U8		payload_scrambling_control;					//2
	U8		address_scrambling_control;					//2
	U8		LLC_SNAP_flag;								//1
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	U8		MAC_address_4;								//8
	U8		MAC_address_3;								//8
	U8		MAC_address_2;								//8
	U8		MAC_address_1;								//8

	int			LLC_SNAP_data_length;
	uint8_t*	LLC_SNAP_data_byte;

	int			IP_datagram_data_length;
	uint8_t*	IP_datagram_data_byte;

	int			stuffing_length;
	uint8_t*	stuffing_byte;

	uint32_t	encodedCheckValue;								//32
	uint32_t	recalculatedCheckValue;							//32

} datagram_section_t, *pdatagram_section_t;

_CDL_EXPORT	int	DVB_IPDC_MPE_DecodeSection(uint8_t *buf, int length, datagram_section_t* pMPESection);


/*------------------------------------------------------------
INT definition
-------------------------------------------------------------*/
typedef struct platform_descriptor_loop_s
{
	U8						reserved;							//4
	S16						platform_descriptor_loop_length;	//12

																//	IPMAC_platform_name_descriptor_t			IPMAC_platform_name_descriptor;
																//	IPMAC_platform_provider_name_descriptor_t	IPMAC_platform_provider_name_descriptor;

	S32						reserved_count;
	reserved_descriptor_t	reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} platform_descriptor_loop_t;

typedef struct target_descriptor_loop_s
{
	U8						reserved;							//4
	S16						target_descriptor_loop_length;	//12

															//	target_IPv6_slash_descriptor_t	target_IPv6_slash_descriptor;

	S32						reserved_count;
	reserved_descriptor_t	reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} target_descriptor_loop_t;

typedef struct operational_descriptor_loop_s
{
	U8						reserved;							//4
	S16						operational_descriptor_loop_length;	//12

																//	IPMAC_stream_location_descriptor_t		IPMAC_stream_location_descriptor;
																//	time_slice_fec_identifier_descriptor_t	time_slice_fec_identifier_descriptor;

	S32						reserved_count;
	reserved_descriptor_t	reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} operational_descriptor_loop_t;

typedef struct IP_MAC_notification_section_s
{
	U8						table_id;						//8
	U8						section_syntax_indicator;		//1
	U8						reserved_for_future_use;		//1
	U8						reserved0;						//2
	U16						section_length;					//12
	U8						action_type;					//8
	U8						platform_id_hash;				//8
	U8						reserved1;						//2
	U8						version_number;					//5
	U8						current_next_indicator;			//1
	U8						section_number;					//8
	U8						last_section_number;			//8
	U32						platform_id;					//24
	U8						processing_order;				//8

	platform_descriptor_loop_t	platform_descriptor_loop;

	S32								N;
	target_descriptor_loop_t		target_descriptor_loop[MAX_INT_TARGETS];
	operational_descriptor_loop_t	operational_descriptor_loop[MAX_INT_TARGETS];

	U32								CRC_32;					//32
	U32								CRC_32_verify;			//32

} IP_MAC_notification_section_t, *pIP_MAC_notification_section_t;

_CDL_EXPORT	int	DVB_IPDC_INT_DecodeSection(uint8_t* buf, int length, IP_MAC_notification_section_t* pINTSection);


#endif
