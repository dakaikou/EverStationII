#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../../Stream_AL/BitStream.h"

#include "../Include/DVB_SI_Utilities.h"
#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

/////////////////////////////////////////////
int DVB_SI_RST_DecodeSection(uint8_t *section_buf, int section_size, running_status_section_t* prst_section)
{
	int	 rtcode = SECTION_PARSE_NO_ERROR;

	int		 N = 0;
	int		 loop_length;

	if ((section_buf != NULL) && 
		((section_size >= DVB_SI_RST_SECTION_MIN_SIZE) && (section_size <= DVB_SI_RST_SECTION_MAX_SIZE)) &&
		(prst_section != NULL))
	{
		memset(prst_section, 0x00, sizeof(running_status_section_t));

		BITS_t bs;
		BITS_map(&bs, section_buf, section_size);

		prst_section->table_id = BITS_get(&bs, 8);

		prst_section->section_syntax_indicator = BITS_get(&bs, 1);
		prst_section->reserved_future_use = BITS_get(&bs, 1);
		prst_section->reserved = BITS_get(&bs, 2);
		prst_section->section_length = BITS_get(&bs, 12);

		if (prst_section->section_length > 0)
		{
			loop_length = prst_section->section_length;

			while ((loop_length >= 9) && (N < MAX_STREAMS_PER_NETWORK))
			{
				prst_section->astRunStatus[N].transport_stream_id = BITS_get(&bs, 16);

				prst_section->astRunStatus[N].original_network_id = BITS_get(&bs, 16);

				prst_section->astRunStatus[N].service_id = BITS_get(&bs, 16);

				prst_section->astRunStatus[N].event_id = BITS_get(&bs, 16);

				prst_section->astRunStatus[N].reserved_future_use = BITS_get(&bs, 5);
				prst_section->astRunStatus[N].running_status = BITS_get(&bs, 3);

				loop_length -= 9;
				N++;
			}
			prst_section->N = N;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


