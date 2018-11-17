#ifndef __DVB_DATA_BROADCAST_H__
#define __DVB_DATA_BROADCAST_H__

#include <stdint.h>

typedef struct _data_carousel_info_s
{
	uint8_t		carousel_type_id;			//2
	uint8_t		reserved0;					//6

	uint32_t	transaction_id;				//32
	uint32_t	time_out_value_DSI;			//32
	uint32_t	time_out_value_DII;			//32

	uint32_t	reserved1;					//2
	uint32_t	leak_rate;					//22

} data_carousel_info_t, *pdata_carousel_info_t;

typedef struct _object_carousel_info_s
{
	uint8_t		carousel_type_id;				//2
	uint8_t		reserved0;				//6

	uint32_t	transaction_id;			//32
	uint32_t	time_out_value_DSI;			//32
	uint32_t	time_out_value_DII;			//32

	uint32_t	reserved1;					//2
	uint32_t	leak_rate;					//22

	int			N;

	struct
	{
		//uint32_t	ISO_639_language_code[MAX_OBJECTS_PER_SERVICE];
		char		ISO_639_language_code_char[4];

		int			object_name_length;
		uint8_t*	object_name_char;

		int			trimmed_object_name_length;
		uint8_t*	trimmed_object_name_char;

	} st[MAX_OBJECTS_PER_SERVICE];

} object_carousel_info_t, *pobject_carousel_info_t;

typedef struct multiprotocol_encapsulation_info_s
{
	uint8_t		MAC_address_range;					//3
	uint8_t		MAC_IP_mapping_flag;				//1
	uint8_t		alignment_indicator;				//1
	uint8_t		reserved;							//3
	uint8_t		max_sections_per_datagram;			//8

} multiprotocol_encapsulation_info_t, *pmultiprotocol_encapsulation_info_t;

typedef struct IP_MAC_notification_info_s
{
	uint8_t		platform_id_data_length;			//8
	int			N;

	uint32_t	platform_id[MAX_IP_PLATFORMS];
	uint8_t		action_type[MAX_IP_PLATFORMS];
	uint8_t		reserved[MAX_IP_PLATFORMS];
	uint8_t		INT_versioning_flag[MAX_IP_PLATFORMS];
	uint8_t		INT_version[MAX_IP_PLATFORMS];

	int			private_data_length;
	uint8_t		private_data_byte[16];

} IP_MAC_notification_info_t, *pIP_MAC_notification_info_t;

int DVB_Databroadcast_decode_multiprotocol_encapsulation_info(uint8_t* buf, int length, multiprotocol_encapsulation_info_t* pmultiprotocol_encapsulation_info);
int DVB_Databroadcast_decode_data_carousel_info(uint8_t* buf, int length, data_carousel_info_t* pdata_carousel_info);
int DVB_Databroadcast_decode_object_carousel_info(uint8_t* buf, int length, object_carousel_info_t* pobject_carousel_info);

int INT_decode_IP_MAC_notification_info(uint8_t* buf, int length, IP_MAC_notification_info_t* pIP_MAC_notification_info);


#endif