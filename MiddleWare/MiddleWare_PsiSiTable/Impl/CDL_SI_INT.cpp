#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../Include/MiddleWare_DSMCC_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_IPDC_Section.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"

/////////////////////////////////////////////
CINT::CINT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CINT::~CINT(void)
{
	Reset();
}

void CINT::Init(void)
{
	CPVT::Init();

	Reset();
}

void CINT::Reset(void)
{
	m_uiPlatformID = 0xFFFFFF;

	CPVT::Reset();
}

int CINT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int			rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	IP_MAC_notification_section_t	int_section;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = DVB_IPDC_INT_DecodeSection(buf, length, &int_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
			m_uiPlatformID = int_section.platform_id;
		}
	}

	return rtcode;
}

