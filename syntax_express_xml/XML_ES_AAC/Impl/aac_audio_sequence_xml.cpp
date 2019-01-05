#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "syntax_translate_layer/ES_AAC/Include/aac_audio_sequence.h"
#include "syntax_translate_layer/ES_AAC/Include/aac_audio_errorcode.h"
#include "../Include/aac_audio_sequence_xml.h"

int aac_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AAC_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[]", nal_buf, nal_length);
	}
	else
	{
		rtcode = AAC_PARAMETER_ERROR;
	}

	return rtcode;
}

int aac_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AAC_NO_ERROR;

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[]", nal_buf, nal_length);
	}
	else
	{
		rtcode = AAC_PARAMETER_ERROR;
	}

	return rtcode;
}

/*********************************************************************************
	������Decode_header()
	����������AAC��Ƶ��������Ҫ�����ݱ���֡����
	���룺pbs				-- ��Ƶ������
		  pheader			-- �ο�����Ϣͷ���ݣ���������������ݷ���
	���أ�0					-- ��Ϣͷδ�����仯
		  1					-- ��Ϣͷ�����仯
*********************************************************************************/
int	aac_decode_adts_fixed_header_to_xml(uint8_t* adts_fixed_header_buf, int adts_fixed_header_length, TALForXMLDoc* pxmlDoc, AAC_adts_fixed_header_t* padts_fixed_header)
{
	int		rtcode = AAC_UNKNOWN_ERROR;
	//int     header_size = 4;

	//if ((adts_fixed_header_buf != NULL) && (adts_fixed_header_length >= 4) && (padts_fixed_header != NULL))
	//{
	//	memset(padts_fixed_header, 0x00, sizeof(AAC_adts_fixed_header_t));
	//	BITS_map(&bs, adts_fixed_header_buf, adts_fixed_header_length);

	//	padts_fixed_header->Syncword = BITS_get(&bs, 12);
	//	padts_fixed_header->ID = BITS_get(&bs, 1);
	//	padts_fixed_header->Layer = BITS_get(&bs, 2);
	//	padts_fixed_header->protection_absent = BITS_get(&bs, 1);

	//	padts_fixed_header->Profile = BITS_get(&bs, 2);
	//	padts_fixed_header->sampling_frequency_index = BITS_get(&bs, 4);
	//	padts_fixed_header->private_bit = BITS_get(&bs, 1);
	//	padts_fixed_header->channel_configuration = BITS_get(&bs, 3);
	//	padts_fixed_header->original_or_copy = BITS_get(&bs, 1);
	//	padts_fixed_header->Home = BITS_get(&bs, 1);
	//	padts_fixed_header->Emphasis = BITS_get(&bs, 2);

	//	rtcode = AAC_NO_ERROR;
	//}
	//else
	//{
	//	rtcode = AAC_PARAMETER_ERROR;
	//}

	return rtcode;
}

//int	aac_decode_adts_variable_header(FIFO_t* pbs)
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

//int	aac_decode_adts_error_check(FIFO_t* pbs)
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
