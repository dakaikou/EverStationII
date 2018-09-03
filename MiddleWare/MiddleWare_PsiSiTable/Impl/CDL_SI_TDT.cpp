#include <string.h>
#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"

/////////////////////////////////////////////
CTDT::CTDT(uint16_t Key, uint16_t PID, uint8_t table_id) : CPVT(Key, PID, table_id, 0xffff)
{
	m_bNormalSectionSyntax = 0;

	Init();
}

CTDT::~CTDT(void)
{
	Reset();
}

void CTDT::Init(void)
{
	CPVT::Init();

	Reset();
}

void CTDT::Reset(void)
{
	m_UTC_time = 0;

	CPVT::Reset();
}

S32 CTDT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	S32						rtcode = MIDDLEWARE_PSISI_NO_ERROR;
	time_date_section_t		tdt_section;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_SI_TDT_DecodeSection(buf, length, &tdt_section);
		if (tdt_section.UTC_time != m_UTC_time)
		{
			m_UTC_time = tdt_section.UTC_time;
		}
	}

	return rtcode;
}
