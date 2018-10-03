#include <stdio.h>

#include "../Include/HALForTinyXML2.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

HALForXMLDoc::HALForXMLDoc(void)
{
	m_bs_tracer.offset = 0;
	m_bs_tracer.i_left = 8;
}

HALForXMLDoc::~HALForXMLDoc(void)
{
}

void HALForXMLDoc::SetSyncOffset(int offset)
{
	assert(m_bs_tracer.i_left == 8);
	m_bs_tracer.offset = offset;
}

//int HALForXMLDoc::GetSyncOffset(void)
//{
//	return m_bs_tracer.offset;
//}

//XMLElement * HALForXMLDoc::NewElementForString(XMLElement* pxmlParent, const char* key_name, const char* string, const char* pszComment)
XMLElement * HALForXMLDoc::NewElementForString(XMLElement* pxmlParent, const char* key_name, const char* pszComment)
{
	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

	//if (string != NULL)
	//{
	//	pxmlNewElement->SetAttribute("value", string);
	//}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	if (pxmlParent != NULL)
	{
		pxmlParent->InsertEndChild(pxmlNewElement);
	}

	return pxmlNewElement;
}

XMLElement * HALForXMLDoc::NewElementForBits(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits, const char* mnemonic, const char* pszComment)
{
	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);

		int next_offset = m_bs_tracer.offset;
		int length = 0;
		while (m_bs_tracer.i_left < bits)
		{
			bits -= m_bs_tracer.i_left;
			m_bs_tracer.i_left = 8;
			next_offset++;
			length++;
		}

		m_bs_tracer.i_left -= bits;
		if (m_bs_tracer.i_left == 0)
		{
			m_bs_tracer.i_left = 8;
			next_offset++;
		}
		length++;

		pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);
		pxmlNewElement->SetAttribute("length", length);

		m_bs_tracer.offset = next_offset;
	}

	if (mnemonic != NULL)
	{
		pxmlNewElement->SetAttribute("mnemonic", mnemonic);
	}
	pxmlNewElement->SetAttribute("value", key_value);

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	if (pxmlParent != NULL)
	{
		pxmlParent->InsertEndChild(pxmlNewElement);
	}

	return pxmlNewElement;
}

XMLElement * HALForXMLDoc::NewElementForBytes(XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment)
{
	char pszTemp[48];
	int	 i;
	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

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

	assert(m_bs_tracer.i_left == 8);

	int next_offset = m_bs_tracer.offset + byte_length;
	int length = byte_length;

	pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);
	pxmlNewElement->SetAttribute("length", length);

	m_bs_tracer.offset = next_offset;

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	if (pxmlParent != NULL)
	{
		pxmlParent->InsertEndChild(pxmlNewElement);
	}

	return pxmlNewElement;
}


XMLElement * HALForXMLDoc::NewElementForX64Bits(XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits, const char* mnemonic, const char* pszComment)
{
	assert((bits % 8) == 0);

	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);
		pxmlNewElement->SetAttribute("mnemonic", mnemonic);
		pxmlNewElement->SetAttribute("value", (int64_t)key_value);

		int next_offset = m_bs_tracer.offset;
		int length = 0;
		while (m_bs_tracer.i_left < bits)
		{
			bits -= m_bs_tracer.i_left;
			m_bs_tracer.i_left = 8;
			next_offset++;
			length++;
		}

		m_bs_tracer.i_left -= bits;
		if (m_bs_tracer.i_left == 0)
		{
			m_bs_tracer.i_left = 8;
			next_offset++;
		}
		length++;

		pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);
		pxmlNewElement->SetAttribute("length", length);

		m_bs_tracer.offset = next_offset;
	}

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

XMLElement* HALForXMLDoc::QueryNodeByKeyName(XMLElement* pxmlParentNode, const char* pszNodeName)
{
	XMLElement* pxmlMatchNode = NULL;

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

