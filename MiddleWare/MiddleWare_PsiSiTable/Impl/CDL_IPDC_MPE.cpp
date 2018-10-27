#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_IPDC_section.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/DVB_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/Include\MPEG_DVB_ErrorCode.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_ErrorCode.h"

#include "../Include/MiddleWare_DSMCC_Table.h"

/////////////////////////////////////////////
CMPE::CMPE(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 1;

	Init();
}

CMPE::~CMPE(void)
{
	Reset();
}

void CMPE::Init(void)
{
	CPVT::Init();

	Reset();
}

void CMPE::Reset(void)
{
	CPVT::Reset();
}

int CMPE::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int	rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;

	//S32 i;
	//S32	collect_over;
	//datagram_section_t		datagram_section;
	
	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		//rtcode = DVB_IPDC_MPE_DecodeSection(buf, length, &datagram_section);
		//if (rtcode == SECTION_PARSE_NO_ERROR)
		//{
		//}
	}

	return rtcode;
}

