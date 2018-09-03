#include <stdio.h>

#include "../Include/HALForTinyXML2Doc.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

XMLDocForMpegSyntax::XMLDocForMpegSyntax(void)
{
	m_origin_ptr = NULL;
}

XMLDocForMpegSyntax::~XMLDocForMpegSyntax(void)
{
}

void XMLDocForMpegSyntax::SetOrigin(unsigned char* origin_ptr)
{
	m_origin_ptr = origin_ptr;
}

//void XMLDocForMpegSyntax::SetActiveNode(tinyxml2::XMLElement* pActive)
//{
//	m_pActive = pActive;
//}
//
//tinyxml2::XMLElement* XMLDocForMpegSyntax::GetActiveNode(void)
//{
//	return m_pActive;
//}

void XMLDocForMpegSyntax::UpdateBufMark(tinyxml2::XMLElement* pxmlNode, uint8_t* start_ptr, uint8_t* end_ptr)
{
	if (pxmlNode != NULL)
	{
		int offset = (int)(start_ptr - m_origin_ptr);
		int length = (int)(end_ptr - start_ptr);
		if (length == 0)
		{
			length = 1;
		}

		pxmlNode->SetAttribute("offset", offset);
		pxmlNode->SetAttribute("length", length);

	}
}

//tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValueElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, unsigned int key_value, int bits, const char* key_comment)
//{
//	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(key_name);
//
//	pxmlNewElement->SetAttribute("value", key_value);
//
//	if (bits > 0)
//	{
//		pxmlNewElement->SetAttribute("bits", bits);
//	}
//
//	if (key_comment != NULL)
//	{
//		pxmlNewElement->SetAttribute("comment", key_comment);
//	}
//
//	pxmlParent->InsertEndChild(pxmlNewElement);
//
//	return pxmlNewElement;
//}

//tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValueElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, unsigned int key_value, int bits)
//{
//	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(key_name);
//
//	pxmlNewElement->SetAttribute("value", key_value);
//
//	if (bits > 0)
//	{
//		pxmlNewElement->SetAttribute("bits", bits);
//	}
//
//	pxmlParent->InsertEndChild(pxmlNewElement);
//
//	return pxmlNewElement;
//}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, const char* string)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(key_name);

	pxmlNewElement->SetAttribute("value", string);

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

//tinyxml2::XMLElement * XMLDocForMpegSyntax::NewTitleElement(tinyxml2::XMLElement* pxmlParent, const char* title_name)
//{
//	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(title_name);
//
//	pxmlParent->InsertEndChild(pxmlNewElement);
//
//	return pxmlNewElement;
//}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits, const char* mnemonic, const char* pszComment, const BITS_t* pbits_map)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(key_name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);
		if (mnemonic != NULL)
		{
			pxmlNewElement->SetAttribute("mnemonic", mnemonic);
		}
		pxmlNewElement->SetAttribute("value", key_value);
	}

	if (pbits_map != NULL)
	{
		int offset = (int)(pbits_map->p_old - m_origin_ptr);
		int length = (int)(pbits_map->p_cur - pbits_map->p_old);

		if (pbits_map->i_left < 8)
		{
			length ++;
		}

		pxmlNewElement->SetAttribute("offset", offset);
		pxmlNewElement->SetAttribute("length", length);
	}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

//tinyxml2::XMLElement * XMLDocForMpegSyntax::NewSyntaxElement(tinyxml2::XMLElement* pxmlParent, const char* field_name, unsigned int value, int bits, const char* mnemonic, const char* pszComment, uint8_t* start_buf, uint8_t* end_buf)
//{
//	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(field_name);
//
//	if (bits > 0)
//	{
//		pxmlNewElement->SetAttribute("bits", bits);
//		pxmlNewElement->SetAttribute("mnemonic", mnemonic);
//		pxmlNewElement->SetAttribute("value", value);
//	}
//
//	if (start_buf != NULL)
//	{
//		int offset = (int)(start_buf - m_origin_ptr);
//		int length = (int)(end_buf - start_buf);
//
//		int byte_count = (bits + 7) / 8;
//		if (length < byte_count)
//		{
//			length = byte_count;
//		}
//
//		pxmlNewElement->SetAttribute("offset", offset);
//		pxmlNewElement->SetAttribute("length", length);
//	}
//
//	if (pszComment != NULL)
//	{
//		pxmlNewElement->SetAttribute("comment", pszComment);
//	}
//
//	pxmlParent->InsertEndChild(pxmlNewElement);
//
//	return pxmlNewElement;
//}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment, const BITS_t* pbits_map)
{
	char pszTemp[48];
	int	 i;
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(key_name);

	if (byte_buf != NULL)
	{
		memset(pszTemp, 0x00, sizeof(pszTemp));

		for (i = 0; i < min(8, byte_length); i++)
		{
			sprintf_s(pszTemp + i * 3, 4, "%02X ", byte_buf[i]);
		}
		if (i < byte_length)
		{
			sprintf_s(pszTemp + i * 3, 4, "...");
		}
		else
		{
			pszTemp[i * 3] = '\0';
		}

		pxmlNewElement->SetAttribute("value", pszTemp);
	}

	if (pbits_map != NULL)
	{
		int offset = (int)(pbits_map->p_old - m_origin_ptr);
		int length = (int)(pbits_map->p_cur - pbits_map->p_old);

		if (pbits_map->i_left < 8)
		{
			length++;
		}

		pxmlNewElement->SetAttribute("offset", offset);
		pxmlNewElement->SetAttribute("length", length);
	}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}


tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValuePairElementX64Mode(tinyxml2::XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits, const char* mnemonic, const char* pszComment, const BITS_t* pbits_map)
{
	assert((bits % 8) == 0);

	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(key_name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);
		pxmlNewElement->SetAttribute("mnemonic", mnemonic);
		pxmlNewElement->SetAttribute("value", (int64_t)key_value);
	}

	if (pbits_map != NULL)
	{
		int offset = (int)(pbits_map->p_old - m_origin_ptr);
		int length = (int)(pbits_map->p_cur - pbits_map->p_old);
		if (pbits_map->i_left < 8)
		{
			length++;
		}

		pxmlNewElement->SetAttribute("offset", offset);
		pxmlNewElement->SetAttribute("length", length);
	}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValuePairElementByteMode(tinyxml2::XMLElement* pxmlParent, const char* field_name, unsigned int value, int bytes, const char* pszComment, const BYTES_t* pbytes_map)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(field_name);

	if (bytes > 0)
	{
		pxmlNewElement->SetAttribute("bytes", bytes);
		pxmlNewElement->SetAttribute("value", value);
	}

	if (pbytes_map != NULL)
	{
		int offset = (int)(pbytes_map->p_old - m_origin_ptr);
		int length = (int)(pbytes_map->p_cur - pbytes_map->p_old);

		assert(length >= 1);

		pxmlNewElement->SetAttribute("offset", offset);
		pxmlNewElement->SetAttribute("length", length);
	}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewKeyValuePairElementByteMode(tinyxml2::XMLElement* pxmlParent, const char* field_name, const uint8_t* byte_buf, int byte_length, const char* pszComment, const BYTES_t* pbytes_map)
{
	char pszTemp[48];
	int	 i;
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(field_name);

	if (byte_buf != NULL)
	{
		memset(pszTemp, 0x00, sizeof(pszTemp));

		for (i = 0; i < min(8, byte_length); i++)
		{
			sprintf_s(pszTemp + i * 3, 4, "%02X ", byte_buf[i]);
		}
		if (i < byte_length)
		{
			sprintf_s(pszTemp + i * 3, 4, "...");
		}
		else
		{
			pszTemp[i * 3] = '\0';
		}

		pxmlNewElement->SetAttribute("value", pszTemp);
	}

	if (pbytes_map != NULL)
	{
		int offset = (int)(pbytes_map->p_old - m_origin_ptr);
		int length = (int)(pbytes_map->p_cur - pbytes_map->p_old);

		assert(length >= 1);

		pxmlNewElement->SetAttribute("offset", offset);
		pxmlNewElement->SetAttribute("length", length);
	}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

tinyxml2::XMLElement* XMLDocForMpegSyntax::QueryNodeByKeyName(tinyxml2::XMLElement* pxmlParentNode, const char* pszNodeName)
{
	tinyxml2::XMLElement* pxmlMatchNode = NULL;

	if (pxmlParentNode != NULL)
	{
		const char* pszName = pxmlParentNode->Name();
		if (strcmp(pszName, pszNodeName) == 0)
		{
			pxmlMatchNode = pxmlParentNode;
		}
		else
		{
			pxmlMatchNode = QueryNodeByKeyName(pxmlParentNode->FirstChildElement(), pszNodeName);
			if (pxmlMatchNode == NULL)
			{
				pxmlMatchNode = QueryNodeByKeyName(pxmlParentNode->NextSiblingElement(), pszNodeName);
			}
		}
	}

	return pxmlMatchNode;
}

