#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "../../Include/MPEG_DVB_Common.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "../../Include/DVB_SI_Utilities.h"
#include "../../Include/Mpeg2_PSI_Utilities.h"
#include "../../Include/Mpeg2_PSI_Descriptor.h"

#include "../Include/Mpeg2_PSI_Descriptor_XML.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif


//功能：解应用描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2005.5 chendelin

//功能：解视频流描述子		0x02
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_video_stream_descriptor_to_xml(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, video_stream_descriptor_t* pVideoStreamDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszProfile[32];
	char	pszLevel[32];
	char	pszComment[128];

	video_stream_descriptor_t* pvideo_stream_descriptor = (pVideoStreamDescriptor == NULL) ? new video_stream_descriptor_t : pVideoStreamDescriptor;
	rtcode = MPEG2_PSI_decode_video_stream_descriptor(buf, length, pvideo_stream_descriptor);

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, "video_stream_descriptor()", NULL);
		pxmlDoc->SetAnchor(pxmlDescriptorNode);

		sprintf_s(pszComment, sizeof(pszComment), "tag: 0x%02X, %d字节", pvideo_stream_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszComment);

		if (rtcode != SECTION_PARSE_NO_ERROR)
		{
			sprintf_s(pszComment, sizeof(pszComment), "ErrorCode=0x%08x", rtcode);
			pxmlDescriptorNode->SetAttribute("error", pszComment);
		}

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "descriptor_tag", pvideo_stream_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "descriptor_length", pvideo_stream_descriptor->descriptor_length, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "multiple_frame_rate_flag", pvideo_stream_descriptor->multiple_frame_rate_flag, 1, "bslbf", NULL);
		MPGV_DecodeFrameRateCodetoText(pvideo_stream_descriptor->frame_rate_code, pszComment, sizeof(pszComment));
		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "frame_rate_code", pvideo_stream_descriptor->frame_rate_code, 4, "uimsbf", pszComment);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "MPEG_1_only_flag", pvideo_stream_descriptor->MPEG_1_only_flag, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "constrained_parameter_flag", pvideo_stream_descriptor->constrained_parameter_flag, 1, "bslbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "still_picture_flag", pvideo_stream_descriptor->still_picture_flag, 1, "bslbf", NULL);

		if (pvideo_stream_descriptor->MPEG_1_only_flag == 0)
		{
			MPGV_DecodeProfileCodetoText((pvideo_stream_descriptor->profile_and_level_indication & 0x70) >> 4, pszProfile, sizeof(pszProfile));
			MPGV_DecodeLevelCodetoText((pvideo_stream_descriptor->profile_and_level_indication & 0x0F), pszLevel, sizeof(pszLevel));
			sprintf_s(pszComment, sizeof(pszComment), "%s profile @ %s level", pszProfile, pszLevel);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "profile_and_level_indication", pvideo_stream_descriptor->profile_and_level_indication, 8, "uimsbf", pszComment);

			MPGV_DecodeChromaFormatCodetoText(pvideo_stream_descriptor->chroma_format, pszComment, sizeof(pszComment));
			XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "chroma_format", pvideo_stream_descriptor->chroma_format, 2, "uimsbf", pszComment);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "frame_rate_extension_flag", pvideo_stream_descriptor->frame_rate_extension_flag, 1, "uimsbf", NULL);
			XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "reserved", pvideo_stream_descriptor->reserved, 5, "uimsbf", NULL);
		}

		pxmlDoc->ClearAnchor(pxmlDescriptorNode);
	}

	if (pVideoStreamDescriptor == NULL)
	{
		delete pvideo_stream_descriptor;
	}

	return rtcode;
}

//功能：解音频流描述子			0x03
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_audio_stream_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, audio_stream_descriptor_t* pAudioStreamDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszTemp[64];
	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "audio_stream_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		audio_stream_descriptor_t* paudio_stream_descriptor = (pAudioStreamDescriptor == NULL) ? new audio_stream_descriptor_t : pAudioStreamDescriptor;
		memset(paudio_stream_descriptor, 0x00, sizeof(audio_stream_descriptor_t));

		BITS_map(&bs, buf, length);

		paudio_stream_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", paudio_stream_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		paudio_stream_descriptor->descriptor_length = BITS_get(&bs, 8);
		assert(paudio_stream_descriptor->descriptor_length == 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", paudio_stream_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		paudio_stream_descriptor->free_format_flag = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "free_format_flag", paudio_stream_descriptor->free_format_flag, 1, "bslbf", NULL, &bs);
		paudio_stream_descriptor->ID = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "ID", paudio_stream_descriptor->ID, 1, "bslbf", NULL, &bs);
		paudio_stream_descriptor->layer = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "layer", paudio_stream_descriptor->layer, 2, "bslbf", NULL, &bs);
		paudio_stream_descriptor->variable_rate_audio_indicator = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "variable_rate_audio_indicator", paudio_stream_descriptor->variable_rate_audio_indicator, 1, "bslbf", NULL, &bs);
		paudio_stream_descriptor->reserved = BITS_get(&bs, 3);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", paudio_stream_descriptor->reserved, 3, "bslbf", NULL, &bs);

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", paudio_stream_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (pAudioStreamDescriptor == NULL)
		{
			delete paudio_stream_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解Hierarchy描述子				0x04
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin

//功能：解注册描述子			0x05
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_registration_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, registration_descriptor_t* pRegistrationDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	int		copy_length = 0;
	char    pszTemp[64];
	//uint8_t* ptemp;
	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 6))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "registration_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		registration_descriptor_t* pregistration_descriptor = (pRegistrationDescriptor == NULL) ? new registration_descriptor_t : pRegistrationDescriptor;
		memset(pregistration_descriptor, 0x00, sizeof(registration_descriptor_t));

		BITS_map(&bs, buf, length);

		pregistration_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pregistration_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		pregistration_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pregistration_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		memcpy(pregistration_descriptor->format_identifier_char, bs.p_cur, 4);
		//pregistration_descriptor->format_identifier_char[0] = (char)((pregistration_descriptor->format_identifier & 0xff000000) >> 24);
		//pregistration_descriptor->format_identifier_char[1] = (char)((pregistration_descriptor->format_identifier & 0x00ff0000) >> 16);
		//pregistration_descriptor->format_identifier_char[2] = (char)((pregistration_descriptor->format_identifier & 0x0000ff00) >> 8);
		//pregistration_descriptor->format_identifier_char[3] = (char)(pregistration_descriptor->format_identifier & 0x000000ff);
		pregistration_descriptor->format_identifier_char[4] = '\0';
		pregistration_descriptor->format_identifier = BITS_get(&bs, 32);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "format_identifier", pregistration_descriptor->format_identifier, 32, "uimsbf", pregistration_descriptor->format_identifier_char, &bs);

		int info_length = pregistration_descriptor->descriptor_length - 4;
		if (info_length > 0)
		{
			BITS_byteCopy(pregistration_descriptor->additional_identification_info, sizeof(pregistration_descriptor->additional_identification_info), &bs, info_length);
			pregistration_descriptor->additional_identification_info[info_length] = '\0';
		}
		//else
		//{
		//	memset(pregistration_descriptor->additional_identification_info, 0x00, sizeof(pregistration_descriptor->additional_identification_info));
		//}

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节, %s", pregistration_descriptor->descriptor_tag, length, pregistration_descriptor->format_identifier_char);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (pRegistrationDescriptor == NULL)
		{
			delete pregistration_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

//功能：解数据流对齐描述子				0x06
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_data_stream_alignment_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, data_stream_alignment_descriptor_t* pDataStreamAlignmentDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszTemp[64];
	BITS_t bs;

	assert(length == 3);
	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "data_stream_alignment_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		data_stream_alignment_descriptor_t* pdata_stream_alignment_descriptor = (pDataStreamAlignmentDescriptor == NULL) ? new data_stream_alignment_descriptor_t : pDataStreamAlignmentDescriptor;
		memset(pdata_stream_alignment_descriptor, 0x00, sizeof(data_stream_alignment_descriptor_t));

		BITS_map(&bs, buf, length);

		pdata_stream_alignment_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pdata_stream_alignment_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		pdata_stream_alignment_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pdata_stream_alignment_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		pdata_stream_alignment_descriptor->alignment_type = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "alignment_type", pdata_stream_alignment_descriptor->alignment_type, 8, "uimsbf", NULL, &bs);

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pdata_stream_alignment_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (pDataStreamAlignmentDescriptor == NULL)
		{
			delete pdata_stream_alignment_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}


//功能：解目标背景栅格描述子				0x07
//输入：buffer,起始位置:nIndex
//返回：LPVOID指针
//备注：2000.12.15 chendelin

//功能：解视频窗口描述子							0x08
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解CA描述子									0x09
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_CA_descriptor_to_xml(uint8_t* buf, int length, HALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, CA_descriptor_t* pCADescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[128];
	char   pszCASystem[64];
	//char   pszComment[128];

	CA_descriptor_t* pCA_descriptor = (pCADescriptor == NULL) ? new CA_descriptor_t : pCADescriptor;
	rtcode = MPEG2_PSI_decode_CA_descriptor(buf, length, pCA_descriptor);

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		MPEG_DVB_NumericCoding2Text_CASystemID(pCA_descriptor->CA_system_ID, pszCASystem, sizeof(pszCASystem));
		sprintf_s(pszField, sizeof(pszField), "CA_descriptor(<tag: 0x%02X, %d字节, CA_PID=0x%04X, %s>)", pCA_descriptor->descriptor_tag, length, pCA_descriptor->CA_PID, pszCASystem);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL);
		pxmlDoc->SetAnchor(pxmlDescriptorNode);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "descriptor_tag", pCA_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "descriptor_length", pCA_descriptor->descriptor_length, 8, "uimsbf", NULL);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "CA_system_ID", pCA_descriptor->CA_system_ID, 16, "uimsbf", pszCASystem);

		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "reserved", pCA_descriptor->reserved, 3, "uimsbf", NULL);
		XMLDOC_NewElementForBits(pxmlDoc, pxmlDescriptorNode, "CA_PID", pCA_descriptor->CA_PID, 13, "uimsbf", NULL);

		if (pCA_descriptor->private_data_length > 0)
		{
			XMLDOC_NewElementForByteBuf(pxmlDoc, pxmlDescriptorNode, "private_data_byte[ ]", pCA_descriptor->private_data_byte, pCA_descriptor->private_data_length, NULL);
		}

		pxmlDoc->ClearAnchor(pxmlDescriptorNode);
	}

	if (pCADescriptor == NULL)
	{
		delete pCA_descriptor;
	}

	return rtcode;
}

//功能：解ISO639语言描述子							0x0A
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_ISO_639_language_descriptor(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, ISO_639_language_descriptor_t* p639LanguageDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszTemp[64];
	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "ISO_639_language_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		ISO_639_language_descriptor_t* pISO_639_language_descriptor = (p639LanguageDescriptor == NULL) ? new ISO_639_language_descriptor_t : p639LanguageDescriptor;
		memset(pISO_639_language_descriptor, 0x00, sizeof(ISO_639_language_descriptor_t));

		BITS_map(&bs, buf, length);

		pISO_639_language_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pISO_639_language_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		pISO_639_language_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pISO_639_language_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		int N = 0;
		if (pISO_639_language_descriptor->descriptor_length > 0)
		{
			tinyxml2::XMLElement* pxmlLangsLoopNode = pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "LANGUAGES()");

			int loop_length = pISO_639_language_descriptor->descriptor_length;
			while ((loop_length >= 4) && (N < MAX_LANGUAGES))
			{
				memcpy(pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code_char, bs.p_cur, 3);
				pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code_char[3] = '\0';

				tinyxml2::XMLElement* pxmlLangNode = pxmlDoc->NewKeyValuePairElement(pxmlLangsLoopNode, "LANGUAGE()");
				pxmlLangNode->SetAttribute("comment", pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code_char);

				pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code = BITS_get(&bs, 24);
				pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "ISO_639_language_code", pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code, 24, "uimsbf", pISO_639_language_descriptor->ISO_639_language[N].ISO_639_language_code_char, &bs);

				pISO_639_language_descriptor->ISO_639_language[N].audio_type = BITS_get(&bs, 8);
				pxmlDoc->NewKeyValuePairElement(pxmlLangNode, "audio_type", pISO_639_language_descriptor->ISO_639_language[N].audio_type, 8, "uimsbf", NULL, &bs);

				N++;
				loop_length -= 4;
			}
		}
		pISO_639_language_descriptor->N = N;

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pISO_639_language_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (p639LanguageDescriptor == NULL)
		{
			delete pISO_639_language_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解系统时钟描述子描述子				0x0B
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
/*TAG = PSI_SYSTEM_CLOCK_DESCRIPTOR			0x0B*/
int MPEG2_PSI_decode_system_clock_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, system_clock_descriptor_t* pSystemClockDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char  pszTemp[64];
	BITS_t bs;

	assert(length == 4);
	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 4))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "system_clock_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		uint8_t descriptor_tag = buf[0];
		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		system_clock_descriptor_t* psystem_clock_descriptor = (pSystemClockDescriptor == NULL) ? new system_clock_descriptor_t : pSystemClockDescriptor;
		memset(psystem_clock_descriptor, 0x00, sizeof(system_clock_descriptor_t));

		BITS_map(&bs, buf, length);

		psystem_clock_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", psystem_clock_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		psystem_clock_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", psystem_clock_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		psystem_clock_descriptor->external_clock_reference_indicator = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "external_clock_reference_indicator", psystem_clock_descriptor->external_clock_reference_indicator, 1, "bslbf", NULL, &bs);
		psystem_clock_descriptor->reserved0 = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", psystem_clock_descriptor->reserved0, 1, "bslbf", NULL, &bs);
		psystem_clock_descriptor->clock_accuracy_integer = BITS_get(&bs, 6);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "clock_accuracy_integer", psystem_clock_descriptor->clock_accuracy_integer, 6, "uimsbf", NULL, &bs);

		psystem_clock_descriptor->clock_accuracy_exponent = BITS_get(&bs, 3);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "clock_accuracy_exponent", psystem_clock_descriptor->clock_accuracy_exponent, 3, "uimsbf", NULL, &bs);
		psystem_clock_descriptor->reserved1 = BITS_get(&bs, 5);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", psystem_clock_descriptor->reserved1, 5, "bslbf", NULL, &bs);

		if (pSystemClockDescriptor == NULL)
		{
			delete psystem_clock_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解复用缓冲器使用描述子				0x0C
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
int MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, multiplex_buffer_utilization_descriptor_t* pMultiplexBufferUtilizationDescriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;
	char  pszTemp[64];
	BITS_t bs;

	assert(length == 6);
	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 6))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "multiplex_buffer_utilization_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor = (pMultiplexBufferUtilizationDescriptor == NULL) ? new multiplex_buffer_utilization_descriptor_t : pMultiplexBufferUtilizationDescriptor;
		memset(pmultiplex_buffer_utilization_descriptor, 0x00, sizeof(multiplex_buffer_utilization_descriptor_t));

		BITS_map(&bs, buf, length);

		pmultiplex_buffer_utilization_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pmultiplex_buffer_utilization_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		pmultiplex_buffer_utilization_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pmultiplex_buffer_utilization_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		pmultiplex_buffer_utilization_descriptor->bound_valid_flag = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "bound_valid_flag", pmultiplex_buffer_utilization_descriptor->bound_valid_flag, 1, "bslbf", NULL, &bs);
		pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound = BITS_get(&bs, 15);
		DVB_SI_NumericCoding2Text_FrequencyCode(pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "LTW_offset_lower_bound", pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound, 15, "uimsbf", pszTemp, &bs);

		pmultiplex_buffer_utilization_descriptor->reserved = BITS_get(&bs, 1);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", 8, pmultiplex_buffer_utilization_descriptor->reserved, "bslbf", NULL, &bs);
		pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound = BITS_get(&bs, 15);
		DVB_SI_NumericCoding2Text_FrequencyCode(pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound, pszTemp, sizeof(pszTemp));
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "LTW_offset_upper_bound", pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound, 15, "uimsbf", pszTemp, &bs);

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pmultiplex_buffer_utilization_descriptor->descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (pMultiplexBufferUtilizationDescriptor == NULL)
		{
			delete pmultiplex_buffer_utilization_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解版权描述子				0x0D
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解最大比特率描述子					0x0E
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, maximum_bitrate_descriptor_t* pMaximumBitrateDescriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;
	char  pszTemp[64];
	BITS_t bs;

	assert(length == 5);
	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 5))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "maximum_bitrate_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor = (pMaximumBitrateDescriptor == NULL) ? new maximum_bitrate_descriptor_t : pMaximumBitrateDescriptor;
		memset(pmaximum_bitrate_descriptor, 0x00, sizeof(maximum_bitrate_descriptor_t));

		BITS_map(&bs, buf, length);

		pmaximum_bitrate_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pmaximum_bitrate_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		pmaximum_bitrate_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pmaximum_bitrate_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		pmaximum_bitrate_descriptor->reserved = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", pmaximum_bitrate_descriptor->reserved, 2, "bslbf", NULL, &bs);
		pmaximum_bitrate_descriptor->maximum_bitrate = BITS_get(&bs, 22);
		int maximum_bitrate = pmaximum_bitrate_descriptor->maximum_bitrate * 400; //bps
		sprintf_s(pszTemp, sizeof(pszTemp), "%d bps", maximum_bitrate);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "maximum_bitrate", pmaximum_bitrate_descriptor->maximum_bitrate, 22, "uimsbf", pszTemp, &bs);

		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节, maximum_bitrate = %d bps", pmaximum_bitrate_descriptor->descriptor_tag, length, maximum_bitrate);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		if (pMaximumBitrateDescriptor == NULL)
		{
			delete pmaximum_bitrate_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解私有数据指示器描述子				0x0F
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解平滑缓冲器描述子			0x10
//输入：buffer, 开始位置nIndex
//返回：LPVOID指针
//备注：2000.12.14 chendelin
int MPEG2_PSI_decode_smoothing_buffer_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, smoothing_buffer_descriptor_t* pSmoothingBufferDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char  pszTemp[64];
	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "smoothing_buffer_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		uint8_t descriptor_tag = buf[0];
		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", descriptor_tag, length);
		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

		smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor = (pSmoothingBufferDescriptor == NULL) ? new smoothing_buffer_descriptor_t : pSmoothingBufferDescriptor;
		memset(psmoothing_buffer_descriptor, 0x00, sizeof(smoothing_buffer_descriptor_t));

		BITS_map(&bs, buf, length);

		psmoothing_buffer_descriptor->descriptor_tag = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", psmoothing_buffer_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

		psmoothing_buffer_descriptor->descriptor_length = BITS_get(&bs, 8);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", psmoothing_buffer_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

		psmoothing_buffer_descriptor->reserved0 = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", psmoothing_buffer_descriptor->reserved0, 2, "bslbf", NULL, &bs);
		psmoothing_buffer_descriptor->sb_leak_rate = BITS_get(&bs, 22);
		int sb_leak_rate = psmoothing_buffer_descriptor->sb_leak_rate * 400; //bps
		sprintf_s(pszTemp, sizeof(pszTemp), "%d bps", sb_leak_rate);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "sb_leak_rate", psmoothing_buffer_descriptor->sb_leak_rate, 22, "uimsbf", pszTemp, &bs);

		psmoothing_buffer_descriptor->reserved1 = BITS_get(&bs, 2);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "reserved", psmoothing_buffer_descriptor->reserved1, 2, "bslbf", NULL, &bs);
		psmoothing_buffer_descriptor->sb_size = BITS_get(&bs, 22);
		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "sb_size", psmoothing_buffer_descriptor->sb_size, 22, "uimsbf", NULL, &bs);

		if (pSmoothingBufferDescriptor == NULL)
		{
			delete psmoothing_buffer_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//功能：解STD描述子						0x11
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//功能：解IBP描述子						0x12
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针

//0x13
int MPEG2_DSMCC_decode_carousel_identifier_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, carousel_identifier_descriptor_t* pCarouselIdentifierDescriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;
	char  pszTemp[64];
	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "carousel_identifier_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		if ((buf != NULL) && (length >= 6))
		{
			carousel_identifier_descriptor_t* pcarousel_identifier_descriptor = (pCarouselIdentifierDescriptor == NULL) ? new carousel_identifier_descriptor_t : pCarouselIdentifierDescriptor;
			memset(pcarousel_identifier_descriptor, 0x00, sizeof(carousel_identifier_descriptor_t));

			BITS_map(&bs, buf, length);

			pcarousel_identifier_descriptor->descriptor_tag = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pcarousel_identifier_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

			pcarousel_identifier_descriptor->descriptor_length = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pcarousel_identifier_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

			pcarousel_identifier_descriptor->carousel_id = BITS_get(&bs, 32);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "carousel_id", pcarousel_identifier_descriptor->carousel_id, 32, "uimsbf", NULL, &bs);

			int N = pcarousel_identifier_descriptor->descriptor_length - 4;

			if (N > 0)
			{
				pcarousel_identifier_descriptor->N = min(N, 64);
				memcpy(pcarousel_identifier_descriptor->private_data_byte, bs.p_cur, pcarousel_identifier_descriptor->N);
				BITS_byteSkip(&bs, N);

				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "private_data_byte[ ]", pcarousel_identifier_descriptor->private_data_byte, pcarousel_identifier_descriptor->N, NULL, &bs);
			}

			sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pcarousel_identifier_descriptor->descriptor_tag, length);
			pxmlDescriptorNode->SetAttribute("comment", pszTemp);

			if (pCarouselIdentifierDescriptor == NULL)
			{
				delete pcarousel_identifier_descriptor;
			}
		}
		else
		{
			pxmlDescriptorNode->SetAttribute("error", "Parameter Error!");

			rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}

//0x14
int MPEG2_DSMCC_decode_association_tag_descriptor_to_xml(uint8_t* buf, int length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode, association_tag_descriptor_t* pAssociationTagDescriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	int			N2;
	uint8_t*	porigin = buf;
	char		pszTemp[64];

	BITS_t bs;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "association_tag_descriptor()");
		pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

		if ((buf != NULL) && (length >= 7))
		{
			association_tag_descriptor_t* passociation_tag_descriptor = (pAssociationTagDescriptor == NULL) ? new association_tag_descriptor_t : pAssociationTagDescriptor;
			memset(passociation_tag_descriptor, 0x00, sizeof(association_tag_descriptor_t));

			BITS_map(&bs, buf, length);

			passociation_tag_descriptor->descriptor_tag = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", passociation_tag_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

			passociation_tag_descriptor->descriptor_length = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", passociation_tag_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

			passociation_tag_descriptor->association_tag = BITS_get(&bs, 16);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "association_tag", passociation_tag_descriptor->association_tag, 16, "uimsbf", NULL, &bs);

			passociation_tag_descriptor->use = BITS_get(&bs, 16);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "use", passociation_tag_descriptor->use, 16, "uimsbf", NULL, &bs);

			passociation_tag_descriptor->selector_byte_length = BITS_get(&bs, 8);
			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_byte_length", passociation_tag_descriptor->selector_byte_length, 8, "uimsbf", NULL, &bs);

			if (passociation_tag_descriptor->selector_byte_length > 0)
			{
				if (passociation_tag_descriptor->use == 0x0000)
				{
					assert(passociation_tag_descriptor->selector_byte_length == 8);

					passociation_tag_descriptor->u.use_0x0000.transaction_id = BITS_get(&bs, 32);
					pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "transaction_id", passociation_tag_descriptor->u.use_0x0000.transaction_id, 32, "uimsbf", NULL, &bs);

					passociation_tag_descriptor->u.use_0x0000.timeout = BITS_get(&bs, 32);
					pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "timeout", passociation_tag_descriptor->u.use_0x0000.timeout, 32, "uimsbf", NULL, &bs);
				}
				//else if (passociation_tag_descriptor->use == 0x0001)
				//{
				//	passociation_tag_descriptor->selector_length = *buf++;
				//}
				else
				{
					passociation_tag_descriptor->u.use_other.N1 = min(64, passociation_tag_descriptor->selector_byte_length);
					memcpy(passociation_tag_descriptor->u.use_other.selector_byte, bs.p_cur, passociation_tag_descriptor->u.use_other.N1);
					BITS_byteSkip(&bs, passociation_tag_descriptor->selector_byte_length);

					pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_byte[ ]", passociation_tag_descriptor->u.use_other.selector_byte, passociation_tag_descriptor->selector_byte_length, NULL, &bs);
				}
			}
			else
			{
				passociation_tag_descriptor->u.use_0x0000.transaction_id = 0xFFFFFFFF;
			}

			N2 = length - (int)(bs.p_cur - porigin);
			passociation_tag_descriptor->N2 = min(64, N2);
			if (passociation_tag_descriptor->N2 > 0)
			{
				memcpy(passociation_tag_descriptor->private_data_byte, bs.p_cur, passociation_tag_descriptor->N2);
				BITS_byteSkip(&bs, N2);

				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "private_data_byte[ ]", passociation_tag_descriptor->private_data_byte, passociation_tag_descriptor->N2, NULL, &bs);
			}

			sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", passociation_tag_descriptor->descriptor_tag, length);
			pxmlDescriptorNode->SetAttribute("comment", pszTemp);

			if (pAssociationTagDescriptor == NULL)
			{
				delete passociation_tag_descriptor;
			}
		}
		else
		{
			pxmlDescriptorNode->SetAttribute("error", "Parameter Error!");

			rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	}

	return rtcode;
}


//功能：解析deferred_association_tags_descriptor   tag = 0x15    EN301192
// 2009.03.03	added by chendelin	


//0x1B

//0x1C

//  0x20

//0x28

//0x2A



