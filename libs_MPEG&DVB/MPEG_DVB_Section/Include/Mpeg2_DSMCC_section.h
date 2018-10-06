#ifndef _MPEG_DSMCC_SECTION_H_
#define _MPEG_DSMCC_SECTION_H_

#include <stdint.h>

#include "../compile.h"
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
	U8		table_id;									//8

	U8		section_syntax_indicator;					//1
	U8		private_indicator;							//1
	U8		reserved0;									//2
	U16		dsmcc_section_length;						//12

	U16		table_id_extension;							//16

	U8		reserved1;									//2
	U8		version_number;								//5
	U8		current_next_indicator;						//1

	U8		section_number;								//8
	U8		last_section_number;						//8

	dsmccMessageHeader_t		dsmccMessageHeader;		//至少12个字节

	int			dsmccMessagePayloadLength;
	uint8_t*	dsmccMessagePayloadBuf;					//maximum length 4072, lifecycle is the same as section buffer

	uint32_t	encodedCheckValue;								//32
	uint32_t	recalculatedCheckValue;							//32

} dsmcc_section_t;

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeSection(uint8_t *buf, int length, dsmcc_section_t* pdsmcc_section);

#endif
