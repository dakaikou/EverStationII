#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/Mpeg2_PSI_Descriptor.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

using namespace std;
#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//功能：解视频流描述子		0x02
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_video_stream_descriptor(uint8_t *buf, int length, pvideo_stream_descriptor_t pvideo_stream_descriptor)
{
	int		rtcode = 0;

	if ((buf != NULL) && 
		(length >= 3) &&
		(pvideo_stream_descriptor != NULL))
	{
		pvideo_stream_descriptor->descriptor_tag = *buf++;

		pvideo_stream_descriptor->descriptor_length = *buf++;
		assert(pvideo_stream_descriptor->descriptor_length >= 1);

		pvideo_stream_descriptor->multiple_frame_rate_flag = (*buf & 0x80) >> 7;
		pvideo_stream_descriptor->frame_rate_code = (*buf & 0x78) >> 3;
		pvideo_stream_descriptor->MPEG_1_only_flag = (*buf & 0x04) >> 2;
		pvideo_stream_descriptor->constrained_parameter_flag = (*buf & 0x02) >> 1;
		pvideo_stream_descriptor->still_picture_flag = (*buf++ & 0x01);

		if (pvideo_stream_descriptor->MPEG_1_only_flag == 0)
		{
			assert(pvideo_stream_descriptor->descriptor_length == 3);

			pvideo_stream_descriptor->profile_and_level_indication = *buf++;

			pvideo_stream_descriptor->chroma_format = (*buf & 0xC0) >> 6;
			pvideo_stream_descriptor->frame_rate_extension_flag= (*buf & 0x20) >> 5;
			pvideo_stream_descriptor->reserved = (*buf++ & 0x1F);
		}
	}
	else
	{
		rtcode = 1;					//输入参数错误
	}

	return rtcode;
}

//功能：解音频流描述子			0x03
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_audio_stream_descriptor(uint8_t *buf, int length, paudio_stream_descriptor_t paudio_stream_descriptor)
{
	int		rtcode = 0;

	if ((buf != NULL) &&
		(length >= 3) &&
		(paudio_stream_descriptor != NULL))
	{
		paudio_stream_descriptor->descriptor_tag = *buf++; 
		paudio_stream_descriptor->descriptor_length = *buf++;
		assert(paudio_stream_descriptor->descriptor_length == 1);

		paudio_stream_descriptor->free_format_flag = (*buf & 0x80) >> 7;
		paudio_stream_descriptor->ID = (*buf & 0x40) >> 6;
		paudio_stream_descriptor->layer =(*buf & 0x30) >> 4;
		paudio_stream_descriptor->variable_rate_audio_indicator = (*buf & 0x04) >> 3;
		paudio_stream_descriptor->reserved = (*buf++ & 0x07);
	}
	else
	{
		rtcode = 1;					//输入参数错误
	}

	return rtcode;
}

//功能：解Hierarchy描述子				0x04
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_hierarchy_descriptor(uint8_t *buf, int length, phierarchy_descriptor_t phierarchy_descriptor)
{
	int		rtcode = 0;

	if ((buf != NULL) &&
		(length >= 6) &&
		(phierarchy_descriptor != NULL))
	{
		phierarchy_descriptor->descriptor_tag = *buf++;

		phierarchy_descriptor->descriptor_length = *buf++;
		assert(phierarchy_descriptor->descriptor_length == 4);

		phierarchy_descriptor->reserved0 = (*buf & 0xF0) >> 4;
		phierarchy_descriptor->hierarchy_type = (*buf++ & 0x0F);
	             
		phierarchy_descriptor->reserved1 = (*buf & 0xC0) >> 6;
		phierarchy_descriptor->hierarchy_layer_index = (*buf++ & 0x3F);

		phierarchy_descriptor->reserved2 = (*buf & 0xC0) >> 6;
		phierarchy_descriptor->hierarchy_embedded_layer_index = (*buf++ & 0x3F);

		phierarchy_descriptor->reserved3 = (*buf & 0xC0) >> 6;
		phierarchy_descriptor->hierarchy_channel = (*buf++ & 0x3F);
	}
	else
	{
		rtcode = 1;					//输入参数错误
	}

	return rtcode;
}

//功能：解注册描述子			0x05
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_registration_descriptor(uint8_t* buf, int length, registration_descriptor_t* pregistration_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) &&
		(length >= 6) &&
		(pregistration_descriptor != NULL))
	{
		memset(pregistration_descriptor, 0x00, sizeof(registration_descriptor_t));

		pregistration_descriptor->descriptor_tag = *buf++;

		pregistration_descriptor->descriptor_length = *buf++;

		pregistration_descriptor->format_identifier_char[0] = *buf++;
		pregistration_descriptor->format_identifier_char[1] = *buf++;
		pregistration_descriptor->format_identifier_char[2] = *buf++;
		pregistration_descriptor->format_identifier_char[3] = *buf++;
		pregistration_descriptor->format_identifier_char[4] = '\0';

		//pregistration_descriptor->format_identifier = pregistration_descriptor->format_identifier_char[0];
		//pregistration_descriptor->format_identifier <<= 8;
		//pregistration_descriptor->format_identifier |= pregistration_descriptor->format_identifier_char[1];
		//pregistration_descriptor->format_identifier <<= 8;
		//pregistration_descriptor->format_identifier |= pregistration_descriptor->format_identifier_char[2];
		//pregistration_descriptor->format_identifier <<= 8;
		//pregistration_descriptor->format_identifier |= pregistration_descriptor->format_identifier_char[3];

		pregistration_descriptor->info_length = pregistration_descriptor->descriptor_length - 4;

		if (pregistration_descriptor->info_length > 0)
		{
			memcpy(pregistration_descriptor->additional_identification_info, buf, pregistration_descriptor->info_length);
			pregistration_descriptor->additional_identification_info[pregistration_descriptor->info_length] = '\0';
			buf += pregistration_descriptor->info_length;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解数据流对齐描述子				0x06
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_data_stream_alignment_descriptor(uint8_t* buf, int length, pdata_stream_alignment_descriptor_t pdata_stream_alignment_descriptor)
{
	int		rtcode = 0;

	if ((buf != NULL) &&
		(length >= 3) &&
		(pdata_stream_alignment_descriptor != NULL))
	{
		pdata_stream_alignment_descriptor->descriptor_tag = *buf++; 
		pdata_stream_alignment_descriptor->descriptor_length = *buf++;
		pdata_stream_alignment_descriptor->alignment_type = *buf++;
	}
	else
	{
		rtcode = 1;
	}

	return rtcode;
}	

//功能：解目标背景栅格描述子				0x07
//输入：buffer,起始位置:nIndex
//返回：LPVOID指针
//备注：2000.12.15 chendelin
int MPEG2_PSI_decode_target_background_grid_descriptor(uint8_t* buf, int length, ptarget_background_grid_descriptor_t ptarget_background_grid_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(ptarget_background_grid_descriptor != NULL))
	{
		ptarget_background_grid_descriptor->descriptor_tag = *buf++;
		ptarget_background_grid_descriptor->descriptor_length = *buf++;

		ptarget_background_grid_descriptor->horizontal_size = *buf++ << 6;
		ptarget_background_grid_descriptor->horizontal_size |= (*buf & 0xFC) >> 2;

		ptarget_background_grid_descriptor->vertical_size = (*buf++ & 0x03) << 12;
		ptarget_background_grid_descriptor->vertical_size |= *buf++ << 4;
		ptarget_background_grid_descriptor->vertical_size |= (*buf & 0xF0) >> 4;

		ptarget_background_grid_descriptor->aspect_ratio_information = *buf++ & 0x0F;
	}

	return 0;
}	

//功能：解视频窗口描述子							0x08
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_video_window_descriptor(uint8_t* buf, int length, pvideo_window_descriptor_t pvideo_window_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pvideo_window_descriptor != NULL))
	{
		pvideo_window_descriptor->descriptor_tag = *buf++;
		pvideo_window_descriptor->descriptor_length = *buf++;

		pvideo_window_descriptor->horizontal_offset = *buf++ << 6;
		pvideo_window_descriptor->horizontal_offset |= (*buf & 0xFC) >> 2;

		pvideo_window_descriptor->vertical_offset = (*buf++ & 0x03) << 12;
		pvideo_window_descriptor->vertical_offset |= *buf++ << 4;
		pvideo_window_descriptor->vertical_offset |= (*buf & 0xF0) >> 4;

		pvideo_window_descriptor->window_priority = *buf++ & 0x0F;
	}

	return 0;
}	

//功能：解CA描述子									0x09
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_CA_descriptor(uint8_t* buf, int length, CA_descriptor_t* pCA_descriptor)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) &&
		(length >= 6) &&
		(pCA_descriptor != NULL))
	{
		memset(pCA_descriptor, 0x00, sizeof(CA_descriptor_t));

		pCA_descriptor->descriptor_tag = *buf++;
		pCA_descriptor->descriptor_length = *buf++;
		
		pCA_descriptor->CA_system_ID = *buf++;
		pCA_descriptor->CA_system_ID <<= 8;
		pCA_descriptor->CA_system_ID |= *buf++;

		pCA_descriptor->reserved = (*buf & 0xE0) >> 5;

		pCA_descriptor->CA_PID = (*buf++ & 0x1F);
		pCA_descriptor->CA_PID <<= 8; 
		pCA_descriptor->CA_PID |= *buf++;
		
		pCA_descriptor->private_data_length = pCA_descriptor->descriptor_length - 4;
		//copy_length = (pCA_descriptor->private_data_length < MAX_PRIVATE_DATA_LENGTH) ? pCA_descriptor->private_data_length : MAX_PRIVATE_DATA_LENGTH;

		//if (copy_length > 0)
		//{
		//	memcpy(pCA_descriptor->private_data_byte, buf, copy_length);
		//}
		//else
		//{
		//	memset(pCA_descriptor->private_data_byte, 0x00, sizeof(pCA_descriptor->private_data_byte));
		//}
		pCA_descriptor->private_data_byte = buf;
		buf += pCA_descriptor->private_data_length;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解ISO639语言描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_ISO_639_language_descriptor(uint8_t* buf, int length, ISO_639_language_descriptor_t* pISO_639_language_descriptor)
{
	int rtcode = SECTION_PARSE_UNKNOWN_ERROR;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pISO_639_language_descriptor != NULL))
	{
		memset(pISO_639_language_descriptor, 0x00, sizeof(ISO_639_language_descriptor_t));

		pISO_639_language_descriptor->descriptor_tag = *buf++; 
		pISO_639_language_descriptor->descriptor_length = *buf++;

		int loop_length = pISO_639_language_descriptor->descriptor_length;
		int N = 0;

		while ((loop_length >= 4) && (N < MAX_LANGUAGES))
		{
			//pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code = buf[0];
			//pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code <<= 8;
			//pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code |= buf[1];
			//pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code <<= 8;
			//pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code |= buf[2];

			memcpy(pISO_639_language_descriptor->st[N].ISO_639_language_code_char, buf, 3);
			pISO_639_language_descriptor->st[N].ISO_639_language_code_char[3] = '\0';
			buf += 3;

			pISO_639_language_descriptor->st[N].audio_type = *buf++;

			loop_length -= 4;
			N ++;
		}

		pISO_639_language_descriptor->N = N;

		rtcode = SECTION_PARSE_NO_ERROR;
	}

	return rtcode;
}

//功能：解系统时钟描述子描述子				0x0B
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_system_clock_descriptor(uint8_t *buf, int length, psystem_clock_descriptor_t psystem_clock_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(psystem_clock_descriptor != NULL))
	{
		psystem_clock_descriptor->descriptor_tag = *buf++;
		psystem_clock_descriptor->descriptor_length = *buf++;
			
		psystem_clock_descriptor->external_clock_reference_indicator = (*buf & 0x80) >> 7;
		psystem_clock_descriptor->reserved0 = (*buf & 0x40) >> 6;
		psystem_clock_descriptor->clock_accuracy_integer = *buf++ & 0x3F;

		psystem_clock_descriptor->clock_accuracy_exponent = (*buf & 0xE0) >> 5;
		psystem_clock_descriptor->reserved1 = (*buf++ & 0x1F);
	}

	return 0;
}

//功能：解复用缓冲器使用描述子				0x0C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(uint8_t *buf, int length, pmultiplex_buffer_utilization_descriptor_t pmultiplex_buffer_utilization_descriptor)
{
	if ((buf != NULL) &&
		(length >= 3) &&
		(pmultiplex_buffer_utilization_descriptor != NULL))
	{
		pmultiplex_buffer_utilization_descriptor->descriptor_tag = *buf++;
		pmultiplex_buffer_utilization_descriptor->descriptor_length = *buf++;
			
		pmultiplex_buffer_utilization_descriptor->bound_valid_flag = (*buf & 0x80) >> 7;		

		if (pmultiplex_buffer_utilization_descriptor->bound_valid_flag)
		{
			pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound = (*buf++ & 0x7F) << 8;
			pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound |= *buf++;

			pmultiplex_buffer_utilization_descriptor->reserved = (*buf & 0x80) >> 7;
			pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound = (*buf++ & 0x7F) << 8;
			pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound |= *buf++;
		}
		else
		{
			pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound = 0;
			pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound = 0;
		}
	}

	return 0;
}


//功能：解版权描述子				0x0D
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_copyright_descriptor(uint8_t* buf, int length, pcopyright_descriptor_t pcopyright_descriptor)
{
	int	copy_length;
	int loop_length;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pcopyright_descriptor != NULL))
	{
		pcopyright_descriptor->descriptor_tag = *buf++; 
		pcopyright_descriptor->descriptor_length = *buf++;

		pcopyright_descriptor->copyright_identifier = *buf++ << 24;
		pcopyright_descriptor->copyright_identifier |= *buf++ << 16;
		pcopyright_descriptor->copyright_identifier |= *buf++ << 8;
		pcopyright_descriptor->copyright_identifier |= *buf++;

		loop_length = pcopyright_descriptor->descriptor_length - 4;

		if (loop_length > 0)
		{
			copy_length = (loop_length < MAX_COPYRIGHT_INFO_LENGTH) ? loop_length : MAX_COPYRIGHT_INFO_LENGTH;
			memcpy(pcopyright_descriptor->additional_copyright_info, buf, copy_length);
			pcopyright_descriptor->additional_copyright_info[copy_length] = '\0';
		}
	}

	return 0;
}

//功能：解最大比特率描述子					0x0E
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_maximum_bitrate_descriptor(uint8_t *buf, int length, pmaximum_bitrate_descriptor_t pmaximum_bitrate_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pmaximum_bitrate_descriptor != NULL))
	{
		pmaximum_bitrate_descriptor->descriptor_tag = *buf++;
		pmaximum_bitrate_descriptor->descriptor_length = *buf++;

		pmaximum_bitrate_descriptor->reserved = (*buf & 0xC0) >> 6;

		pmaximum_bitrate_descriptor->maximum_bitrate = (*buf++ & 0x3F) << 16;
		pmaximum_bitrate_descriptor->maximum_bitrate |= *buf++ << 8;
		pmaximum_bitrate_descriptor->maximum_bitrate |= *buf++;
	}

	return 0;
}

//功能：解私有数据指示器描述子				0x0F
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_private_data_indicator_descriptor(uint8_t* buf, int length, pprivate_data_indicator_descriptor_t pprivate_data_indicator_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pprivate_data_indicator_descriptor != NULL))
	{
		pprivate_data_indicator_descriptor->descriptor_tag = *buf++; 
		pprivate_data_indicator_descriptor->descriptor_length = *buf++;

		pprivate_data_indicator_descriptor->private_data_indicator = *buf++ << 24;
		pprivate_data_indicator_descriptor->private_data_indicator |= *buf++ << 16;
		pprivate_data_indicator_descriptor->private_data_indicator |= *buf++ << 8;
		pprivate_data_indicator_descriptor->private_data_indicator |= *buf++;
	}

	return 0;
}

//功能：解平滑缓冲器描述子			0x10
//输入：buffer, 开始位置nIndex
//返回：LPVOID指针
//备注：2000.12.14 chendelin
int MPEG2_PSI_decode_smoothing_buffer_descriptor(uint8_t* buf, int length, psmoothing_buffer_descriptor_t psmoothing_buffer_descriptor)
{
	if ((buf != NULL) && (length >= 2) && (psmoothing_buffer_descriptor != NULL))
	{
		psmoothing_buffer_descriptor->descriptor_tag = *buf++;
		psmoothing_buffer_descriptor->descriptor_length = *buf++;

		psmoothing_buffer_descriptor->reserved0 = (*buf & 0xC0) >> 6;

		psmoothing_buffer_descriptor->sb_leak_rate = (*buf++ & 0x3F) << 16;
		psmoothing_buffer_descriptor->sb_leak_rate |= *buf++ << 8;
		psmoothing_buffer_descriptor->sb_leak_rate |= *buf++;

		psmoothing_buffer_descriptor->reserved1 = (*buf & 0xC0) >> 6;

		psmoothing_buffer_descriptor->sb_size = (*buf++ & 0x3F) << 16;
		psmoothing_buffer_descriptor->sb_size |= *buf++ << 8;
		psmoothing_buffer_descriptor->sb_size |= *buf++;
	}

	return 0;
}

//功能：解STD描述子						0x11
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_STD_descriptor(uint8_t* buf, int length, pSTD_descriptor_t pSTD_descriptor)
{
	if ((buf != NULL) &&
		(length >= 3) &&
		(pSTD_descriptor != NULL))
	{
		pSTD_descriptor->descriptor_tag = *buf++; 
		pSTD_descriptor->descriptor_length = *buf++;

		pSTD_descriptor->reserved = (*buf & 0xFE) >> 1;
		pSTD_descriptor->leak_valid_flag = *buf++ & 0x01;
	}

	return 0;
}

//功能：解IBP描述子						0x12
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_IBP_descriptor(uint8_t* buf, int length, pIBP_descriptor_t pIBP_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pIBP_descriptor != NULL))
	{
		pIBP_descriptor->descriptor_tag = *buf++; 
		pIBP_descriptor->descriptor_length = *buf++;
		
		pIBP_descriptor->closed_gop_flag = (*buf & 0x80) >> 7;
		pIBP_descriptor->identical_gop_flag = (*buf & 0x40) >> 6;
				
		pIBP_descriptor->max_gop_length = *buf++ & 0x03F;
		pIBP_descriptor->max_gop_length |= *buf++;
	}

	return 0;
}

//0x13
int MPEG2_DSMCC_decode_carousel_identifier_descriptor(uint8_t* buf, int length, carousel_identifier_descriptor_t* pcarousel_identifier_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	int		N;

	if ((buf != NULL) &&
		(length >= 6) &&
		(pcarousel_identifier_descriptor != NULL))
	{
		pcarousel_identifier_descriptor->descriptor_tag = *buf ++;
		pcarousel_identifier_descriptor->descriptor_length = *buf++;

		pcarousel_identifier_descriptor->carousel_id = *buf ++;
		pcarousel_identifier_descriptor->carousel_id <<= 8;
		pcarousel_identifier_descriptor->carousel_id |= *buf ++;
		pcarousel_identifier_descriptor->carousel_id <<= 8;
		pcarousel_identifier_descriptor->carousel_id |= *buf ++;
		pcarousel_identifier_descriptor->carousel_id <<= 8;
		pcarousel_identifier_descriptor->carousel_id |= *buf ++;

		N = pcarousel_identifier_descriptor->descriptor_length - 4;

		pcarousel_identifier_descriptor->N = min(N, 64);
		if (pcarousel_identifier_descriptor->N > 0)
		{
			memcpy(pcarousel_identifier_descriptor->private_data_byte, buf, pcarousel_identifier_descriptor->N);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//0x14
int MPEG2_DSMCC_decode_association_tag_descriptor(uint8_t* buf, int length, association_tag_descriptor_t* passociation_tag_descriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
//	int			N1;
	int			N2;
	uint8_t*	porigin = buf;

	if ((buf != NULL) &&
		(length >= 7) &&
		(passociation_tag_descriptor != NULL))
	{
		memset(passociation_tag_descriptor, 0x00, sizeof(association_tag_descriptor_t));

		passociation_tag_descriptor->descriptor_tag = *buf ++;
		passociation_tag_descriptor->descriptor_length = *buf++;

		passociation_tag_descriptor->association_tag = *buf ++;
		passociation_tag_descriptor->association_tag <<= 8;
		passociation_tag_descriptor->association_tag |= *buf ++;

		passociation_tag_descriptor->use = *buf ++;
		passociation_tag_descriptor->use <<= 8;
		passociation_tag_descriptor->use |= *buf ++;

		passociation_tag_descriptor->selector_byte_length = *buf++;

		if (passociation_tag_descriptor->selector_byte_length > 0)
		{
			if (passociation_tag_descriptor->use == 0x0000)
			{
				passociation_tag_descriptor->u.use_0x0000.transaction_id = *buf++;
				passociation_tag_descriptor->u.use_0x0000.transaction_id <<= 8;
				passociation_tag_descriptor->u.use_0x0000.transaction_id |= *buf++;
				passociation_tag_descriptor->u.use_0x0000.transaction_id <<= 8;
				passociation_tag_descriptor->u.use_0x0000.transaction_id |= *buf++;
				passociation_tag_descriptor->u.use_0x0000.transaction_id <<= 8;
				passociation_tag_descriptor->u.use_0x0000.transaction_id |= *buf++;

				passociation_tag_descriptor->u.use_0x0000.timeout = *buf++;
				passociation_tag_descriptor->u.use_0x0000.timeout <<= 8;
				passociation_tag_descriptor->u.use_0x0000.timeout |= *buf++;
				passociation_tag_descriptor->u.use_0x0000.timeout <<= 8;
				passociation_tag_descriptor->u.use_0x0000.timeout |= *buf++;
				passociation_tag_descriptor->u.use_0x0000.timeout <<= 8;
				passociation_tag_descriptor->u.use_0x0000.timeout |= *buf++;
			}
			//else if (passociation_tag_descriptor->use == 0x0001)
			//{
			//	passociation_tag_descriptor->selector_length = *buf++;
			//}
			else
			{
				passociation_tag_descriptor->u.use_other.N1 = min(64, passociation_tag_descriptor->selector_byte_length);
				if (passociation_tag_descriptor->u.use_other.N1 > 0)
				{
					memcpy(passociation_tag_descriptor->u.use_other.selector_byte, buf, passociation_tag_descriptor->u.use_other.N1);
					buf += passociation_tag_descriptor->selector_byte_length;
				}
			}
		}
		else
		{
			passociation_tag_descriptor->u.use_0x0000.transaction_id = 0xFFFFFFFF;
		}

		N2 = length - (int)(buf - porigin);
		passociation_tag_descriptor->N2 = min(64, N2);
		if (passociation_tag_descriptor->N2 > 0)
		{
			memcpy(passociation_tag_descriptor->private_data_byte, buf, passociation_tag_descriptor->N2);
			buf += N2;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解析deferred_association_tags_descriptor   tag = 0x15    EN301192
// 2009.03.03	added by chendelin	
int DSMCC_decode_deferred_association_tags_descriptor(uint8_t* buf, int length, pdeferred_association_tags_descriptor_t pdeferred_association_tags_descriptor)
{
	int		rtcode = 0;
	int		loop_length;
	uint8_t		N1, N2;

	if ((buf != NULL) &&
		(length >= 2) &&
		(pdeferred_association_tags_descriptor != NULL))
	{
		pdeferred_association_tags_descriptor->descriptor_tag = *buf++;
		pdeferred_association_tags_descriptor->descriptor_length = *buf++;

		if (length >= (pdeferred_association_tags_descriptor->descriptor_length + 2))
		{
			pdeferred_association_tags_descriptor->association_tags_loop_length = *buf++;

			N1 = 0;
			if (pdeferred_association_tags_descriptor->association_tags_loop_length > 0)
			{
				loop_length = pdeferred_association_tags_descriptor->association_tags_loop_length;
				while (loop_length > 0)
				{
					if (N1 < 16)
					{
						pdeferred_association_tags_descriptor->association_tag[N1] = *buf++;
						pdeferred_association_tags_descriptor->association_tag[N1] <<= 8;
						pdeferred_association_tags_descriptor->association_tag[N1] |= *buf++;

						N1 ++;
					}
					else
					{
						buf++;
						buf++;
					}
					loop_length -= 2;
				}
			}
			pdeferred_association_tags_descriptor->N1 = N1;

			pdeferred_association_tags_descriptor->transport_stream_id = *buf++;
			pdeferred_association_tags_descriptor->transport_stream_id <<= 8;
			pdeferred_association_tags_descriptor->transport_stream_id |= *buf++;

			pdeferred_association_tags_descriptor->program_number = *buf++;
			pdeferred_association_tags_descriptor->program_number <<= 8;
			pdeferred_association_tags_descriptor->program_number |= *buf++;

			N2 = 0;
			loop_length = pdeferred_association_tags_descriptor->descriptor_length - 1 - pdeferred_association_tags_descriptor->association_tags_loop_length - 4;
			while (loop_length > 0)
			{
				if (N2 < 64)
				{
					pdeferred_association_tags_descriptor->private_data_byte[N2] = *buf++;

					N2 ++;
				}
				else
				{
					buf++;
				}
				loop_length --;
			}
			pdeferred_association_tags_descriptor->N2 = N2;
		}
		else
		{
			rtcode = 2;							//语法解析错误——长度不足
		}
	}
	else
	{
		rtcode = 1;					//输入参数错误
	}

	return rtcode;
}


//0x1B
int MPEG2_PSI_decode_MPEG4_video_descriptor(uint8_t* buf, int length, pMPEG4_video_descriptor_t pMPEG4_video_descriptor)
{
	if ((buf != NULL) &&
		(length >= 3) &&
		(pMPEG4_video_descriptor != NULL))
	{
		pMPEG4_video_descriptor->descriptor_tag = *buf++;
		pMPEG4_video_descriptor->descriptor_length = *buf++;
		pMPEG4_video_descriptor->MPEG4_visual_profile_and_level = *buf++;
	}

	return 0;
}

//0x1C
int MPEG2_PSI_decode_MPEG4_audio_descriptor(uint8_t* buf, int length, pMPEG4_audio_descriptor_t pMPEG4_audio_descriptor)
{
	if ((buf != NULL) &&
		(length >= 3) &&
		(pMPEG4_audio_descriptor != NULL))
	{
		pMPEG4_audio_descriptor->descriptor_tag = *buf++;
		pMPEG4_audio_descriptor->descriptor_length = *buf++;
		pMPEG4_audio_descriptor->MPEG4_audio_profile_and_level = *buf++;
	}

	return 0;
}

//  0x20
int MPEG2_PSI_decode_External_ES_ID_descriptor(uint8_t* buf, int length, pExternal_ES_ID_descriptor_t pExternal_ES_ID_descriptor)
{
	if ((buf != NULL) &&
		(length >= 4) &&
		(pExternal_ES_ID_descriptor != NULL))
	{
		pExternal_ES_ID_descriptor->descriptor_tag = *buf++;
		pExternal_ES_ID_descriptor->descriptor_length = *buf++;

		pExternal_ES_ID_descriptor->External_ES_ID = *buf++;
		pExternal_ES_ID_descriptor->External_ES_ID <<= 8;
		pExternal_ES_ID_descriptor->External_ES_ID |= *buf++;
	}

	return 0;
}

//0x28
int MPEG2_PSI_decode_AVC_video_descriptor(uint8_t* buf, int length, pAVC_video_descriptor_t pAVC_video_descriptor)
{
	if ((buf != NULL) &&
		(length >= 6) &&
		(pAVC_video_descriptor != NULL))
	{
		pAVC_video_descriptor->descriptor_tag = *buf++;
		pAVC_video_descriptor->descriptor_length = *buf++;

		pAVC_video_descriptor->profile_idc = *buf++;

		pAVC_video_descriptor->constraint_set0_flag = (*buf & 0x80) >> 7;
		pAVC_video_descriptor->constraint_set1_flag = (*buf & 0x40) >> 6;
		pAVC_video_descriptor->constraint_set2_flag = (*buf & 0x20) >> 5;
		pAVC_video_descriptor->AVC_compatible_flags = (*buf++ & 0x1F);

		pAVC_video_descriptor->level_idc = *buf++;

		pAVC_video_descriptor->AVC_still_present = (*buf & 0x80) >> 7;
		pAVC_video_descriptor->AVC_24_hour_picture_flag = (*buf & 0x40) >> 6;
		pAVC_video_descriptor->reserved = (*buf++ & 0x3F);
	}

	return 0;
}

//0x2A
int MPEG2_PSI_decode_AVC_timing_and_HRD_descriptor(uint8_t* buf, int length, pAVC_timing_and_HRD_descriptor_t pAVC_timing_and_HRD_descriptor)
{
	if ((buf != NULL) &&
		(length >= 2) &&
		(pAVC_timing_and_HRD_descriptor != NULL))
	{
		pAVC_timing_and_HRD_descriptor->descriptor_tag = *buf++;
		pAVC_timing_and_HRD_descriptor->descriptor_length = *buf++;

		pAVC_timing_and_HRD_descriptor->hrd_management_valid_flag = (*buf & 0x80) >> 7;
		pAVC_timing_and_HRD_descriptor->reserved0 = (*buf & 0x7e) >> 1;
		pAVC_timing_and_HRD_descriptor->picture_and_timing_info_present = (*buf++ & 0x01);

		if (pAVC_timing_and_HRD_descriptor->picture_and_timing_info_present)
		{
			pAVC_timing_and_HRD_descriptor->flag_90kHz = (*buf & 0x80) >> 7;
			pAVC_timing_and_HRD_descriptor->reserved1 = (*buf++ & 0x7f);

			if (pAVC_timing_and_HRD_descriptor->flag_90kHz == 0)
			{
				pAVC_timing_and_HRD_descriptor->N = *buf++;
				pAVC_timing_and_HRD_descriptor->N <<= 8;
				pAVC_timing_and_HRD_descriptor->N |= *buf++;
				pAVC_timing_and_HRD_descriptor->N <<= 8;
				pAVC_timing_and_HRD_descriptor->N |= *buf++;
				pAVC_timing_and_HRD_descriptor->N <<= 8;
				pAVC_timing_and_HRD_descriptor->N |= *buf++;

				pAVC_timing_and_HRD_descriptor->K = *buf++;
				pAVC_timing_and_HRD_descriptor->K <<= 8;
				pAVC_timing_and_HRD_descriptor->K |= *buf++;
				pAVC_timing_and_HRD_descriptor->K <<= 8;
				pAVC_timing_and_HRD_descriptor->K |= *buf++;
				pAVC_timing_and_HRD_descriptor->K <<= 8;
				pAVC_timing_and_HRD_descriptor->K |= *buf++;
			}

			pAVC_timing_and_HRD_descriptor->num_units_in_tick = *buf++;
			pAVC_timing_and_HRD_descriptor->num_units_in_tick <<= 8;
			pAVC_timing_and_HRD_descriptor->num_units_in_tick |= *buf++;
			pAVC_timing_and_HRD_descriptor->num_units_in_tick <<= 8;
			pAVC_timing_and_HRD_descriptor->num_units_in_tick |= *buf++;
			pAVC_timing_and_HRD_descriptor->num_units_in_tick <<= 8;
			pAVC_timing_and_HRD_descriptor->num_units_in_tick |= *buf++;
		}

		pAVC_timing_and_HRD_descriptor->fixed_frame_rate_flag = (*buf & 0x80) >> 7;
		pAVC_timing_and_HRD_descriptor->temporal_poc_flag = (*buf & 0x40) >> 6;
		pAVC_timing_and_HRD_descriptor->picture_to_display_conversion_flag = (*buf & 0x20) >> 5;
		pAVC_timing_and_HRD_descriptor->reserved2 = (*buf++ & 0x1f);
	}

	return 0;
}




