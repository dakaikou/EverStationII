#ifndef _DVB_IPDC_SECTION_H_
#define _DVB_IPDC_SECTION_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DVB_IPDC_SECTION_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DVB_IPDC_SECTION_LIB __declspec(dllimport)
#   else
#       define DVB_IPDC_SECTION_LIB
#   endif
#elif __GNUC__ >= 4
#   define DVB_IPDC_SECTION_LIB __attribute__((visibility("default")))
#else
#   define DVB_IPDC_SECTION_LIB
#endif

#include "DVB_SI_Descriptor.h"
#include "MPEG2_DVB_SysCapability.h"

#define DVB_IPDC_MPE_SECTION_MAX_SIZE			4096
#define DVB_IPDC_MPE_SECTION_MIN_SIZE			16

#define DVB_IPDC_MPE_FEC_SECTION_MAX_SIZE		4096
#define DVB_IPDC_MPE_FEC_SECTION_MIN_SIZE		16

#define DVB_IPDC_INT_SECTION_MAX_SIZE			4096
#define DVB_IPDC_INT_SECTION_MIN_SIZE			16


//MPE datagram
typedef struct datagram_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		private_indicator;							//1
	uint8_t		reserved0;									//2
	uint16_t		section_length;								//12

	uint8_t		MAC_address_6;								//8
	uint8_t		MAC_address_5;								//8

	uint8_t		reserved1;									//2
	uint8_t		payload_scrambling_control;					//2
	uint8_t		address_scrambling_control;					//2
	uint8_t		LLC_SNAP_flag;								//1
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	uint8_t		MAC_address_4;								//8
	uint8_t		MAC_address_3;								//8
	uint8_t		MAC_address_2;								//8
	uint8_t		MAC_address_1;								//8

	int			LLC_SNAP_data_length;
	uint8_t*	LLC_SNAP_data_byte;

	int			IP_datagram_data_length;
	uint8_t*	IP_datagram_data_byte;

	int			stuffing_length;
	uint8_t*	stuffing_byte;

	uint32_t	encodedCheckValue;								//32
	uint32_t	recalculatedCheckValue;							//32

} datagram_section_t, *pdatagram_section_t;

DVB_IPDC_SECTION_LIB int	DVB_IPDC_MPE_DecodeSection(uint8_t *buf, int length, datagram_section_t* pMPESection);


/*------------------------------------------------------------
INT definition
-------------------------------------------------------------*/
typedef struct platform_descriptor_loop_s
{
	uint8_t					reserved;							//4
	int						platform_descriptor_loop_length;	//12

																//	IPMAC_platform_name_descriptor_t			IPMAC_platform_name_descriptor;
																//	IPMAC_platform_provider_name_descriptor_t	IPMAC_platform_provider_name_descriptor;

	int						reserved_count;
	reserved_descriptor_t	reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} platform_descriptor_loop_t;

typedef struct target_descriptor_loop_s
{
	uint8_t					reserved;							//4
	int						target_descriptor_loop_length;	//12

	int						target_descriptor_count;
	reserved_descriptor_t	target_descriptors[MAX_RESERVED_DESCRIPTORS];

} target_descriptor_loop_t;

typedef struct operational_descriptor_loop_s
{
	uint8_t					reserved;							//4
	int						operational_descriptor_loop_length;	//12

																//	IPMAC_stream_location_descriptor_t		IPMAC_stream_location_descriptor;
																//	time_slice_fec_identifier_descriptor_t	time_slice_fec_identifier_descriptor;

	int						operational_descriptor_count;
	reserved_descriptor_t	operational_descriptors[MAX_RESERVED_DESCRIPTORS];

} operational_descriptor_loop_t;

typedef struct IP_MAC_notification_section_s
{
	uint8_t						table_id;						//8
	uint8_t						section_syntax_indicator;		//1
	uint8_t						reserved_for_future_use;		//1
	uint8_t						reserved0;						//2
	uint16_t						section_length;					//12
	uint8_t						action_type;					//8
	uint8_t						platform_id_hash;				//8
	uint8_t						reserved1;						//2
	uint8_t						version_number;					//5
	uint8_t						current_next_indicator;			//1
	uint8_t						section_number;					//8
	uint8_t						last_section_number;			//8
	uint32_t					platform_id;					//24
	uint8_t						processing_order;				//8

	platform_descriptor_loop_t	platform_descriptor_loop;

	int							notification_count;
	struct
	{
		target_descriptor_loop_t		target_descriptor_loop;
		operational_descriptor_loop_t	operational_descriptor_loop;
	} notifications[MAX_INT_TARGETS];

	uint32_t						CRC_32;							//32
	uint32_t						CRC_32_recalculated;			//32

} IP_MAC_notification_section_t, *pIP_MAC_notification_section_t;

DVB_IPDC_SECTION_LIB int DVB_IPDC_INT_DecodeSection(uint8_t* section_buf, int section_size, IP_MAC_notification_section_t* pINTSection);


#endif
