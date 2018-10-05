#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_SDT_DecodeSection(uint8_t *section_buf, int section_size, service_description_section_t* psdt_section)
{
	int		 rtcode = SECTION_PARSE_NO_ERROR;

	S32		 N = 0;
	S32		 service_loop_length;
	S32		 descriptor_loop_length;
	U8*		 pl1temp;
	U8*		 pl2temp;
	U16		 descriptor_tag;
	U8		 descriptor_length;
	S32		 move_length;
	S32		 reserved_count;

	BITS_t   bs;

	SERVICE_DESCRIPTION_t*	pstServiceDescription;

	//SDT表的最小长度为15字节
	if ((section_buf != NULL) && 
		((section_size >= DVB_SI_SDT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_SDT_SECTION_MAX_SIZE)) && 
		(psdt_section != NULL))
	{
		memset(psdt_section, 0x00, sizeof(service_description_section_t));

		psdt_section->CRC_32_recalculated = Encode_CRC_32(section_buf, section_size - 4);
		uint32_t CRC_32_encoded = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (CRC_32_verify == CRC_32_code)
		{
			int table_id = section_buf[0];

			if ((table_id == TABLE_ID_SDT_ACTUAL) ||
				(table_id == TABLE_ID_SDT_OTHER))
			{
				BITS_map(&bs, section_buf, section_size);

				psdt_section->table_id = BITS_get(&bs, 8);

				psdt_section->section_syntax_indicator = BITS_get(&bs, 1);
				psdt_section->reserved_future_use0 = BITS_get(&bs, 1);
				psdt_section->reserved0 = BITS_get(&bs, 2);

				psdt_section->section_length = BITS_get(&bs, 12);

				if ((psdt_section->section_length >= (DVB_SI_SDT_SECTION_MIN_SIZE - 3)) && (psdt_section->section_length <= (DVB_SI_SDT_SECTION_MAX_SIZE - 3)))
				{
					psdt_section->transport_stream_id = BITS_get(&bs, 16);

					psdt_section->reserved1 = BITS_get(&bs, 2);
					psdt_section->version_number = BITS_get(&bs, 5);
					psdt_section->current_next_indicator = BITS_get(&bs, 1);

					psdt_section->section_number = BITS_get(&bs, 8);

					psdt_section->last_section_number = BITS_get(&bs, 8);

					if (psdt_section->section_number <= psdt_section->last_section_number)			//特征点
					{
						psdt_section->original_network_id = BITS_get(&bs, 16);

						psdt_section->reserved_future_use1 = BITS_get(&bs, 8);

						service_loop_length = psdt_section->section_length - 12;

						N = 0;
						if (service_loop_length > 0)
						{
							pl1temp = bs.p_cur;
							BITS_byteSkip(&bs, service_loop_length);

							BITS_t service_loop_bs;
							BITS_map(&service_loop_bs, pl1temp, service_loop_length);

							while ((service_loop_length >= 5) && (N < MAX_SERVICES_PER_STREAM))
							{
								unsigned char* old_service_ptr = service_loop_bs.p_cur;

								pstServiceDescription = psdt_section->astServiceDescriptions + N;

								pstServiceDescription->service_id = BITS_get(&service_loop_bs, 16);

								pstServiceDescription->reserved_future_use = BITS_get(&service_loop_bs, 6);

								pstServiceDescription->EIT_schedule_flag = BITS_get(&service_loop_bs, 1);
								pstServiceDescription->EIT_present_following_flag = BITS_get(&service_loop_bs, 1);

								pstServiceDescription->running_status = BITS_get(&service_loop_bs, 3);
								pstServiceDescription->free_CA_mode = BITS_get(&service_loop_bs, 1);

								pstServiceDescription->descriptors_loop_length = BITS_get(&service_loop_bs, 12);

								reserved_count = 0;
								if (pstServiceDescription->descriptors_loop_length > 0)
								{
									if (pstServiceDescription->descriptors_loop_length <= (service_loop_length - 5))
									{
										pl2temp = service_loop_bs.p_cur;
										BITS_byteSkip(&service_loop_bs, pstServiceDescription->descriptors_loop_length);
										uint8_t* pend = service_loop_bs.p_cur;

										descriptor_loop_length = pstServiceDescription->descriptors_loop_length;
										while ((pl2temp <= (pend - 2)) && (descriptor_loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
										{
											descriptor_tag = pl2temp[0];
											descriptor_length = pl2temp[1];
											move_length = descriptor_length + 2;

											if (move_length <= descriptor_loop_length)
											{

												pstServiceDescription->service_descriptors[reserved_count].descriptor_tag = descriptor_tag;
												pstServiceDescription->service_descriptors[reserved_count].descriptor_length = descriptor_length;
												pstServiceDescription->service_descriptors[reserved_count].descriptor_buf = pl2temp;
												pstServiceDescription->service_descriptors[reserved_count].descriptor_size = move_length;

												reserved_count++;
											}
											else
											{
												break;
											}

											pl2temp += move_length;
											descriptor_loop_length -= move_length;
										}
									}
									else
									{
										BITS_byteSkip(&service_loop_bs, service_loop_length - 5);
										//rtcode = SECTION_PARSE_SYNTAX_ERROR;
									}
								}
								pstServiceDescription->service_descriptor_count = reserved_count;

								service_loop_length -= (pstServiceDescription->descriptors_loop_length + 5);
								N++;
							}
						}
						psdt_section->service_count = N;

						psdt_section->CRC_32 = BITS_get(&bs, 32);
						assert(psdt_section->CRC_32 == CRC_32_encoded);			//再次校验，检验解析过程中指针偏移是否有错
					}
					else
					{
						//rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
				else
				{
					//rtcode = SECTION_PARSE_SYNTAX_ERROR;
				}
			}
			else
			{
				//rtcode = SECTION_PARSE_SYNTAX_ERROR;						//table_id解析错误
			}
		}

		if (psdt_section->CRC_32_recalculated != psdt_section->CRC_32)
		{
			rtcode = SECTION_PARSE_CRC_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;						//输入参数错误
	}

	return rtcode;
}


