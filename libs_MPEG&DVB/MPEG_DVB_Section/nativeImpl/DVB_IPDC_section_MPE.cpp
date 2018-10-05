#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../nativeInclude/DVB_IPDC_section.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "libs_Math/Include/CRC_32.h"

/////////////////////////////////////////////
int DVB_IPDC_MPE_DecodeSection(uint8_t *buf, int length, datagram_section_t* pdatagram_section)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
//	S32  N;
	S32  loop_length;
	U8*  ptemp;

	if ((buf != NULL) && (pdatagram_section != NULL) && (length > 4))
	{
		memset(pdatagram_section, 0x00, sizeof(datagram_section_t));

		pdatagram_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);

		pdatagram_section->table_id = *buf++;

		pdatagram_section->section_syntax_indicator = (*buf & 0x80) >> 7;
		pdatagram_section->private_indicator = (*buf & 0x40) >> 6;
		pdatagram_section->reserved0 = (*buf & 0x30) >> 4;

		pdatagram_section->section_length = (*buf++ & 0x0f);
		pdatagram_section->section_length <<= 8;
		pdatagram_section->section_length |= *buf++;

		pdatagram_section->MAC_address_6 = *buf++;
		pdatagram_section->MAC_address_5 = *buf++;

		pdatagram_section->reserved1 = (*buf & 0xc0) >> 6;
		pdatagram_section->payload_scrambling_control = (*buf & 0x30) >> 4;
		pdatagram_section->address_scrambling_control = (*buf & 0x0c) >> 2;
		pdatagram_section->LLC_SNAP_flag = (*buf & 0x02) >> 1;
		pdatagram_section->current_next_indicator = (*buf++ & 0x01);

		pdatagram_section->section_number = *buf++;
		pdatagram_section->last_section_number = *buf++;

		pdatagram_section->MAC_address_4 = *buf++;
		pdatagram_section->MAC_address_3 = *buf++;
		pdatagram_section->MAC_address_2 = *buf++;
		pdatagram_section->MAC_address_1 = *buf++;

		loop_length = pdatagram_section->section_length - 9 - 4;			//exclude CRC32
		ptemp = buf;

		if (pdatagram_section->LLC_SNAP_flag == 1)
		{
		}
		else
		{
			//get the IP data
			FILE* fp = NULL;
			fopen_s(&fp, "e:\\ip_data.dat", "wb");
			if (fp != NULL)
			{
				fwrite(ptemp, 1, loop_length, fp);
				fclose(fp);
			}
		}

		buf += loop_length;

		pdatagram_section->checksum = 0x00000000;
		pdatagram_section->CRC_32 = 0x00000000;

		if (pdatagram_section->section_syntax_indicator == 0)
		{
			pdatagram_section->checksum = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
		}
		else
		{
			pdatagram_section->CRC_32 = (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
		}

		buf += 4;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

