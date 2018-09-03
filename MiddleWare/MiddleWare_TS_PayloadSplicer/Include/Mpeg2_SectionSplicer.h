#ifndef __API_PSISI_SECTION_SPLICER_H__
#define __API_PSISI_SECTION_SPLICER_H__

//#include <windows.h>
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"

typedef enum
{
	PARSER_WAITING		= 0,
	PARSER_FINISH,
} PARSER_STATE_e;

class _CDL_EXPORT CSectionSplicer
{
public:
	CSectionSplicer(void);

public:
	U16			m_usPID;
	U8			m_ucTableID;

	U8*			m_pucSectionBuf;
	S32			m_nSectionLength;

private:
	S32			m_nPkIndex;
	S32			m_nWriteSize;

public:

	S32			WriteTSPacket(transport_packet_t* ptransport_packet);
	void		Reset(void);
	void		Init(void);

public:
	~CSectionSplicer();
};

#endif	//__API_PSISI_SECTION_SPLICER_H__

