#ifndef _MPEG_DSMCC_UNM_H_
#define _MPEG_DSMCC_UNM_H_

#include "../../compile.h"

#include "Mpeg2_DSMCC_UNM_DII.h"
#include "Mpeg2_DSMCC_UNM_DSI.h"


/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

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

	compatibilityDescriptor_t		compatibilityDescriptor;

	uint16_t		numberOfModules;						//16

	uint16_t		N;
	moduleInfo_t	moduleInfo[MAX_MODULES_PER_GROUP];

	uint16_t	privateDataLength;
	uint8_t		privateDataByte[64];

} DownloadInfoIndication_t, *pDownloadInfoIndication_t;

typedef struct DownloadCancel_s
{
} DownloadCancel_t, *pDownloadCancel_t;

typedef struct DownloadServerInitiate_s
{
	uint8_t							serverId[20];								//20x8
	compatibilityDescriptor_t		compatibilityDescriptor;
	uint16_t						privateDataLength;

	int								data_broadcast_type;						//0x0006		-- DC - GroupInfoIndication
																				//0x0007		-- OC - ServiceGatewayInfo
	union
	{
		uint8_t							privateDataByte[128];
		GroupInfoIndication_t			GroupInfoIndication;					//DC所使用到的数据结构
		ServiceGatewayInfo_t			ServiceGatewayInfo;						//OC所使用到的数据结构
	} u;

} DownloadServerInitiate_t, *pDownloadServerInitiate_t;

_CDL_EXPORT	int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication);
_CDL_EXPORT	int	MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate);

#endif
