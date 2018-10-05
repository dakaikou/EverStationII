#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "libs_MPEG&DVB\MPEG_TSPacket\Include\Mpeg2_TS_Utilities.h"
#include "../../Include/Mpeg2_StreamType.h"
#include "../../Include/Mpeg2_PSI_Utilities.h"
#include "../../Include/Mpeg2_PSI_section.h"

#include "../../Include/MPEG_DVB_ErrorCode.h"

void MPGV_DecodeFrameRateCodetoText(uint8_t code, char* pszText, int size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "��ֹ");
		break;
	case 1:
		strcpy_s(pszText, size, "23.976 ֡/��");
		break;
	case 2:
		strcpy_s(pszText, size, "24 ֡/��");
		break;
	case 3:
		strcpy_s(pszText, size, "25 ֡/��");
		break;
	case 4:
		strcpy_s(pszText, size, "29.97 ֡/��");
		break;
	case 5:
		strcpy_s(pszText, size, "30 ֡/��");
		break;
	case 6:
		strcpy_s(pszText, size, "50 ֡/��");
		break;
	case 7:
		strcpy_s(pszText, size, "59.94 ֡/�� ");
		break;
	case 8:
		strcpy_s(pszText, size, "60 ֡/��");
		break;
	default:
		strcpy_s(pszText, size, "����");
		break;
	}
}

void MPGV_DecodeChromaFormatCodetoText(uint8_t code, char* pszText, int size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "reserved");
		break;
	case 1:
		strcpy_s(pszText, size, "4:2:0");
		break;
	case 2:
		strcpy_s(pszText, size, "4:2:2");
		break;
	case 3:
		strcpy_s(pszText, size, "4:2:4");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodeProfileCodetoText(uint8_t code, char* pszText, int size)
{
	switch (code)
	{
	case 0:
		strcpy_s(pszText, size, "reserved");
		break;
	case 1:
		strcpy_s(pszText, size, "High");
		break;
	case 2:
		strcpy_s(pszText, size, "Spatially Scalable");
		break;
	case 3:
		strcpy_s(pszText, size, "SNR Scalable");
		break;
	case 4:
		strcpy_s(pszText, size, "Main");
		break;
	case 5:
		strcpy_s(pszText, size, "Simple");
		break;
	case 6:
	case 7:
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}

void MPGV_DecodeLevelCodetoText(uint8_t code, char* pszText, int size)
{
	switch (code)
	{
	case 4:
		strcpy_s(pszText, size, "High");
		break;
	case 6:
		strcpy_s(pszText, size, "High 1440");
		break;
	case 8:
		strcpy_s(pszText, size, "Main");
		break;
	case 10:
		strcpy_s(pszText, size, "Low");
		break;
	default:
		strcpy_s(pszText, size, "reserved");
		break;
	}
}


int MPEG2_PSI_NumericCoding2Text_StreamType(uint8_t stream_type, uint8_t subtype, char* pszText, int size)
{
	int		rtcode = SECTION_PARSE_UNKNOWN_ERROR;

	if (pszText != NULL)
	{
		if (stream_type == 0x00)
		{
			strcpy_s(pszText, size, "������ITU-T|ISO/IEC��");
		}
		else if (stream_type == 0x01)
		{
			strcpy_s(pszText, size, "MPEG1��Ƶ��ISO/IEC 11172-2|GB/T 17191-2��");
		}
		else if (stream_type == 0x02)
		{
//			strcpy_s(pszText, size, "GB/T 17975-2(ISO/IEC 13818-2)��Ƶ����GB/T 17191.2(ISO/IEC 11172.2)Լ��������Ƶ��");
			strcpy_s(pszText, size, "MPEG2��Ƶ��ITU-T H.262|ISO/IEC 13818-2|GB/T 17975-2��");
		}
		else if (stream_type == 0x03)
		{
			strcpy_s(pszText, size, "MPEG1��Ƶ��ISO/IEC 11172-3|GB/T 17191-3��");
		}
		else if (stream_type == 0x04)
		{
			strcpy_s(pszText, size, "MPEG2��Ƶ��ISO/IEC 13818-3|GB/T 17975-3��");
		}
		else if (stream_type == 0x05)
		{
			switch (subtype)
			{
			case STREAM_SUBTYPE_INT:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)˽�зֶ�(INT)");
				strcpy_s(pszText, size, "INT");
				break;
			case STREAM_SUBTYPE_AIT:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)˽�зֶ�(AIT)");
				strcpy_s(pszText, size, "AIT");
				break;
			default:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)˽�зֶ�");
				strcpy_s(pszText, size, "˽��Section��ITU-T H.222.0|ISO/IEC 13818-1��");
				break;
			}
		}
		else if (stream_type == 0x06)
		{
			switch (subtype)
			{
			case STREAM_SUBTYPE_TELETEXT:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)PES��Я��˽������(ͼ��)");
				strcpy_s(pszText, size, "ͼ�� (ISO/IEC 13818-1��");
				break;
			case STREAM_SUBTYPE_SUBTITLE:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)PES��Я��˽������(��Ļ)");
				strcpy_s(pszText, size, "��Ļ (ISO/IEC 13818-1��");
				break;
			case STREAM_SUBTYPE_AC3:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)PES��Я��˽������(Dolby AC-3��Ƶ)");
				strcpy_s(pszText, size, "AC3��Ƶ��Dolby AC-3��");
				break;
			case STREAM_SUBTYPE_DRA:
				strcpy_s(pszText, size, "DRA��Ƶ");
				break;
			default:
//				strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)PES��Я��˽������");
				strcpy_s(pszText, size, "PES˽�����ݣ�ITU-T H.222.0|ISO/IEC 13818-1��");
				break;
			}
		}
		else if (stream_type == 0x07)
		{
			strcpy_s(pszText, size, "MHEG��ISO/IEC 13522��");
		}
		else if (stream_type == 0x08)
		{
			//			strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)��¼A DSM CC");
			strcpy_s(pszText, size, "DSM CC��ITU-T H.222.0|ISO/IEC 13818-1 ��¼A��");
		}
		else if (stream_type == 0x09)
		{
			strcpy_s(pszText, size, "ITU-T H.222.1");
		}
		else if (stream_type == 0x0A)
		{
			//			strcpy_s(pszText, size, "Multi-protocol Encapsulation");
			strcpy_s(pszText, size, "MPE��ISO/IEC 13818-6 type A��");
		}
		else if (stream_type == 0x0B)
		{
			strcpy_s(pszText, size, "DSM-CC U-N Messages��ISO/IEC 13818-6 type B)");
		}
		else if (stream_type == 0x0C)
		{
			strcpy_s(pszText, size, "DSM-CC ��������(ISO/IEC 13818-6 type C)");
		}
		else if (stream_type == 0x0D)
		{
			strcpy_s(pszText, size, "DSM-CC Sections (any type including private data(ISO/IEC 13818-6 type D)");
		}
		else if (stream_type == 0x0E)
		{
			strcpy_s(pszText, size, "��������ITU-T H.222.0|ISO/IEC 13818-1|GB/T 17975.1��");
		}
		else if (stream_type == 0x0F)
		{
			//			strcpy_s(pszText, size, "ISO/IEC 13818-7(ADTS�﷨) ��Ƶ ��MPEG2 LC AAC��");
			strcpy_s(pszText, size, "AAC��Ƶ��ISO/IEC 13818-7 ADTS�﷨[LC AAC]��");
		}
		else if (stream_type == 0x10)
		{
			//			strcpy_s(pszText, size, "ISO/IEC 14496-2(mpeg4) ��Ƶ");
			strcpy_s(pszText, size, "MPEG4��Ƶ��ISO/IEC 14496-2��");
		}
		else if (stream_type == 0x11)
		{
//			strcpy_s(pszText, size, "ISO/IEC 14496-3(LATM�﷨) ��Ƶ (MPEG4 LC AAC)");
			strcpy_s(pszText, size, "AAC��Ƶ��ISO/IEC 14496-3 LATM�﷨[LC AAC]��");
		}
		else if (stream_type == 0x12)
		{
			strcpy_s(pszText, size, "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in PES packet");
		}
		else if (stream_type == 0x13)
		{
			strcpy_s(pszText, size, "ISO/IEC 14496-1 SL-packetized stream or FlexMux stream carried in 14496 sections");
		}
		else if (stream_type == 0x14)
		{
			strcpy_s(pszText, size, "ISO/IEC 14496-6 Synchronized Download Protocol");
		}
		else if (stream_type == 0x15)
		{
			strcpy_s(pszText, size, "Metadata carried in PES packet");
		}
		else if (stream_type == 0x16)
		{
			strcpy_s(pszText, size, "Metadata carried in metadata sections");
		}
		else if (stream_type == 0x17)
		{
			strcpy_s(pszText, size, "Metadata carried in 13818-6 Data Carousel");
		}
		else if (stream_type == 0x18)
		{
			strcpy_s(pszText, size, "Metadata carried in 13818-6 Object Carousel");
		}
		else if (stream_type == 0x19)
		{
			strcpy_s(pszText, size, "Metadata carried in 13818-6 Synchronized Download Protocol");
		}
		else if (stream_type == 0x1A)
		{
			strcpy_s(pszText, size, "IPMP����ISO/IEC 13818-11��");
		}
		else if (stream_type == 0x1B)
		{
			//			strcpy_s(pszText, size, "ISO/IEC 14496-10(H.264/AVC) ��Ƶ");
			strcpy_s(pszText, size, "H264��Ƶ��ITU-T H.264|ISO/IEC 14496-10 ��¼A��");
		}
		else if (stream_type == 0x1C)
		{
			strcpy_s(pszText, size, "MPEG4��Ƶ��ISO/IEC 14496-3��");
		}
		else if (stream_type == 0x1D)
		{
			strcpy_s(pszText, size, "ͼ�ģ�ISO/IEC 14496-17��");
		}
		else if (stream_type == 0x1E)
		{
			strcpy_s(pszText, size, "������Ƶ��ISO/IEC 23002-3��");
		}
		else if (stream_type == 0x1F)
		{
			strcpy_s(pszText, size, "MPEG4 SVC��Ƶ��ITU-T H.264|ISO/IEC 14496-10 ��¼G��");
		}
		else if (stream_type == 0x20)
		{
			strcpy_s(pszText, size, "MPEG4 MVC��Ƶ��ITU-T H.264|ISO/IEC 14496-10 ��¼H��");
		}
		else if (stream_type == 0x21)
		{
			strcpy_s(pszText, size, "��Ƶ��ITU-T T.800|ISO/IEC 15444-1��");
		}
		else if (stream_type == 0x22)
		{
			strcpy_s(pszText, size, "MPEG2 3D��Ƶ��ITU-T H.262|ISO/IEC 13818-2��");
		}
		else if (stream_type == 0x23)
		{
			strcpy_s(pszText, size, "H264 3D��Ƶ��ITU-T H.264|ISO/IEC 14496-10 ��¼A��");
		}
		else if (stream_type == 0x24)
		{
			strcpy_s(pszText, size, "��Ƶ��ITU-T H.265|ISO/IEC 23008-2��");
		}
		else if (stream_type == 0x25)
		{
			strcpy_s(pszText, size, "H265��Ƶ��ITU-T H.265|ISO/IEC 23008-2 ��¼A��");
		}
		else if (stream_type == 0x26)
		{
			strcpy_s(pszText, size, "MPEG4 MVCD��Ƶ��ITU-T H.264|ISO/IEC 14496-10 ��¼I��");
		}
		else if (stream_type >= 0x27 && stream_type <= 0x41)
		{
			//			strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1) ����");
			strcpy_s(pszText, size, "������ITU-T H.222.0|ISO/IEC 13818-1��");
		}
		else if (stream_type == 0x42)
		{
			//			strcpy_s(pszText, size, "GB/T 20090.2 ��Ƶ(AVS)");
			strcpy_s(pszText, size, "AVS��Ƶ��GB/T 20090.2��");			//defined in GB/T 20090.1
		}
		else if (stream_type == 0x43)
		{
			strcpy_s(pszText, size, "��Ƶ��GB/T 20090.3��");			//defined in GB/T 20090.1
		}
		else if (stream_type >= 0x44 && stream_type <= 0x7E)
		{
			//			strcpy_s(pszText, size, "GB/T 17975.1(ISO/IEC 13818-1)����");
			strcpy_s(pszText, size, "������ITU-T H.222.0|ISO/IEC 13818-1��");
		}
		else if (stream_type == 0x7F)
		{
			strcpy_s(pszText, size, "IPMP��");
		}
		else if (stream_type >= 0x80 && stream_type <= 0xFF)
		{
			if (stream_type == 0x81)
			{
				if (subtype == STREAM_SUBTYPE_AC3)
				{
					strcpy_s(pszText, size, "AC3 ��Ƶ��Dolby AC-3��");
				}
				else
				{
					strcpy_s(pszText, size, "�û�˽��");
				}
			}
			else
			{
				strcpy_s(pszText, size, "�û�˽��");
			}
		}
		else
		{
			strcpy_s(pszText, size, "δ֪������");
		}

		rtcode = SECTION_PARSE_NO_ERROR;
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

uint8_t GetTSPayloadClassByStreamType(uint8_t stream_type, uint8_t sub_type)
{
	uint8_t class_type = TSPAYLOAD_CLASS_UNKNOWN;

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
	else if (stream_type == 0x06)				//����
	{
		if (sub_type == STREAM_SUBTYPE_AC3)
		{
			class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		}
		else if (sub_type == STREAM_SUBTYPE_DRA)
		{
			class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		}
		else
		{
			class_type = TSPAYLOAD_CLASS_PES_DATA;
		}
	}
	else if (stream_type == 0x81)				//�û�˽��
	{
		if (sub_type == STREAM_SUBTYPE_AC3)
		{
			class_type = TSPAYLOAD_CLASS_PES_AUDIO;
		}
		else
		{
			class_type = TSPAYLOAD_CLASS_PES_DATA;
		}
	}
	else if ((stream_type == 0x12) ||			//����
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

