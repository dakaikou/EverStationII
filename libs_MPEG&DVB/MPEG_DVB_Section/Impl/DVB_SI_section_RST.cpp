#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../Include/DVB_SI_Utilities.h"
#include "../Include/DVB_SI_section.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
int DVB_SI_RST_DecodeSection(uint8_t *buf, int length, running_status_section_t* prst_section)
{
	S32	 rtcode = SECTION_PARSE_NO_ERROR;

	S32	 N = 0;
	S32	 loop_length;
	U8* ptemp;

	if ((buf != NULL) && (prst_section != NULL))
	{
		memset(prst_section, 0x00, sizeof(running_status_section_t));

		prst_section->table_id = *buf++;

		prst_section->section_syntax_indicator = (*buf & 0x80) >> 7;
		prst_section->reserved_future_use = (*buf & 0x40) >> 6;
		prst_section->reserved = (*buf & 0x30) >> 4;


		prst_section->section_length = (*buf++ & 0x0f) << 8;
		prst_section->section_length |= *buf++;

		loop_length = prst_section->section_length;
		ptemp = buf;
		buf += loop_length;
			
		while (loop_length > 0)
		{
			prst_section->astRunStatus[N].transport_stream_id = (*ptemp++) << 8;
			prst_section->astRunStatus[N].transport_stream_id |= *ptemp++;

			prst_section->astRunStatus[N].original_network_id = (*ptemp++) << 8;
			prst_section->astRunStatus[N].original_network_id |= *ptemp++;

			prst_section->astRunStatus[N].service_id = (*ptemp++) << 8;
			prst_section->astRunStatus[N].service_id |= *ptemp++;

			prst_section->astRunStatus[N].event_id = (*ptemp++) << 8;
			prst_section->astRunStatus[N].event_id |= *ptemp++;

			prst_section->astRunStatus[N].reserved_future_use = (*ptemp & 0xF8) >> 3;
			prst_section->astRunStatus[N].running_status = *ptemp++ & 0x07;

			loop_length -= 9;
			N ++;

			if (N >= MAX_STREAMS_PER_NETWORK)
			{
				break;
			}
		}

		prst_section->N = N;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


