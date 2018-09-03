#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/DVB_MHP_Descriptor.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

using namespace std;
#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//标签：tag = 0x00，defined in TS 101 812
//功能：解析应用描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_application_descriptor(uint8_t* buf, int length, application_descriptor_t* papplication_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	S32		i;

	if ((buf != NULL) &&
		(length >= 5) &&
		(papplication_descriptor != NULL))
	{
		papplication_descriptor->descriptor_tag = *buf++;
		papplication_descriptor->descriptor_length = *buf++;

		if (length >= (papplication_descriptor->descriptor_length + 2))
		{
			papplication_descriptor->application_profiles_length = *buf++;

			papplication_descriptor->application_profiles_N = papplication_descriptor->application_profiles_length / 5;

			for (i = 0; i < papplication_descriptor->application_profiles_N; i++)
			{
				papplication_descriptor->application_profile[i] = *buf++;
				papplication_descriptor->application_profile[i] <<= 8;
				papplication_descriptor->application_profile[i] |= *buf++;

				papplication_descriptor->version_major[i] = *buf++;
				papplication_descriptor->version_minor[i] = *buf++;
				papplication_descriptor->version_micro[i] = *buf++;
			}

			papplication_descriptor->service_bound_flag = (*buf & 0x80) >> 7;
			papplication_descriptor->visibility = (*buf & 0x60) >> 5;
			papplication_descriptor->reserved_future_use = (*buf++ & 0x1f);

			papplication_descriptor->application_priority = *buf++;

			papplication_descriptor->transport_protocols_N = papplication_descriptor->descriptor_length - papplication_descriptor->application_profiles_length - 3;

			for (i = 0; i < papplication_descriptor->transport_protocols_N; i++)
			{
				papplication_descriptor->transport_protocol_lable[i] = *buf++;
			}
		}
		else
		{
			rtcode = SECTION_PARSE_PARAMETER_ERROR;				//语法解析错误——长度不够
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//标签：tag = 0x01，defined in TS 101 812
//功能：解析应用名称描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_application_name_descriptor(uint8_t* buf, int length, application_name_descriptor_t* papplication_name_descriptor)
{
	S32		rtcode = 0;
	U8		N;
	U8*		ptemp;
	U8*		pend;
	U8		copy_length;

	if ((buf != NULL) && (length >= 2) && (papplication_name_descriptor != NULL))
	{
		papplication_name_descriptor->descriptor_tag = *buf++;
		papplication_name_descriptor->descriptor_length = *buf++;

		ptemp = buf;
		pend = ptemp + papplication_name_descriptor->descriptor_length;
		buf = ptemp;

		N = 0;
		while (ptemp < pend)
		{
			if (N < MAX_LANGUAGES)
			{
				papplication_name_descriptor->ISO_639_language_code[N] = ptemp[0];
				papplication_name_descriptor->ISO_639_language_code[N] <<= 8;
				papplication_name_descriptor->ISO_639_language_code[N] |= ptemp[1];
				papplication_name_descriptor->ISO_639_language_code[N] <<= 8;
				papplication_name_descriptor->ISO_639_language_code[N] |= ptemp[2];

				memcpy(papplication_name_descriptor->ISO_639_language_char[N], ptemp, 3);
				papplication_name_descriptor->ISO_639_language_char[N][3] = '\0';
				ptemp += 3;

				papplication_name_descriptor->application_name_length[N] = *ptemp++;
				copy_length = papplication_name_descriptor->application_name_length[N];

				if (copy_length > 63)
				{
					copy_length = 63;
				}
				memcpy(papplication_name_descriptor->application_name_char[N], ptemp, copy_length);
				papplication_name_descriptor->application_name_char[N][copy_length] = '\0';
				ptemp += papplication_name_descriptor->application_name_length[N];

				N++;
			}
		}

		papplication_name_descriptor->N = N;
	}

	return rtcode;
}

//标签：tag = 0x02，defined in TS 101 812
//功能：解析传输协议描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_transport_protocol_descriptor(uint8_t* buf, int length, transport_protocol_descriptor_t* ptransport_protocol_descriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*	pstart;
	int			copy_length;

	if ((buf != NULL) && (length >= 2) && (ptransport_protocol_descriptor != NULL))
	{
		pstart = buf;

		ptransport_protocol_descriptor->descriptor_tag = *buf++;
		ptransport_protocol_descriptor->descriptor_length = *buf++;

		ptransport_protocol_descriptor->protocol_id = *buf++;
		ptransport_protocol_descriptor->protocol_id <<= 8;
		ptransport_protocol_descriptor->protocol_id |= *buf++;

		ptransport_protocol_descriptor->transport_protocol_label = *buf++;

		copy_length = length - (int)(buf - pstart);

		if (copy_length > 64)
		{
			copy_length = 64;
		}
		ptransport_protocol_descriptor->N = copy_length;

		if (copy_length > 0)
		{
			memcpy(ptransport_protocol_descriptor->selector_byte, buf, copy_length);
		}
	}

	return rtcode;
}


//标签：tag = 0x04，defined in TS 101 812
//功能：解析DVB-J应用位置描述子
//备注：2018.7 chendelin
int DVB_MHP_decode_dvb_j_application_location_descriptor(uint8_t* buf, int length, dvb_j_application_location_descriptor_t* pdvb_j_application_location_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U16		copy_length;
	U8		N;

	if ((buf != NULL) && (length >= 2) && (pdvb_j_application_location_descriptor != NULL))
	{
		pdvb_j_application_location_descriptor->descriptor_tag = *buf++;
		pdvb_j_application_location_descriptor->descriptor_length = *buf++;

		pdvb_j_application_location_descriptor->base_directory_length = *buf++;

		copy_length = min(127, pdvb_j_application_location_descriptor->base_directory_length);
		if (copy_length > 0)
		{
			memcpy(pdvb_j_application_location_descriptor->base_directory_byte, buf, copy_length);
			pdvb_j_application_location_descriptor->base_directory_byte[copy_length] = '\0';
			buf += pdvb_j_application_location_descriptor->base_directory_length;
		}
		else
		{
			memset(pdvb_j_application_location_descriptor->base_directory_byte, 0x00, sizeof(pdvb_j_application_location_descriptor->base_directory_byte));
		}

		pdvb_j_application_location_descriptor->classpath_extension_length = *buf++;

		copy_length = min(127, pdvb_j_application_location_descriptor->classpath_extension_length);
		if (copy_length > 0)
		{
			memcpy(pdvb_j_application_location_descriptor->classpath_extension_byte, buf, copy_length);
			pdvb_j_application_location_descriptor->classpath_extension_byte[copy_length] = '\0';
			buf += pdvb_j_application_location_descriptor->classpath_extension_length;
		}
		else
		{
			memset(pdvb_j_application_location_descriptor->classpath_extension_byte, 0x00, sizeof(pdvb_j_application_location_descriptor->classpath_extension_byte));
		}

		N = pdvb_j_application_location_descriptor->descriptor_length - pdvb_j_application_location_descriptor->base_directory_length - pdvb_j_application_location_descriptor->classpath_extension_length - 2;
		pdvb_j_application_location_descriptor->N = N;

		copy_length = min(127, N);
		if (copy_length > 0)
		{
			memcpy(pdvb_j_application_location_descriptor->initial_class_byte, buf, copy_length);
			pdvb_j_application_location_descriptor->initial_class_byte[copy_length] = '\0';
		}
		else
		{
			memset(pdvb_j_application_location_descriptor->initial_class_byte, 0x00, sizeof(pdvb_j_application_location_descriptor->initial_class_byte));
		}
	}

	return rtcode;
}
