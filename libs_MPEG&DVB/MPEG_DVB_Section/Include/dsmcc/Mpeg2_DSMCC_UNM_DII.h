#ifndef _MPEG_DSMCC_UNM_DII_H_
#define _MPEG_DSMCC_UNM_DII_H_

#include "../../compile.h"

#include "../MPEG_DVB_SysCapability.h"
#include "../MPEG_DVB_Common.h"
#include "../Mpeg2_DSMCC_Descriptor.h"
#include "Mpeg2_DSMCC_BIOP.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"


/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

//for DC
typedef struct DC_moduleInfo_s
{
	name_descriptor_t			name_descriptor;
	location_descriptor_t		location_descriptor;

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} DC_moduleInfo_t;

typedef struct BIOP_moduleInfo_s
{
	U32			moduleTimeOut;				//32
	U32			blockTimeOut;				//32
	U32			minBlockTime;				//32

	U8			taps_count;					//8
	BIOP::TAP_t TAP[4];
	//struct
	//{
	//	U16			id;							//16
	//	U16			use;						//16
	//	U16			association_tag;			//16
	//	U8			selector_length;			//8
	//	//U8			selector_data_byte[256];	//8
	//}TAP[4];											//为什么数组长度为4? chendelin 2018.10.4

	U8			userInfoLength;				//8
	U8			userInfo_data_byte[256];	//8

} BIOP_moduleInfo_t;

typedef struct moduleInfo_s
{
	U16			moduleId;
	U32			moduleSize;
	U8			moduleVersion;

	U8			moduleInfoLength;

	int								data_broadcast_type;						//0x0006		-- DC - GroupInfoIndication
																				//0x0007		-- OC - ServiceGatewayInfo
	union
	{
		DC_moduleInfo_t			DC_moduleInfo;
		BIOP_moduleInfo_t		BIOP_moduleInfo;
	} u;

} moduleInfo_t;

//_CDL_EXPORT int	MPEG2_DSMCC_DSI_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication);
//_CDL_EXPORT int	MPEG2_DSMCC_DSI_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo);

#endif
