#ifndef _TSMAGIC_TRIGGER_TSPACKET_H_
#define _TSMAGIC_TRIGGER_TSPACKET_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "TSMagic_Trigger.h"

#define MAX_TSPACKET_TRIGGER_COUNT			MAX_TRIGGER_COUNT

class CTrigger_TSPacket : public CTrigger
{
protected:
	U16			m_usPID;
	S32			m_nFilterID;

public:
	CTrigger_TSPacket(void);
	~CTrigger_TSPacket(void);
public:
};

U32 TSMagic_packet_trigger_thread(LPVOID lpParam);

#endif		//_TSMAGIC_TRIGGER_TSPACKET_H_
