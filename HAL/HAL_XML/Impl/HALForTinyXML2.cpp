#include <stdio.h>
#include <assert.h>

#include "../Include/HALForTinyXML2.h"


#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

HALForXMLDoc::HALForXMLDoc(void)
{
	m_bs_tracer.offset = 0;
	m_bs_tracer.i_left = 8;
	m_bs_tracer.max_length = -1;

	m_nTreeLevel = 0;
	for (int i = 0; i < MAX_TREE_LEVEL; i++)
	{
		m_stBranchStack[i].pxmlBranchNode = NULL;
		//m_stBranchStack[i].nOffset = 0;
	}
	//m_lastBranchNode = NULL;

	const char* pszDeclaration = "xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"";

	XMLDeclaration* pxmlDeclaration = NewDeclaration(pszDeclaration);
	InsertFirstChild(pxmlDeclaration);
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

//if we don't know the field length, then we can use SetAnchor() and ClearAncher() paire to trake it 
//void HALForXMLDoc::SetAnchor(XMLElement* pxmlBranchNode)
//{
//	if (pxmlBranchNode != NULL)
//	{
//		//m_lastBranchNode = pxmlBranchNode;
//
//		m_nTreeLevel++;
//		assert(m_nTreeLevel <= MAX_TREE_LEVEL);
//
//		m_stBranchStack[m_nTreeLevel-1].pxmlBranchNode = pxmlBranchNode;
//
//		//if (offset >= 0)
//		//{
//		//	SetSyncOffset(offset);
//		//}
//
//		//pxmlBranchNode->SetAttribute("offset", m_bs_tracer.offset);
//		//m_stBranchStack[m_nTreeLevel-1].nOffset = m_bs_tracer.offset;
//	}
//}

//void HALForXMLDoc::ClearAnchor(XMLElement* pxmlBranchNode)
//{
//	if (pxmlBranchNode != NULL)
//	{
//		//assert(pxmlBranchNode == m_lastBranchNode);
//		assert(pxmlBranchNode == m_stBranchStack[m_nTreeLevel - 1].pxmlBranchNode);
//
//		int offset = pxmlBranchNode->IntAttribute("offset", -1);
//		int field_length = m_bs_tracer.offset - offset;
//		pxmlBranchNode->SetAttribute("length", field_length);
//		pxmlBranchNode->SetAttribute("field_length", field_length);
//
//		m_stBranchStack[m_nTreeLevel - 1].pxmlBranchNode = NULL;
//		m_nTreeLevel--;
//
//		//if (m_nTreeLevel > 0)
//		//{
//		//	m_lastBranchNode = m_stBranchStack[m_nTreeLevel - 1].pxmlBranchNode;
//		//}
//		//else
//		//{
//		//	m_lastBranchNode = NULL;
//		//}
//	}
//}

//void HALForXMLDoc::Align(void)
//{
//	if (m_bs_tracer.i_left < 8)		// 1~7
//	{
//		m_bs_tracer.i_left = 8;
//		m_bs_tracer.offset++;
//	}
//}

XMLElement* HALForXMLDoc::NewRootElement(const char* key_name, const char* pszComment, int field_length)
{
	m_nTreeLevel = 0;
	for (int i = 0; i < MAX_TREE_LEVEL; i++)
	{
		m_stBranchStack[i].pxmlBranchNode = NULL;
	}

	return NewBranchElement(NULL, key_name, pszComment, field_length, 0);
}

// field_length -- the branch element length, default value is -1
// sync_offset -- the branch element offset, default value is -1, at this chance we can reset the offset
XMLElement * HALForXMLDoc::NewBranchElement(XMLElement* pxmlParent, const char* key_name, const char* pszComment, int field_length, int sync_offset)
{
	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	if (sync_offset >= 0)	//at this chance, we can resync the offset.
	{
		SetSyncOffset(sync_offset);
	}
	pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);

	if (field_length > 0)	//if the field length is known, then set it.
	{
		pxmlNewElement->SetAttribute("length", field_length);
		pxmlNewElement->SetAttribute("field_length", field_length);
	}

	if (pxmlParent != NULL)
	{
		//this is a child node
		pxmlParent->InsertEndChild(pxmlNewElement);
	}
	else
	{
		// this is a root node
		InsertEndChild(pxmlNewElement);
	}

	return pxmlNewElement;
}

XMLElement * HALForXMLDoc::NewElementForString(XMLElement* pxmlParent, const char* key_name, const char* pszComment)
{
	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

	if (pszComment != NULL)
	{
		pxmlNewElement->SetAttribute("comment", pszComment);
	}

	if (pxmlParent != NULL)
	{
		//this is a child node
		pxmlParent->InsertEndChild(pxmlNewElement);
	}
	else
	{
		// this is a root node
		InsertEndChild(pxmlNewElement);
	}

	return pxmlNewElement;
}

XMLElement * HALForXMLDoc::NewElementForBits(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits, const char* mnemonic, const char* pszComment)
{
	TrackTheBranchNode(pxmlParent);

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

XMLElement * HALForXMLDoc::NewElementForByteMode(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bytes, const char* pszComment)
{
	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);

	if (bytes > 0)
	{
		pxmlNewElement->SetAttribute("bytes", bytes);

		assert(m_bs_tracer.i_left == 8);		//must be byte aligned   chendelin  2018.10.3
		int next_offset = m_bs_tracer.offset + bytes;
		int length = bytes;

		pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);
		pxmlNewElement->SetAttribute("length", length);

		m_bs_tracer.offset = next_offset;
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

XMLElement * HALForXMLDoc::NewElementForByteBuf(XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment)
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

	//assert(m_bs_tracer.i_left == 8);

	int next_offset = m_bs_tracer.offset + byte_length;
	int length = byte_length;
	if (m_bs_tracer.i_left != 8) length += 1;

	pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);
	pxmlNewElement->SetAttribute("length", length);
	pxmlNewElement->SetAttribute("field_length", byte_length);

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

//XMLElement * HALForXMLDoc::NewElementForCharBuf(XMLElement* pxmlParent, const char* key_name, const uint8_t* char_buf, int char_length, const uint8_t* trimmed_char)
//{
//	char* pszComment;
//	char pszTemp[48];
//	int	 i;
//	XMLElement* pxmlNewElement = XMLDocument::NewElement(key_name);
//
//	if (char_buf != NULL)
//	{
//		memset(pszTemp, 0x00, sizeof(pszTemp));
//
//		for (i = 0; i < min(8, char_length); i++)
//		{
//			sprintf_s(pszTemp + i * 3, 4, "%02X ", char_buf[i]);
//		}
//		if (i < char_length)
//		{
//			sprintf_s(pszTemp + i * 3, 4, "...");
//		}
//		else
//		{
//			pszTemp[i * 3] = '\0';
//		}
//
//		pxmlNewElement->SetAttribute("value", pszTemp);
//	}
//
//	//assert(m_bs_tracer.i_left == 8);
//
//	int next_offset = m_bs_tracer.offset + char_length;
//	int length = char_length;
//	if (m_bs_tracer.i_left != 8) length += 1;
//
//	pxmlNewElement->SetAttribute("offset", m_bs_tracer.offset);
//	pxmlNewElement->SetAttribute("length", length);
//	pxmlNewElement->SetAttribute("field_length", char_length);
//
//	m_bs_tracer.offset = next_offset;
//
//	if (trimmed_char != NULL)
//	{
//		int trimmed_char_length = char_length - (int)(trimmed_char - char_buf);
//		pszComment = (char*)malloc(trimmed_char_length + 1);
//		memcpy(pszComment, trimmed_char, trimmed_char_length);
//		pszComment[trimmed_char_length] = '\0';
//		pxmlNewElement->SetAttribute("comment", pszComment);
//		free(pszComment);
//	}
//
//	if (pxmlParent != NULL)
//	{
//		pxmlParent->InsertEndChild(pxmlNewElement);
//	}
//
//	return pxmlNewElement;
//}


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

void HALForXMLDoc::TrackTheBranchNode(XMLElement* pxmlBranchNode)
{

}

