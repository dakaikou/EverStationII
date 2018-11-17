#ifndef __API_PSISI_SECTION_SPLICER_H__
#define __API_PSISI_SECTION_SPLICER_H__

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

#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"

typedef enum
{
	PARSER_WAITING		= 0,
	PARSER_FINISH,
} PARSER_STATE_e;

class MW_PES_LIB CSectionSplicer
{
public:
	CSectionSplicer(void);

public:
	uint16_t			m_usPID;
	uint8_t			m_ucTableID;

	uint8_t*			m_pucSectionBuf;
	int			m_nSectionLength;

private:
	int			m_nPkIndex;
	int			m_nWriteSize;

public:

	int			WriteTSPacket(transport_packet_t* ptransport_packet);
	void		Reset(void);
	void		Init(void);

public:
	~CSectionSplicer();
};

#endif	//__API_PSISI_SECTION_SPLICER_H__

