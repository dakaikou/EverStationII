#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"
#include "HAL/HAL_BitStream/Include/HALForBitStream.h"

int MPEG2_PSI_PAT_DecodeSection(uint8_t *section_buf, int section_size, program_association_section_t* ppat_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	if ((section_buf != NULL) && 
		((section_size >= MPEG2_PSI_PAT_SECTION_MIN_SIZE) && (section_size <= MPEG2_PSI_PAT_SECTION_MAX_SIZE)) &&
		(ppat_section != NULL))
	{
		//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
		//这是内部处理的策略问题，不是必须这么做的
		unsigned int CRC_32_verify = Encode_CRC_32(section_buf, section_size - 4);
		unsigned int CRC_32_code = (section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1];

		//if (CRC_32_verify == CRC_32_code)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			memset(ppat_section, 0x00, sizeof(program_association_section_t));

			ppat_section->table_id = BITS_get(&bs, 8);

			ppat_section->section_syntax_indicator = BITS_get(&bs, 1);
			ppat_section->reserved_future_use = BITS_get(&bs, 1);
			ppat_section->reserved0 = BITS_get(&bs, 2);
			ppat_section->section_length = BITS_get(&bs, 12);

			ppat_section->transport_stream_id = BITS_get(&bs, 16);

			ppat_section->reserved1 = BITS_get(&bs, 2);
			ppat_section->version_number = BITS_get(&bs, 5);
			ppat_section->current_next_indicator = BITS_get(&bs, 1);

			ppat_section->section_number = BITS_get(&bs, 8);

			ppat_section->last_section_number = BITS_get(&bs, 8);

			int  N = 0;
			int  program_loop_length = ppat_section->section_length - 9;
			if (program_loop_length > 0)
			{
				uint8_t* loop_start_ptr = bs.p_cur;

				BITS_byteSkip(&bs, program_loop_length);

				BITS_t program_loop_bs;
				BITS_map(&program_loop_bs, loop_start_ptr, program_loop_length);

				PROGRAM_MAP_DESCRIPTION_t* pProgram = ppat_section->astProgramMaps;
				while ((program_loop_length >= 4) && (N < MAX_PROGRAMS_PER_PAT_SECTION))		//N为啥需要小于64？ chendelin  2018/5/19
				{
					uint8_t* program_map_start_ptr = program_loop_bs.p_cur;

					pProgram->program_number = BITS_get(&program_loop_bs, 16);

					pProgram->reserved = BITS_get(&program_loop_bs, 3);
					pProgram->program_map_PID = BITS_get(&program_loop_bs, 13);

					program_loop_length -= 4;
					N++;
					pProgram++;
				}
			}
			ppat_section->program_map_count = N;

			unsigned int CRC_32 = BITS_get(&bs, 32);
			assert(CRC_32 == CRC_32_code);			//再次校验，检验解析过程中指针偏移是否有错
			ppat_section->CRC_32 = CRC_32_code;
			ppat_section->CRC_32_verify = CRC_32_verify;

			if (CRC_32_verify != CRC_32_code)
			{
				rtcode = SECTION_PARSE_CRC_ERROR;
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




