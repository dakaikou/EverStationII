#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "../Include/Mpeg2_TS_PID.h"
#include "../Include/Mpeg2_TS_Utilities.h"
#include "../Include/Mpeg2_TS_ErrorCode.h"

/////////////////////////////////////////////
int MPEG2_TS_NumericCoding2Text_PID(uint16_t PID, char* pszText, int size)
{
	int		rtcode = TSPACKET_PARSE_UNKNOWN_ERROR;

	if (pszText != NULL)
	{
		if (PID == PAT_PID)
		{
			strcpy_s(pszText, size, "PAT");
		}
		else if (PID == CAT_PID)
		{
			strcpy_s(pszText, size, "CAT");
		}
		else if (PID == TSDT_PID)
		{
			strcpy_s(pszText, size, "TSDT");
		}
		else if (PID >= 0x0003 && PID <= 0x000F)
		{
			strcpy_s(pszText, size, "reserved");
		}
		//	else if( PID == 0x0010 )
		//	{
		//		strcpy_s( buffer, size, "NIT/ST" );
		//	}
		else if (PID == SDT_BAT_PID)
		{
			strcpy_s(pszText, size, "SDT/BAT/ST");
		}
		else if (PID == EIT_PID)
		{
			strcpy_s(pszText, size, "EIT/ST");
		}
		else if (PID == RST_PID)
		{
			strcpy_s(pszText, size, "RST/ST");
		}
		else if (PID == TDT_TOT_PID)
		{
			strcpy_s(pszText, size, "TDT/TOT/ST");
		}
		else if (PID == 0x0015)
		{
			strcpy_s(pszText, size, "network synchronization");
		}
		else if (PID >= 0x0016 && PID <= 0x001D)
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if (PID == DIT_PID)
		{
			strcpy_s(pszText, size, "DIT");
		}
		else if (PID == SIT_PID)
		{
			strcpy_s(pszText, size, "SIT");
		}
		else if (PID == NULL_PID)
		{
			strcpy_s(pszText, size, "null packet");
		}
		else
		{
			strcpy_s(pszText, size, "unknown");
		}

		rtcode = TSPACKET_PARSE_NO_ERROR;
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_TS_GetPayloadClassByPID(uint16_t PID)
{
	int  class_type = TSPAYLOAD_CLASS_UNKNOWN;

	if (PID == PAT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == CAT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == TSDT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == SDT_BAT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == EIT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == RST_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == TDT_TOT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	//	else if (PID == 0x0015)
	//	{
	//		class_type = PID_CLASS_SECTION_SURE;
	////		strcpy_s(pszText, size, "network synchronization");
	//	}
	//	else if (PID >= 0x0016 && PID <= 0x001D)
	//	{
	//		class_type = PID_CLASS_SECTION_SURE;
	//		strcpy_s(pszText, size, "reserved for future use");
	//	}
	else if (PID == DIT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else if (PID == SIT_PID)
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	//else if (PID == 0x1FFF)
	//{
	//	class_type = PID_CLASS_NULLPACKET;
	//	//strcpy_s(pszText, size, "null packet");
	//}

	return class_type;
}


int MPEG2_TS_NumericCoding2Text_transport_scrambling_control(uint8_t transport_scrambling_control_code, char* pszText, int size)
{
	int		rtcode = TSPACKET_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (transport_scrambling_control_code)
		{
		case 0b00:
			strcpy_s(pszText, size, "Î´¼ÓÈÅ");
			break;
		case 0b01:
			strcpy_s(pszText, size, "±£Áô");
			break;
		case 0b10:
			strcpy_s(pszText, size, "Å¼ÃÜÔ¿¼ÓÈÅ");
			break;
		case 0b11:
			strcpy_s(pszText, size, "ÆæÃÜÔ¿¼ÓÈÅ");
			break;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_TS_NumericCoding2Text_adaptation_field_control(uint8_t adaptation_field_control_code, char* pszText, int size)
{
	int		rtcode = TSPACKET_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (adaptation_field_control_code)
		{
		case 0b00:
			strcpy_s(pszText, size, "±£Áô");
			break;
		case 0b01:
			strcpy_s(pszText, size, "½öÓÐ¾»ºÉ");
			break;
		case 0b10:
			strcpy_s(pszText, size, "½öÓÐÊÊÅä¶Î");
			break;
		case 0b11:
			strcpy_s(pszText, size, "ÊÊÅä¶Î+¾»ºÉ");
			break;
		}
	}
	else
	{
		rtcode = TSPACKET_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
