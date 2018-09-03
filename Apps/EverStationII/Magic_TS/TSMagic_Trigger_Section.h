#ifndef _TSMAGIC_TRIGGER_SECTION_H_
#define _TSMAGIC_TRIGGER_SECTION_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "TSMagic_Trigger.h"

uint32_t TSMagic_section_trigger_thread(LPVOID lpParam);

class CTrigger_PsiSiSection : public CTrigger
{
protected:
	uint16_t m_usPID;

public:
	CTrigger_PsiSiSection(void);
	~CTrigger_PsiSiSection(void);

public:

	void SetMatchParamsForNormalSection(uint16_t PID, uint8_t table_id, uint16_t table_id_extension, uint8_t section_number);
	void SetMatchParamsForOtherSection(uint16_t PID, uint8_t table_id);

	void Reset(void);
	uint16_t GetPID(void);
	int	  IsMatched(U8* buf, S32 length);
};

#endif		//_TSMAGIC_TRIGGER_SECTION_H_
