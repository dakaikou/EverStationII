#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../../Include/aac_audio_sequence.h"
#include "../../Include/aac_audio_errorcode.h"
#include "../Include/aac_audio_sequence_xml.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

int aac_decode_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = AAC_NO_ERROR;
	char			pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unaligned nal()", nal_buf, nal_length);
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);
	}
	else
	{
		rtcode = AAC_PARAMETER_ERROR;
	}

	return rtcode;
}

int aac_decode_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc, tinyxml2::XMLElement* pxmlParentNode)
{
	int rtcode = AAC_NO_ERROR;
	char			pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		tinyxml2::XMLElement* pxmlUnknownNalNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "unknown nal()", nal_buf, nal_length);
		pxmlDoc->UpdateBufMark(pxmlUnknownNalNode, nal_buf, nal_buf + nal_length);
		sprintf_s(pszTemp, sizeof(pszTemp), "%d字节", nal_length);
		pxmlUnknownNalNode->SetAttribute("comment", pszTemp);
	}
	else
	{
		rtcode = AAC_PARAMETER_ERROR;
	}

	return rtcode;
}

/*********************************************************************************
	函数：Decode_header()
	描述：解析AAC音频数据流，要求数据必须帧对齐
	输入：pbs				-- 音频数据流
		  pheader			-- 参考的信息头数据，并将解析后的数据返回
	返回：0					-- 信息头未发生变化
		  1					-- 信息头发生变化
*********************************************************************************/
int	aac_decode_adts_fixed_header_to_xml(uint8_t* adts_fixed_header_buf, int adts_fixed_header_length, XMLDocForMpegSyntax* pxmlDoc, AAC_adts_fixed_header_t* padts_fixed_header)
{
	BITS_t	bs;
	int		rtcode = AAC_UNKNOWN_ERROR;
	int     header_size = 4;

	if ((adts_fixed_header_buf != NULL) && (adts_fixed_header_length >= 4) && (padts_fixed_header != NULL))
	{
		memset(padts_fixed_header, 0x00, sizeof(AAC_adts_fixed_header_t));
		BITS_map(&bs, adts_fixed_header_buf, adts_fixed_header_length);

		padts_fixed_header->Syncword = BITS_get(&bs, 12);
		padts_fixed_header->ID = BITS_get(&bs, 1);
		padts_fixed_header->Layer = BITS_get(&bs, 2);
		padts_fixed_header->protection_absent = BITS_get(&bs, 1);

		padts_fixed_header->Profile = BITS_get(&bs, 2);
		padts_fixed_header->sampling_frequency_index = BITS_get(&bs, 4);
		padts_fixed_header->private_bit = BITS_get(&bs, 1);
		padts_fixed_header->channel_configuration = BITS_get(&bs, 3);
		padts_fixed_header->original_or_copy = BITS_get(&bs, 1);
		padts_fixed_header->Home = BITS_get(&bs, 1);
		padts_fixed_header->Emphasis = BITS_get(&bs, 2);

		rtcode = AAC_NO_ERROR;
	}
	else
	{
		rtcode = AAC_PARAMETER_ERROR;
	}

	return rtcode;
}

//S32	aac_decode_adts_variable_header(FIFO_t* pbs)
//{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	adts_variable_header.copyright_identification_bit = FIFO_Get(pbs, 1);
//	adts_variable_header.copyright_identification_start = FIFO_Get(pbs, 1);
//	adts_variable_header.aac_frame_length = FIFO_Get(pbs, 13);
//	adts_variable_header.adts_buffer_fullness = FIFO_Get(pbs, 11);
//	adts_variable_header.no_raw_data_blocks_in_frame = FIFO_Get(pbs, 2);
//
////	::SetEvent(ghAccessEvent);
//
//	return 0;
//}

//S32	aac_decode_adts_error_check(FIFO_t* pbs)
//{
////	::WaitForSingleObject(ghAccessEvent, INFINITE);
//
//	if (adts_fixed_header.protection_absent == 0)
//	{
//		adts_error_check.crc_check = FIFO_Get(pbs, 16);
//	}
//	else
//	{
//		adts_error_check.crc_check = 0x0000;
//	}
//	
////	::SetEvent(ghAccessEvent);
//
//	return 0;
//}
