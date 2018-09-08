#ifndef _MPEG_DSMCC_SECTION_UNM_H_
#define _MPEG_DSMCC_SECTION_UNM_H_

#include "../compile.h"
#include "MPEG_DVB_SysCapability.h"
#include "MPEG_DVB_Common.h"
#include "Mpeg2_DSMCC_Descriptor.h"
#include "Mpeg2_DSMCC_section.h"
#include "HAL/HAL_Sys/Include/INTTYPES.H"


/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/
namespace BIOP
{
	typedef struct _ObjectLocation_s
	{
		U32			componentId_tag;					//32
		U8			component_data_length;				//8
		U32			carouselId;							//32
		U16			moduleId;							//16
		struct
		{
			U8			major;							//8
			U8			minor;							//8

		} version;

		U8			objectKey_length;					//8
	//	U8			objectKey_data_byte[4];				//8 x N2
		U32			objectKey_data;

	} ObjectLocation_t;

	typedef struct _Tap_s
	{
		U16			id;									//16
		U16			use;								//16
		U16			association_tag;					//16
		U8			selector_length;					//8
		U16			selector_type;						//16
		U32			transactionId;						//32
		U32			timeout;							//32

	} Tap_t;

	typedef struct _Name_s
	{
		U8			nameComponents_count;						//8

		U8			id_length[1];								//8		标准中规定是1			
		S8			id_data_byte[1][65];						//
		U8			kind_length[1];
		S8			kind_data_byte[1][4];
	} Name_t;
}

namespace DSM
{
	typedef struct _ConnBinder_s
	{
		U32			componentId_tag;					//32
		int			component_data_length;				//8
		U8			taps_count;							//8

		BIOP::Tap_t	Tap;

	} ConnBinder_t;
};

typedef struct _BIOPProfileBody_s
{
	uint32_t					profileId_tag;						//32
	int							profile_data_length;				//32
	uint8_t						profile_data_byte_order;			//8
	uint8_t						liteComponents_count;				//8
	BIOP::ObjectLocation_t		ObjectLocation;
	DSM::ConnBinder_t			ConnBinder;

} BIOPProfileBody_t;

typedef struct _LiteOptionsProfileBody_s
{
	U32							profileId_tag;						//32
	int							profile_data_length;				//32
	U8							profile_data_byte_order;			//8

} LiteOptionsProfileBody_t;

namespace IOP
{
	typedef struct _IOR_s
	{
		uint32_t				type_id_length;						//32
		char					type_id_byte[4];					//8 x N1
		uint8_t					alignment_gap[4];					//8 x ???

		int					taggedProfiles_count;				//32
		struct
		{
			uint32_t		profileId_tag;
			int		profile_data_length;

			union
			{
				BIOPProfileBody_t			BIOPProfileBody;
				LiteOptionsProfileBody_t	LiteOptionsProfileBody;
			} u;
		
		}taggedProfile[4];

	} IOR_t;
};

typedef struct _ServiceGatewayInfo_s
{
	IOP::IOR_t	IOR;

	U8			downloadTaps_count;						//8	
	U8			Tap[1];									//

	U8			serviceContextList_count;				//8
	U32			context_id[1];							//32
	U16			context_data_length[1];					//16
	U8			context_data_byte[1][1];				//8
	U16			userInfoLength;							//16
	U8			userInfo_data_byte[256];						

} ServiceGatewayInfo_t;

//typedef struct compatibilityDescriptor_s
//{
//	U16			compatibilityDescriptorLength;								//16			
//} compatibilityDescriptor_t, *pcompatibilityDescriptor_t;

//typedef struct dsmccAdaptationHeader_s
//{
//	U8			adaptationType;						//8
//	U8			N;
//	U8			adaptationDataByte[64];
//
//} dsmccAdaptationHeader_t, *pdsmccAdaptationHeader_t;

//typedef struct LLCSNAP_s
//{
//	U8			reserved;
//} LLCSNAP_t, *pLLCSNAP_t;

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

	struct
	{
		U16			id;							//16
		U16			use;						//16
		U16			association_tag;			//16
		U8			selector_length;			//8
		//U8			selector_data_byte[256];	//8
	}TAP[4];

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

	S32							reserved_count;
	reserved_descriptor_t		reserved_descriptor[MAX_RESERVED_DESCRIPTORS];

} GroupInfo_t, *pGroupInfo_t;

typedef struct GroupInfoIndication_s
{
	int							NumberOfGroups;							//16

	int							N;
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
		ServiceGatewayInfo_t			ServiceGatewayInfo;				//OC所使用到的数据结构
	} u;

} DownloadServerInitiate_t, *pDownloadServerInitiate_t;

typedef struct DSMCC_DSI_s
{
	int				data_broadcast_type;						//0x0006 -- DC, 0x0007 -- OC

	//DC
	S32				NumberOfGroups;
	GroupInfo_t*	astGroupInfo;

	//OC
	//业务网关信息
	U32				carouselId;
	U16				moduleId_for_srg;
	U32				objectKey_data_for_srg;

//	U16				moduleId_for_dii;
	U16				table_id_extension_for_dii;

} DSMCC_DSI_t;

typedef struct DSMCC_DII_s
{
	U32			downloadId;								//32
	U16			blockSize;								//16
	U8			windowSize;								//8
	U8			ackPeriod;								//8
	U32			tCDownloadWindow;						//32
	U32			tCDownloadScenario;						//32

	U16				numberOfModules;					//16
	moduleInfo_t*	astModuleInfo;

} DSMCC_DII_t;

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
		//DownloadDataBlock_t					DownloadDataBlock;
		DownloadServerInitiate_t			DownloadServerInitiate;
		DownloadInfoIndication_t			DownloadInfoIndication;
	} u;

	U32		CRC_32;										//32
	U32		CRC_32_verify;								//32

} dsmcc_unm_section_t;

_CDL_EXPORT int	MPEG2_DSMCC_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication);

_CDL_EXPORT int	MPEG2_DSMCC_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo);

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication);
_CDL_EXPORT	int	MPEG2_DSMCC_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate);

_CDL_EXPORT	int	MPEG2_DSMCC_UNM_DecodeSection(uint8_t *buf, int length, dsmcc_unm_section_t* pDSMCCSection);

#endif
