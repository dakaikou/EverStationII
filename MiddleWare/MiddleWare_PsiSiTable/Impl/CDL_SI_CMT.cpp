#include <string.h>
#include <stdlib.h>
#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "syntax_translate_layer/MPEG2_DVB_Section\Include\MPEG2_DVB_ErrorCode.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif

/////////////////////////////////////////////
CCMT::CCMT(uint16_t Key, uint16_t PID, uint8_t table_id) : CPVT(Key, PID, table_id, 0xffff)
{
	m_bNormalSectionSyntax = 0;

	Init();
}

CCMT::~CCMT(void)
{
	Reset();
}

void CCMT::Init(void)
{
	Reset();
}

void CCMT::Reset(void)
{
}

int CCMT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int		rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;
	
	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
	}

	return rtcode;
}


