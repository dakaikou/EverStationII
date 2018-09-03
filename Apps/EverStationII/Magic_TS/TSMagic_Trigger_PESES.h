#ifndef _TSMAGIC_TRIGGER_PESES_H_
#define _TSMAGIC_TRIGGER_PESES_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "MiddleWare/MiddleWare_ESDecoder/Include/ESDecoder.h"
//#include "libMpeg2/Mpeg2_TS_packet.h"

#include "..\Common\Define.h"

#include "TSMagic_GuiAPi.h"

U32 TSMagic_pes_trigger_thread(LPVOID lpParam);
U32 TSMagic_es_trigger_thread(LPVOID lpParam);			//to be obseleted			chendelin 2018.2.4

class CTrigger_PESPacket : public CTrigger
{
protected:
	U16			m_usPID;
	U32			m_dwStyle;
	//	S32			m_nFilterID;

public:
	CTrigger_PESPacket(void);
	~CTrigger_PESPacket(void);

public:
	void SetMatchParams(S32 filter_id, U16 PID, U32 style, S32 demand_count);

	void Reset(void);
	//	S32 GetFilterID(void);
	U16 GetPID(void);
	U32 GetStyle(void);
	S32  IsMatched(U8* buf, S32 length);
};

#endif	//_TSMAGIC_TRIGGER_PESES_H_
