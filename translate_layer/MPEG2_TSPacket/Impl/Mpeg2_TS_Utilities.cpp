#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "../Include/Mpeg_stream_id.h"
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

int MPEG2_TS_NumericCoding2Text_transport_scrambling_control(uint8_t transport_scrambling_control_code, char* pszText, int size)
{
	int		rtcode = TSPACKET_PARSE_NO_ERROR;

	if (pszText != NULL)
	{
		switch (transport_scrambling_control_code)
		{
		case 0b00:
			strcpy_s(pszText, size, "未加扰");
			break;
		case 0b01:
			strcpy_s(pszText, size, "保留");
			break;
		case 0b10:
			strcpy_s(pszText, size, "偶密钥加扰");
			break;
		case 0b11:
			strcpy_s(pszText, size, "奇密钥加扰");
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
			strcpy_s(pszText, size, "保留");
			break;
		case 0b01:
			strcpy_s(pszText, size, "仅有净荷");
			break;
		case 0b10:
			strcpy_s(pszText, size, "仅有适配段");
			break;
		case 0b11:
			strcpy_s(pszText, size, "适配段+净荷");
			break;
		}
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

//typedef enum
//{
//	STREAM_SUBTYPE_UNKNOWN = 0,
//	STREAM_SUBTYPE_AIT = 1,
//	STREAM_SUBTYPE_INT = 2,
//	STREAM_SUBTYPE_TELETEXT = 32,
//	STREAM_SUBTYPE_SUBTITLE = 33,
//	STREAM_SUBTYPE_AC3 = 34,
//	STREAM_SUBTYPE_DRA = 35,
//} STREAM_SUBTYPE_e;

int MPEG2_TS_GetPayloadClassByStreamType(uint8_t stream_type, uint8_t sub_type)
{
	int class_type = TSPAYLOAD_CLASS_UNKNOWN;

	if ((stream_type == 0x01) ||				//ISO/IEC 11172-2 Video
		(stream_type == 0x02) ||				//ISO/IEC 13818-2 Video
		(stream_type == 0x10) ||				//ISO/IEC 14496-2 Video
		(stream_type == 0x1B) ||				//ISO/IEC 14496-10 Video, AVC\H.264
		(stream_type == 0x1E) ||				//ISO/IEC 23002-3 Video
		(stream_type == 0x1F) ||				//ISO/IEC 14496-10 Annex G Video
		(stream_type == 0x20) ||				//ISO/IEC 14496-10 Annex H Video
		(stream_type == 0x21) ||				//ISO/IEC 15444-1 Video
		(stream_type == 0x22) ||				//ISO/IEC 13818-2 3D Video
		(stream_type == 0x23) ||				//
		(stream_type == 0x24) ||
		(stream_type == 0x25) ||
		(stream_type == 0x26) ||
		(stream_type == 0x42))
	{
		class_type = TSPAYLOAD_CLASS_PES_VIDEO;
	}
	else if ((stream_type == 0x03) ||			//ISO/IEC 11172-3 Audio
		(stream_type == 0x04) ||				//ISO/IEC 13818-3 Audio
		(stream_type == 0x0F) ||				//ISO/IEC 13818-7 Audio
		(stream_type == 0x11) ||				//ISO/IEC 14496-3 Audio
		(stream_type == 0x1C) ||				//ISO/IEC 14496-3 Audio
		(stream_type == 0x43))
	{
		class_type = TSPAYLOAD_CLASS_PES_AUDIO;
	}
	else if (stream_type == 0x06)				//数据
	{
		if (sub_type == 0x34)
		{
			class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		}
		else if (sub_type == 0x35)
		{
			class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		}
		else
		{
			class_type = TSPAYLOAD_CLASS_PES_DATA;
		}
	}
	else if (stream_type == 0x81)				//用户私有
	{
		if (sub_type == 0x34)
		{
			class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		}
		else
		{
			class_type = TSPAYLOAD_CLASS_PES_DATA;
		}
	}
	else if ((stream_type == 0x12) ||			//数据
		(stream_type == 0x15))					//Metadata carried in PES packets
	{
		class_type = TSPAYLOAD_CLASS_PES_DATA;
	}
	else if ((stream_type == 0x05) ||			//13818.1 Private Section
		(stream_type == 0x08) ||	// 13818-1 Annex A DSM-CC
		(stream_type == 0x0A) ||	//DSM-CC MPE
		(stream_type == 0x0B) ||	//DSM-CC U-N Message
		(stream_type == 0x0C) ||	//DSM-CC Stream Descriptor
		(stream_type == 0x0D) ||	//DSM-CC Sections
		(stream_type == 0x13) ||	//
		(stream_type == 0x16))		//Metadata carried in metadata section
	{
		class_type = TSPAYLOAD_CLASS_SECTION;
	}
	else
	{
		class_type = TSPAYLOAD_CLASS_PES_DATA;
	}

	return class_type;
}

int MPEG2_TS_GetPayloadClassByStreamID(uint8_t stream_id)
{
	int class_type = TSPAYLOAD_CLASS_PES_DATA;

	if (stream_id == PROGRAM_STREAM_MAP)		//0xBC
	{
		//sprintf_s(pszText, size, "program_stream_map (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == PRIVATE_STREAM_1)			//0xBD
	{
		//sprintf_s(pszText, size, "private_stream_1 (stream_id=0x%02X)", stream_id);				//AC-3, DRA
	}
	else if (stream_id == PADDING_STREAM)				//0xBE
	{
		//sprintf_s(pszText, size, "padding_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == PRIVATE_STREAM_2)				//0xBF
	{
		//sprintf_s(pszText, size, "private_stream_2 (stream_id=0x%02X)", stream_id);
	}
	else if ((stream_id >= AUDIO_STREAM_MIN) && (stream_id <= AUDIO_STREAM_MAX))
	{
		class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		//sprintf_s(pszText, size, "13818-3/11172-3/13818-7/14496-3 audio stream (stream_id=0x%02X)", stream_id);
	}
	else if ((stream_id >= VIDEO_STREAM_MIN) && (stream_id <= VIDEO_STREAM_MAX))
	{
		class_type = TSPAYLOAD_CLASS_PES_VIDEO;
		//sprintf_s(pszText, size, "13818-2/11172-2/14496-2/14496-10 video stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == ECM_STREAM)
	{
		//sprintf_s(pszText, size, "ECM_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == EMM_STREAM)
	{
		//sprintf_s(pszText, size, "EMM_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == DSMCC_STREAM)
	{
		//sprintf_s(pszText, size, "13818-1 Annex A or 13818-6_DSMCC_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == ISO_13522_STREAM)
	{
		//sprintf_s(pszText, size, "13522_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == TYPE_A_STREAM)
	{
		//sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type A (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == TYPE_B_STREAM)
	{
		//sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type B (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == TYPE_C_STREAM)
	{
		//sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type C (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == TYPE_D_STREAM)
	{
		//sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type D (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == TYPE_E_STREAM)
	{
		//sprintf_s(pszText, size, "ITU-T Rec. H.222.1 type E (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == ANCILLARY_STREAM)
	{
		//sprintf_s(pszText, size, "ancillary_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == RESERVED_DATA_STREAM_A)
	{
		//sprintf_s(pszText, size, "14496-1_SL-packetized_stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == RESERVED_DATA_STREAM_B)
	{
		//sprintf_s(pszText, size, "14496-1_FlexMux_stream (stream_id=0x%02X)", stream_id);
	}
	else if ((stream_id >= 0xFC) && (stream_id <= 0xFE))
	{
		//sprintf_s(pszText, size, "reserved data stream (stream_id=0x%02X)", stream_id);
	}
	else if (stream_id == PROGRAM_STREAM_DIRECTORY)
	{
		//sprintf_s(pszText, size, "program_stream_directory (stream_id=0x%02X)", stream_id);
	}
	else
	{
		class_type = TSPAYLOAD_CLASS_UNKNOWN;
	}

	return class_type;
}
