#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../Include/Mpeg2_DSMCC_Utilities.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_Sys\Include\INTTYPES.H"

int MPEG2_DSMCC_NumericCoding2Text_dsmccType(uint8_t dsmccType, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (dsmccType)
		{
		case 0x00:
			strcpy_s(pszText, size, "Reserved");
			break;
		case 0x01:
			strcpy_s(pszText, size, "User-to-Network configuration message");
			break;
		case 0x02:
			strcpy_s(pszText, size, "User-to-Network session message");
			break;
		case 0x03:
			strcpy_s(pszText, size, "Download message");
			break;
		case 0x04:
			strcpy_s(pszText, size, "SDB Channel Change Protocol message");
			break;
		case 0x05:
			strcpy_s(pszText, size, "User-to-Network pass-thru message");
			break;
		default:
			if ((dsmccType >= 0x06) && (dsmccType <= 0x7F))
			{
				strcpy_s(pszText, size, "Reserved");
			}
			else if ((dsmccType >= 0x80) && (dsmccType <= 0xFF))
			{
				strcpy_s(pszText, size, "User defined");
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

int MPEG2_DSMCC_NumericCoding2Text_messageId(uint16_t messageId, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (messageId)
		{
		case 0x1001:
			strcpy_s(pszText, size, "DownloadInfoRequest");
			break;
		case 0x1002:
			strcpy_s(pszText, size, "DownloadInfoIndication");
			break;
		case 0x1003:
			strcpy_s(pszText, size, "DownloadDataBlock");
			break;
		case 0x1004:
			strcpy_s(pszText, size, "DownloadDataRequest");
			break;
		case 0x1005:
			strcpy_s(pszText, size, "DownloadCancel");
			break;
		case 0x1006:
			strcpy_s(pszText, size, "DownloadServerInitiate");
			break;
		default:
			strcpy_s(pszText, size, "unknown message");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_DSMCC_NumericCoding2Text_byteOrder(uint8_t byte_order, char* pszText, int size)
{
	int		rtcode = 0;

	if (pszText != NULL)
	{
		switch (byte_order)
		{
		case 0x00:
			strcpy_s(pszText, size, "big-endian");
			break;
		case 0x01:
			strcpy_s(pszText, size, "little-endian");
			break;
		default:
			strcpy_s(pszText, size, "undefined");
			break;
		}
	}

	return rtcode;
}

int MPEG2_DSMCC_NumericCoding2Text_tapUse(uint16_t tapUse, char* pszText, int size)
{
	int		rtcode = 0;

	if (pszText != NULL)
	{
		switch (tapUse)
		{
		case 0x0016:
			strcpy_s(pszText, size, "BIOP_DELIVERY_PARA_USE");
			break;
		case 0x0017:
			strcpy_s(pszText, size, "BIOP_OBJECT_USE");
			break;
		default:
			strcpy_s(pszText, size, "undefined");
			break;
		}
	}

	return rtcode;
}

int MPEG2_DSMCC_NumericCoding2Text_tagId(uint32_t tag_id, char* pszText, int size)
{
	S32		rtcode = 0;

	if (pszText != NULL)
	{
		switch (tag_id)
		{
		case 0x49534F05:
			strcpy_s(pszText, size, "TAG_LITE_OPTIONS(Lite Options Profile Body)");
			break;
		case 0x49534F06:
			strcpy_s(pszText, size, "TAG_BIOP(BIOP ProfileBody)");
			break;
		case 0x49534F40:
			strcpy_s(pszText, size, "TAG_ConnBinder");
			break;
		case 0x49534F46:
			strcpy_s(pszText, size, "TAG_ServiceLocation");
			break;
		case 0x49534F50:
			strcpy_s(pszText, size, "TAG_ObjectLocation");
			break;
		default:
			strcpy_s(pszText, size, "undefined");
			break;
		}
	}

	return rtcode;
}

int MPEG2_DSMCC_NumericCoding2Text_bindingType(uint8_t bindingType, char* pszText, int size)
{
	int		rtcode = 0;

	if (pszText != NULL)
	{
		switch (bindingType)
		{
		case 0x01:
			strcpy_s(pszText, size, "nobject");
			break;
		case 0x02:
			strcpy_s(pszText, size, "ncontext");
			break;
		default:
			strcpy_s(pszText, size, "undefined");
			break;
		}
	}

	return rtcode;
}

