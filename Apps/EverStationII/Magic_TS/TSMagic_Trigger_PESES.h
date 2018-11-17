#ifndef _TSMAGIC_TRIGGER_PESES_H_
#define _TSMAGIC_TRIGGER_PESES_H_

#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
//#include "libMpeg2/Mpeg2_TS_packet.h"

#include "..\Common\Define.h"

#include "TSMagic_GuiAPi.h"

uint32_t TSMagic_pes_trigger_thread(LPVOID lpParam);
uint32_t TSMagic_es_trigger_thread(LPVOID lpParam);			//to be obseleted			chendelin 2018.2.4

class CTrigger_PESPacket : public CTrigger
{
protected:
	uint16_t			m_usPID;
	uint32_t			m_dwStyle;
	//	int			m_nFilterID;

public:
	CTrigger_PESPacket(void);
	~CTrigger_PESPacket(void);

public:
	void SetMatchParams(int filter_id, uint16_t PID, uint32_t style, int demand_count);

	void Reset(void);
	//	int GetFilterID(void);
	uint16_t GetPID(void);
	uint32_t GetStyle(void);
	int  IsMatched(uint8_t* buf, int length);
};

#endif	//_TSMAGIC_TRIGGER_PESES_H_
