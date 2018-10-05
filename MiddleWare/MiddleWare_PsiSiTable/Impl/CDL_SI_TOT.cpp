#include <string.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/nativeInclude/DVB_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/nativeInclude/MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
CTOT::CTOT(uint16_t Key, uint16_t PID, uint8_t table_id) : CPVT(Key, PID, table_id, 0xffff)
{
	m_bNormalSectionSyntax = 0;

	Reset();
}

CTOT::~CTOT(void)
{
	Reset();
}

void CTOT::Init(void)
{
	CPVT::Init();

	Reset();
}

void CTOT::Reset(void)
{
	m_UTC_time = 0;

	CPVT::Reset();
}

int CTOT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int		rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;
	
	time_offset_section_t tot_section;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_SI_TOT_DecodeSection(buf, length, &tot_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			if (tot_section.UTC_time != m_UTC_time)
			{
				m_UTC_time = tot_section.UTC_time;
			}
		}
	}

	return rtcode;
}
