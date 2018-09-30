#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

int MPEG2_PSI_PAT_DecodeSection(uint8_t *buf, int length, program_association_section_t* ppat_section)
{
	S32	 rtcode = SECTION_PARSE_NO_ERROR;
	S32  N;
	S32  loop_length;
	U8*  ptemp;

	PROGRAM_MAP_DESCRIPTION_t* pProgram;

	if ((buf != NULL) && (length >= MPEG2_PSI_PAT_SECTION_MIN_SIZE) && (length <= MPEG2_PSI_PAT_SECTION_MAX_SIZE) && (ppat_section != NULL))
	{
		memset(ppat_section, 0x00, sizeof(program_association_section_t));

		//如果外部没有传入参考的CRC32校验码，内部再计算一下，以便判断是否有CRC校验错误
		//这是内部处理的策略问题，不是必须这么做的
		if (ppat_section->CRC_32_verify == 0x00000000)
		{
			ppat_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		}

		ppat_section->CRC_32 = (buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1];

		if (ppat_section->CRC_32 == ppat_section->CRC_32_verify)
		{
			ppat_section->table_id = *buf++;
			
			ppat_section->section_syntax_indicator = (*buf & 0x80) >> 7;
			ppat_section->reserved_future_use = (*buf & 0x40) >> 6;
			ppat_section->reserved0 = (*buf & 0x30) >> 4;

			ppat_section->section_length = (*buf++ & 0x0f) << 8;
			ppat_section->section_length |= *buf++;

			ppat_section->transport_stream_id = (*buf++) << 8;
			ppat_section->transport_stream_id |= *buf++;

			ppat_section->reserved1 = (*buf & 0xC0) >> 6;
			ppat_section->version_number = (*buf & 0x3E) >> 1;
			ppat_section->current_next_indicator = (*buf++ & 0x01);

			ppat_section->section_number = *buf++;
			ppat_section->last_section_number = *buf++;

			loop_length = ppat_section->section_length - 9;

			ptemp = buf;
			buf += loop_length;

			N = 0;
			pProgram = ppat_section->astProgram;
			while (loop_length > 0)
			{
				pProgram->program_number = *ptemp++;
				pProgram->program_number <<= 8;
				pProgram->program_number |= *ptemp++;

				pProgram->reserved = (*ptemp & 0xE0) >> 5;

				pProgram->program_map_PID = (*ptemp++ & 0x1F);
				pProgram->program_map_PID <<= 8;
				pProgram->program_map_PID |= *ptemp++;

				loop_length -= 4;
				N++;
				pProgram ++;
			}

			ppat_section->N = N;
		}
		else
		{
			rtcode = SECTION_PARSE_CRC_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}





