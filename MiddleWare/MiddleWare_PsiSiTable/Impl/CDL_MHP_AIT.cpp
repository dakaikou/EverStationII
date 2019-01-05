#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_MHP_Section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include\MPEG2_DVB_ErrorCode.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "../Include/MiddleWare_DSMCC_Table.h"

/////////////////////////////////////////////
CAIT::CAIT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CAIT::~CAIT(void)
{
	Reset();
}

void CAIT::Init(void)
{
	CPVT::Init();

	Reset();
}

void CAIT::Reset(void)
{
	CPVT::Reset();
}

int CAIT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	application_information_section_t		ait_section;
	
	rtcode = CPVT::AddSection(usPID, pprivate_section);
	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_MHP_AIT_DecodeSection(buf, length, &ait_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
		}
	}

	return rtcode;
}

