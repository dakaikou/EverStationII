#include <stdio.h>

#include "../Include/tinyxml2.h"

using namespace tinyxml2;

void XMLElement::SetStartEndPtr(int offset, int length)
{
	SetAttribute("offset", offset);
	SetAttribute("length", length);
}
