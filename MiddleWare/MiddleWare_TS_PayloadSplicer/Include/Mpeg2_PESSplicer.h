#ifndef __API_MPEG_PES_SPLICER_H__
#define __API_MPEG_PES_SPLICER_H__

#include "libs_Mpeg&DVB\Mpeg_TSPacket\Include\Mpeg2_TS_packet.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"

class _CDL_EXPORT CPESSplicer
{
public:
	CPESSplicer(void);

private:
	U16			m_usPID;

	U8*			m_pucPESBuf;
	S32			m_nBufSize;
	S32			m_nPESLength;

	S32			m_nPkIndex;
	S32			m_nWriteSize;

	S32			m_pes_payload_unit_started;

public:

	S32			SetPID(U16 usPID);
	S32			WriteTSPacket(transport_packet_t* ptransport_packet);
	void		Reset(void);
	void		Init(void);
	U8*			GetPESPacket(S32* plength);

public:
	~CPESSplicer();
};

#endif	//__API_MPEG_PES_SPLICER_H__

