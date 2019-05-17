#ifndef __API_MPEG_PES_SPLICER_H__
#define __API_MPEG_PES_SPLICER_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_PES_EXPORT
#       define MW_PES_LIB __declspec(dllexport)
#   elif defined(_MW_PES_IMPORT)
#       define MW_PES_LIB __declspec(dllimport)
#   else
#       define MW_PES_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_PES_LIB __attribute__((visibility("default")))
#else
#   define MW_PES_LIB
#endif

#include <stdint.h>

#include "syntax_translate_layer\Mpeg2_TSPacket\Include\Mpeg2_TS_packet.h"
#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"

class MW_PES_LIB CPESSplicer
{
public:
	CPESSplicer(void);

private:
	uint16_t	m_usPID;

	uint8_t*	m_pucPESBuf;
	int			m_nBufSize;
	int			m_nPESLength;

	int			m_nPkIndex;
	int			m_nWriteSize;

	//int			m_pes_payload_unit_started;
	int			m_bSplicingStarted;

public:

	int			SetPID(uint16_t usPID);
	int			WriteTSPacket(transport_packet_t* ptransport_packet);
	void		Reset(void);
	void		Init(void);
	uint8_t*	GetPESPacket(int* plength);

public:
	~CPESSplicer();
};

#endif	//__API_MPEG_PES_SPLICER_H__

