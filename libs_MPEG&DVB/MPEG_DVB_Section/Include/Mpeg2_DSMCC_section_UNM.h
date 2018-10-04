#ifndef _MPEG_DSMCC_SECTION_UNM_H_
#define _MPEG_DSMCC_SECTION_UNM_H_

#include "../compile.h"
#include "MPEG_DVB_SysCapability.h"
#include "MPEG_DVB_Common.h"
#include "Mpeg2_DSMCC_Descriptor.h"
#include "Mpeg2_DSMCC_section.h"
#include "Mpeg2_DSMCC_common.h"

#include "HAL/HAL_Sys/Include/INTTYPES.H"


/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

typedef struct _ServiceGatewayInfo_s
{
	IOP::IOR_t	IOR;

	U8			downloadTaps_count;						//8	, N1
	BIOP::TAP_t	Taps[1];								//为什么数组长度为1？  chendelin 2018.10.4

	U8			serviceContextList_count;				//8, N2
	uint8_t		serviceContextList_data_byte[255];
	//struct {
	//	U32			context_id;							//32
	//	U16			context_data_length;				//16
	//	U8			context_data_byte[1];				//8
	//} serviceContexts[1];								//为什么数组长度为1？  chendelin 2018.10.4

	U16			userInfoLength;							//16
	U8			userInfo_data_byte[256];						

} ServiceGatewayInfo_t;

typedef struct DownloadInfoRequest_s
{
} DownloadInfoRequest_t, *pDownloadInfoRequest_t;

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

typedef struct DownloadInfoIndication_s
{
	U32			downloadId;								//32
	U16			blockSize;								//16
	U8			windowSize;								//8
	U8			ackPeriod;								//8
	U32			tCDownloadWindow;						//32
	U32			tCDownloadScenario;						//32

	compatibilityDescriptor_t		compatibilityDescriptor;

	U16				numberOfModules;						//16

	U16				N;
	moduleInfo_t	moduleInfo[MAX_MODULES_PER_GROUP];

	U16			privateDataLength;
	U8			privateDataByte[64];

} DownloadInfoIndication_t, *pDownloadInfoIndication_t;

////DDB的封装格式只有一种，DDB的净荷可能有多种情况
//typedef struct DownloadDataBlock_s
//{
//	U16			moduleId;								//16
//	U8			moduleVersion;							//8
//	U8			reserved;								//8
//	U16			blockNumber;							//16
//
//	U16			N;
//	U8*			blockDataByte;
//
//} DownloadDataBlock_t, *pDownloadDataBlock_t;
//
//typedef struct DownloadDataRequest_s
//{
//} DownloadDataRequest_t, *pDownloadDataRequest_t;

typedef struct DownloadCancel_s
{
} DownloadCancel_t, *pDownloadCancel_t;

typedef struct GroupInfo_s
{
	U32							GroupId;
	U32							GroupSize;
	compatibilityDescriptor_t	GroupCompatibility;
	U16							GroupInfoLength;

	name_descriptor_t			name_descriptor;
	location_descriptor_t		location_descriptor;

	S32							group_descriptor_count;
	reserved_descriptor_t		group_descriptors[MAX_RESERVED_DESCRIPTORS];

} GroupInfo_t, *pGroupInfo_t;

typedef struct GroupInfoIndication_s
{
	int							NumberOfGroups;							//16
	GroupInfo_t					GroupInfo[MAX_GROUPS];

	int							PrivateDataLength;
	uint8_t						privateDataByte[64];

} GroupInfoIndication_t, *pGroupInfoIndication_t;

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


typedef struct dsmcc_unm_section_s
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

	dsmccMessageHeader_t		dsmccMessageHeader;

	union
	{
		DownloadServerInitiate_t			DownloadServerInitiate;
		DownloadInfoIndication_t			DownloadInfoIndication;
		//DownloadDataBlock_t					DownloadDataBlock;
	} u;

	union
	{
		uint32_t	CRC_32;										//32
		uint32_t	checksum;									//32
	} encode;

	union
	{
		uint32_t	CRC_32;										//32
		uint32_t	checksum;									//32
	} recalculated;

} dsmcc_unm_section_t;

_CDL_EXPORT int	MPEG2_DSMCC_DSI_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication);
_CDL_EXPORT int	MPEG2_DSMCC_DSI_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo);

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication);
_CDL_EXPORT	int	MPEG2_DSMCC_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate);

_CDL_EXPORT	int	MPEG2_DSMCC_UNM_DecodeSection(uint8_t *buf, int length, dsmcc_unm_section_t* pDSMCCSection);

#endif
