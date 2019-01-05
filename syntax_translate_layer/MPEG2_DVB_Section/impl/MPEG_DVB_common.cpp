#include <string.h>

#include "../../Common/BitStream.h"

#include "../Include/MPEG2_DVB_Common.h"
#include "../Include/MPEG2_DVB_ErrorCode.h"

#include "../Include/DVB_SI_section.h"
#include "../Include/DVB_MHP_section.h"
#include "../Include/DVB_IPDC_section.h"
#include "../Include/DVB_table_id.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
/////////////////////////////////////////////

//对section数据做个简单的体检，以确定是不是section数据
int MPEG_DVB_PVT_DecodeSection(uint8_t *section_buf, int section_size, private_section_t* pprivate_section)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	//uint8_t*		porigin = buf;

	if ((section_buf != NULL) && (section_size >= 3) && (section_size <= 4096) && (pprivate_section != NULL))
	{
		memset(pprivate_section, 0x00, sizeof(private_section_t));

		uint8_t* buf = section_buf;

		pprivate_section->table_id = *buf++;

		pprivate_section->section_syntax_indicator = (*buf & 0x80) >> 7;
		pprivate_section->private_indicator = (*buf & 0x40) >> 6;
		pprivate_section->reserved0 = (*buf & 0x30) >> 4;
	
		pprivate_section->section_length = (*buf++ & 0x0f) << 8;
		pprivate_section->section_length |= *buf++;
	
		if ((pprivate_section->section_length > 0) && (pprivate_section->section_length <= (section_size - 3)))		//section特征1
		{
			//pprivate_section->private_data_length = 0;
			//memset(pprivate_section->private_data_byte, 0x00, sizeof(pprivate_section->private_data_byte));

			if (pprivate_section->section_syntax_indicator == 0)
			{
				pprivate_section->private_data_length = pprivate_section->section_length;
				memcpy(pprivate_section->private_data_byte, buf, pprivate_section->private_data_length);
			}
			else
			{
				if ((pprivate_section->table_id != TABLE_ID_TDT) &&
					(pprivate_section->table_id != TABLE_ID_TOT) &&
					(pprivate_section->table_id != TABLE_ID_RST))
				{
					pprivate_section->table_id_extension = *buf++;
					pprivate_section->table_id_extension <<= 8;
					pprivate_section->table_id_extension |= *buf++;

					pprivate_section->reserved1 = (*buf & 0xC0) >> 6;
					pprivate_section->version_number = (*buf & 0x3E) >> 1;
					pprivate_section->current_next_indicator = (*buf++ & 0x01);

					pprivate_section->section_number = *buf++;
					pprivate_section->last_section_number = *buf++;

					pprivate_section->private_data_length = pprivate_section->section_length - 5;
					if (pprivate_section->private_data_length > 0)
					{
						memcpy(pprivate_section->private_data_byte, buf, pprivate_section->private_data_length);
					}

					if (pprivate_section->section_number <= pprivate_section->last_section_number)			//section特征2
					{
						//TDT±í??óDCRC×???
						//if (pprivate_section->table_id != TABLE_ID_TDT)
						//{
						//	pprivate_section->CRC_32_verify = Encode_CRC_32(porigin, length - 4);
						//	pprivate_section->CRC_32 = (porigin[length - 4] << 24) | (porigin[length - 3] << 16) | (porigin[length - 2] << 8) | porigin[length - 1];

						//	if (pprivate_section->CRC_32 == pprivate_section->CRC_32_verify)
						//	{

						//	}
						//	else
						//	{
						//		rtcode = SECTION_PARSE_CRC_ERROR;
						//	}
						//}
					}
					else
					{
						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
			}
		}
		else
		{
			rtcode = SECTION_PARSE_LENGTH_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


int MPEG_DVB_GetSectionMinMaxLength(uint8_t table_id, int* pmin_length, int* pmax_length)
{
	int		rtcode = SECTION_PARSE_UNKNOWN_ERROR;

	if ((pmin_length != NULL) && (pmax_length != NULL))
	{
		switch (table_id)
		{
		case TABLE_ID_PAT:
			*pmin_length = MPEG2_PSI_PAT_SECTION_MIN_SIZE;
			*pmax_length = MPEG2_PSI_PAT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_PMT:
			*pmin_length = MPEG2_PSI_PMT_SECTION_MIN_SIZE;
			*pmax_length = MPEG2_PSI_PMT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_CAT:
			*pmin_length = MPEG2_PSI_CAT_SECTION_MIN_SIZE;
			*pmax_length = MPEG2_PSI_CAT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_NIT_ACTUAL:
		case TABLE_ID_NIT_OTHER:
			*pmin_length = DVB_SI_NIT_SECTION_MIN_SIZE;
			*pmax_length = DVB_SI_NIT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_TOT:
			*pmin_length = DVB_SI_TOT_SECTION_MIN_SIZE;
			*pmax_length = DVB_SI_TOT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_TDT:
			*pmin_length = DVB_SI_TDT_SECTION_MIN_SIZE;
			*pmax_length = DVB_SI_TDT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_RST:
			*pmin_length = DVB_SI_RST_SECTION_MIN_SIZE;
			*pmax_length = DVB_SI_RST_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_SDT_ACTUAL:
		case TABLE_ID_SDT_OTHER:
			*pmin_length = DVB_SI_SDT_SECTION_MIN_SIZE;
			*pmax_length = DVB_SI_SDT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_INT:
			*pmin_length = DVB_IPDC_INT_SECTION_MIN_SIZE;
			*pmax_length = DVB_IPDC_INT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_BAT:
			*pmin_length = DVB_SI_BAT_SECTION_MIN_SIZE;
			*pmax_length = DVB_SI_BAT_SECTION_MAX_SIZE;
			break;
		case TABLE_ID_AIT:
			*pmin_length = DVB_MHP_AIT_SECTION_MIN_SIZE;
			*pmax_length = DVB_MHP_AIT_SECTION_MAX_SIZE;
			break;
		default:
			*pmin_length = 3;
			*pmax_length = 4096;
			break;
		}

		rtcode = SECTION_PARSE_NO_ERROR;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

// Module name:		TableIdLookup()
// Parameter:		table_id---the table identifier of PSI/SI section
//					strDescription---the CString* pointer of table_id description
// Return value:	void
// Remarks:			this function is only supplyed to DVB specification
// Copyright (c) 2001.4.2 chendelin All rights reserved

//int TableIdLookup(uint8_t table_id, S8* buf)

int MPEG_DVB_NumericCoding2Text_TableID(uint16_t PID, uint8_t table_id, char* pszText, int size)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		//Note: table_id---- 8 bit
		if (table_id == 0x00)
		{
			if (PID == 0x0000)
			{
				strcpy_s(pszText, size, "PAT");
			}
		}
		else if (table_id == 0x01)
		{
			if (PID == 0x0001)
			{
				strcpy_s(pszText, size, "CAT");
			}
		}
		else if (table_id == 0x02)
		{
			strcpy_s(pszText, size, "PMT");
		}
		else if (table_id == 0x03)
		{
			if (PID == 0x0002)
			{
				strcpy_s(pszText, size, "TSDT");
			}
		}
		else if ((table_id >= 0x04) && (table_id <= 0x39))
		{
			strcpy_s(pszText, size, "reserved section");
		}
		else if (table_id == 0x3a)
		{
			strcpy_s(pszText, size, "DSM-CC MPE data");
		}
		else if (table_id == 0x3b)
		{
			strcpy_s(pszText, size, "DSM_CC U-N Messages");
		}
		else if (table_id == 0x3c)
		{
			//		strcpy_s(buf, size, "DSM_CC Download Data Messages");
			strcpy_s(pszText, size, "DSM-CC DDM");
		}
		else if (table_id == 0x3d)
		{
			strcpy_s(pszText, size, "DSM_CC Stream Descriptor");
		}
		else if (table_id == 0x3e)
		{
			strcpy_s(pszText, size, "DSM_CC private data");
		}
		else if (table_id == 0x3f)
		{
			strcpy_s(pszText, size, "DSM_CC U-N reserved");
		}
		else if (table_id == 0x40)
		{
			//		strcpy_s(buf, size, "network_information_section - actual_network" );
			strcpy_s(pszText, size, "NIT/ST Actual");
		}
		else if (table_id == 0x41)
		{
			//		strcpy_s(buf, size, "network_information_section - other_network" );
			strcpy_s(pszText, size, "NIT/ST Other");
		}
		else if (table_id == 0x42)
		{
			if (PID == 0x0011)
			{
				strcpy_s(pszText, size, "SDT - actual_transport_stream");
			}
		}
		else if ((table_id >= 0x43) && (table_id <= 0x45))
		{
			strcpy_s(pszText, size, "reserved for future use section");
		}
		else if (table_id == 0x46)
		{
			if (PID == 0x0011)
			{
				strcpy_s(pszText, size, "SDT - other_transport_stream");
			}
		}
		else if ((table_id >= 0x47) && (table_id <= 0x49))
		{
			strcpy_s(pszText, size, "reserved for future use section");
		}
		else if (table_id == 0x4A)
		{
			if (PID == 0x0011)
			{
				strcpy_s(pszText, size, "BAT");
			}
		}
		else if (table_id == 0x4B)
		{
			strcpy_s(pszText, size, "reserved for future use section");
		}
		else if (table_id == 0x4C)
		{
			strcpy_s(pszText, size, "IP/MAC notification section");
		}
		else if (table_id == 0x4D)
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if (table_id == 0x4E)
		{
			if (PID == 0x0012)
			{
				strcpy_s(pszText, size, "EIT - actual_transport_stream,present/following");
			}
		}
		else if (table_id == 0x4F)
		{
			if (PID == 0x0012)
			{
				strcpy_s(pszText, size, "EIT - other_transport_stream,present/following");
			}
		}
		else if ((table_id >= 0x50) && (table_id <= 0x5F))
		{
			if (PID == 0x0012)
			{
				strcpy_s(pszText, size, "EIT - actual_transport_stream,schedule");
			}
		}
		else if ((table_id >= 0x60) && (table_id <= 0x6F))
		{
			if (PID == 0x0012)
			{
				strcpy_s(pszText, size, "EIT - other_transport_stream,schedule");
			}
		}
		else if (table_id == 0x70)
		{
			if (PID == 0x0014)
			{
				strcpy_s(pszText, size, "TDT");
			}
		}
		else if (table_id == 0x71)
		{
			if (PID == 0x0013)
			{
				strcpy_s(pszText, size, "RST");
			}
		}
		else if (table_id == 0x72)
		{
			strcpy_s(pszText, size, "ST");
		}
		else if (table_id == 0x73)
		{
			if (PID == 0x0014)
			{
				strcpy_s(pszText, size, "TOT");
			}
		}
		else if (table_id == 0x74)
		{
			strcpy_s(pszText, size, "AIT");
		}
		else if ((table_id >= 0x75) && (table_id <= 0x7D))
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if (table_id == 0x7E)
		{
			if (PID == 0x001E)
			{
				strcpy_s(pszText, size, "DIT");
			}
		}
		else if (table_id == 0x7F)
		{
			if (PID == 0x001F)
			{
				strcpy_s(pszText, size, "SIT");
			}
		}
		else if (table_id == 0x80)
		{
			strcpy_s(pszText, size, "CA_message_section, ECM");
		}
		else if (table_id == 0x81)
		{
			strcpy_s(pszText, size, "CA_message_section, ECM");
		}
		else if ((table_id >= 0x82) && (table_id <= 0x8F))
		{
			strcpy_s(pszText, size, "CA_message_section, EMM");
		}
		else if ((table_id >= 0x90) && (table_id <= 0xFE))
		{
			strcpy_s(pszText, size, "user defined section");
		}
		else if (table_id == 0xFF)
		{
			strcpy_s(pszText, size, "reserved");
		}
	}

	return rtcode;
}

int MPEG_DVB_NumericCoding2Text_CASystemID(uint16_t CA_system_ID, char* pszText, int size)
{
	int		rtcode = 0;

	if (pszText != NULL)
	{
		if (CA_system_ID == 0x0000)
		{
			strcpy_s(pszText, size, "保留");
		}
		else if ((CA_system_ID >= 0x0001) && (CA_system_ID <= 0x00ff))
		{
			strcpy_s(pszText, size, "标准化的系统");
		}
		else if ((CA_system_ID >= 0x0100) && (CA_system_ID <= 0x01ff))
		{
			strcpy_s(pszText, size, "Canal Plus");
		}
		else if ((CA_system_ID >= 0x0400) && (CA_system_ID <= 0x04ff))
		{
			strcpy_s(pszText, size, "Eurodec");
		}
		else if ((CA_system_ID >= 0x0500) && (CA_system_ID <= 0x05ff))
		{
			strcpy_s(pszText, size, "France Telecom");
		}
		else if ((CA_system_ID >= 0x0600) && (CA_system_ID <= 0x06ff))
		{
			strcpy_s(pszText, size, "Irdeto");
		}
		else if ((CA_system_ID >= 0x0900) && (CA_system_ID <= 0x09ff))
		{
			strcpy_s(pszText, size, "NDS");
		}
		else if ((CA_system_ID >= 0x0E00) && (CA_system_ID <= 0x0Eff))
		{
			strcpy_s(pszText, size, "Scientific Atlanta");
		}
		else if ((CA_system_ID >= 0x4900) && (CA_system_ID <= 0x49ff))
		{
			strcpy_s(pszText, size, "Irdeto");
		}
		else if (CA_system_ID == 0x4A02)
		{
			strcpy_s(pszText, size, "永新视博");
		}
		else if ((CA_system_ID >= 0x4AD2) && (CA_system_ID <= 0x4AD3))
		{
			strcpy_s(pszText, size, "数码视讯");
		}
		else if ((CA_system_ID >= 0x4B00) && (CA_system_ID <= 0x4B02))
		{
			strcpy_s(pszText, size, "永新同方");
		}
		else if ((CA_system_ID >= 0x4B0A) && (CA_system_ID <= 0x4B0B))
		{
			strcpy_s(pszText, size, "数码视讯");
		}
		else
		{
			strcpy_s(pszText, size, "未知");
		}
	}

	return rtcode;
}
