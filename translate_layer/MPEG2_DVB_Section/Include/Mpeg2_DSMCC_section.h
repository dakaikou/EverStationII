#ifndef _MPEG_DSMCC_SECTION_H_
#define _MPEG_DSMCC_SECTION_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define MPEG2_DSMCC_SECTION_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define MPEG2_DSMCC_SECTION_LIB __declspec(dllimport)
#   else
#       define MPEG2_DSMCC_SECTION_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_DSMCC_SECTION_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_DSMCC_SECTION_LIB
#endif

#include <stdint.h>

#include "dsmcc\Mpeg2_DSMCC_UNM.h"
#include "dsmcc\Mpeg2_DSMCC_DDM.h"

#define MPEG2_DSMCC_SECTION_MAX_SIZE			4096
#define MPEG2_DSMCC_SECTION_MIN_SIZE			12

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/
typedef struct dsmccAdaptationHeader_s
{
	uint8_t		adaptationType;						//8

	uint8_t		adaptationDataLength;
	//uint8_t		adaptationDataByte[64];
	uint8_t*		adaptationDataByte;

} dsmccAdaptationHeader_t, *pdsmccAdaptationHeader_t;

typedef struct dsmccMessageHeader_s
{
	uint8_t			protocolDiscriminator;					//8
	uint8_t			dsmccType;								//8
	uint16_t		messageId;								//16
	uint32_t		TxOrDnloadID;							//32		UNM transactionId or DDM downloadId

	uint8_t			reserved;								//8
	uint8_t			adaptationLength;						//8
	uint16_t		messageLength;							//16

	dsmccAdaptationHeader_t		dsmccAdaptationHeader;

} dsmccMessageHeader_t, *pdsmccMessageHeader_t;

typedef struct dsmcc_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		private_indicator;							//1
	uint8_t		reserved0;									//2
	uint16_t	dsmcc_section_length;						//12

	uint16_t	table_id_extension;							//16

	uint8_t		reserved1;									//2
	uint8_t		version_number;								//5
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	dsmccMessageHeader_t		dsmccMessageHeader;		//至少12个字节

	int			dsmccMessagePayloadLength;
	uint8_t*	dsmccMessagePayloadBuf;					//maximum length 4072, lifecycle is the same as section buffer

	uint32_t	encodedCheckValue;								//32
	uint32_t	recalculatedCheckValue;							//32

} dsmcc_section_t;

MPEG2_DSMCC_SECTION_LIB	int	MPEG2_DSMCC_DecodeSection(uint8_t *buf, int length, dsmcc_section_t* pdsmcc_section);

#endif
