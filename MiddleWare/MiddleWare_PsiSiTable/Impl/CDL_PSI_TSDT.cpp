#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../Include/MiddleWare_PSISI_Table.h"
#include "../Include/MiddleWare_PSISI_ErrorCode.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section/nativeInclude/Mpeg2_table_id.h"
#include "libs_MPEG&DVB/MPEG_DVB_Section\nativeInclude\MPEG_DVB_ErrorCode.h"

/////////////////////////////////////////////
CTSDT::CTSDT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension) : CPVT(Key, PID, table_id, table_id_extension)
{
	m_bNormalSectionSyntax = 0;

	Init();
}

CTSDT::~CTSDT(void)
{
	Reset();
}

void CTSDT::Init(void)
{
	CPVT::Init();

	Reset();
}

void CTSDT::Reset(void)
{
	CPVT::Reset();
}

int CTSDT::AddSection(uint16_t usPID, uint8_t* buf, int length, private_section_t* pprivate_section)
{
	int		rtcode = MIDDLEWARE_PSISI_UNKNOWN_ERROR;							//0 -- fail

	CA_section_t	cat_section;
	
	rtcode = CPVT::AddSection(usPID, pprivate_section);

	if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
	{
		rtcode = MPEG2_PSI_CAT_DecodeSection(buf, length, &cat_section);
		if (rtcode == SECTION_PARSE_NO_ERROR)
		{
		}
	}

	return rtcode;
}


