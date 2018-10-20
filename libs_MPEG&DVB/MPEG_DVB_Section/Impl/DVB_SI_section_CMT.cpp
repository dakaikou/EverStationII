#include <string.h>
#include <stdlib.h>

#include "../../Include/DVB_SI_section.h"
#include "../../Include/DVB_table_id.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

int DVB_SI_CMT_DecodeSection(uint8_t *section_buf, int section_size, CA_message_section_t* pcmt_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((section_buf != NULL) && 
		((section_size >= DVB_SI_CMT_SECTION_MIN_SIZE) && (section_size <= DVB_SI_CMT_SECTION_MAX_SIZE)) &&
		(pcmt_section != NULL))
	{
		memset(pcmt_section, 0x00, sizeof(CA_message_section_t));

		BITS_t bs;
		BITS_map(&bs, section_buf, section_size);

		pcmt_section->table_id = BITS_get(&bs, 8);

		pcmt_section->section_syntax_indicator = BITS_get(&bs, 1);
		pcmt_section->DVB_reserved = BITS_get(&bs, 1);
		pcmt_section->ISO_reserved = BITS_get(&bs, 2);
		pcmt_section->CA_section_length = BITS_get(&bs, 12);
		
		pcmt_section->CA_data_byte = bs.p_cur;
		BITS_byteSkip(&bs, pcmt_section->CA_section_length);
		//BITS_byteCopy(pcmt_section->CA_data_byte, sizeof(pcmt_section->CA_data_byte), &bs, pcmt_section->CA_section_length);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


