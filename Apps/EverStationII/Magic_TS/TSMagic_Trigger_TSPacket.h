#ifndef _TSMAGIC_TRIGGER_TSPACKET_H_
#define _TSMAGIC_TRIGGER_TSPACKET_H_

#include "TSMagic_Trigger.h"

#define MAX_TSPACKET_TRIGGER_COUNT			MAX_TRIGGER_COUNT

class CTrigger_TSPacket : public CTrigger
{
protected:
	uint16_t	m_usPID;
	int			m_nFilterID;

public:
	CTrigger_TSPacket(void);
	~CTrigger_TSPacket(void);
public:
};

uint32_t TSMagic_packet_trigger_thread(LPVOID lpParam);

#endif		//_TSMAGIC_TRIGGER_TSPACKET_H_
