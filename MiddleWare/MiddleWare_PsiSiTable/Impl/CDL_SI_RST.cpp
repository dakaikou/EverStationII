#include <stdio.h>
#include <string.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"

/////////////////////////////////////////////
CRST::CRST(uint16_t Key, uint16_t PID, uint8_t table_id) : CPVT(Key, PID, table_id, 0xffff)
{
	m_bNormalSectionSyntax = 0;

	Reset();
}

CRST::~CRST(void)
{
	Reset();
}

void CRST::Init(void)
{
	CPVT::Init();

	Reset();
}

void CRST::Reset(void)
{
	CPVT::Reset();
}

int CRST::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int		rtcode = MIDDLEWARE_PSISI_NO_ERROR;

	running_status_section_t rst_section;

	rtcode = DVB_SI_RST_DecodeSection(buf, length, &rst_section);

	return rtcode;
}

