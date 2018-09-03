#ifndef _MPEG_DSMCC_SECTION_H_
#define _MPEG_DSMCC_SECTION_H_

#include <stdint.h>

#include "../compile.h"

#define MPEG2_DSMCC_SECTION_MAX_SIZE			4096
#define MPEG2_DSMCC_SECTION_MIN_SIZE			12

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

typedef struct compatibilityDescriptor_s
{
	uint16_t	compatibilityDescriptorLength;								//16		
	uint8_t		compatibilityDescriptorBuf[64];
} compatibilityDescriptor_t, *pcompatibilityDescriptor_t;

typedef struct dsmccAdaptationHeader_s
{
	uint8_t		adaptationType;						//8
	uint8_t		N;
	uint8_t		adaptationDataByte[64];

} dsmccAdaptationHeader_t, *pdsmccAdaptationHeader_t;

typedef struct dsmccMessageHeader_s
{
	uint8_t			protocolDiscriminator;					//8
	uint8_t			dsmccType;								//8
	uint16_t		messageId;								//16
	uint32_t		transactionId;							//32

	uint8_t			reserved;								//8
	uint8_t			adaptationLength;						//8
	uint16_t		messageLength;							//16

	dsmccAdaptationHeader_t		dsmccAdaptationHeader;

} dsmccMessageHeader_t, *pdsmccMessageHeader_t;

typedef struct DSMCC_section_s
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

	dsmccMessageHeader_t		dsmccMessageHeader;

	uint32_t		CRC_32;										//32
	uint32_t		CRC_32_verify;								//32

} dsmcc_section_t;

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeSection(uint8_t *buf, int length, dsmcc_section_t* pDSMCCSection);

#endif
