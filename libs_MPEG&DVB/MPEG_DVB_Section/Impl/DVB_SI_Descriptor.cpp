#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/DVB_SI_Descriptor.h"
#include "../Include/DVB_SI_Utilities.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "DVB_SI_Utilities_Inner.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif


//功能：解网络名称描述子			0x40
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_network_name_descriptor(uint8_t* buf, int length, network_name_descriptor_t* pnetwork_name_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	S32		copy_length = 0;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pnetwork_name_descriptor != NULL))
	{
		pnetwork_name_descriptor->descriptor_tag = *buf++;
		pnetwork_name_descriptor->descriptor_length = *buf++;

		if (length >= (pnetwork_name_descriptor->descriptor_length + 2))
		{
			if (pnetwork_name_descriptor->descriptor_length > 0)
			{
				copy_length = min(pnetwork_name_descriptor->descriptor_length, MAX_NETWORK_NAME_LENGTH);

				memcpy(pnetwork_name_descriptor->network_name, buf, copy_length);
				pnetwork_name_descriptor->network_name[copy_length] = '\0';

				pnetwork_name_descriptor->trimmed_network_name = DVB_SI_StringPrefixTrim(pnetwork_name_descriptor->network_name);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解服务列表描述子				0x41
//输入：buffer, 起始位置nIndex
//返回：状态码
int DVB_SI_decode_service_list_descriptor(uint8_t* buf, int length, service_list_descriptor_t* pservice_list_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;

	S32	N = 0;
	S32 loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pservice_list_descriptor != NULL))
	{
		pservice_list_descriptor->descriptor_tag = *buf++;
		pservice_list_descriptor->descriptor_length = *buf++;

		loop_length = pservice_list_descriptor->descriptor_length;

		while (loop_length > 0)
		{
			pservice_list_descriptor->service_id[N] = *buf++ << 8;
			pservice_list_descriptor->service_id[N] |= *buf++;

			pservice_list_descriptor->service_type[N] = *buf++;

			loop_length -= 3;
			N++;
		}

		pservice_list_descriptor->N = N;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}


//功能：解填充描述子					0x42
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_stuffing_descriptor(U8* buf, S32 length, pstuffing_descriptor_t pstuffing_descriptor)
{
	S32		copy_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pstuffing_descriptor != NULL))
	{
		pstuffing_descriptor->descriptor_tag = *buf++;
		pstuffing_descriptor->descriptor_length = *buf++;

		copy_length = (pstuffing_descriptor->descriptor_length < MAX_STUFFING_LENGTH) ? 
									pstuffing_descriptor->descriptor_length : MAX_STUFFING_LENGTH;
		memcpy(pstuffing_descriptor->stuffing_byte, buf, copy_length);
		pstuffing_descriptor->stuffing_byte[copy_length] = '\0';
	}

	return 0;
}

//功能：解卫星传送系统描述子				0x43
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_satellite_delivery_system_descriptor(U8* buf, S32 length, psatellite_delivery_system_descriptor_t psatellite_delivery_system_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(psatellite_delivery_system_descriptor != NULL))
	{
		psatellite_delivery_system_descriptor->descriptor_tag = *buf++; 
		psatellite_delivery_system_descriptor->descriptor_length = *buf++;
	    	
		psatellite_delivery_system_descriptor->frequency = *buf++ << 24;
		psatellite_delivery_system_descriptor->frequency |= *buf++ << 16;
		psatellite_delivery_system_descriptor->frequency |= *buf++ << 8;
		psatellite_delivery_system_descriptor->frequency |= *buf++;

		psatellite_delivery_system_descriptor->orbital_position = *buf++ << 8;
		psatellite_delivery_system_descriptor->orbital_position |= *buf++;

		psatellite_delivery_system_descriptor->west_east_flag = (*buf & 0x80) >> 7;
		psatellite_delivery_system_descriptor->polarization = (*buf & 0x60) >> 5;

//		psatellite_delivery_system_descriptor->modulation = *buf++ & 0x1F;

		psatellite_delivery_system_descriptor->roll_off = (*buf & 0x18) >> 3;					//if modulation_system == 0, this field has no meaning
		psatellite_delivery_system_descriptor->modulation_system = (*buf & 0x04) >> 2;
		psatellite_delivery_system_descriptor->modulation_type = (*buf++ & 0x03);

		psatellite_delivery_system_descriptor->symbol_rate = *buf++ << 20;
		psatellite_delivery_system_descriptor->symbol_rate |= *buf++ << 12;
		psatellite_delivery_system_descriptor->symbol_rate |= *buf++ << 4;
		psatellite_delivery_system_descriptor->symbol_rate |= (*buf & 0xF0) >> 4;

		psatellite_delivery_system_descriptor->FEC_inner = *buf++ & 0x0F;
	}

	return 0;
}

//功能：解有线传送系统描述子			0x44
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_cable_delivery_system_descriptor(U8* buf, S32 length, pcable_delivery_system_descriptor_t pcable_delivery_system_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pcable_delivery_system_descriptor != NULL))
	{
		pcable_delivery_system_descriptor->descriptor_tag = *buf++;
		pcable_delivery_system_descriptor->descriptor_length = *buf++;
	    	
		pcable_delivery_system_descriptor->frequency = *buf++ << 24;
		pcable_delivery_system_descriptor->frequency |= *buf++ << 16;
		pcable_delivery_system_descriptor->frequency |= *buf++ << 8;
		pcable_delivery_system_descriptor->frequency |= *buf++;

		pcable_delivery_system_descriptor->reserved_future_use = *buf++ << 4;
		pcable_delivery_system_descriptor->reserved_future_use |= (*buf & 0xF0) >> 4;

		pcable_delivery_system_descriptor->FEC_outer = *buf++ & 0x0F;

		pcable_delivery_system_descriptor->modulation = *buf++;

		pcable_delivery_system_descriptor->symbol_rate = *buf++ << 20;
		pcable_delivery_system_descriptor->symbol_rate |= *buf++ << 12;
		pcable_delivery_system_descriptor->symbol_rate |= *buf++ << 4;
		pcable_delivery_system_descriptor->symbol_rate |= (*buf & 0xF0) >> 4;

		pcable_delivery_system_descriptor->FEC_inner = *buf++ & 0x0F;
	}

	return 0;
}

//0x45
S32	DVB_SI_decode_VBI_data_descriptor(U8* buf, S32 length, pVBI_data_descriptor_t pVBI_data_descriptor)
{
	S32		N = 0;
	U8*		pend;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pVBI_data_descriptor != NULL))
	{
		pVBI_data_descriptor->descriptor_tag = *buf++;
		pVBI_data_descriptor->descriptor_length = *buf++;

		pend = buf + pVBI_data_descriptor->descriptor_length;

		while (buf < pend)
		{
			pVBI_data_descriptor->data_service_id[N] = *buf++;
			pVBI_data_descriptor->data_service_descriptor_length[N] = *buf++;

			//
			buf += pVBI_data_descriptor->data_service_descriptor_length[N];

			N ++;
		}

		pVBI_data_descriptor->N = N;
	}

	return 0;
}

//功能：解业务群名称描述子				0x47
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_bouquet_name_descriptor(U8* buf, S32 length, pbouquet_name_descriptor_t pbouquet_name_descriptor)
{
	S32		copy_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pbouquet_name_descriptor != NULL))
	{
		pbouquet_name_descriptor->descriptor_tag = *buf++;
		pbouquet_name_descriptor->descriptor_length = *buf++;

		copy_length = (pbouquet_name_descriptor->descriptor_length < MAX_BOUQUET_NAME_LENGTH) ? 
												pbouquet_name_descriptor->descriptor_length : MAX_BOUQUET_NAME_LENGTH;

		if (copy_length > 0)
		{
			memcpy(pbouquet_name_descriptor->bouquet_name, buf, copy_length);
			pbouquet_name_descriptor->bouquet_name[copy_length] = '\0';

			pbouquet_name_descriptor->trimmed_bouquet_name = DVB_SI_StringPrefixTrim(pbouquet_name_descriptor->bouquet_name);
		}
		else
		{
			memset(pbouquet_name_descriptor->bouquet_name, 0x00, sizeof(pbouquet_name_descriptor->bouquet_name));
		}
	}

	return 0;
}

//功能：解业务描述子				0x48
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_service_descriptor(uint8_t* buf, int length, service_descriptor_t* pservice_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pservice_descriptor != NULL))
	{
		pservice_descriptor->descriptor_tag = *buf++;
		pservice_descriptor->descriptor_length = *buf++;
		pservice_descriptor->service_type = *buf++;

		pservice_descriptor->service_provider_name_length = *buf++;

		int copy_length = (pservice_descriptor->service_provider_name_length < MAX_SERVICE_PROVIDER_NAME_LENGTH) ?
			pservice_descriptor->service_provider_name_length : MAX_SERVICE_PROVIDER_NAME_LENGTH;

		if (copy_length > 0)
		{
			memcpy(pservice_descriptor->service_provider_name, buf, copy_length);
			pservice_descriptor->service_provider_name[copy_length] = '\0';

			pservice_descriptor->trimmed_service_provider_name = DVB_SI_StringPrefixTrim(pservice_descriptor->service_provider_name);
		}
		else
		{
			memset(pservice_descriptor->service_provider_name, 0x00, sizeof(pservice_descriptor->service_provider_name));
		}
		buf += pservice_descriptor->service_provider_name_length;

		pservice_descriptor->service_name_length = *buf++;
		
		copy_length = (pservice_descriptor->service_name_length < MAX_SERVICE_NAME_LENGTH) ? 
			pservice_descriptor->service_name_length : MAX_SERVICE_NAME_LENGTH;

		if (copy_length > 0)
		{
			memcpy(pservice_descriptor->service_name, buf, copy_length);
			pservice_descriptor->service_name[copy_length] = '\0';

			pservice_descriptor->trimmed_service_name = DVB_SI_StringPrefixTrim(pservice_descriptor->service_name);
		}
		else
		{
			memset(pservice_descriptor->service_name, 0x00, sizeof(pservice_descriptor->service_name));
		}

		buf += pservice_descriptor->service_name_length;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解有效性国家描述子					0x49
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_country_availability_descriptor(U8* buf, S32 length, pcountry_availability_descriptor_t pcountry_availability_descriptor)
{
	S32 loop_length;
	S32 N = 0;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pcountry_availability_descriptor != NULL))
	{
		pcountry_availability_descriptor->descriptor_tag = *buf++; 
		pcountry_availability_descriptor->descriptor_length = *buf++;
			
		pcountry_availability_descriptor->country_availability_flag = (*buf & 0x80) >> 7;
		pcountry_availability_descriptor->reserved_future_use = (*buf++ & 0x7f);
		
		loop_length = pcountry_availability_descriptor->descriptor_length - 1;
		while (loop_length >= 3)
		{
			if (N < MAX_COUNTRIES)
			{
				pcountry_availability_descriptor->country_code[N] = buf[0];
				pcountry_availability_descriptor->country_code[N] <<= 8;
				pcountry_availability_descriptor->country_code[N] |= buf[1];
				pcountry_availability_descriptor->country_code[N] <<= 8;
				pcountry_availability_descriptor->country_code[N] |= buf[2];

				memcpy(pcountry_availability_descriptor->country_code_char[N], buf, 3);
				pcountry_availability_descriptor->country_code_char[N][3] = '\0';

				buf += 3;

				N++;
			}
			else
			{
				break;
			}

			loop_length -= 3;
		}

		pcountry_availability_descriptor->N = N;
	}

	return 0;
}

//功能：解链接描述子					0x4A
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_linkage_descriptor(U8* buf, S32 length, plinkage_descriptor_t plinkage_descriptor)
{
	S32		copy_length;
	S32		N;
	S32		M;
	U8*		porigin = buf;
	U8*		pend;
	U8*		pnext;

	if ((buf != NULL) &&
		(length >= 2) &&
		(plinkage_descriptor != NULL))
	{
		plinkage_descriptor->descriptor_tag = *buf++;						//0x4A
		plinkage_descriptor->descriptor_length = *buf++;

		plinkage_descriptor->transport_stream_id = *buf++;
		plinkage_descriptor->transport_stream_id <<= 8;
		plinkage_descriptor->transport_stream_id |= *buf++;

		plinkage_descriptor->original_network_id = *buf++;
		plinkage_descriptor->original_network_id <<= 8;
		plinkage_descriptor->original_network_id |= *buf++;

		plinkage_descriptor->service_id = *buf++;
		plinkage_descriptor->service_id <<= 8;
		plinkage_descriptor->service_id |= *buf++;

		plinkage_descriptor->linkage_type = *buf++;

		switch (plinkage_descriptor->linkage_type)
		{
		case 0x0B:										//announce the DVB services containing INT sub_tables(s)
			plinkage_descriptor->platform_id_data_length = *buf++;

			pend = buf + plinkage_descriptor->platform_id_data_length;
			N = 0;
			while (buf < pend)
			{
				plinkage_descriptor->platform_id[N] = *buf++;

				plinkage_descriptor->platform_id[N] <<= 8;
				plinkage_descriptor->platform_id[N] |= *buf++;
				
				plinkage_descriptor->platform_id[N] <<= 8;
				plinkage_descriptor->platform_id[N] |= *buf++;

				plinkage_descriptor->platform_name_loop_length[N] = *buf++;
				pnext = buf + plinkage_descriptor->platform_name_loop_length[N];
				
				M = 0;
				while (buf < pnext)
				{
					plinkage_descriptor->ISO_639_language_code[N][M] = buf[0];
					plinkage_descriptor->ISO_639_language_code[N][M] <<= 8;
					plinkage_descriptor->ISO_639_language_code[N][M] |= buf[1];
					plinkage_descriptor->ISO_639_language_code[N][M] <<= 8;
					plinkage_descriptor->ISO_639_language_code[N][M] |= buf[2];

					memcpy(plinkage_descriptor->ISO_639_language_code_char[N][M], buf, 3);
					buf += 3;
					plinkage_descriptor->ISO_639_language_code_char[N][M][3] = '\0';

					plinkage_descriptor->platform_name_length[N][M] = *buf++;

					copy_length = (plinkage_descriptor->platform_name_length[N][M] < MAX_IP_PLATFORM_NAME_LENGTH) ? 
															plinkage_descriptor->platform_name_length[N][M] : MAX_IP_PLATFORM_NAME_LENGTH - 1;
					if (copy_length > 0)
					{
						memcpy(plinkage_descriptor->platform_name_text_char[N][M], buf, copy_length);
						plinkage_descriptor->platform_name_text_char[N][M][copy_length] = '\0';
					}

					buf += plinkage_descriptor->platform_name_length[N][M];

					M ++;
				}

				plinkage_descriptor->platform_name_N[N] = M;
				
				N ++;
			}

			plinkage_descriptor->platform_N = N;

			break;
		default:
			break;
		}

		plinkage_descriptor->private_data_length = (plinkage_descriptor->descriptor_length + 2) - (int)(buf - porigin);
		copy_length = (plinkage_descriptor->private_data_length < MAX_PRIVATE_DATA_LENGTH) ? 
												plinkage_descriptor->private_data_length : MAX_PRIVATE_DATA_LENGTH - 1;
		if (copy_length > 0)
		{
			memcpy(plinkage_descriptor->private_data_byte, buf, copy_length);
			plinkage_descriptor->private_data_byte[copy_length] = '\0';
		}
	}

	return 0;
}

//功能：解NVOD参考描述子					0x4B
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_NVOD_reference_descriptor(U8* buf, S32 length, pNVOD_reference_descriptor_t pNVOD_reference_descriptor)
{
	S32 N = 0;
	S32 loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pNVOD_reference_descriptor != NULL))
	{
		pNVOD_reference_descriptor->descriptor_tag = *buf++; 
		pNVOD_reference_descriptor->descriptor_length = *buf++;

		loop_length = pNVOD_reference_descriptor->descriptor_length;
		
		while (loop_length > 0)
		{
			if (N < 32)
			{
				pNVOD_reference_descriptor->transport_stream_id[N] = *buf++ << 8;
				pNVOD_reference_descriptor->transport_stream_id[N] |= *buf++;

				pNVOD_reference_descriptor->original_network_id[N] = *buf++ << 8;
				pNVOD_reference_descriptor->original_network_id[N] |= *buf++;

				pNVOD_reference_descriptor->service_id[N] = *buf++ << 8;
				pNVOD_reference_descriptor->service_id[N] |= *buf++;

				loop_length -= 6;
				N++;
			}
			else
			{
				break;
			}
		}

		pNVOD_reference_descriptor->N = N;
	}

	return 0;
}

//功能：解时间偏移业务描述子					0x4C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_time_shifted_service_descriptor(U8* buf, S32 length, ptime_shifted_service_descriptor_t ptime_shifted_service_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(ptime_shifted_service_descriptor != NULL))
	{
		ptime_shifted_service_descriptor->descriptor_tag = *buf++; 
		ptime_shifted_service_descriptor->descriptor_length = *buf++;

		ptime_shifted_service_descriptor->reference_service_id = *buf++ << 8;
		ptime_shifted_service_descriptor->reference_service_id |= *buf++;
	}

	return 0;
}

//功能：解短节目段描述子					0x4D
//输入：buffer, 起始位置nIndex
//返回：解析状态码
int DVB_SI_decode_short_event_descriptor(uint8_t* buf, int length, short_event_descriptor_t* pshort_event_descriptor)
{
	int		rtcode = SECTION_PARSE_UNKNOWN_ERROR;
	int		copy_length;

	if ((buf != NULL) &&
		(length >= 6) &&
		(pshort_event_descriptor != NULL))
	{
		pshort_event_descriptor->descriptor_tag = *buf++; 
		pshort_event_descriptor->descriptor_length = *buf++;

		pshort_event_descriptor->ISO_639_language_code = buf[0];
		pshort_event_descriptor->ISO_639_language_code <<= 8;
		pshort_event_descriptor->ISO_639_language_code |= buf[1];
		pshort_event_descriptor->ISO_639_language_code <<= 8;
		pshort_event_descriptor->ISO_639_language_code |= buf[2];

		memcpy(pshort_event_descriptor->ISO_639_language_code_char, buf, 3);
		pshort_event_descriptor->ISO_639_language_code_char[3] = '\0';
		buf += 3;

		pshort_event_descriptor->event_name_length = *buf++;
		if (pshort_event_descriptor->event_name_length > 0)
		{
			copy_length = min(pshort_event_descriptor->event_name_length, MAX_EVENT_NAME_LENGTH);

			memcpy(pshort_event_descriptor->event_name_char, buf, copy_length);
			pshort_event_descriptor->event_name_char[copy_length] = '\0';
			pshort_event_descriptor->trimmed_event_name_char = DVB_SI_StringPrefixTrim(pshort_event_descriptor->event_name_char);

			buf += pshort_event_descriptor->event_name_length;
		}
		else
		{
			memset(pshort_event_descriptor->event_name_char, 0x00, sizeof(pshort_event_descriptor->event_name_char));
			pshort_event_descriptor->trimmed_event_name_char = pshort_event_descriptor->event_name_char;
		}

		pshort_event_descriptor->text_length = *buf++;
		if (pshort_event_descriptor->text_length > 0)
		{
			copy_length = min(pshort_event_descriptor->text_length, MAX_EVENT_TEXT_LENGTH);

			memcpy(pshort_event_descriptor->text_char, buf, copy_length);
			pshort_event_descriptor->text_char[copy_length] = '\0';
			pshort_event_descriptor->trimmed_text_char = DVB_SI_StringPrefixTrim(pshort_event_descriptor->text_char);

			buf += pshort_event_descriptor->text_length;
		}
		else
		{
			memset(pshort_event_descriptor->text_char, 0x00, sizeof(pshort_event_descriptor->text_char));
			pshort_event_descriptor->trimmed_text_char = pshort_event_descriptor->text_char;
		}

		rtcode = SECTION_PARSE_NO_ERROR;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解扩展节目段描述子					0x4E	
//输入：buffer, 起始位置nIndex
//返回：解析状态码
int DVB_SI_decode_extended_event_descriptor(uint8_t* buf, int length, extended_event_descriptor_t* pextended_event_descriptor)
{
	S32	copy_length;
	S32 N = 0;
	S32 loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pextended_event_descriptor != NULL))
	{
		pextended_event_descriptor->descriptor_tag = *buf++;
		pextended_event_descriptor->descriptor_length = *buf++;

		pextended_event_descriptor->descriptor_number = (*buf & 0xF0) >> 4;
		pextended_event_descriptor->last_descriptor_number = *buf++ & 0x0F;

		pextended_event_descriptor->ISO_639_language_code = buf[0];
		pextended_event_descriptor->ISO_639_language_code <<= 8;
		pextended_event_descriptor->ISO_639_language_code |= buf[1];
		pextended_event_descriptor->ISO_639_language_code <<= 8;
		pextended_event_descriptor->ISO_639_language_code |= buf[2];

		memcpy(pextended_event_descriptor->ISO_639_language_code_char, buf, 3);
		pextended_event_descriptor->ISO_639_language_code_char[3] = '\0';

		buf += 3;

		pextended_event_descriptor->length_of_items = *buf++;

		loop_length = pextended_event_descriptor->length_of_items;
		while (loop_length > 0)
		{
			if (N < MAX_EXTENDED_EVENTS)
			{
				pextended_event_descriptor->item_description_length[N] = *buf++;
				memcpy(pextended_event_descriptor->item_description_char[N], buf, pextended_event_descriptor->item_description_length[N]);
				pextended_event_descriptor->item_description_char[N][pextended_event_descriptor->item_description_length[N]] = '\0';

				buf += pextended_event_descriptor->item_description_length[N];

				pextended_event_descriptor->item_length[N] = *buf++;
				memcpy(pextended_event_descriptor->item_char[N], buf, pextended_event_descriptor->item_length[N]);
				pextended_event_descriptor->item_char[N][pextended_event_descriptor->item_length[N]] = '\0';

				buf += pextended_event_descriptor->item_length[N];

				loop_length -= (2 + pextended_event_descriptor->item_description_length[N] + pextended_event_descriptor->item_length[N]);

				N ++;
			}
			else
			{
				break;
			}
		}

		pextended_event_descriptor->N = N;

		pextended_event_descriptor->text_length = *buf++;
		copy_length = (pextended_event_descriptor->text_length < MAX_EVENT_TEXT_LENGTH) ? pextended_event_descriptor->text_length : MAX_EVENT_TEXT_LENGTH;
		memcpy(pextended_event_descriptor->text_char, buf, copy_length);
		pextended_event_descriptor->text_char[copy_length] = '\0';
	}

	return 0;
}

//功能：解时间偏移节目段描述子				0x4F
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_time_shifted_event_descriptor(U8* buf, S32 length, ptime_shifted_event_descriptor_t ptime_shifted_event_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(ptime_shifted_event_descriptor != NULL))
	{
		ptime_shifted_event_descriptor->descriptor_tag = *buf++; 
		ptime_shifted_event_descriptor->descriptor_length = *buf++;

		ptime_shifted_event_descriptor->reference_service_id = *buf++ << 8;
		ptime_shifted_event_descriptor->reference_service_id |= *buf++;

		ptime_shifted_event_descriptor->reference_event_id = *buf++ << 8;
		ptime_shifted_event_descriptor->reference_event_id |= *buf++;
	}

	return 0;
}

//功能：解组件描述子							0x50
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_component_descriptor(U8* buf, S32 length, pcomponent_descriptor_t pcomponent_descriptor)
{
	S32 rtcode = SECTION_PARSE_UNKNOWN_ERROR;
	S32	copy_length;
	S32 text_length;

	if ((buf != NULL) &&
		(length >= 8) &&
		(pcomponent_descriptor != NULL))
	{
		pcomponent_descriptor->descriptor_tag = *buf++;
		pcomponent_descriptor->descriptor_length = *buf++;

		pcomponent_descriptor->reserved_future_use = (*buf & 0xF0) >> 4;
		pcomponent_descriptor->stream_content = *buf++ & 0x0F;

		pcomponent_descriptor->component_type = *buf++;
		pcomponent_descriptor->component_tag = *buf++;
	 
		pcomponent_descriptor->ISO_639_language_code = buf[0];
		pcomponent_descriptor->ISO_639_language_code <<= 8;
		pcomponent_descriptor->ISO_639_language_code |= buf[1];
		pcomponent_descriptor->ISO_639_language_code <<= 8;
		pcomponent_descriptor->ISO_639_language_code |= buf[2];

		memcpy(pcomponent_descriptor->ISO_639_language_code_char, buf, 3);
		pcomponent_descriptor->ISO_639_language_code_char[3] = '\0';
		buf += 3;

		text_length = pcomponent_descriptor->descriptor_length - 6;

		if (text_length > 0)
		{
			copy_length = (text_length < MAX_COMPONENT_TEXT_LENGTH) ? text_length : MAX_COMPONENT_TEXT_LENGTH;
			memcpy(pcomponent_descriptor->text_char, buf, copy_length);
			pcomponent_descriptor->text_char[copy_length] = '\0';
		}

		rtcode = SECTION_PARSE_NO_ERROR;
	}

	return rtcode;
}

//功能：解马赛克描述子					SI_MOSAIC_DESCRIPTOR  = 0x51
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_mosaic_descriptor(U8* buf, S32 length, pmosaic_descriptor_t pmosaic_descriptor)
{
	S32 loop_length;
	S32	N = 0;
	S32 i;

	if ((buf != NULL) &&
		(length >= 3) &&
		(pmosaic_descriptor != NULL))
	{
		pmosaic_descriptor->descriptor_tag = *buf++;
		pmosaic_descriptor->descriptor_length = *buf++;

		pmosaic_descriptor->mosaic_entry_point = (*buf & 0x80) >> 7;
		pmosaic_descriptor->number_of_horizontal_elementary_cells = (*buf & 0x70) >> 4;
		pmosaic_descriptor->reserved_future_use0 = (*buf & 0x08) >> 3;
		pmosaic_descriptor->number_of_vertical_elementary_cells = (*buf++ & 0x07);

		loop_length = pmosaic_descriptor->descriptor_length - 1;

		while (loop_length > 0)
		{
			pmosaic_descriptor->logical_cell_id[N] = (*buf & 0xFC) >> 2;

			pmosaic_descriptor->reserved_future_use1[N] = (*buf++ & 0x03);
			pmosaic_descriptor->reserved_future_use1[N] <<= 5;
			pmosaic_descriptor->reserved_future_use1[N] |= (*buf & 0xF8) >> 3;
			pmosaic_descriptor->logical_cell_presentation_info[N] = (*buf++ & 0x07);

			pmosaic_descriptor->elementary_cell_field_length[N] = *buf++;

			for (i = 0; i < pmosaic_descriptor->elementary_cell_field_length[N]; i++)
			{
				pmosaic_descriptor->elementary_cell_id[N][i] = *buf++ & 0x3F;
			}

			pmosaic_descriptor->cell_linkage_info[N] = *buf++;

			if (pmosaic_descriptor->cell_linkage_info[N] == 0x01)
			{
				pmosaic_descriptor->bouquet_id[N] = *buf++ << 8;
				pmosaic_descriptor->bouquet_id[N] |= *buf++;

				loop_length -= pmosaic_descriptor->elementary_cell_field_length[N] + 6;
			}
			if (pmosaic_descriptor->cell_linkage_info[N] == 0x02)
			{
				pmosaic_descriptor->original_network_id[N] = *buf++ << 8;
				pmosaic_descriptor->original_network_id[N] |= *buf++;

				pmosaic_descriptor->transport_stream_id[N] = *buf++ << 8;
				pmosaic_descriptor->transport_stream_id[N] |= *buf++;

				pmosaic_descriptor->service_id[N] = *buf++ << 8;
				pmosaic_descriptor->service_id[N] |= *buf++;

				loop_length -= pmosaic_descriptor->elementary_cell_field_length[N] + 10;
			}
			if (pmosaic_descriptor->cell_linkage_info[N] == 0x03)
			{
				pmosaic_descriptor->original_network_id[N] = *buf++ << 8;
				pmosaic_descriptor->original_network_id[N] |= *buf++;

				pmosaic_descriptor->transport_stream_id[N] = *buf++ << 8;
				pmosaic_descriptor->transport_stream_id[N] |= *buf++;

				pmosaic_descriptor->service_id[N] = *buf++ << 8;
				pmosaic_descriptor->service_id[N] |= *buf++;

				loop_length -= pmosaic_descriptor->elementary_cell_field_length[N] + 10;
			}
			if (pmosaic_descriptor->cell_linkage_info[N] == 0x04)
			{
				pmosaic_descriptor->original_network_id[N] = *buf++ << 8;
				pmosaic_descriptor->original_network_id[N] |= *buf++;

				pmosaic_descriptor->transport_stream_id[N] = *buf++ << 8;
				pmosaic_descriptor->transport_stream_id[N] |= *buf++;

				pmosaic_descriptor->service_id[N] = *buf++ << 8;
				pmosaic_descriptor->service_id[N] |= *buf++;

				pmosaic_descriptor->event_id[N] = *buf++ << 8;
				pmosaic_descriptor->event_id[N] |= *buf++;

				loop_length -= pmosaic_descriptor->elementary_cell_field_length[N] + 12;
			}
			else
			{
				loop_length -= pmosaic_descriptor->elementary_cell_field_length[N] + 4;
			}

			N++;
		}

		pmosaic_descriptor->N = N;
	}

	return 0;
}

//功能：解流识别描述子					0x52
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin；
S32 DVB_SI_decode_stream_identifier_descriptor(U8* buf, S32 length, pstream_identifier_descriptor_t pstream_identifier_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pstream_identifier_descriptor != NULL))
	{
		pstream_identifier_descriptor->descriptor_tag = *buf++;
		pstream_identifier_descriptor->descriptor_length = *buf++;
		pstream_identifier_descriptor->component_tag = *buf++;
	}

	return 0;
}

//功能：解CA标识描述子				0x53
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_CA_identifier_descriptor(U8* buf, S32 length, pCA_identifier_descriptor_t pCA_identifier_descriptor)
{
	S32 i;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pCA_identifier_descriptor != NULL))
	{
		pCA_identifier_descriptor->descriptor_tag = *buf++; 
		pCA_identifier_descriptor->descriptor_length = *buf++;

		pCA_identifier_descriptor->N = pCA_identifier_descriptor->descriptor_length / 2;

		for (i = 0; i < pCA_identifier_descriptor->N; i++)
		{
			pCA_identifier_descriptor->CA_system_id[i] = *buf++ << 8;
			pCA_identifier_descriptor->CA_system_id[i] |= *buf++;
		}
	}

	return 0;
}

//功能：解内容描述子				0x54
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_content_descriptor(U8* buf, S32 length, pcontent_descriptor_t pcontent_descriptor)
{
	S32 N = 0;
	S32 loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pcontent_descriptor != NULL))
	{
		pcontent_descriptor->descriptor_tag = *buf++; 
		pcontent_descriptor->descriptor_length = *buf++;
	    
		loop_length = pcontent_descriptor->descriptor_length;

		while (loop_length >= 2)
		{
			if (N < MAX_NIBBLES)
			{
				pcontent_descriptor->content_nibble_level_1[N] = (*buf & 0xF0) >> 4;
				pcontent_descriptor->content_nibble_level_2[N] = *buf++ & 0x0F;
				pcontent_descriptor->user_nibble_1[N] = (*buf & 0xF0) >> 4;
				pcontent_descriptor->user_nibble_2[N] = *buf++ & 0x0F;

				N ++;
			}
			else
			{
				break;
			}

			loop_length -= 2;
		}

		pcontent_descriptor->N = N;
	}

	return 0;
}

//功能：解家长级别描述子				0x55
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_parental_rating_descriptor(U8* buf, S32 length, pparental_rating_descriptor_t pparental_rating_descriptor)
{	
	S32 rtcode = SECTION_PARSE_UNKNOWN_ERROR;
	S32 N = 0;
	S32 loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pparental_rating_descriptor != NULL))
	{
		pparental_rating_descriptor->descriptor_tag = *buf++;
		pparental_rating_descriptor->descriptor_length = *buf++;

		loop_length = pparental_rating_descriptor->descriptor_length;

		while (loop_length >= 4)
		{
			if (N < MAX_COUNTRIES)
			{
				pparental_rating_descriptor->country_code[N] = buf[0];
				pparental_rating_descriptor->country_code[N] <<= 8;
				pparental_rating_descriptor->country_code[N] |= buf[1];
				pparental_rating_descriptor->country_code[N] <<= 8;
				pparental_rating_descriptor->country_code[N] |= buf[2];

				memcpy(pparental_rating_descriptor->country_code_char[N], buf, 3);
				buf += 3;
				pparental_rating_descriptor->country_code_char[N][3] = '\0';

				pparental_rating_descriptor->rating[N] = *buf++;
				N ++;
			}
			else
			{
				break;
			}

			loop_length -= 4;
		}

		pparental_rating_descriptor->N = N;

		rtcode = SECTION_PARSE_NO_ERROR;
	}

	return rtcode;
}

//功能：解图文描述子						0x56
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_teletext_descriptor(uint8_t* buf, int length, teletext_descriptor_t* pteletext_descriptor)
{
	int rtcode = SECTION_PARSE_UNKNOWN_ERROR;
	int N = 0;
	int loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pteletext_descriptor != NULL))
	{
		pteletext_descriptor->descriptor_tag = *buf++;
		pteletext_descriptor->descriptor_length = *buf++;

		loop_length = pteletext_descriptor->descriptor_length;

		while ((loop_length >= 5) && (N < MAX_LANGUAGES))
		{
			pteletext_descriptor->description_loop[N].ISO_639_language_code = buf[0];
			pteletext_descriptor->description_loop[N].ISO_639_language_code <<= 8;
			pteletext_descriptor->description_loop[N].ISO_639_language_code |= buf[1];
			pteletext_descriptor->description_loop[N].ISO_639_language_code <<= 8;
			pteletext_descriptor->description_loop[N].ISO_639_language_code |= buf[2];

			memcpy(pteletext_descriptor->description_loop[N].ISO_639_language_code_char, buf, 3);
			pteletext_descriptor->description_loop[N].ISO_639_language_code_char[3] = '\0';
			buf += 3;

			pteletext_descriptor->description_loop[N].teletext_type = (*buf & 0xF8) >> 3;
			pteletext_descriptor->description_loop[N].teletext_magazine_number = *buf++ & 0x07;

			pteletext_descriptor->description_loop[N].teletext_page_number = *buf++;

			N ++;
			loop_length -= 5;
		}

		pteletext_descriptor->N = N;

		rtcode = SECTION_PARSE_NO_ERROR;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解电话描述子					0x57
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_telephone_descriptor(U8* buf, S32 length, ptelephone_descriptor_t ptelephone_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(ptelephone_descriptor != NULL))
	{
		ptelephone_descriptor->descriptor_tag = *buf++;
		ptelephone_descriptor->descriptor_length = *buf++;
		
		ptelephone_descriptor->reserved_future_use0 = (*buf & 0xC0) >> 6;
		ptelephone_descriptor->foreign_availability = (*buf & 0x20) >> 5;
		ptelephone_descriptor->connection_type = *buf++ & 0x1F;

		ptelephone_descriptor->reserved_future_use1 = (*buf & 0x80) >> 7;
		ptelephone_descriptor->country_prefix_length = (*buf & 0x60) >> 5;
		ptelephone_descriptor->international_area_code_length = (*buf & 0x1C) >> 2;
		ptelephone_descriptor->operator_code_length = *buf++ & 0x03;

		ptelephone_descriptor->reserved_future_use2 = (*buf & 0x80) >> 7;
		ptelephone_descriptor->national_area_code_length = (*buf & 0x70) >> 4;
		ptelephone_descriptor->core_number_length = *buf++ & 0x0F;

		memcpy(ptelephone_descriptor->country_prefix_char, buf, ptelephone_descriptor->country_prefix_length);
		ptelephone_descriptor->country_prefix_char[ptelephone_descriptor->country_prefix_length] = '\0';
		buf += ptelephone_descriptor->country_prefix_length;

		memcpy(ptelephone_descriptor->international_area_code_char, buf, ptelephone_descriptor->country_prefix_length);
		ptelephone_descriptor->international_area_code_char[ptelephone_descriptor->country_prefix_length] = '\0';
		buf += ptelephone_descriptor->country_prefix_length;
		
		memcpy(ptelephone_descriptor->operator_code_char, buf, ptelephone_descriptor->country_prefix_length);
		ptelephone_descriptor->operator_code_char[ptelephone_descriptor->country_prefix_length] = '\0';
		buf += ptelephone_descriptor->country_prefix_length;

		memcpy(ptelephone_descriptor->national_area_code_char, buf, ptelephone_descriptor->country_prefix_length);
		ptelephone_descriptor->national_area_code_char[ptelephone_descriptor->country_prefix_length] = '\0';
		buf += ptelephone_descriptor->country_prefix_length;

		memcpy(ptelephone_descriptor->core_number_char, buf, ptelephone_descriptor->country_prefix_length);
		ptelephone_descriptor->core_number_char[ptelephone_descriptor->country_prefix_length] = '\0';
		buf += ptelephone_descriptor->country_prefix_length;
	}

	return 0;
}

//功能：解当地时间偏移描述子			0x58
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_local_time_offset_descriptor(uint8_t* buf, int length, local_time_offset_descriptor_t* plocal_time_offset_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	S32 N = 0;
	S32 loop_length;
	U8*		ptemp;
	char   pszCountry[4];

	if ((buf != NULL) &&
		(length >= 2) &&
		(plocal_time_offset_descriptor != NULL))
	{
		memset(plocal_time_offset_descriptor, 0x00, sizeof(local_time_offset_descriptor_t));

		plocal_time_offset_descriptor->descriptor_tag = *buf++;
		plocal_time_offset_descriptor->descriptor_length = *buf++;

		N = 0;
		if (plocal_time_offset_descriptor->descriptor_length > 0)
		{
			ptemp = buf;
			buf += plocal_time_offset_descriptor->descriptor_length;

			loop_length = plocal_time_offset_descriptor->descriptor_length;
			while ((loop_length >= 13) && (N < MAX_LOCAL_TIME_AREA))
			{
				memcpy(pszCountry, ptemp, 3);
				pszCountry[3] = '\0';

				unsigned int country_code = *ptemp++;
				country_code <<= 8;
				country_code |= *ptemp++;
				country_code <<= 8;
				country_code |= *ptemp++;
				plocal_time_offset_descriptor->local_time_offset[N].country_code = country_code;
				plocal_time_offset_descriptor->local_time_offset[N].country_region_id = (*ptemp & 0xFC) >> 2;
				plocal_time_offset_descriptor->local_time_offset[N].reserved = (*ptemp & 0x02) >> 1;
				plocal_time_offset_descriptor->local_time_offset[N].local_time_offset_polarity = (*ptemp++ & 0x01);
				plocal_time_offset_descriptor->local_time_offset[N].local_time_offset = *ptemp++ << 8;
				plocal_time_offset_descriptor->local_time_offset[N].local_time_offset |= *ptemp++;

				int64_t time_of_change = *ptemp++;
				time_of_change <<= 8;
				time_of_change |= *ptemp++;
				time_of_change <<= 8;
				time_of_change |= *ptemp++;
				time_of_change <<= 8;
				time_of_change |= *ptemp++;
				time_of_change <<= 8;
				time_of_change |= *ptemp++;
				plocal_time_offset_descriptor->local_time_offset[N].time_of_change = time_of_change;

				plocal_time_offset_descriptor->local_time_offset[N].next_time_offset = *ptemp++ << 8;
				plocal_time_offset_descriptor->local_time_offset[N].next_time_offset |= *ptemp++;

				loop_length -= 13;
				N++;
			}
		}
		plocal_time_offset_descriptor->N = N;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解字幕描述子						0x59
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_subtitling_descriptor(uint8_t* buf, int length, subtitling_descriptor_t* psubtitling_descriptor)
{
	S32 N = 0;
	S32 loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(psubtitling_descriptor != NULL))
	{
		psubtitling_descriptor->descriptor_tag = *buf++;
		psubtitling_descriptor->descriptor_length = *buf++;

		loop_length = psubtitling_descriptor->descriptor_length;

		while (loop_length > 0)
		{
			psubtitling_descriptor->description_loop[N].ISO_639_language_code = buf[0];
			psubtitling_descriptor->description_loop[N].ISO_639_language_code <<= 8;
			psubtitling_descriptor->description_loop[N].ISO_639_language_code |= buf[1];
			psubtitling_descriptor->description_loop[N].ISO_639_language_code <<= 8;
			psubtitling_descriptor->description_loop[N].ISO_639_language_code |= buf[2];

			memcpy(psubtitling_descriptor->description_loop[N].ISO_639_language_code_char, buf, 3);
			psubtitling_descriptor->description_loop[N].ISO_639_language_code_char[3] = '\0';
			buf += 3;

			psubtitling_descriptor->description_loop[N].subtitling_type = *buf++;

			psubtitling_descriptor->description_loop[N].composition_page_id = (*buf++ << 8);
			psubtitling_descriptor->description_loop[N].composition_page_id |= *buf++;

			psubtitling_descriptor->description_loop[N].ancillary_page_id = (*buf++ << 8);
			psubtitling_descriptor->description_loop[N].ancillary_page_id |= *buf++;

			loop_length -= 8;
			N ++;
		}

		psubtitling_descriptor->N = N;
	}

	return 0;
}

//功能：解地面传送系统描述子				0x5A
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_terrestrial_delivery_system_descriptor(U8* buf, S32 length, pterrestrial_delivery_system_descriptor_t pterrestrial_delivery_system_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pterrestrial_delivery_system_descriptor != NULL))
	{
		pterrestrial_delivery_system_descriptor->descriptor_tag = *buf++;
		pterrestrial_delivery_system_descriptor->descriptor_length = *buf++;

		pterrestrial_delivery_system_descriptor->centre_frequency = *buf++;
		pterrestrial_delivery_system_descriptor->centre_frequency <<= 8;
		pterrestrial_delivery_system_descriptor->centre_frequency |= *buf++;
		pterrestrial_delivery_system_descriptor->centre_frequency <<= 8;
		pterrestrial_delivery_system_descriptor->centre_frequency |= *buf++;
		pterrestrial_delivery_system_descriptor->centre_frequency <<= 8;
		pterrestrial_delivery_system_descriptor->centre_frequency |= *buf++;				//x10 Hz

		pterrestrial_delivery_system_descriptor->bandwidth = (*buf & 0xE0) >> 5;			//000 -- 8MHz
																							//001 -- 7MHz
																							//010 ~ 111 reserved for future use
		pterrestrial_delivery_system_descriptor->priority = ((*buf & 0x10) >> 4);
		pterrestrial_delivery_system_descriptor->time_slicing_indicator = ((*buf & 0x08) >> 3);
		pterrestrial_delivery_system_descriptor->MPE_FEC_indecator = ((*buf & 0x04) >> 2);
		pterrestrial_delivery_system_descriptor->reserved_future_use0 = (*buf++ & 0x03);

		pterrestrial_delivery_system_descriptor->constellation = (*buf & 0xC0) >> 6;		//00 -- QPSK
																							//01 -- 16-QAM
																							//10 -- 64-QAM
																							//11 -- reserved for future use

		pterrestrial_delivery_system_descriptor->hierarchy_information = (*buf & 0x38) >> 3;	//000 -- non-hierarchical
																								//001 -- alpha = 1
																								//010 -- alpha = 2
																								//011 -- alpha = 4
																								//100 ~ 111 reserved for future use

		pterrestrial_delivery_system_descriptor->code_rate_HP_stream = *buf++ & 0x07;			//000 -- 1/2
																								//001 -- 2/3
																								//010 -- 3/4
																								//011 -- 5/6
																								//100 -- 7/8
																								//101 ~ 111 reserved for future use

		pterrestrial_delivery_system_descriptor->code_rate_LP_stream = (*buf & 0xE0) >> 5;

		pterrestrial_delivery_system_descriptor->guard_interval = (*buf & 0x18) >> 3;			//00 -- 1/32
																								//01 -- 1/16
																								//10 -- 1/8
																								//11 -- 1/4

		pterrestrial_delivery_system_descriptor->transmission_mode = (*buf & 0x06) >> 1;		//OFDM carriers
																								//00 -- 2k mode
																								//01 -- 8k mode
																								//10 -- xx mode
																								//11 -- xx mode

		pterrestrial_delivery_system_descriptor->other_frequency_flag = *buf++ & 0x01;			//0 - on other frequency in use
																								//1 - one or more other frequencies in use
	}

	return 0;
}

//功能：解多语言网络名称描述子				0x5B
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_multilingual_network_name_descriptor(U8* buf, S32 length, pmultilingual_network_name_descriptor_t pmultilingual_network_name_descriptor)
{
	S32 rtcode = SECTION_PARSE_UNKNOWN_ERROR;
	S32 N = 0;
	S32 loop_length;
	S32 copy_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pmultilingual_network_name_descriptor != NULL))
	{
		pmultilingual_network_name_descriptor->descriptor_tag = *buf++;
		pmultilingual_network_name_descriptor->descriptor_length = *buf++;

		loop_length = pmultilingual_network_name_descriptor->descriptor_length;
		while (loop_length >= 4)
		{
			if (N < MAX_LANGUAGES)
			{
				pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code = buf[0];
				pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code <<= 8;
				pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code |= buf[1];
				pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code <<= 8;
				pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code |= buf[2];

				memcpy(pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, buf, 3);
				pmultilingual_network_name_descriptor->LANGUAGE[N].ISO_639_language_code_char[3] = '\0';
				buf += 3;

				pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length = *buf++;

				copy_length = min(pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length, MAX_NETWORK_NAME_LENGTH);

				memcpy(pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char, buf, copy_length);
				pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char[copy_length] = '\0';

				pmultilingual_network_name_descriptor->LANGUAGE[N].trimmed_network_name_char = DVB_SI_StringPrefixTrim(pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_char);

				buf += pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length;

				N ++;
			}
			else
			{
				break;
			}
			loop_length -= (4 + pmultilingual_network_name_descriptor->LANGUAGE[N].network_name_length);
		}

		pmultilingual_network_name_descriptor->N = N;

		rtcode = SECTION_PARSE_NO_ERROR;
	}

	return rtcode;
}

//功能：解多语言业务群名称描述子			0x5C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_multilingual_bouquet_name_descriptor(U8* buf, S32 length, pmultilingual_bouquet_name_descriptor_t pmultilingual_bouquet_name_descriptor)
{
	S32 N = 0;
	S32 loop_length;
	S32 copy_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pmultilingual_bouquet_name_descriptor != NULL))
	{
		pmultilingual_bouquet_name_descriptor->descriptor_tag = *buf++;
		pmultilingual_bouquet_name_descriptor->descriptor_length = *buf++;

		loop_length = pmultilingual_bouquet_name_descriptor->descriptor_length;
		while (loop_length >= 4)
		{
			if (N < MAX_LANGUAGES)
			{
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code = buf[0];
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code <<= 8;
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code |= buf[1];
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code <<= 8;
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code |= buf[2];

				memcpy(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, buf, 3);
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].ISO_639_language_code_char[3] = '\0';
				buf += 3;

				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length = *buf++;
				copy_length = min(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length, MAX_BOUQUET_NAME_LENGTH);

				memcpy(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char, buf, copy_length);
				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char[copy_length] = '\0';

				pmultilingual_bouquet_name_descriptor->LANGUAGE[N].trimmed_bouquet_name_char = DVB_SI_StringPrefixTrim(pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_char);

				buf += pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length;

				N ++;
			}
			else
			{
				break;
			}
			loop_length -= (4 + pmultilingual_bouquet_name_descriptor->LANGUAGE[N].bouquet_name_length);
		}

		pmultilingual_bouquet_name_descriptor->N = N;
	}

	return 0;
}

//功能：解多语言业务名称描述子				0x5D
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_multilingual_service_name_descriptor(U8* buf, S32 length, pmultilingual_service_name_descriptor_t pmultilingual_service_name_descriptor)
{
	S32 N = 0;
	S32 loop_length;
	S32	copy_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pmultilingual_service_name_descriptor != NULL))
	{
		pmultilingual_service_name_descriptor->descriptor_tag = *buf++;
		pmultilingual_service_name_descriptor->descriptor_length = *buf++;

		loop_length = pmultilingual_service_name_descriptor->descriptor_length;
		while (loop_length >= 4)
		{
			if (N < MAX_LANGUAGES)
			{
				pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code = buf[0];
				pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code <<= 8;
				pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code |= buf[1];
				pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code <<= 8;
				pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code |= buf[2];

				memcpy(pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code_char, buf, 3);
				pmultilingual_service_name_descriptor->LANGUAGE[N].ISO_639_language_code_char[3] = '\0';
				buf += 3;

				pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length = *buf++;
				copy_length = min(pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length, MAX_SERVICE_PROVIDER_NAME_LENGTH);

				memcpy(pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char, buf, copy_length);
				pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char[copy_length] = '\0';
				pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_provider_name_char = DVB_SI_StringPrefixTrim(pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_char);
				buf += pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length;

				pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length = *buf++;
				copy_length = min(pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length, MAX_SERVICE_NAME_LENGTH);

				memcpy(pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char, buf, copy_length);
				pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char[copy_length] = '\0';
				pmultilingual_service_name_descriptor->LANGUAGE[N].trimmed_service_name_char = DVB_SI_StringPrefixTrim(pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_char);
				buf += pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length;

				N ++;
			}
			else
			{
				break;
			}

			loop_length -= 5 + pmultilingual_service_name_descriptor->LANGUAGE[N].service_name_length + pmultilingual_service_name_descriptor->LANGUAGE[N].service_provider_name_length;
		}

		pmultilingual_service_name_descriptor->N = N;
	}

	return 0;
}


//功能：解多语言组成描述子					0x5E
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_multilingual_component_descriptor(U8* buf, S32 length, pmultilingual_component_descriptor_t pmultilingual_component_descriptor)
{
	S32	copy_length;
	S32 loop_length;
	S32 N = 0;

	if ((buf != NULL) && (length >= 2) && (pmultilingual_component_descriptor != NULL))
	{
		pmultilingual_component_descriptor->descriptor_tag = *buf++;
		pmultilingual_component_descriptor->descriptor_length = *buf++;

		pmultilingual_component_descriptor->component_tag = *buf++;

		loop_length = pmultilingual_component_descriptor->descriptor_length - 1;

		while (loop_length > 0)
		{
			if (N < MAX_LANGUAGES)
			{
				pmultilingual_component_descriptor->ISO_639_language_code[N] = buf[0];
				pmultilingual_component_descriptor->ISO_639_language_code[N] <<= 8;
				pmultilingual_component_descriptor->ISO_639_language_code[N] |= buf[1];
				pmultilingual_component_descriptor->ISO_639_language_code[N] <<= 8;
				pmultilingual_component_descriptor->ISO_639_language_code[N] |= buf[2];

				memcpy(pmultilingual_component_descriptor->ISO_639_language_code_char[N], buf, 3);
				pmultilingual_component_descriptor->ISO_639_language_code_char[N][3] = '\0';
				buf += 3;

				pmultilingual_component_descriptor->text_description_length[N] = *buf++;

				copy_length = (pmultilingual_component_descriptor->text_description_length[N] < MAX_COMPONENT_TEXT_LENGTH) ? 
										pmultilingual_component_descriptor->text_description_length[N] : MAX_COMPONENT_TEXT_LENGTH;

				memcpy(pmultilingual_component_descriptor->text_char[N], buf, copy_length);
				pmultilingual_component_descriptor->text_char[N][copy_length] = '\0';
				buf += pmultilingual_component_descriptor->text_description_length[N];

				N ++;
			}
			else
			{
				break;
			}

			loop_length -= 4 + pmultilingual_component_descriptor->text_description_length[N];
		}

		pmultilingual_component_descriptor->N = N;
	}

	return 0;
}

//SI_PRIVATE_DATA_SPECIFIER_DESCRIPTOR			0x5F
//功能：解私有数据说明描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_private_data_specifier_descriptor(uint8_t* buf, int length, private_data_specifier_descriptor_t* pprivate_data_specifier_descriptor)
{
	if ((buf != NULL) &&
		(length >= 6) &&
		(pprivate_data_specifier_descriptor != NULL))
	{
		pprivate_data_specifier_descriptor->descriptor_tag = *buf++;				//0x5F
		pprivate_data_specifier_descriptor->descriptor_length = *buf++;

		pprivate_data_specifier_descriptor->private_data_specifier = *buf++;
		pprivate_data_specifier_descriptor->private_data_specifier <<= 8;

		pprivate_data_specifier_descriptor->private_data_specifier |= *buf++;
		pprivate_data_specifier_descriptor->private_data_specifier <<= 8;

		pprivate_data_specifier_descriptor->private_data_specifier |= *buf++;
		pprivate_data_specifier_descriptor->private_data_specifier <<= 8;

		pprivate_data_specifier_descriptor->private_data_specifier |= *buf++;
	}

	return 0;
}

//功能：解业务迁移描述子				0x60
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_service_move_descriptor(U8* buf, S32 length, pservice_move_descriptor_t pservice_move_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pservice_move_descriptor != NULL))
	{
		pservice_move_descriptor->descriptor_tag = *buf++;
		pservice_move_descriptor->descriptor_length = *buf++;
		pservice_move_descriptor->new_original_network_id = *buf++ << 8;
		pservice_move_descriptor->new_original_network_id |= *buf++;

		pservice_move_descriptor->new_transport_stream_id = *buf++ << 8;
		pservice_move_descriptor->new_transport_stream_id |= *buf++;

		pservice_move_descriptor->new_service_id = *buf++ << 8;
		pservice_move_descriptor->new_service_id |= *buf++;
	}

	return 0;
}

//功能：解短平滑缓冲器描述子				0x61
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_short_smoothing_buffer_descriptor(U8* buf, S32 length, pshort_smoothing_buffer_descriptor_t pshort_smoothing_buffer_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pshort_smoothing_buffer_descriptor != NULL))
	{
		pshort_smoothing_buffer_descriptor->descriptor_tag = *buf++;
		pshort_smoothing_buffer_descriptor->descriptor_length = *buf++;

		pshort_smoothing_buffer_descriptor->sb_size = (*buf & 0xC0) >> 6;
		pshort_smoothing_buffer_descriptor->sb_leak_rate = *buf++ & 0x3F;
	}

	return 0;
}

//功能：解频率列表描述子						0x62
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_frequency_list_descriptor(U8* buf, S32 length, pfrequency_list_descriptor_t pfrequency_list_descriptor)
{
	S32 N = 0;
	S32 loop_length = 0;

	if ((buf != NULL) &&
		(length >= 3) &&
		(pfrequency_list_descriptor != NULL))
	{
		pfrequency_list_descriptor->descriptor_tag = *buf++;
		pfrequency_list_descriptor->descriptor_length = *buf++;

		pfrequency_list_descriptor->reserved_future_use = (*buf & 0xFC) >> 2;
		pfrequency_list_descriptor->coding_type = *buf++ & 0x03;

		while (loop_length > 0)
		{
			pfrequency_list_descriptor->centre_frequency[N] = *buf++ << 24;
			pfrequency_list_descriptor->centre_frequency[N] |= *buf++ << 16;
			pfrequency_list_descriptor->centre_frequency[N] |= *buf++ << 8;
			pfrequency_list_descriptor->centre_frequency[N] |= *buf++;

			loop_length -= 4;
			N ++;
		}

		pfrequency_list_descriptor->N = N;
	}

	return 0;
}

//功能：解部分传送流描述子						0x63
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
S32 DVB_SI_decode_partial_transport_stream_descriptor(U8* buf, S32 length, ppartial_transport_stream_descriptor_t ppartial_transport_stream_descriptor)
{
	if ((buf != NULL) &&
		(length >= 8) &&
		(ppartial_transport_stream_descriptor != NULL))
	{
		ppartial_transport_stream_descriptor->descriptor_tag = *buf++;
		ppartial_transport_stream_descriptor->descriptor_length = *buf++;

		ppartial_transport_stream_descriptor->DVB_reserved_future_use0 = (*buf & 0xc0) >> 6;
		ppartial_transport_stream_descriptor->peak_rate = (*buf++ & 0x3F) << 16;
		ppartial_transport_stream_descriptor->peak_rate |= *buf++ << 8;
		ppartial_transport_stream_descriptor->peak_rate |= *buf++;

		ppartial_transport_stream_descriptor->DVB_reserved_future_use1 = (*buf & 0xc0) >> 6;
		ppartial_transport_stream_descriptor->minimum_overall_smoothing_rate = (*buf++ & 0x3F) << 16;
		ppartial_transport_stream_descriptor->minimum_overall_smoothing_rate |= *buf++ << 8;
		ppartial_transport_stream_descriptor->minimum_overall_smoothing_rate |= *buf++;

		ppartial_transport_stream_descriptor->DVB_reserved_future_use2 = (*buf & 0xc0) >> 6;
		ppartial_transport_stream_descriptor->maximum_overall_smoothing_buffer = (*buf++ & 0x3F) << 8;
		ppartial_transport_stream_descriptor->maximum_overall_smoothing_buffer |= *buf++;
	}

	return 0;
}

//功能：解数据广播描述子				0x64
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_data_broadcast_descriptor(uint8_t* buf, int length, data_broadcast_descriptor_t* pdata_broadcast_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;
	int	copy_length;
	
	if ((buf != NULL) && (length >= 10) && (pdata_broadcast_descriptor != NULL))
	{
		memset(pdata_broadcast_descriptor, 0x00, sizeof(data_broadcast_descriptor_t));

		pdata_broadcast_descriptor->descriptor_tag = *buf++;
		pdata_broadcast_descriptor->descriptor_length = *buf++;

		pdata_broadcast_descriptor->data_broadcast_id = *buf++ << 8;
		pdata_broadcast_descriptor->data_broadcast_id |= *buf++;

		pdata_broadcast_descriptor->component_tag = *buf++;
		pdata_broadcast_descriptor->selector_length = *buf++;

		if (pdata_broadcast_descriptor->data_broadcast_id == 0x0005)
		{
			DVB_Databroadcast_decode_multiprotocol_encapsulation_info(buf, pdata_broadcast_descriptor->selector_length, &(pdata_broadcast_descriptor->selector_byte.multiprotocol_encapsulation_info));
		}
		else if (pdata_broadcast_descriptor->data_broadcast_id == 0x0006)
		{
			DVB_Databroadcast_decode_data_carousel_info(buf, pdata_broadcast_descriptor->selector_length, &(pdata_broadcast_descriptor->selector_byte.data_carousel_info));
		}
		else if (pdata_broadcast_descriptor->data_broadcast_id == 0x0007)
		{
			DVB_Databroadcast_decode_object_carousel_info(buf, pdata_broadcast_descriptor->selector_length, &(pdata_broadcast_descriptor->selector_byte.object_carousel_info));
		}
		else if (pdata_broadcast_descriptor->data_broadcast_id == 0x000B)
		{
			INT_decode_IP_MAC_notification_info(buf, pdata_broadcast_descriptor->selector_length, &(pdata_broadcast_descriptor->selector_byte.IP_MAC_notification_info));
		}
		else
		{
			copy_length = (pdata_broadcast_descriptor->selector_length < MAX_DATABROADCAST_SELECT_LENGTH) ? 
										pdata_broadcast_descriptor->selector_length : MAX_DATABROADCAST_SELECT_LENGTH;
			memcpy(pdata_broadcast_descriptor->selector_byte.buf, buf, copy_length);
		}

		buf += pdata_broadcast_descriptor->selector_length;

		pdata_broadcast_descriptor->ISO_639_language_code = buf[0];
		pdata_broadcast_descriptor->ISO_639_language_code <<= 8;
		pdata_broadcast_descriptor->ISO_639_language_code |= buf[1];
		pdata_broadcast_descriptor->ISO_639_language_code <<= 8;
		pdata_broadcast_descriptor->ISO_639_language_code |= buf[2];

		memcpy(pdata_broadcast_descriptor->ISO_639_language_code_char, buf, 3);
		pdata_broadcast_descriptor->ISO_639_language_code_char[3] = '\0';
		buf += 3;

		pdata_broadcast_descriptor->text_length = *buf++;
		copy_length = (pdata_broadcast_descriptor->text_length < MAX_DATABROADCAST_TEXT_LENGTH) ? pdata_broadcast_descriptor->text_length : MAX_DATABROADCAST_TEXT_LENGTH;
		memcpy(pdata_broadcast_descriptor->text_char, buf, copy_length);
		pdata_broadcast_descriptor->text_char[copy_length] = '\0';

	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解加扰描述子					0x65
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//S32 SI_decode_CA_system_descriptor(U8* buf, S32 length, pCA_system_descriptor_t pCA_system_descriptor)
S32 DVB_SI_decode_scrambling_descriptor(U8* buf, S32 length, pscrambling_descriptor_t pscrambling_descriptor)
{
	S32	rtcode = 0;

	if ((buf != NULL) && 
		(length >= 3) && 
		(pscrambling_descriptor != NULL))
	{
		pscrambling_descriptor->descriptor_tag = *buf++;
		pscrambling_descriptor->descriptor_length = *buf++;
		pscrambling_descriptor->scrambling_mode = *buf++;
	}
	else
	{
		rtcode = 1;
	}

	return rtcode;
}

//功能：解数据广播标识描述子				0x66
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_data_broadcast_id_descriptor(uint8_t* buf, int length, data_broadcast_id_descriptor_t* pdata_broadcast_id_descriptor)
{
	S32		copy_length;

	if ((buf != NULL) && (length >= 2) && (pdata_broadcast_id_descriptor != NULL))
	{
		pdata_broadcast_id_descriptor->descriptor_tag = *buf++;
		pdata_broadcast_id_descriptor->descriptor_length = *buf++;

		pdata_broadcast_id_descriptor->data_broadcast_id = *buf++;
		pdata_broadcast_id_descriptor->data_broadcast_id <<= 8;
		pdata_broadcast_id_descriptor->data_broadcast_id |= *buf++;

		copy_length = pdata_broadcast_id_descriptor->descriptor_length - 2;

		pdata_broadcast_id_descriptor->id_selector_length = copy_length;

		if (copy_length > 0)
		{
			if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x0005)
			{
				DVB_Databroadcast_decode_multiprotocol_encapsulation_info(buf, copy_length, &(pdata_broadcast_id_descriptor->id_selector_byte.multiprotocol_encapsulation_info));
			}
			else if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x0006)
			{
				DVB_Databroadcast_decode_data_carousel_info(buf, copy_length, &(pdata_broadcast_id_descriptor->id_selector_byte.data_carousel_info));
			}
			else if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x0007)
			{
				DVB_Databroadcast_decode_object_carousel_info(buf, copy_length, &(pdata_broadcast_id_descriptor->id_selector_byte.object_carousel_info));
			}
			else if (pdata_broadcast_id_descriptor->data_broadcast_id == 0x000B)
			{
				INT_decode_IP_MAC_notification_info(buf, copy_length, &(pdata_broadcast_id_descriptor->id_selector_byte.IP_MAC_notification_info));
			}
			else
			{
				copy_length = (copy_length < MAX_DATABROADCAST_SELECT_LENGTH) ? copy_length : MAX_DATABROADCAST_SELECT_LENGTH;
				memcpy(pdata_broadcast_id_descriptor->id_selector_byte.buf, buf, copy_length);
				pdata_broadcast_id_descriptor->id_selector_length = copy_length;
			}
		}
	}

	return 0;
}

//功能：解传送流描述符					0x67
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int DVB_SI_decode_transport_stream_descriptor(uint8_t* buf, int length, transport_stream_descriptor_t* ptransport_stream_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length > 0) && (ptransport_stream_descriptor != NULL))
	{
		ptransport_stream_descriptor->descriptor_tag = *buf++;
		ptransport_stream_descriptor->descriptor_length = *buf++;

//		(ptransport_stream_descriptor->descriptor_length == 3);			//在DVB系统中，该值固定为3

		ptransport_stream_descriptor->byte_char[0] = *buf++;
		ptransport_stream_descriptor->byte_char[1] = *buf++;
		ptransport_stream_descriptor->byte_char[2] = *buf++;
		ptransport_stream_descriptor->byte_char[3] = '\0';

		//ptransport_stream_descriptor->byte_code = ptransport_stream_descriptor->byte_char[0];
		//ptransport_stream_descriptor->byte_code <<= 8;
		//ptransport_stream_descriptor->byte_code |= ptransport_stream_descriptor->byte_char[1];
		//ptransport_stream_descriptor->byte_code <<= 8;
		//ptransport_stream_descriptor->byte_code |= ptransport_stream_descriptor->byte_char[2];
	}

	return rtcode;
}


//功能：解AC3描述子					0x6A
//输入：
//返回：
S32 DVB_SI_decode_ac3_descriptor(U8* buf, S32 length, pac3_descriptor_t pac3_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		porigin = buf;
	U8		N;
	U8		copy_length;

	if ((buf != NULL) &&
		(length >= 3) &&
		(pac3_descriptor != NULL))
	{
		pac3_descriptor->descriptor_tag = *buf++;
		pac3_descriptor->descriptor_length = *buf++;

		pac3_descriptor->component_type_flag = (*buf & 0x80) >> 7;
		pac3_descriptor->bsid_flag = (*buf & 0x40) >> 6;
		pac3_descriptor->mainid_flag = (*buf & 0x20) >> 5;
		pac3_descriptor->asvc_flag = (*buf & 0x10) >> 4;
		pac3_descriptor->reserved_flags = (*buf++ & 0x0f);

		if (pac3_descriptor->component_type_flag)
		{
			pac3_descriptor->component_type = *buf++;
		}
		if (pac3_descriptor->bsid_flag)
		{
			pac3_descriptor->bsid = *buf++;
		}
		if (pac3_descriptor->mainid_flag)
		{
			pac3_descriptor->mainid = *buf++;
		}
		if (pac3_descriptor->asvc_flag)
		{
			pac3_descriptor->asvc = *buf++;
		}

		N = length - (unsigned char)(buf - porigin);
		copy_length = min(N, 16);
		if (copy_length > 0)
		{
			memcpy(pac3_descriptor->additional_info, buf, copy_length);
			buf += N;
		}
		pac3_descriptor->N = N;
	}

	return rtcode;
}


//功能：解APPLICATION_SIGNALLING描述子			0x6F
//输入：
//返回：
S32 DVB_SI_decode_application_signalling_descriptor(U8* buf, S32 length, papplication_signalling_descriptor_t papplication_signalling_descriptor)
{
	S32		rtcode = 0;
	U8*		porigin = buf;
	U8		i;

	if ((buf != NULL) && (papplication_signalling_descriptor != NULL))
	{
		papplication_signalling_descriptor->descriptor_tag = *buf++;
		papplication_signalling_descriptor->descriptor_length = *buf++;

		papplication_signalling_descriptor->N = papplication_signalling_descriptor->descriptor_length / 3;

		for (i = 0; i < papplication_signalling_descriptor->N; i++)
		{
			papplication_signalling_descriptor->reserved_future_use0[i] = (*buf & 0x80) >> 7;

			papplication_signalling_descriptor->application_type[i] = (*buf++ & 0x7f);
			papplication_signalling_descriptor->application_type[i] <<= 8;
			papplication_signalling_descriptor->application_type[i] |= *buf++;

			papplication_signalling_descriptor->reserved_future_use1[i] = (*buf & 0xe0) >> 5;

			papplication_signalling_descriptor->AIT_version_number[i] = (*buf++ & 0x1f);
		}
	}
	else
	{
		rtcode = -1;
	}

	return rtcode;
}

//0x6C
S32 DVB_SI_decode_cell_list_descriptor(U8* buf, S32 length, pcell_list_descriptor_t pcell_list_descriptor)
{
	U8*		porigin = buf;
	U8*		pend;
	U8*		pnext;

	S8		cell_N = 0;
	S8		subcell_N = 0;

	if ((buf != NULL) && (pcell_list_descriptor != NULL))
	{
		pcell_list_descriptor->descriptor_tag = *buf++;
		pcell_list_descriptor->descriptor_length = *buf++;

		pend = buf + pcell_list_descriptor->descriptor_length;

		cell_N = 0;
		while (buf < pend)
		{
			pcell_list_descriptor->cell_id[cell_N] = *buf++;
			pcell_list_descriptor->cell_id[cell_N] <<= 8;
			pcell_list_descriptor->cell_id[cell_N] |= *buf++;

			pcell_list_descriptor->cell_latitude[cell_N] = *buf++;
			pcell_list_descriptor->cell_latitude[cell_N] <<= 8;
			pcell_list_descriptor->cell_latitude[cell_N] |= *buf++;

			pcell_list_descriptor->cell_longitude[cell_N] = *buf++;
			pcell_list_descriptor->cell_longitude[cell_N] <<= 8;
			pcell_list_descriptor->cell_longitude[cell_N] |= *buf++;

			pcell_list_descriptor->cell_extent_of_latitude[cell_N] = *buf++;
			pcell_list_descriptor->cell_extent_of_latitude[cell_N] <<= 4;
			pcell_list_descriptor->cell_extent_of_latitude[cell_N] |= (*buf & 0xf0) >> 4;

			pcell_list_descriptor->cell_extent_of_longitude[cell_N] = (*buf++ & 0x0f);
			pcell_list_descriptor->cell_extent_of_longitude[cell_N] <<= 8;
			pcell_list_descriptor->cell_extent_of_longitude[cell_N] |= *buf++;

			pcell_list_descriptor->subcell_info_loop_length[cell_N] = *buf++;

			pnext = buf + pcell_list_descriptor->subcell_info_loop_length[cell_N];

			subcell_N = 0;
			
			while (buf < pnext)
			{
				pcell_list_descriptor->cell_id_extension[cell_N][subcell_N] = *buf++;

				pcell_list_descriptor->subcell_latitude[cell_N][subcell_N] = *buf++;
				pcell_list_descriptor->subcell_latitude[cell_N][subcell_N] <<= 8;
				pcell_list_descriptor->subcell_latitude[cell_N][subcell_N] |= *buf++;

				pcell_list_descriptor->subcell_longitude[cell_N][subcell_N] = *buf++;
				pcell_list_descriptor->subcell_longitude[cell_N][subcell_N] <<= 8;
				pcell_list_descriptor->subcell_longitude[cell_N][subcell_N] |= *buf++;

				pcell_list_descriptor->subcell_extent_of_latitude[cell_N][subcell_N] = *buf++;
				pcell_list_descriptor->subcell_extent_of_latitude[cell_N][subcell_N] <<= 4;
				pcell_list_descriptor->subcell_extent_of_latitude[cell_N][subcell_N] |= (*buf & 0xf0) >> 4;

				pcell_list_descriptor->subcell_extent_of_longitude[cell_N][subcell_N] = (*buf++ & 0x0f);
				pcell_list_descriptor->subcell_extent_of_longitude[cell_N][subcell_N] <<= 8;
				pcell_list_descriptor->subcell_extent_of_longitude[cell_N][subcell_N] |= *buf++;

				subcell_N ++;
			}

			pcell_list_descriptor->subcell_N[cell_N] = subcell_N;

			cell_N ++;
		}

		pcell_list_descriptor->cell_N = cell_N;
	}

	return 0;
}

//0x6D
S32 DVB_SI_decode_cell_frequency_link_descriptor(U8* buf, S32 length, pcell_frequency_link_descriptor_t pcell_frequency_link_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		porigin = buf;
	U8*		pend;
	U8*		pnext;

	S8		cell_N = 0;
	S8		subcell_N = 0;

	if ((buf != NULL) && (pcell_frequency_link_descriptor != NULL))
	{
		pcell_frequency_link_descriptor->descriptor_tag = *buf++;
		pcell_frequency_link_descriptor->descriptor_length = *buf++;

		pend = buf + pcell_frequency_link_descriptor->descriptor_length;

		cell_N = 0;
		while (buf < pend)
		{
			pcell_frequency_link_descriptor->cell_id[cell_N] = *buf++;
			pcell_frequency_link_descriptor->cell_id[cell_N] <<= 8;
			pcell_frequency_link_descriptor->cell_id[cell_N] |= *buf++;

			pcell_frequency_link_descriptor->frequency[cell_N] = *buf++;
			pcell_frequency_link_descriptor->frequency[cell_N] <<= 8;
			pcell_frequency_link_descriptor->frequency[cell_N] |= *buf++;
			pcell_frequency_link_descriptor->frequency[cell_N] <<= 8;
			pcell_frequency_link_descriptor->frequency[cell_N] |= *buf++;
			pcell_frequency_link_descriptor->frequency[cell_N] <<= 8;
			pcell_frequency_link_descriptor->frequency[cell_N] |= *buf++;

			pcell_frequency_link_descriptor->subcell_info_loop_length[cell_N] = *buf++;
			pnext = buf + pcell_frequency_link_descriptor->subcell_info_loop_length[cell_N];

			subcell_N = 0;
			while (buf < pnext)
			{
				pcell_frequency_link_descriptor->cell_id_extension[cell_N][subcell_N] = *buf++;

				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] = *buf++;
				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] <<= 8;
				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] |= *buf++;
				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] <<= 8;
				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] |= *buf++;
				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] <<= 8;
				pcell_frequency_link_descriptor->transposer_frequency[cell_N][subcell_N] |= *buf++;

				subcell_N ++;
			}
			pcell_frequency_link_descriptor->subcell_N[cell_N] = subcell_N;

			cell_N ++;
		}

		pcell_frequency_link_descriptor->cell_N = cell_N;
	}

	return rtcode;
}

//0x77
S32 DVB_SI_decode_time_slice_fec_identifier_descriptor(U8* buf, S32 length, ptime_slice_fec_identifier_descriptor_t ptime_slice_fec_identifier_descriptor)
{
	S32		copy_length;

	if ((buf != NULL) && (ptime_slice_fec_identifier_descriptor != NULL))
	{ 
		ptime_slice_fec_identifier_descriptor->descriptor_tag = *buf++;
		ptime_slice_fec_identifier_descriptor->descriptor_length = *buf++;

		ptime_slice_fec_identifier_descriptor->time_slicing = (*buf & 0x80) >> 7;
		ptime_slice_fec_identifier_descriptor->mpe_fec = (*buf & 0x60) >> 5;
		ptime_slice_fec_identifier_descriptor->reserved_for_future_use = (*buf & 0x18) >> 3;
		ptime_slice_fec_identifier_descriptor->frame_size = (*buf++) & 0x07;

		ptime_slice_fec_identifier_descriptor->max_burst_duration = *buf++;

		ptime_slice_fec_identifier_descriptor->max_average_rate = (*buf & 0xF0) >> 4;
		ptime_slice_fec_identifier_descriptor->time_slice_fec_id = (*buf++ & 0x0F);

		copy_length = ptime_slice_fec_identifier_descriptor->descriptor_length - 3;
		if (copy_length > 16)
		{
			copy_length = 16;
		}

		ptime_slice_fec_identifier_descriptor->N = copy_length;
		memset(ptime_slice_fec_identifier_descriptor->id_selector_byte, 0x00, 16);
		memcpy(ptime_slice_fec_identifier_descriptor->id_selector_byte, buf, copy_length);
	}

	return 0;
}

//0x7F
S32 DVB_SI_decode_extension_descriptor(U8* buf, S32 length, pextension_descriptor_t pextension_descriptor)
{
	if ((buf != NULL) && (pextension_descriptor != NULL))
	{ 
		pextension_descriptor->descriptor_tag = *buf++;
		pextension_descriptor->descriptor_length = *buf++;
		pextension_descriptor->descriptor_tag_extension = *buf++;
	}

	return 0;
}

//0x81
//功能：解AC3_AUDIO描述子
//输入：
//返回：
S32 DVB_SI_decode_AC3_audio_stream_descriptor(U8* buf, S32 length, pAC3_audio_stream_descriptor_t pAC3_audio_stream_descriptor)
{
	U8*		porigin = buf;

	if ((buf != NULL) &&
		(length >= 5) &&
		(pAC3_audio_stream_descriptor != NULL))
	{
		pAC3_audio_stream_descriptor->descriptor_tag = *buf++;
		pAC3_audio_stream_descriptor->descriptor_length = *buf++;

		pAC3_audio_stream_descriptor->sample_rate_code = (*buf & 0xE0) >> 5;
		pAC3_audio_stream_descriptor->bsid = *buf++ & 0x1F;

		pAC3_audio_stream_descriptor->bit_rate_code = (*buf & 0xFC) >> 2;
		pAC3_audio_stream_descriptor->surround_mode = *buf++ & 0x03;

		pAC3_audio_stream_descriptor->bsmod = (*buf & 0xE0) >> 5;
		pAC3_audio_stream_descriptor->num_channels = (*buf & 0x1E) >> 1;
		pAC3_audio_stream_descriptor->full_svc = *buf++ & 0x01;

		if ((buf - porigin) < length)
		{
			pAC3_audio_stream_descriptor->langcod = *buf++;
		}

		if ((buf - porigin) < length)
		{
			if (pAC3_audio_stream_descriptor->num_channels == 0)
			{
				pAC3_audio_stream_descriptor->langcod2 = *buf++;
			}
		}

		if ((buf - porigin) < length)
		{
			if (pAC3_audio_stream_descriptor->bsmod < 2)
			{
				pAC3_audio_stream_descriptor->mainid = (*buf++ & 0xC0) >> 6;
			}
			else
			{
				pAC3_audio_stream_descriptor->asvcflags = *buf++;
			}
		}

		if ((buf - porigin) < length)
		{
			pAC3_audio_stream_descriptor->textlen = (*buf & 0xFE) >> 1;
			pAC3_audio_stream_descriptor->text_code = *buf++ & 0x01;

			memcpy(pAC3_audio_stream_descriptor->text, buf, pAC3_audio_stream_descriptor->textlen);

			buf += pAC3_audio_stream_descriptor->textlen;
		}

		if ((buf - porigin) < length)
		{
			memcpy(pAC3_audio_stream_descriptor->additional_info, buf, length - (buf - porigin));
		}
	}

	return 0;
}

//功能：解CHANNEL_ORDER描述子
//输入：
//返回：
S32 SI_decode_channel_order_descriptor(U8* buf, S32 length, pchannel_order_descriptor_t pchannel_order_descriptor)
{
	U8*		porigin = buf;
	U8		i;

	if ((buf != NULL) && (pchannel_order_descriptor != NULL))
	{
		pchannel_order_descriptor->descriptor_tag = *buf++;
		pchannel_order_descriptor->descriptor_length = *buf++;

		pchannel_order_descriptor->N = pchannel_order_descriptor->descriptor_length / 4;

		for (i = 0; i < pchannel_order_descriptor->N; i++)
		{
			pchannel_order_descriptor->service_id[i] = *buf++;
			pchannel_order_descriptor->service_id[i] <<= 8;
			pchannel_order_descriptor->service_id[i] |= *buf++;

			pchannel_order_descriptor->channel_index_no[i] = *buf++;
			pchannel_order_descriptor->channel_index_no[i] <<= 8;
			pchannel_order_descriptor->channel_index_no[i] |= *buf++;
		}
	}

	return 0;
}

//功能：解CHANNEL_VOLUME_COMPENSATING描述子
//输入：
//返回：
S32 SI_decode_channel_volume_compensating_descriptor(U8* buf, S32 length, pchannel_volume_compensating_descriptor_t pchannel_volume_compensating_descriptor)
{
	U8*		porigin = buf;
	U8		i;

	if ((buf != NULL) && (pchannel_volume_compensating_descriptor != NULL))
	{
		pchannel_volume_compensating_descriptor->descriptor_tag = *buf++;
		pchannel_volume_compensating_descriptor->descriptor_length = *buf++;

		pchannel_volume_compensating_descriptor->N = pchannel_volume_compensating_descriptor->descriptor_length / 3;

		for (i = 0; i < pchannel_volume_compensating_descriptor->N; i++)
		{
			pchannel_volume_compensating_descriptor->service_id[i] = *buf++;
			pchannel_volume_compensating_descriptor->service_id[i] <<= 8;
			pchannel_volume_compensating_descriptor->service_id[i] |= *buf++;

			pchannel_volume_compensating_descriptor->channel_volume_compensat[i] = *buf++;
		}
	}

	return 0;
}

//功能：解CHANNEL_CLASSIFY描述子
//输入：
//返回：
S32 SI_decode_channel_classify_descriptor(U8* buf, S32 length, pchannel_classify_descriptor_t pchannel_classify_descriptor)
{
	U8*		porigin = buf;
	U8		i;

	if ((buf != NULL) && (pchannel_classify_descriptor != NULL))
	{
		pchannel_classify_descriptor->descriptor_tag = *buf++;
		pchannel_classify_descriptor->descriptor_length = *buf++;

		pchannel_classify_descriptor->N = pchannel_classify_descriptor->descriptor_length / 3;

		for (i = 0; i < pchannel_classify_descriptor->N; i++)
		{
			pchannel_classify_descriptor->service_id[i] = *buf++;
			pchannel_classify_descriptor->service_id[i] <<= 8;
			pchannel_classify_descriptor->service_id[i] |= *buf++;

			pchannel_classify_descriptor->channel_class[i] = *buf++;
		}
	}

	return 0;
}




