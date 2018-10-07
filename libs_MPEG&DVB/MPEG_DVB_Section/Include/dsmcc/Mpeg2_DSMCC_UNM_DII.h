#ifndef _MPEG_DSMCC_UNM_DII_H_
#define _MPEG_DSMCC_UNM_DII_H_

#include <stdint.h>

#include "Mpeg2_DSMCC_UNM_DII_DC.h"
#include "Mpeg2_DSMCC_UNM_DII_OC.h"
#include "Mpeg2_DSMCC_define.h"

typedef struct ModuleInfo_s
{
	uint16_t	moduleId;
	uint32_t	moduleSize;
	uint8_t		moduleVersion;

	uint8_t		moduleInfoLength;
	uint8_t*	moduleInfoByte;

	int								data_broadcast_type;						//0x0006		-- DC - GroupInfoIndication
																				//0x0007		-- OC - ServiceGatewayInfo
} ModuleInfo_t;

typedef struct DownloadInfoRequest_s
{
} DownloadInfoRequest_t, *pDownloadInfoRequest_t;

typedef struct DownloadInfoIndication_s
{
	uint32_t	downloadId;								//32
	uint16_t	blockSize;								//16
	uint8_t		windowSize;								//8
	uint8_t		ackPeriod;								//8
	uint32_t	tCDownloadWindow;						//32
	uint32_t	tCDownloadScenario;						//32

	compatibilityDescriptor_t		compatibilityDescriptor;	//>= 2 bytes

	uint16_t		 numberOfModules;						//16
	ModuleInfo_t	 ModuleInfo[MAX_MODULES_PER_GROUP];

	uint16_t	privateDataLength;							//16
	uint8_t*	privateDataByte;

} DownloadInfoIndication_t, *pDownloadInfoIndication_t;

_CDL_EXPORT	int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication);

#endif
