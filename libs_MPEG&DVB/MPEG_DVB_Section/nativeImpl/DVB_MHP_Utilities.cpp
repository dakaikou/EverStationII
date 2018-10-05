#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../nativeInclude/DVB_SI_Utilities.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

int DVB_MHP_NumericCoding2Text_ApplicationServiceBoundFlag(int service_bound_flag, char* pszText, int strSize)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	switch (service_bound_flag)
	{
	case 0:
		strcpy_s(pszText, strSize, "not associated with the current service");
		break;
	case 1:
		strcpy_s(pszText, strSize, "only associated with the current service");
		break;
	}

	return rtcode;
}

int DVB_MHP_NumericCoding2Text_ApplicationVisibility(int visibility_code, char* pszText, int strSize)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if ((pszText != NULL) && (strSize < 68))
	{
		switch (visibility_code)
		{
		case 0:
			strcpy_s(pszText, strSize, "shall not be visible either to applications or to users");
			break;
		case 1:
			strcpy_s(pszText, strSize, "shall not be visible to users but shall be visible to applications");
			break;
		case 2:
			strcpy_s(pszText, strSize, "reserved_future_use");
			break;
		case 3:
			strcpy_s(pszText, strSize, "can be visible to users and shall be visible to applications");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int AIT_ApplicationControlCodeLookup(uint16_t application_type, uint8_t application_control_code, char* pszText, int size)
{
	int 	rtcode = SECTION_PARSE_NO_ERROR;

	if ((pszText != NULL) && (application_type < 0x8000))
	{
		if (application_type == 0x0001)		//DVB-J application
		{
			switch (application_control_code)
			{
			case 0x00:
				strcpy_s(pszText, size, "reserved_future_usse");
				break;
			case 0x01:
				strcpy_s(pszText, size, "AUTOSTART");
				break;
			case 0x02:
				strcpy_s(pszText, size, "PRESENT");
				break;
			case 0x03:
				strcpy_s(pszText, size, "DESTROY");
				break;
			case 0x04:
				strcpy_s(pszText, size, "KILL");
				break;
			case 0x05:
				strcpy_s(pszText, size, "reserved_future_usse");
				break;
			case 0x06:
				strcpy_s(pszText, size, "REMOTE");
				break;
			default:
				strcpy_s(pszText, size, "reserved_future_use");
				break;
			}
		}
		else if (application_type == 0x0002)	//DVB-H application
		{
			switch (application_control_code)
			{
			case 0x00:
				strcpy_s(pszText, size, "reserved_future_usse");
				break;
			case 0x01:
				strcpy_s(pszText, size, "AUTOSTART");
				break;
			case 0x02:
				strcpy_s(pszText, size, "PRESENT");
				break;
			case 0x03:
				strcpy_s(pszText, size, "DESTROY");
				break;
			case 0x04:
				strcpy_s(pszText, size, "KILL");
				break;
			case 0x05:
				strcpy_s(pszText, size, "PREFETCH");
				break;
			case 0x06:
				strcpy_s(pszText, size, "REMOTE");
				break;
			default:
				strcpy_s(pszText, size, "reserved_future_use");
				break;
			}
		}
		else
		{
			strcpy_s(pszText, size, "unknown");
		}
	}

	return rtcode;
}

int DVB_MHP_NumericCoding2Text_ApplicationTransportProtocolID(uint16_t protocol_id, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (protocol_id)
		{
		case 0x0000:
			strcpy_s(pszText, size, "reserved_future_usse");
			break;
		case 0x0001:
			strcpy_s(pszText, size, "MHP Object Carousel(annex B)");
			break;
		case 0x0002:
			strcpy_s(pszText, size, "IP via DVB MPE");
			break;
		case 0x0003:
			strcpy_s(pszText, size, "Transport via HTTP");
			break;
		default:

			if ((protocol_id >= 0x0004) && (protocol_id <= 0x00FF))
			{
				strcpy_s(pszText, size, "Reserved for use by DVB");
			}
			else
			{
				strcpy_s(pszText, size, "Subject to registration in ETSI TR 101 162");
			}
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

// Module name:		ApplicationTypeLookup()
// Description:		Convert ushort style of application_type to char*
// Parameter:		application_type---the application's type indicated in the AIT
//					buffer---the char* pointer contanining the application type description
// Return value:	S32
// Copyright (c) 2009.3.18 chendelin All rights reserved
int AIT_ApplicationTypeLookup(uint16_t application_type, char* pszText, int size)
{
	int	rtcode = 0;

#if	CHINESE_VERSION
	if ((pszText != NULL) && (application_type < 0x8000))
	{
		switch (application_type)
		{
		case 0x0000:
			strcpy_s(pszText, size, "保留将来使用");
			break;
		case 0x0001:
			strcpy_s(pszText, size, "DVB-J 应用");
			break;
		case 0x0002:
			strcpy_s(pszText, size, "DVB-HTML 应用");
			break;
		default:
			strcpy_s(pszText, size, "未在DVB注册");
			break;
		}
	}
#else
	if ((pszText != NULL) && (application_type < 0x8000))
	{
		switch (application_type)
		{
		case 0x0000:
			strcpy_s(pszText, size, "reserved_future_use");
			break;
		case 0x0001:
			strcpy_s(pszText, size, "DVB-J application");
			break;
		case 0x0002:
			strcpy_s(pszText, size, "DVB-HTML application");
			break;
		default:
			strcpy_s(pszText, size, "subject to registration with DVB");
			break;
		}
	}
#endif

	return rtcode;
}
