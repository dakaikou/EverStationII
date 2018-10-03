#include <stdio.h>
#include <assert.h>

#include "../Include/tinyxml2.h"
#include "../Include/tinyxml2_ext.h"

using namespace tinyxml2;

void XMLDocument::GetBitsTracer(BITS_TRACER_t* pbs_tracer)
{
	if (pbs_tracer != NULL)
	{
		pbs_tracer->i_left = m_bs_tracer.i_left;
		pbs_tracer->offset = m_bs_tracer.offset;
	}
}

void XMLElement::SetFieldLength(int length)
{
	BITS_TRACER_t bs_tracer;
	XMLDocument* pxmlDoc = this->GetDocument();

	pxmlDoc->GetBitsTracer(&bs_tracer);
	assert(bs_tracer.i_left == 8);

	int offset = bs_tracer.offset;
	SetAttribute("offset", offset);
	SetAttribute("length", length);

	SetAttribute("field_length", length);
}
