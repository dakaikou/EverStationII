#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#include "../../Include/Mpeg2_DSMCC_Descriptor.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "HAL\HAL_Sys\Include\INTTYPES.H"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

//DSM-CC  0x02
int MPEG2_DSMCC_decode_name_descriptor(uint8_t* buf, int length, name_descriptor_t* pname_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;

	if ((buf != NULL) && (length >= 2) && (pname_descriptor != NULL))
	{
		ptemp = buf;

		pname_descriptor->descriptor_tag = *ptemp++;
		pname_descriptor->descriptor_length = *ptemp++;

		if (pname_descriptor->descriptor_length > 0)
		{
			memcpy(pname_descriptor->text_char, ptemp, pname_descriptor->descriptor_length);
			ptemp += pname_descriptor->descriptor_length;
			pname_descriptor->text_char[pname_descriptor->descriptor_length] = '\0';
		}
		else
		{
			memset(pname_descriptor->text_char, 0x00, sizeof(pname_descriptor->text_char));
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//DSM-CC  0x06
int MPEG2_DSMCC_decode_location_descriptor(uint8_t* buf, int length, location_descriptor_t* plocation_descriptor)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;

	if ((buf != NULL) && (length > 2) && (plocation_descriptor != NULL))
	{
		ptemp = buf;

		plocation_descriptor->descriptor_tag = *ptemp++;
		plocation_descriptor->descriptor_length = *ptemp++;

		if (plocation_descriptor->descriptor_length == 1)
		{
			plocation_descriptor->location_tag = *ptemp++;
		}
		else
		{
			rtcode = SECTION_PARSE_SYNTAX_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}



