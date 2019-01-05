#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "syntax_translate_layer/ES_AC3/Include/ac3_audio_sequence.h"
#include "syntax_translate_layer/ES_AC3/Include/ac3_audio_errorcode.h"
#include "syntax_translate_layer/ES_AC3/Include/ac3_audio_utilities.h"

#include "thirdparty_abstract_layer\TALForXML\Include\TALForXML.h"

#include "../Include/ac3_audio_sequence_xml.h"

int ac3_present_unaligned_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AC3_NO_ERROR;

	if ((nal_buf != NULL) && (nal_length > 0) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unaligned nal[ ]", nal_buf, nal_length);
	}
	else
	{
		rtcode = AC3_PARAMETER_ERROR;
	}

	return rtcode;
}

int ac3_present_unknown_nal_to_xml(uint8_t* nal_buf, int nal_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode)
{
	int rtcode = AC3_NO_ERROR;

	if ((nal_buf != NULL) && (nal_length > 0) && (pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		XMLElement* pxmlUnknownNalNode = pxmlDoc->NewElementForByteBuf(pxmlParentNode, "unknown nal[ ]", nal_buf, nal_length);
	}
	else
	{
		rtcode = AC3_PARAMETER_ERROR;
	}

	return rtcode;
}

int	ac3_decode_syncinfo_to_xml(uint8_t* syncinfo_buf, int syncinfo_length, TALForXMLDoc* pxmlDoc, AC3_syncinfo_t* pac3_syncinfo)
{
	int		rtcode = AC3_UNKNOWN_ERROR;
	int     header_size = 4;

	if ((syncinfo_buf != NULL) && (syncinfo_length >= 5) && (pac3_syncinfo != NULL))
	{
		//memset(pac3_syncinfo, 0x00, sizeof(AC3_syncinfo_t));
		//BITS_map(&bs, syncinfo_buf, syncinfo_length);

		//pac3_syncinfo->syncword = BITS_get(&bs, 16);
		//pac3_syncinfo->crc1 = BITS_get(&bs, 16);
		//pac3_syncinfo->fscod = BITS_get(&bs, 2);
		//pac3_syncinfo->frmsizecod = BITS_get(&bs, 6);

		rtcode = AC3_NO_ERROR;
	}
	else
	{
		rtcode = AC3_PARAMETER_ERROR;
	}

	return rtcode;
}

int	ac3_decode_syncframe_to_xml(uint8_t* es_frame_buf, int es_frame_length, TALForXMLDoc* pxmlDoc, XMLElement* pxmlParentNode, AC3_syncframe_t* pAC3Syncframe)
{
	int		rtcode = AC3_UNKNOWN_ERROR;
	int     header_size = 4;
	//char    pszTemp[64];

	if ((pxmlDoc != NULL) && (pxmlParentNode != NULL))
	{
		//tinyxml2::XMLElement* pxmlFrameNode = pxmlDoc->NewKeyValuePairElement(pxmlParentNode, "syncframe()");
		//pxmlDoc->UpdateBufMark(pxmlFrameNode, es_frame_buf, es_frame_buf + es_frame_length);
		//sprintf_s(pszTemp, sizeof(pszTemp), "%d×Ö½Ú", es_frame_length);
		//pxmlFrameNode->SetAttribute("comment", pszTemp);

		//if ((es_frame_buf != NULL) && (es_frame_length >= 4))
		//{
		//	AC3_syncframe_t* pac3_syncframe = (pAC3Syncframe == NULL) ? new AC3_syncframe_t : pAC3Syncframe;
		//	memset(pac3_syncframe, 0x00, sizeof(AC3_syncframe_t));
		//	BITS_map(&bs, es_frame_buf, es_frame_length);

		//	uint8_t* old_header_ptr = bs.p_cur;
		//	tinyxml2::XMLElement* pxmlHeaderNode = pxmlDoc->NewKeyValuePairElement(pxmlFrameNode, "header()");

		//	AC3_syncinfo_t* pac3_syncinfo = &(pac3_syncframe->syncinfo);

		//	pac3_syncinfo->syncword = BITS_get(&bs, 16);
		//	pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "syncword", pac3_syncinfo->syncword, 16, "bslbf", NULL, &bs);

		//	pac3_syncinfo->crc1 = BITS_get(&bs, 16);
		//	pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "crc1", pac3_syncinfo->crc1, 16, "bslbf", NULL, &bs);

		//	pac3_syncinfo->fscod = BITS_get(&bs, 2);
		//	AC3_DecodeSamplingFrequency(pac3_syncinfo->fscod, pszTemp, sizeof(pszTemp));
		//	pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "fscod", pac3_syncinfo->fscod, 2, "bslbf", pszTemp, &bs);

		//	pac3_syncinfo->frmsizecod = BITS_get(&bs, 6);
		//	AC3_DecodeFrameSize(pac3_syncinfo->frmsizecod, pac3_syncinfo->fscod, pszTemp, sizeof(pszTemp));
		//	pxmlDoc->NewKeyValuePairElement(pxmlHeaderNode, "frmsizecod", pac3_syncinfo->frmsizecod, 6, "bslbf", pszTemp, &bs);

		//	pxmlDoc->UpdateBufMark(pxmlHeaderNode, old_header_ptr, bs.p_cur);

		//	if (pAC3Syncframe == NULL)
		//	{
		//		delete pac3_syncinfo;
		//	}

		//	rtcode = AC3_NO_ERROR;
		//}
		//else
		//{
		//	pxmlFrameNode->SetAttribute("error", "parameter error !");
		//	rtcode = AC3_PARAMETER_ERROR;
		//}
	}
	else
	{
		rtcode = AC3_PARAMETER_ERROR;
	}

	return rtcode;
}

