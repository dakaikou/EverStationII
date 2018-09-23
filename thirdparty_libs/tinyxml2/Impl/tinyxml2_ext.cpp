#include <stdio.h>

#include "../Include/tinyxml2.h"

using namespace tinyxml2;

void XMLDocument::SetOriginPtr(uint8_t* origin_ptr)
{
	m_origin_ptr = origin_ptr;
}

uint8_t* XMLDocument::GetOriginPtr(void)
{
	return m_origin_ptr;
}

void XMLElement::SetStartEndPtr(uint8_t* start_ptr, uint8_t* end_ptr)
{
	uint8_t* origin_ptr = this->GetDocument()->GetOriginPtr();

	if (origin_ptr != NULL)
	{
		int offset = (int)(start_ptr - origin_ptr);
		int length = (int)(end_ptr - start_ptr);
		if (length == 0)
		{
			length = 1;
		}

		SetAttribute("offset", offset);
		SetAttribute("length", length);
	}
}
