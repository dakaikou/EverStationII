#ifndef _MPEG_DSMCC_SECTION_DDM_H_
#define _MPEG_DSMCC_SECTION_DDM_H_

#include "../compile.h"
#include "Mpeg2_DSMCC_section.h"

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

//DDB的封装格式只有一种，DDB的净荷可能有多种情况
typedef struct DownloadDataBlock_s
{
	uint16_t		moduleId;								//16
	uint8_t			moduleVersion;							//8
	uint8_t			reserved;								//8
	uint16_t		blockNumber;							//16

	uint16_t		N;
	uint8_t*		blockDataByte;

} DownloadDataBlock_t, *pDownloadDataBlock_t;

typedef struct DownloadDataRequest_s
{
	uint8_t		reserved;
} DownloadDataRequest_t, *pDownloadDataRequest_t;

typedef struct dsmccDownloadDataHeader_s
{
	uint8_t			protocolDiscriminator;					//8
	uint8_t			dsmccType;								//8
	uint16_t		messageId;								//16
	uint32_t		downloadId;								//32

	uint8_t			reserved;								//8
	uint8_t			adaptationLength;						//8
	uint16_t		messageLength;							//16

	dsmccAdaptationHeader_t		dsmccAdaptationHeader;

} dsmccDownloadDataHeader_t;

typedef struct DSMCC_DDM_section_s
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

	dsmccDownloadDataHeader_t		dsmccDownloadDataHeader;
	DownloadDataBlock_t				DownloadDataBlock;

	uint32_t		CRC_32;										//32
	uint32_t		CRC_32_verify;								//32

} dsmcc_ddm_section_t;

_CDL_EXPORT	int	MPEG2_DSMCC_DDM_DecodeSection(uint8_t *buf, int length, dsmcc_ddm_section_t* pDSMCCSection);

#endif
