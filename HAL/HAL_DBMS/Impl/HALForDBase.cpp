#include <stdio.h>

#include "../Include/HALForDBase.h"

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

void XMLDocForMpegSyntax::SetActiveNode(tinyxml2::XMLElement* pActive)
{
	m_pActive = pActive;
}

tinyxml2::XMLElement* XMLDocForMpegSyntax::GetActiveNode(void)
{
	return m_pActive;
}

void XMLDocForMpegSyntax::UpdateBufMark(tinyxml2::XMLElement* pElement, unsigned char* start_ptr, unsigned char* end_ptr)
{
	if (pElement != NULL)
	{
		int offset = (int)(start_ptr - m_origin_ptr);
		int length = (int)(end_ptr - start_ptr);
		if (length == 0)
		{
			length = 1;
		}

		pElement->SetAttribute("offset", offset);
		pElement->SetAttribute("length", length);

	}
}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewSimpleElement(tinyxml2::XMLElement* pxmlParent, const char* name)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(name);

	pxmlParent->InsertEndChild(pxmlNewElement);

	return pxmlNewElement;
}

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewComplexElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits, const char* mnemonic, unsigned int value, const BITS_t* pbits_map, const char* pszComment)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);
		if (mnemonic != NULL)
		{
			pxmlNewElement->SetAttribute("mnemonic", mnemonic);
		}
		pxmlNewElement->SetAttribute("value", value);
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

tinyxml2::XMLElement * XMLDocForMpegSyntax::NewComplexElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits, const char* mnemonic, unsigned int value, unsigned char* start_ptr, unsigned char* end_ptr, const char* pszComment)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);
		pxmlNewElement->SetAttribute("mnemonic", mnemonic);
		pxmlNewElement->SetAttribute("value", value);
	}

	if (start_ptr != NULL)
	{
		int offset = (int)(start_ptr - m_origin_ptr);
		int length = (int)(end_ptr - start_ptr);

		int byte_count = (bits + 7) / 8;
		if (length < byte_count)
		{
			length = byte_count;
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

tinyxml2::XMLElement * XMLDocForMpegSyntax::x64NewComplexElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits, const char* mnemonic, uint64_t value, const BITS_t* pbits_map, const char* pszComment)
{
	tinyxml2::XMLElement* pxmlNewElement = tinyxml2::XMLDocument::NewElement(name);

	if (bits > 0)
	{
		pxmlNewElement->SetAttribute("bits", bits);
		pxmlNewElement->SetAttribute("mnemonic", mnemonic);
		pxmlNewElement->SetAttribute("value", (int64_t)value);
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

tinyxml2::XMLElement* XMLDocForMpegSyntax::QueryNodeByName(tinyxml2::XMLElement* pxmlParentNode, const char* pszNodeName)
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
			pxmlMatchNode = QueryNodeByName(pxmlParentNode->FirstChildElement(), pszNodeName);
			if (pxmlMatchNode == NULL)
			{
				pxmlMatchNode = QueryNodeByName(pxmlParentNode->NextSiblingElement(), pszNodeName);
			}
		}
	}

	return pxmlMatchNode;
}

