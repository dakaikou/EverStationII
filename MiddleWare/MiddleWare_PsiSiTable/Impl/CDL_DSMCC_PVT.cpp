#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_IPDC_section.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\Include\MPEG_DVB_ErrorCode.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_ErrorCode.h"

#include "../Include/MiddleWare_DSMCC_Table.h"

/////////////////////////////////////////////
CDSMCC_PVT::CDSMCC_PVT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CDSMCC_PVT::~CDSMCC_PVT(void)
{
	Reset();
}

void CDSMCC_PVT::Init(void)
{
	CPVT::Init();

	Reset();
}

void CDSMCC_PVT::Reset(void)
{
	CPVT::Reset();
}

int CDSMCC_PVT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		dsmcc_section_t dsmcc_section;

		rtcode = MPEG2_DSMCC_DecodeSection(buf, length, &dsmcc_section);
	}

	return rtcode;
}

