#ifndef _MPEG_DSMCC_UNM_DSI_OC_H_
#define _MPEG_DSMCC_UNM_DSI_OC_H_

//#include "../../compile.h"

//#include "../MPEG_DVB_SysCapability.h"
//#include "../MPEG_DVB_Common.h"
#include "Mpeg2_DSMCC_common.h"

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

//for OC
typedef struct _ServiceGatewayInfo_s
{
	IOP::IOR_t	IOR;

	uint8_t		downloadTaps_count;						//8	, N1
	BIOP::TAP_t	Taps[1];								//为什么数组长度为1？  chendelin 2018.10.4

	uint8_t		serviceContextList_count;				//8, N2
	uint8_t		serviceContextList_data_byte[255];
	//struct {
	//	U32			context_id;							//32
	//	U16			context_data_length;				//16
	//	U8			context_data_byte[1];				//8
	//} serviceContexts[1];								//为什么数组长度为1？  chendelin 2018.10.4

	uint16_t	userInfoLength;							//16
	uint8_t		userInfo_data_byte[256];						

} ServiceGatewayInfo_t;

int	MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo);

#endif
