#include <string.h>
#include <stdlib.h>

#include "../Include/DVB_SI_section.h"
#include "../Include/DVB_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

int DVB_SI_CMT_DecodeSection(uint8_t *buf, int length, CA_message_section_t* pcmt_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	int		copy_length;

	if ((buf != NULL) && (pcmt_section != NULL) && (length > 4))
	{
		memset(pcmt_section, 0x00, sizeof(CA_message_section_t));

		pcmt_section->table_id = *buf++;

		pcmt_section->section_syntax_indicator = (*buf & 0x80) >> 7;
		pcmt_section->DVB_reserved = (*buf & 0x40) >> 6;
		pcmt_section->ISO_reserved = (*buf & 0x30) >> 4;
	
		pcmt_section->CA_section_length = (*buf++ & 0x0f) << 8;
		pcmt_section->CA_section_length |= *buf++;
		
		copy_length = MIN(pcmt_section->CA_section_length, 64);
		if (copy_length > 0)
		{
			memcpy(pcmt_section->CA_data_byte, buf, copy_length); 
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


