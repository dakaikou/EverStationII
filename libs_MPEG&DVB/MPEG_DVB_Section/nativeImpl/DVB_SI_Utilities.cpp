#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../nativeInclude/DVB_SI_Utilities.h"
#include "../nativeInclude/MPEG_DVB_ErrorCode.h"

int DVB_SI_NumericCoding2Text_UTCTime(uint64_t time_code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		char		pszYear[5];
		char		pszMonth[3];
		char		pszDay[3];
		char		pszHour[3];
		char		pszMinute[3];
		char		pszSecond[3];

		uint16_t	MJD;
		int		Y, M, D;
		int		K;
		int		Hour, Minute, Second;

		//	MJD = (U16)(code >> 24);
		//MJD = (time_code[0] << 8) | time_code[1];
		//MJD = (time_code & 0xffff000000) >> 24;
		////	Hour = (U16)((code & 0x00ff0000) >> 16);
		////Hour = time_code[2];
		//Hour = (time_code & 0x0000ff0000) >> 16;
		////	Minute = (U16)((code & 0x0000ff00) >> 8);
		////Minute = time_code[3];
		//Minute = (time_code & 0x000000ff00) >> 8;
		////	Second = (U16)(code & 0x000000ff);
		Second = (time_code & 0x00000000ff);
		time_code >>= 8;
		Minute = (time_code & 0x000000ff);
		time_code >>= 8;
		Hour = (time_code & 0x0000ff);
		time_code >>= 8;

		MJD = (time_code & 0xffff);

		Y = (int)((MJD - 15078.2) / 365.25);
		M = (int)((MJD - 14956.1 - (int)(Y*365.25)) / 30.6001);
		D = MJD - 14956 - (int)(Y*365.25) - (int)(M*30.6001);

		if (M == 14 || M == 15)
		{
			K = 1;
		}
		else
		{
			K = 0;
		}

		Y += K + 1900;
		M -= 1 + K * 12;

		sprintf_s(pszYear, sizeof(pszYear), "%04d", Y);
		sprintf_s(pszMonth, sizeof(pszMonth), "%02d", M);
		sprintf_s(pszDay, sizeof(pszDay), "%02d", D);

		sprintf_s(pszHour, sizeof(pszHour), "%02X", Hour);
		sprintf_s(pszMinute, sizeof(pszMinute), "%02X", Minute);
		sprintf_s(pszSecond, sizeof(pszSecond), "%02X", Second);

		sprintf_s(pszText, size, "%s/%s/%s %s:%s:%s\0", pszYear, pszMonth, pszDay, pszHour, pszMinute, pszSecond);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_BCDTime(uint32_t bcd_code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		uint8_t hour;
		uint8_t minute;
		uint8_t second;

		bcd_code &= 0x00ffffff;
		second = (bcd_code & 0x0000ff);
		bcd_code >>= 8;
		minute = (bcd_code & 0x00ff);
		bcd_code >>= 8;
		hour = (uint8_t)bcd_code;

		sprintf_s(pszText, size, "%02X:%02X:%02X", hour, minute, second);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_RunningStatus(uint8_t running_status_code, char* pszText, int strSize)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (running_status_code)
		{
		case 0:
			strcpy_s(pszText, strSize, "undefined");
			break;
		case 1:
			strcpy_s(pszText, strSize, "not running");
			break;
		case 2:
			strcpy_s(pszText, strSize, "starts in a few seconds");
			break;
		case 3:
			strcpy_s(pszText, strSize, "pausing");
			break;
		case 4:
			strcpy_s(pszText, strSize, "running");
			break;
		case 5:
			strcpy_s(pszText, strSize, "service off-air");
			break;
		case 6:
			strcpy_s(pszText, strSize, "reserved for future use");
			break;
		case 7:
			strcpy_s(pszText, strSize, "reserved for future use");
			break;
		default:
			strcpy_s(pszText, strSize, "error status code");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_ServiceType(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		if (code == 0x00)
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if (code == 0x01)
		{
			strcpy_s(pszText, size, "digital television service");
		}
		else if (code == 0x02)
		{
			strcpy_s(pszText, size, "digital radio sound service");
		}
		else if (code == 0x03)
		{
			strcpy_s(pszText, size, "Teletext service");
		}
		else if (code == 0x04)
		{
			strcpy_s(pszText, size, "NVOD reference service");
		}
		else if (code == 0x05)
		{
			strcpy_s(pszText, size, "NVOD time-shifted service");
		}
		else if (code == 0x06)
		{
			strcpy_s(pszText, size, "mosaic service");
		}
		//	else if (code == 0x07)
		//	{
		//		strcpy_s(pszText, size, "PAL coded signal");
		//	}
		//	else if (code == 0x08)
		//	{
		//		strcpy_s(pszText, size, "SECAM coded signal");
		//	}
		//	else if (code == 0x09)
		//	{
		//		strcpy_s(pszText, size, "D/D2-MAC");
		//	}
		//	else if (code == 0x0A)
		//	{
		//		strcpy_s(pszText, size, "FM Radio");
		//	}
		//	else if (code == 0x0B)
		//	{
		//		strcpy_s(pszText, size, "NTSC coded signal");
		//	}
		else if (code == 0x0A)
		{
			strcpy_s(pszText, size, "advanced codec digital radio sound service");
		}
		else if (code == 0x0B)
		{
			strcpy_s(pszText, size, "advanced codec mosaic service");
		}
		else if (code == 0x0C)
		{
			strcpy_s(pszText, size, "data broadcast service");
		}
		else if ((code >= 0x0D) && (code <= 0x0F))
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if (code == 0x10)
		{
			strcpy_s(pszText, size, "DVB MHP service");
		}
		else if (code == 0x11)
		{
			strcpy_s(pszText, size, "MPEG-2 HD digital television service");
		}
		else if ((code >= 0x12) && (code <= 0x15))
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if (code == 0x16)
		{
			strcpy_s(pszText, size, "advanced codec SD digital television service");
		}
		else if (code == 0x17)
		{
			strcpy_s(pszText, size, "advanced codec SD NVOD time-shifted service");
		}
		else if (code == 0x18)
		{
			strcpy_s(pszText, size, "advanced codec SD NVOD reference service");
		}
		else if (code == 0x19)
		{
			strcpy_s(pszText, size, "advanced codec HD digital television service");
		}
		else if (code == 0x1A)
		{
			strcpy_s(pszText, size, "advanced codec HD NVOD time-shifted service");
		}
		else if (code == 0x1B)
		{
			strcpy_s(pszText, size, "advanced codec HD NVOD reference service");
		}
		else if (code == 0x1C)
		{
			strcpy_s(pszText, size, "advanced codec frame compatible plano-stereoscopic HD digital television service");
		}
		else if (code == 0x1D)
		{
			strcpy_s(pszText, size, "advanced codec frame compatible plano-stereoscopic HD time-shifted service");
		}
		else if (code == 0x1E)
		{
			strcpy_s(pszText, size, "advanced codec frame compatible plano-stereoscopic HD reference service");
		}
		else if ((code >= 0x1F) && (code <= 0x7F))
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else if ((code >= 0x80) && (code <= 0xFE))
		{
			strcpy_s(pszText, size, "user defined");
		}
		else if (code == 0xFF)
		{
			strcpy_s(pszText, size, "reserved for future use");
		}
		else
		{
			strcpy_s(pszText, size, "unknown");
		}
	}
	else
	{
		rtcode = SECTION_PARSE_NO_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_FECOuter(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "not defined");
			break;
		case 1:
			strcpy_s(pszText, size, "no outer FEC coding");
			break;
		case 2:
			strcpy_s(pszText, size, "RS(204/188)");
			break;
		default:
			strcpy_s(pszText, size, "reserved for future use");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_FECInner(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "not defined");
			break;
		case 1:
			strcpy_s(pszText, size, "1/2 conv. code rate");
			break;
		case 2:
			strcpy_s(pszText, size, "2/3 conv. code rate");
			break;
		case 3:
			strcpy_s(pszText, size, "3/4 conv. code rate");
			break;
		case 4:
			strcpy_s(pszText, size, "5/6 conv. code rate");
			break;
		case 5:
			strcpy_s(pszText, size, "7/8 conv. code rate");
			break;
		case 15:
			strcpy_s(pszText, size, "No conv. coding");
			break;
		default:
			strcpy_s(pszText, size, "reserved for future use");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_CableModulation(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "not defined");
			break;
		case 1:
			strcpy_s(pszText, size, "16 QAM");
			break;
		case 2:
			strcpy_s(pszText, size, "32 QAM");
			break;
		case 3:
			strcpy_s(pszText, size, "64 QAM");
			break;
		case 4:
			strcpy_s(pszText, size, "128 QAM");
			break;
		case 5:
			strcpy_s(pszText, size, "256 QAM");
			break;
		default:
			strcpy_s(pszText, size, "reserved for future use");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_FrequencyCode(uint16_t frequency_code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		sprintf_s(pszText, size, "%d KHz ", frequency_code * 90);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//int DecodeBitrateCode2Text(uint32_t code, char* pszText, int size)
//{
//	int		rtcode = SECTION_PARSE_NO_ERROR;
//
//	if (pszText != NULL)
//	{
//		sprintf_s(pszText, size, "%d bps ", code * 400);
//	}
//	else
//	{
//		rtcode = SECTION_PARSE_PARAMETER_ERROR;
//	}
//
//	return rtcode;
//}

int DVB_SI_NumericCoding2Text_TerrestrialBandwidth(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "8MHz");
			break;
		case 1:
			strcpy_s(pszText, size, "7MHz");
			break;
		case 2:
			strcpy_s(pszText, size, "6MHz");
			break;
		case 3:
			strcpy_s(pszText, size, "5MHz");
			break;
		default:
			strcpy_s(pszText, size, "reserved for future Use");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_TerrestrialPriority(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "LP(low priority)");
			break;
		case 1:
			strcpy_s(pszText, size, "HP(high priority)");
			break;
		default:
			strcpy_s(pszText, size, "UnKnown");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_TerrestrialConstellation(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "QPSK");
			break;
		case 1:
			strcpy_s(pszText, size, "16-QAM");
			break;
		case 2:
			strcpy_s(pszText, size, "64-QAM");
			break;
		default:
			strcpy_s(pszText, size, "reserved for future Use");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_SatelliteModulationSystem(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "DVB-S");
			break;
		case 1:
			strcpy_s(pszText, size, "DVB-S2");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


int DVB_SI_NumericCoding2Text_SatelliteModulationType(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "Auto");
			break;
		case 1:
			strcpy_s(pszText, size, "QPSK");
			break;
		case 2:
			strcpy_s(pszText, size, "8PSK");
			break;
		case 3:
			strcpy_s(pszText, size, "16-QAM");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_SatelliteModulationRollOff(uint8_t code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (code)
		{
		case 0:
			strcpy_s(pszText, size, "alfa = 0.35");
			break;
		case 1:
			strcpy_s(pszText, size, "alfa = 0.25");
			break;
		case 2:
			strcpy_s(pszText, size, "alfa = 0.20");
			break;
		default:
			strcpy_s(pszText, size, "reserved");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_SatelliteWestEastFlag(uint8_t west_east_flag, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		if (west_east_flag)
		{
			strcpy_s(pszText, size, "eastern");
		}
		else
		{
			strcpy_s(pszText, size, "western");
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_SatellitePolarization(uint8_t polarization_code, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (polarization_code)
		{
		case 0:
			strcpy_s(pszText, size, "线性水平极化");
			break;
		case 1:
			strcpy_s(pszText, size, "线性垂直极化");
			break;
		case 2:
			strcpy_s(pszText, size, "圆左旋极化");
			break;
		case 3:
			strcpy_s(pszText, size, "圆右旋极化");
			break;
		default:
			strcpy_s(pszText, size, "");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_DataBroadcastID(uint16_t data_broadcast_ID, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (data_broadcast_ID)
		{
		case 0x0000:
			strcpy_s(pszText, size, "Reserved for future use");
			break;
		case 0x0001:
			strcpy_s(pszText, size, "Data pipe");
			break;
		case 0x0002:
			strcpy_s(pszText, size, "Asynchronous data stream");
			break;
		case 0x0003:
			strcpy_s(pszText, size, "Synchronous data stream");
			break;
		case 0x0004:
			strcpy_s(pszText, size, "Synchronised data stream");
			break;
		case 0x0005:
			strcpy_s(pszText, size, "Multi protocol encapsulation");
			break;
		case 0x0006:
			strcpy_s(pszText, size, "Data Carousel");
			break;
		case 0x0007:
			strcpy_s(pszText, size, "Object Carousel");
			break;
		case 0x0008:
			strcpy_s(pszText, size, "DVB ATM streams");
			break;
		case 0x0009:
			strcpy_s(pszText, size, "higher protocol");
			break;
		case 0x000A:
			strcpy_s(pszText, size, "system software update");
			break;
		case 0x000B:
			strcpy_s(pszText, size, "IP/MAC notification");
			break;
		case 0x00F0:
			strcpy_s(pszText, size, "MHP object carousel");
			break;
		case 0x00F1:
			strcpy_s(pszText, size, "MHP multi-protocol encapsul");
			break;
		case 0xffff:
			strcpy_s(pszText, size, "Reserved for future use");
			break;
		default:
			if ((data_broadcast_ID >= 0x000C) && (data_broadcast_ID <= 0x00ef))
			{
				strcpy_s(pszText, size, "Reserved for future use by DVB");
			}
			else if ((data_broadcast_ID >= 0x00f2) && (data_broadcast_ID <= 0x00ff))
			{
				strcpy_s(pszText, size, "Reserved for future use by DVB");
			}
			else if ((data_broadcast_ID >= 0x0100) && (data_broadcast_ID <= 0xfffe))
			{
				switch (data_broadcast_ID)
				{
				case 0x0100:
					strcpy_s(pszText, size, "Eutelsat Data Piping");
					break;
				case 0x0101:
					strcpy_s(pszText, size, "Eutelsat Data Stream");
					break;
				case 0x0102:
					strcpy_s(pszText, size, "SAGEM IP encapsulation in MPEG-2 PES packets");
					break;
				case 0x0103:
					strcpy_s(pszText, size, "BARCO Data Broadcasting");
					break;
				case 0x0104:
					strcpy_s(pszText, size, "CyberCity Multiprotocol Encapsulation");
					break;
				case 0x0105:
					strcpy_s(pszText, size, "CyberSat Multiprotocol Encapsulation");
					break;
				case 0x0106:
					strcpy_s(pszText, size, "The Digital Network");
					break;
				case 0x0107:
					strcpy_s(pszText, size, "OpenTV Data Carousel");
					break;
				case 0x0108:
					strcpy_s(pszText, size, "Panasonic");
					break;
				default:
					strcpy_s(pszText, size, "Reserved for registration");
					break;
				}
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

int DVB_SI_NumericCoding2Text_CarouselTypeID(uint8_t carousel_type_id, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (carousel_type_id)
		{
		case 0x01:
			strcpy_s(pszText, size, "one layer carousel");
			break;
		case 0x02:
			strcpy_s(pszText, size, "two layer carousel");
			break;
		default:
			strcpy_s(pszText, size, "reserved");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int DVB_SI_NumericCoding2Text_TeletextType(uint8_t teletext_type, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (teletext_type)
		{
		case 0x00:
			strcpy_s(pszText, size, "reserved for future use");
			break;
		case 0x01:
			strcpy_s(pszText, size, "initial Teletext page");
			break;
		case 0x02:
			strcpy_s(pszText, size, "Teletext subtitle page");
			break;
		case 0x03:
			strcpy_s(pszText, size, "additional information page");
			break;
		case 0x04:
			strcpy_s(pszText, size, "programme schedule page");
			break;
		case 0x05:
			strcpy_s(pszText, size, "Teletext subtitle page for hearing impaired people");
			break;
		default:
			strcpy_s(pszText, size, "reserved for future use");
			break;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

char*	DVB_SI_StringPrefixTrim(char* pstr_src)
{
	char*		pstr_dst = pstr_src;
	unsigned char flag_char;

	if (pstr_src != NULL)
	{
		flag_char = pstr_src[0];

		if (flag_char == 0x00)						//reserved
		{
			pstr_dst = pstr_src;
		}
		else if ((flag_char >= 0x01) && (flag_char <= 0x05))	//GB/T 15273.1-1994
		{
			pstr_dst = pstr_src + 1;
		}
		else if ((flag_char >= 0x06) && (flag_char <= 0x0F))	//reserved
		{
			pstr_dst = pstr_src + 1;
		}
		else if (flag_char == 0x10)			//GB/T 15273.1-1994
		{
			pstr_dst = pstr_src + 3;
		}
		else if (flag_char == 0x11)			//GB 13000.1-1993
		{
			//UNICODE转换GB 18030
			pstr_dst = pstr_src + 1;
		}
		else if (flag_char == 0x12)			//KSC 5601
		{
			pstr_dst = pstr_src + 1;
		}
		else if (flag_char == 0x13)			//GB 2312-1980, GB 18030-2005
		{
			pstr_dst = pstr_src + 1;
		}
		else if ((flag_char >= 0x14) && (flag_char <= 0x1F))		//reserved
		{
			pstr_dst = pstr_src + 1;
		}
		else
		{
			pstr_dst = pstr_src;
		}
	}

	return pstr_dst;
}

