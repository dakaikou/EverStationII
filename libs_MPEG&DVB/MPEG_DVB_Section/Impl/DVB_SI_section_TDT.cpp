#include <string.h>
#include <assert.h>

#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"
#include "../Include/DVB_SI_Utilities.h"

#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_SI_TDT_DecodeSection(uint8_t *buf, int length, time_date_section_t* ptdt_section)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length >= DVB_SI_TDT_SECTION_MIN_SIZE) && (ptdt_section != NULL))
	{
		memset(ptdt_section, 0x00, sizeof(time_date_section_t));

		ptdt_section->table_id = *buf++;
		ptdt_section->section_syntax_indicator = (*buf & 0x80) >> 7;
		ptdt_section->reserved_future_use = (*buf & 0x40) >> 6;
		ptdt_section->reserved = (*buf & 0x30) >> 4;

		ptdt_section->section_length = (*buf++ & 0x0f) << 8;
		ptdt_section->section_length |= *buf++;

		ptdt_section->UTC_time = *buf++;
		ptdt_section->UTC_time <<= 8;
		ptdt_section->UTC_time |= *buf++;
		ptdt_section->UTC_time <<= 8;
		ptdt_section->UTC_time |= *buf++;
		ptdt_section->UTC_time <<= 8;
		ptdt_section->UTC_time |= *buf++;
		ptdt_section->UTC_time <<= 8;
		ptdt_section->UTC_time |= *buf++;
		//memcpy(ptdt_section->UTC_time, buf, 5);
		buf += 5;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

