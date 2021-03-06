#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/DVB_SI_Descriptor.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

#include "DVB_SI_Utilities_Inner.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif


int DVB_Databroadcast_decode_multiprotocol_encapsulation_info(uint8_t* buf, int length, multiprotocol_encapsulation_info_t* pmultiprotocol_encapsulation_info)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pmultiprotocol_encapsulation_info != NULL))
	{
		pmultiprotocol_encapsulation_info->MAC_address_range = (*buf & 0xe0) >> 5;
		pmultiprotocol_encapsulation_info->MAC_IP_mapping_flag = (*buf & 0x10) >> 4;
		pmultiprotocol_encapsulation_info->alignment_indicator = (*buf & 0x08) >> 3;
		pmultiprotocol_encapsulation_info->reserved = (*buf++ & 0x07);

		pmultiprotocol_encapsulation_info->max_sections_per_datagram = *buf++;
	}

	return 0;
}

int DVB_Databroadcast_decode_data_carousel_info(uint8_t* buf, int length, data_carousel_info_t* pdata_carousel_info)
{
	if ((buf != NULL) && (length > 0) && (pdata_carousel_info != NULL))
	{
		pdata_carousel_info->carousel_type_id = (*buf & 0xC0) >> 6;
		pdata_carousel_info->reserved0 = *buf++ & 0x3F;

		pdata_carousel_info->transaction_id = *buf++;

		pdata_carousel_info->transaction_id <<= 8;
		pdata_carousel_info->transaction_id |= *buf++;

		pdata_carousel_info->transaction_id <<= 8;
		pdata_carousel_info->transaction_id |= *buf++;

		pdata_carousel_info->transaction_id <<= 8;
		pdata_carousel_info->transaction_id |= *buf++;

		pdata_carousel_info->time_out_value_DSI = *buf++;

		pdata_carousel_info->time_out_value_DSI <<= 8;
		pdata_carousel_info->time_out_value_DSI |= *buf++;

		pdata_carousel_info->time_out_value_DSI <<= 8;
		pdata_carousel_info->time_out_value_DSI |= *buf++;

		pdata_carousel_info->time_out_value_DSI <<= 8;
		pdata_carousel_info->time_out_value_DSI |= *buf++;

		pdata_carousel_info->time_out_value_DII = *buf++;

		pdata_carousel_info->time_out_value_DII <<= 8;
		pdata_carousel_info->time_out_value_DII |= *buf++;

		pdata_carousel_info->time_out_value_DII <<= 8;
		pdata_carousel_info->time_out_value_DII |= *buf++;

		pdata_carousel_info->time_out_value_DII <<= 8;
		pdata_carousel_info->time_out_value_DII |= *buf++;

		pdata_carousel_info->reserved1 = (*buf & 0xC0) >> 6;

		pdata_carousel_info->leak_rate = *buf++ & 0x3F;

		pdata_carousel_info->leak_rate <<= 8;
		pdata_carousel_info->leak_rate |= *buf++;

		pdata_carousel_info->leak_rate <<= 8;
		pdata_carousel_info->leak_rate |= *buf++;
	}

	return 0;
}

int DVB_Databroadcast_decode_object_carousel_info(uint8_t* buf, int length, object_carousel_info_t* pobject_carousel_info)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	int			N;
	uint8_t*	ptemp;
	uint8_t*	pend;
	//int			copy_length;

	if ((buf != NULL) && (length >= 16) && (pobject_carousel_info != NULL))
	{
		memset(pobject_carousel_info, 0x00, sizeof(object_carousel_info_t));

		pend = buf + length;

		pobject_carousel_info->carousel_type_id = (*buf & 0xC0) >> 6;
		pobject_carousel_info->reserved0 = *buf++ & 0x3F;

		pobject_carousel_info->transaction_id = *buf++;
		pobject_carousel_info->transaction_id <<= 8;
		pobject_carousel_info->transaction_id |= *buf++;
		pobject_carousel_info->transaction_id <<= 8;
		pobject_carousel_info->transaction_id |= *buf++;
		pobject_carousel_info->transaction_id <<= 8;
		pobject_carousel_info->transaction_id |= *buf++;

		pobject_carousel_info->time_out_value_DSI = *buf++;
		pobject_carousel_info->time_out_value_DSI <<= 8;
		pobject_carousel_info->time_out_value_DSI |= *buf++;
		pobject_carousel_info->time_out_value_DSI <<= 8;
		pobject_carousel_info->time_out_value_DSI |= *buf++;
		pobject_carousel_info->time_out_value_DSI <<= 8;
		pobject_carousel_info->time_out_value_DSI |= *buf++;

		pobject_carousel_info->time_out_value_DII = *buf++;
		pobject_carousel_info->time_out_value_DII <<= 8;
		pobject_carousel_info->time_out_value_DII |= *buf++;
		pobject_carousel_info->time_out_value_DII <<= 8;
		pobject_carousel_info->time_out_value_DII |= *buf++;
		pobject_carousel_info->time_out_value_DII <<= 8;
		pobject_carousel_info->time_out_value_DII |= *buf++;

		pobject_carousel_info->reserved1 = (*buf & 0xC0) >> 6;

		pobject_carousel_info->leak_rate = (*buf++ & 0x3F);
		pobject_carousel_info->leak_rate <<= 8;
		pobject_carousel_info->leak_rate |= *buf++;
		pobject_carousel_info->leak_rate <<= 8;
		pobject_carousel_info->leak_rate |= *buf++;

		ptemp = buf;

		N = 0;
		while ((ptemp < pend) && (N < MAX_OBJECTS_PER_SERVICE))
		{
			//pobject_carousel_info->ISO_639_language_code[N] = ptemp[0];
			//pobject_carousel_info->ISO_639_language_code[N] <<= 8;
			//pobject_carousel_info->ISO_639_language_code[N] |= ptemp[1];
			//pobject_carousel_info->ISO_639_language_code[N] <<= 8;
			//pobject_carousel_info->ISO_639_language_code[N] |= ptemp[2];

			memcpy(pobject_carousel_info->st[N].ISO_639_language_code_char, ptemp, 3);
			pobject_carousel_info->st[N].ISO_639_language_code_char[3] = '\0';
			ptemp += 3;

			pobject_carousel_info->st[N].object_name_length = *ptemp ++;

			if (pobject_carousel_info->st[N].object_name_length > 0)
			{
				pobject_carousel_info->st[N].object_name_char = ptemp;
				ptemp += pobject_carousel_info->st[N].object_name_length;

				pobject_carousel_info->st[N].trimmed_object_name_char = DVB_SI_StringPrefixTrim(pobject_carousel_info->st[N].object_name_char);
				pobject_carousel_info->st[N].trimmed_object_name_length = pobject_carousel_info->st[N].object_name_length -
					(int)(pobject_carousel_info->st[N].trimmed_object_name_char - pobject_carousel_info->st[N].object_name_char);
			}

			N++;
		}

		pobject_carousel_info->N = N;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//
int INT_decode_IP_MAC_notification_info(uint8_t* buf, int length, IP_MAC_notification_info_t* pIP_MAC_notification_info)
{
	int		N;
	int		i;
	int		copy_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pIP_MAC_notification_info != NULL))
	{
		pIP_MAC_notification_info->platform_id_data_length = *buf++;

		N = pIP_MAC_notification_info->platform_id_data_length / 5;
		
		for (i = 0; i < N; i++)
		{
			pIP_MAC_notification_info->platform_id[i] = *buf++;
			pIP_MAC_notification_info->platform_id[i] <<= 8;
			pIP_MAC_notification_info->platform_id[i] |= *buf++;
			pIP_MAC_notification_info->platform_id[i] <<= 8;
			pIP_MAC_notification_info->platform_id[i] |= *buf++;

			pIP_MAC_notification_info->action_type[i] = *buf++;
			
			pIP_MAC_notification_info->reserved[i] = (*buf & 0xC0) >> 6;
			pIP_MAC_notification_info->INT_versioning_flag[i] = (*buf & 0x20) >> 5;
			pIP_MAC_notification_info->INT_version[i] = (*buf++ & 0x1f);
		}

		pIP_MAC_notification_info->N = N;

		N = length - pIP_MAC_notification_info->platform_id_data_length - 1;
		copy_length = (N < 16) ? N : 16;
		memcpy(pIP_MAC_notification_info->private_data_byte, buf, copy_length);

		pIP_MAC_notification_info->private_data_length = N;
	}

	return 0;
}





