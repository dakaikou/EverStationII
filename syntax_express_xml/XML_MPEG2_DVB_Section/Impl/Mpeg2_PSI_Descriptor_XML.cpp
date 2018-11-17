#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

#include "translate_layer/MPEG2_DVB_Section/Include/DVB_SI_Utilities.h"

#include "translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_Common.h"
#include "translate_layer/MPEG2_DVB_Section/Include/MPEG2_DVB_ErrorCode.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Utilities.h"
#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_Descriptor.h"

#include "../Include/Mpeg2_PSI_Descriptor_XML.h"

//功能：解应用描述子
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2005.5 chendelin

//功能：解视频流描述子		0x02
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_video_stream_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, video_stream_descriptor_t* pVideoStreamDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		video_stream_descriptor_t* pvideo_stream_descriptor = (pVideoStreamDescriptor == NULL) ? new video_stream_descriptor_t : pVideoStreamDescriptor;
		rtcode = MPEG2_PSI_decode_video_stream_descriptor(buf, length, pvideo_stream_descriptor);
		MPEG2_PSI_present_video_stream_descriptor_to_xml(pxmlDoc, pxmlParentNode, pvideo_stream_descriptor);

		if (pVideoStreamDescriptor == NULL)
		{
			delete pvideo_stream_descriptor;
		}
	}

	return rtcode;
}

int MPEG2_PSI_present_video_stream_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, video_stream_descriptor_t* pvideo_stream_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszProfile[32];
	char	pszLevel[32];
	char	pszFrameRate[32];
	char	pszField[64];
	char	pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pvideo_stream_descriptor != NULL))
	{
		MPGV_DecodeFrameRateCodetoText(pvideo_stream_descriptor->frame_rate_code, pszFrameRate, sizeof(pszFrameRate));

		sprintf_s(pszField, sizeof(pszField), "video_stream_descriptor(tag: 0x%02X)", pvideo_stream_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pvideo_stream_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pvideo_stream_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pvideo_stream_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "multiple_frame_rate_flag", pvideo_stream_descriptor->multiple_frame_rate_flag, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "frame_rate_code", pvideo_stream_descriptor->frame_rate_code, 4, "uimsbf", pszComment);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "MPEG_1_only_flag", pvideo_stream_descriptor->MPEG_1_only_flag, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "constrained_parameter_flag", pvideo_stream_descriptor->constrained_parameter_flag, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "still_picture_flag", pvideo_stream_descriptor->still_picture_flag, 1, "bslbf", NULL);

		if (pvideo_stream_descriptor->MPEG_1_only_flag == 0)
		{
			MPGV_DecodeProfileCodetoText((pvideo_stream_descriptor->profile_and_level_indication & 0x70) >> 4, pszProfile, sizeof(pszProfile));
			MPGV_DecodeLevelCodetoText((pvideo_stream_descriptor->profile_and_level_indication & 0x0F), pszLevel, sizeof(pszLevel));
			sprintf_s(pszComment, sizeof(pszComment), "%s profile @ %s level", pszProfile, pszLevel);
			pxmlDoc->NewElementForBits(pxmlDescriptorNode, "profile_and_level_indication", pvideo_stream_descriptor->profile_and_level_indication, 8, "uimsbf", pszComment);

			MPGV_DecodeChromaFormatCodetoText(pvideo_stream_descriptor->chroma_format, pszComment, sizeof(pszComment));
			pxmlDoc->NewElementForBits(pxmlDescriptorNode, "chroma_format", pvideo_stream_descriptor->chroma_format, 2, "uimsbf", pszComment);
			pxmlDoc->NewElementForBits(pxmlDescriptorNode, "frame_rate_extension_flag", pvideo_stream_descriptor->frame_rate_extension_flag, 1, "uimsbf", NULL);
			pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", pvideo_stream_descriptor->reserved, 5, "uimsbf", NULL);
		}
	}

	return rtcode;
}

//功能：解音频流描述子			0x03
//输入：buf -- 描述符首地址
//      length -- 描述符长度
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_audio_stream_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, audio_stream_descriptor_t* pAudioStreamDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	{
		audio_stream_descriptor_t* paudio_stream_descriptor = (pAudioStreamDescriptor == NULL) ? new audio_stream_descriptor_t : pAudioStreamDescriptor;
		rtcode = MPEG2_PSI_decode_audio_stream_descriptor(buf, length, paudio_stream_descriptor);
		MPEG2_PSI_present_audio_stream_descriptor_to_xml(pxmlDoc, pxmlParentNode, paudio_stream_descriptor);

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

int MPEG2_PSI_present_audio_stream_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, audio_stream_descriptor_t* paudio_stream_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (paudio_stream_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "audio_stream_descriptor(tag: 0x%02X)", paudio_stream_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, paudio_stream_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", paudio_stream_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", paudio_stream_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "free_format_flag", paudio_stream_descriptor->free_format_flag, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "ID", paudio_stream_descriptor->ID, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "layer", paudio_stream_descriptor->layer, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "variable_rate_audio_indicator", paudio_stream_descriptor->variable_rate_audio_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", paudio_stream_descriptor->reserved, 3, "bslbf", NULL);
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
int MPEG2_PSI_decode_registration_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, registration_descriptor_t* pRegistrationDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 6))
	{
		registration_descriptor_t* pregistration_descriptor = (pRegistrationDescriptor == NULL) ? new registration_descriptor_t : pRegistrationDescriptor;
		rtcode = MPEG2_PSI_decode_registration_descriptor(buf, length, pregistration_descriptor);
		MPEG2_PSI_present_registration_descriptor_to_xml(pxmlDoc, pxmlParentNode, pregistration_descriptor);

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

int MPEG2_PSI_present_registration_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, registration_descriptor_t* pregistration_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pregistration_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "registration_descriptor(tag: 0x%02X)", pregistration_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pregistration_descriptor->format_identifier_char, pregistration_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pregistration_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pregistration_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "format_identifier[ ]", (uint8_t*)pregistration_descriptor->format_identifier_char, 4, pregistration_descriptor->format_identifier_char);

		if (pregistration_descriptor->info_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "additional_identification_info[ ]", 
				(uint8_t*)pregistration_descriptor->additional_identification_info, pregistration_descriptor->info_length, 
				pregistration_descriptor->additional_identification_info);
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
int MPEG2_PSI_decode_data_stream_alignment_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, data_stream_alignment_descriptor_t* pDataStreamAlignmentDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	//assert(length == 3);
	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 3))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "data_stream_alignment_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	data_stream_alignment_descriptor_t* pdata_stream_alignment_descriptor = (pDataStreamAlignmentDescriptor == NULL) ? new data_stream_alignment_descriptor_t : pDataStreamAlignmentDescriptor;
	//	memset(pdata_stream_alignment_descriptor, 0x00, sizeof(data_stream_alignment_descriptor_t));

	//	BITS_map(&bs, buf, length);

	//	pdata_stream_alignment_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pdata_stream_alignment_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//	pdata_stream_alignment_descriptor->descriptor_length = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pdata_stream_alignment_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//	pdata_stream_alignment_descriptor->alignment_type = BITS_get(&bs, 8);
	//	pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "alignment_type", pdata_stream_alignment_descriptor->alignment_type, 8, "uimsbf", NULL, &bs);

	//	sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pdata_stream_alignment_descriptor->descriptor_tag, length);
	//	pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//	if (pDataStreamAlignmentDescriptor == NULL)
	//	{
	//		delete pdata_stream_alignment_descriptor;
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

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
int MPEG2_PSI_decode_CA_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, CA_descriptor_t* pCADescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		CA_descriptor_t* pCA_descriptor = (pCADescriptor == NULL) ? new CA_descriptor_t : pCADescriptor;
		rtcode = MPEG2_PSI_decode_CA_descriptor(buf, length, pCA_descriptor);
		MPEG2_PSI_present_CA_descriptor_to_xml(pxmlDoc, pxmlParentNode, pCA_descriptor);

		if (pCADescriptor == NULL)
		{
			delete pCA_descriptor;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_PSI_present_CA_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, CA_descriptor_t* pCA_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char   pszField[32];
	char   pszCASystem[64];
	char   pszComment[128];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pCA_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "CA_descriptor(tag: 0x%02X)", pCA_descriptor->descriptor_tag);

		MPEG_DVB_NumericCoding2Text_CASystemID(pCA_descriptor->CA_system_ID, pszCASystem, sizeof(pszCASystem));
		sprintf_s(pszComment, sizeof(pszComment), "%s, CA_system_ID=0x%04X, CA_PID=0x%04X", pszCASystem, pCA_descriptor->CA_system_ID, pCA_descriptor->CA_PID);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pCA_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pCA_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pCA_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "CA_system_ID", pCA_descriptor->CA_system_ID, 16, "uimsbf", pszCASystem);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", pCA_descriptor->reserved, 3, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "CA_PID", pCA_descriptor->CA_PID, 13, "uimsbf", NULL);

		if (pCA_descriptor->private_data_length > 0)
		{
			pxmlDoc->NewElementForByteBuf(pxmlDescriptorNode, "private_data_byte[ ]", pCA_descriptor->private_data_byte, pCA_descriptor->private_data_length, NULL);
		}
	}

	return rtcode;
}

//功能：解ISO639语言描述子							0x0A
//输入：buffer, 起始位置nIndex
//返回：LPVOID指针
//备注：2000.11 chendelin
int MPEG2_PSI_decode_ISO_639_language_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ISO_639_language_descriptor_t* p639LanguageDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		ISO_639_language_descriptor_t* pISO_639_language_descriptor = (p639LanguageDescriptor == NULL) ? new ISO_639_language_descriptor_t : p639LanguageDescriptor;

		rtcode = MPEG2_PSI_decode_ISO_639_language_descriptor(buf, length, pISO_639_language_descriptor);
		MPEG2_PSI_present_ISO_639_language_descriptor_to_xml(pxmlDoc, pxmlParentNode, pISO_639_language_descriptor);

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

int MPEG2_PSI_present_ISO_639_language_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, ISO_639_language_descriptor_t* pISO_639_language_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pISO_639_language_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "ISO_639_language_descriptor(tag: 0x%02X)", pISO_639_language_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pISO_639_language_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pISO_639_language_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pISO_639_language_descriptor->descriptor_length, 8, "uimsbf", NULL);

		if (pISO_639_language_descriptor->descriptor_length > 0)
		{
			sprintf_s(pszField, sizeof(pszField), "LANGUAGES(%d 个)", pISO_639_language_descriptor->N);
			XMLElement* pxmlLangsLoopNode = pxmlDoc->NewBranchElement(pxmlDescriptorNode, pszField, NULL, pISO_639_language_descriptor->descriptor_length);

			for (int lang_index = 0; lang_index < pISO_639_language_descriptor->N; lang_index++)
			{
				sprintf_s(pszField, sizeof(pszField), "LANGUAGE[%d]()", lang_index);

				XMLElement* pxmlLangNode = pxmlDoc->NewBranchElement(pxmlLangsLoopNode, pszField, pISO_639_language_descriptor->st[lang_index].ISO_639_language_code_char, 4);

				pxmlDoc->NewElementForByteBuf(pxmlLangNode, "ISO_639_language_code[ ]", 
					(uint8_t*)pISO_639_language_descriptor->st[lang_index].ISO_639_language_code_char, 3, 
					pISO_639_language_descriptor->st[lang_index].ISO_639_language_code_char);

				pxmlDoc->NewElementForBits(pxmlLangNode, "audio_type", pISO_639_language_descriptor->st[lang_index].audio_type, 8, "uimsbf", NULL);
			}
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
int MPEG2_PSI_decode_system_clock_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, system_clock_descriptor_t* pSystemClockDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 4))
	{
		system_clock_descriptor_t* psystem_clock_descriptor = (pSystemClockDescriptor == NULL) ? new system_clock_descriptor_t : pSystemClockDescriptor;

		rtcode = MPEG2_PSI_decode_system_clock_descriptor(buf, length, psystem_clock_descriptor);
		MPEG2_PSI_present_system_clock_descriptor_to_xml(pxmlDoc, pxmlParentNode, psystem_clock_descriptor);

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

int MPEG2_PSI_present_system_clock_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, system_clock_descriptor_t* psystem_clock_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (psystem_clock_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "system_clock_descriptor(tag: 0x%02X)", psystem_clock_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, psystem_clock_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", psystem_clock_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", psystem_clock_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "external_clock_reference_indicator", psystem_clock_descriptor->external_clock_reference_indicator, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", psystem_clock_descriptor->reserved0, 1, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "clock_accuracy_integer", psystem_clock_descriptor->clock_accuracy_integer, 6, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "clock_accuracy_exponent", psystem_clock_descriptor->clock_accuracy_exponent, 3, "uimsbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", psystem_clock_descriptor->reserved1, 5, "bslbf", NULL);
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
int MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiplex_buffer_utilization_descriptor_t* pMultiplexBufferUtilizationDescriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;

	assert(length == 6);
	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 6))
	{
		multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor = (pMultiplexBufferUtilizationDescriptor == NULL) ? new multiplex_buffer_utilization_descriptor_t : pMultiplexBufferUtilizationDescriptor;

		rtcode = MPEG2_PSI_decode_multiplex_buffer_utilization_descriptor(buf, length, pmultiplex_buffer_utilization_descriptor);
		MPEG2_PSI_present_multiplex_buffer_utilization_descriptor_to_xml(pxmlDoc, pxmlParentNode, pmultiplex_buffer_utilization_descriptor);

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

int MPEG2_PSI_present_multiplex_buffer_utilization_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, multiplex_buffer_utilization_descriptor_t* pmultiplex_buffer_utilization_descriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;
	char  pszField[64];
	char  pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmultiplex_buffer_utilization_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "multiplex_buffer_utilization_descriptor(tag: 0x%02X)", pmultiplex_buffer_utilization_descriptor->descriptor_tag);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, pmultiplex_buffer_utilization_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pmultiplex_buffer_utilization_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pmultiplex_buffer_utilization_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "bound_valid_flag", pmultiplex_buffer_utilization_descriptor->bound_valid_flag, 1, "bslbf", NULL);
		DVB_SI_NumericCoding2Text_FrequencyCode(pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "LTW_offset_lower_bound", pmultiplex_buffer_utilization_descriptor->LTW_offset_lower_bound, 15, "uimsbf", pszComment);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", 8, pmultiplex_buffer_utilization_descriptor->reserved, "bslbf", NULL);
		DVB_SI_NumericCoding2Text_FrequencyCode(pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound, pszComment, sizeof(pszComment));
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "LTW_offset_upper_bound", pmultiplex_buffer_utilization_descriptor->LTW_offset_upper_bound, 15, "uimsbf", pszComment);
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
int MPEG2_PSI_decode_maximum_bitrate_descriptor_to_xml(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, maximum_bitrate_descriptor_t* pMaximumBitrateDescriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;

	assert(length == 5);
	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 5))
	{
		maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor = (pMaximumBitrateDescriptor == NULL) ? new maximum_bitrate_descriptor_t : pMaximumBitrateDescriptor;

		rtcode = MPEG2_PSI_decode_maximum_bitrate_descriptor(buf, length, pmaximum_bitrate_descriptor);
		MPEG2_PSI_present_maximum_bitrate_descriptor_to_xml(pxmlDoc, pxmlParentNode, pmaximum_bitrate_descriptor);

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

int MPEG2_PSI_present_maximum_bitrate_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, maximum_bitrate_descriptor_t* pmaximum_bitrate_descriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;
	char  pszField[48];
	char  pszMaxium[48];
	char  pszComment[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (pmaximum_bitrate_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "maximum_bitrate_descriptor(tag: 0x%02X)", pmaximum_bitrate_descriptor->descriptor_tag);
		int maximum_bitrate = pmaximum_bitrate_descriptor->maximum_bitrate * 400; //bps
		sprintf_s(pszMaxium, sizeof(pszMaxium), "%d bps", maximum_bitrate);
		sprintf_s(pszComment, sizeof(pszComment), "Max: %s", pszMaxium);

		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, pszComment, pmaximum_bitrate_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", pmaximum_bitrate_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", pmaximum_bitrate_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", pmaximum_bitrate_descriptor->reserved, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "maximum_bitrate", pmaximum_bitrate_descriptor->maximum_bitrate, 22, "uimsbf", pszMaxium);
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
int MPEG2_PSI_decode_smoothing_buffer_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, smoothing_buffer_descriptor_t* pSmoothingBufferDescriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (buf != NULL) && (length >= 2))
	{
		smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor = (pSmoothingBufferDescriptor == NULL) ? new smoothing_buffer_descriptor_t : pSmoothingBufferDescriptor;

		rtcode = MPEG2_PSI_decode_smoothing_buffer_descriptor(buf, length, psmoothing_buffer_descriptor);
		MPEG2_PSI_present_smoothing_buffer_descriptor_to_xml(pxmlDoc, pxmlParentNode, psmoothing_buffer_descriptor);

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

int MPEG2_PSI_present_smoothing_buffer_descriptor_to_xml(TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, smoothing_buffer_descriptor_t* psmoothing_buffer_descriptor)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	char	pszField[48];
	char	pszComment[48];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL) && (psmoothing_buffer_descriptor != NULL))
	{
		sprintf_s(pszField, sizeof(pszField), "smoothing_buffer_descriptor(tag: 0x%02X)", psmoothing_buffer_descriptor->descriptor_tag);
		XMLElement* pxmlDescriptorNode = pxmlDoc->NewBranchElement(pxmlParentNode, pszField, NULL, psmoothing_buffer_descriptor->descriptor_length + 2);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_tag", psmoothing_buffer_descriptor->descriptor_tag, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "descriptor_length", psmoothing_buffer_descriptor->descriptor_length, 8, "uimsbf", NULL);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", psmoothing_buffer_descriptor->reserved0, 2, "bslbf", NULL);

		int sb_leak_rate = psmoothing_buffer_descriptor->sb_leak_rate * 400; //bps
		sprintf_s(pszComment, sizeof(pszComment), "%d bps", sb_leak_rate);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "sb_leak_rate", psmoothing_buffer_descriptor->sb_leak_rate, 22, "uimsbf", pszComment);

		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "reserved", psmoothing_buffer_descriptor->reserved1, 2, "bslbf", NULL);
		pxmlDoc->NewElementForBits(pxmlDescriptorNode, "sb_size", psmoothing_buffer_descriptor->sb_size, 22, "uimsbf", NULL);
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
int MPEG2_DSMCC_decode_carousel_identifier_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, carousel_identifier_descriptor_t* pCarouselIdentifierDescriptor)
{
	int	  rtcode = SECTION_PARSE_NO_ERROR;
	//char  pszTemp[64];
	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	//{
	//	XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "carousel_identifier_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	if ((buf != NULL) && (length >= 6))
	//	{
	//		carousel_identifier_descriptor_t* pcarousel_identifier_descriptor = (pCarouselIdentifierDescriptor == NULL) ? new carousel_identifier_descriptor_t : pCarouselIdentifierDescriptor;
	//		memset(pcarousel_identifier_descriptor, 0x00, sizeof(carousel_identifier_descriptor_t));

	//		BITS_map(&bs, buf, length);

	//		pcarousel_identifier_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", pcarousel_identifier_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//		pcarousel_identifier_descriptor->descriptor_length = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", pcarousel_identifier_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//		pcarousel_identifier_descriptor->carousel_id = BITS_get(&bs, 32);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "carousel_id", pcarousel_identifier_descriptor->carousel_id, 32, "uimsbf", NULL, &bs);

	//		int N = pcarousel_identifier_descriptor->descriptor_length - 4;

	//		if (N > 0)
	//		{
	//			pcarousel_identifier_descriptor->N = min(N, 64);
	//			memcpy(pcarousel_identifier_descriptor->private_data_byte, bs.p_cur, pcarousel_identifier_descriptor->N);
	//			BITS_byteSkip(&bs, N);

	//			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "private_data_byte[ ]", pcarousel_identifier_descriptor->private_data_byte, pcarousel_identifier_descriptor->N, NULL, &bs);
	//		}

	//		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", pcarousel_identifier_descriptor->descriptor_tag, length);
	//		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//		if (pCarouselIdentifierDescriptor == NULL)
	//		{
	//			delete pcarousel_identifier_descriptor;
	//		}
	//	}
	//	else
	//	{
	//		pxmlDescriptorNode->SetAttribute("error", "Parameter Error!");

	//		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}

//0x14
int MPEG2_DSMCC_decode_association_tag_descriptor_to_xml(uint8_t* buf, int length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, association_tag_descriptor_t* pAssociationTagDescriptor)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	//int			N2;
	//uint8_t*	porigin = buf;
	//char		pszTemp[64];

	//BITS_t bs;

	//if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	//{
	//	tinyxml2::XMLElement* pxmlDescriptorNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "association_tag_descriptor()");
	//	pxmlDoc->UpdateBufMark(pxmlDescriptorNode, buf, buf + length);

	//	if ((buf != NULL) && (length >= 7))
	//	{
	//		association_tag_descriptor_t* passociation_tag_descriptor = (pAssociationTagDescriptor == NULL) ? new association_tag_descriptor_t : pAssociationTagDescriptor;
	//		memset(passociation_tag_descriptor, 0x00, sizeof(association_tag_descriptor_t));

	//		BITS_map(&bs, buf, length);

	//		passociation_tag_descriptor->descriptor_tag = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_tag", passociation_tag_descriptor->descriptor_tag, 8, "uimsbf", NULL, &bs);

	//		passociation_tag_descriptor->descriptor_length = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "descriptor_length", passociation_tag_descriptor->descriptor_length, 8, "uimsbf", NULL, &bs);

	//		passociation_tag_descriptor->association_tag = BITS_get(&bs, 16);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "association_tag", passociation_tag_descriptor->association_tag, 16, "uimsbf", NULL, &bs);

	//		passociation_tag_descriptor->use = BITS_get(&bs, 16);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "use", passociation_tag_descriptor->use, 16, "uimsbf", NULL, &bs);

	//		passociation_tag_descriptor->selector_byte_length = BITS_get(&bs, 8);
	//		pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_byte_length", passociation_tag_descriptor->selector_byte_length, 8, "uimsbf", NULL, &bs);

	//		if (passociation_tag_descriptor->selector_byte_length > 0)
	//		{
	//			if (passociation_tag_descriptor->use == 0x0000)
	//			{
	//				assert(passociation_tag_descriptor->selector_byte_length == 8);

	//				passociation_tag_descriptor->u.use_0x0000.transaction_id = BITS_get(&bs, 32);
	//				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "transaction_id", passociation_tag_descriptor->u.use_0x0000.transaction_id, 32, "uimsbf", NULL, &bs);

	//				passociation_tag_descriptor->u.use_0x0000.timeout = BITS_get(&bs, 32);
	//				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "timeout", passociation_tag_descriptor->u.use_0x0000.timeout, 32, "uimsbf", NULL, &bs);
	//			}
	//			//else if (passociation_tag_descriptor->use == 0x0001)
	//			//{
	//			//	passociation_tag_descriptor->selector_length = *buf++;
	//			//}
	//			else
	//			{
	//				passociation_tag_descriptor->u.use_other.N1 = min(64, passociation_tag_descriptor->selector_byte_length);
	//				memcpy(passociation_tag_descriptor->u.use_other.selector_byte, bs.p_cur, passociation_tag_descriptor->u.use_other.N1);
	//				BITS_byteSkip(&bs, passociation_tag_descriptor->selector_byte_length);

	//				pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "selector_byte[ ]", passociation_tag_descriptor->u.use_other.selector_byte, passociation_tag_descriptor->selector_byte_length, NULL, &bs);
	//			}
	//		}
	//		else
	//		{
	//			passociation_tag_descriptor->u.use_0x0000.transaction_id = 0xFFFFFFFF;
	//		}

	//		N2 = length - (int)(bs.p_cur - porigin);
	//		passociation_tag_descriptor->N2 = min(64, N2);
	//		if (passociation_tag_descriptor->N2 > 0)
	//		{
	//			memcpy(passociation_tag_descriptor->private_data_byte, bs.p_cur, passociation_tag_descriptor->N2);
	//			BITS_byteSkip(&bs, N2);

	//			pxmlDoc->NewKeyValuePairElement(pxmlDescriptorNode, "private_data_byte[ ]", passociation_tag_descriptor->private_data_byte, passociation_tag_descriptor->N2, NULL, &bs);
	//		}

	//		sprintf_s(pszTemp, sizeof(pszTemp), "tag: 0x%02X, %d字节", passociation_tag_descriptor->descriptor_tag, length);
	//		pxmlDescriptorNode->SetAttribute("comment", pszTemp);

	//		if (pAssociationTagDescriptor == NULL)
	//		{
	//			delete passociation_tag_descriptor;
	//		}
	//	}
	//	else
	//	{
	//		pxmlDescriptorNode->SetAttribute("error", "Parameter Error!");

	//		rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//	}
	//}
	//else
	//{
	//	rtcode = SECTION_PARSE_PARAMETER_ERROR;					//输入参数错误
	//}

	return rtcode;
}


//功能：解析deferred_association_tags_descriptor   tag = 0x15    EN301192
// 2009.03.03	added by chendelin	


//0x1B

//0x1C

//  0x20

//0x28

//0x2A



