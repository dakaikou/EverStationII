#include <string.h>
#include <assert.h>

#include "../../Common/BitStream.h"

#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"
#include "../Include/DVB_SI_Utilities.h"

/////////////////////////////////////////////
int DVB_SI_TDT_DecodeSection(uint8_t *section_buf, int section_size, time_date_section_t* ptdt_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((section_buf != NULL) &&
		((section_size >= DVB_SI_TDT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_TDT_SECTION_MAX_SIZE))&& 
		(ptdt_section != NULL))
	{
		memset(ptdt_section, 0x00, sizeof(time_date_section_t));

		BITS_t bs;
		BITS_map(&bs, section_buf, section_size);

		ptdt_section->table_id = BITS_get(&bs, 8);

		ptdt_section->section_syntax_indicator = BITS_get(&bs, 1);
		ptdt_section->reserved_future_use = BITS_get(&bs, 1);
		ptdt_section->reserved = BITS_get(&bs, 2);
		ptdt_section->section_length = BITS_get(&bs, 12);

		ptdt_section->UTC_time = BITS_x64_get(&bs, 40);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

