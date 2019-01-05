#ifndef _MPEG_DSMCC_UNM_DSI_OC_H_
#define _MPEG_DSMCC_UNM_DSI_OC_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DSMCC_UNM_DSI_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DSMCC_UNM_DSI_LIB __declspec(dllimport)
#   else
#       define DSMCC_UNM_DSI_LIB
#   endif
#elif __GNUC__ >= 4
#   define DSMCC_UNM_DSI_LIB __attribute__((visibility("default")))
#else
#   define DSMCC_UNM_DSI_LIB
#endif

#include "Mpeg2_DSMCC_BIOP.h"

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

//for OC
typedef struct _ServiceGatewayInfo_s
{
	BIOP::IOR_t	IOR;

	uint8_t		downloadTaps_count;						//8	, N1
	BIOP::TAP_t	Taps[1];								//为什么数组长度为1？  chendelin 2018.10.4

	uint8_t		serviceContextList_count;				//8, N2
	uint8_t		serviceContextList_data_byte[255];
	//struct {
	//	uint32_t			context_id;							//32
	//	uint16_t			context_data_length;				//16
	//	uint8_t			context_data_byte[1];				//8
	//} serviceContexts[1];								//为什么数组长度为1？  chendelin 2018.10.4

	uint16_t	userInfoLength;							//16
	uint8_t		userInfo_data_byte[256];						

} ServiceGatewayInfo_t;

DSMCC_UNM_DSI_LIB int	MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo);

#endif
