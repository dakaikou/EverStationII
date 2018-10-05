#ifndef _MPEG_DSMCC_COMMON_H_
#define _MPEG_DSMCC_COMMON_H_

#include <stdint.h>

#include "../../compile.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

namespace BIOP
{
	typedef struct _ObjectLocation_s
	{
		uint32_t	componentId_tag;					//32
		uint8_t		component_data_length;				//8
		uint32_t	carouselId;							//32
		uint16_t	moduleId;							//16
		struct
		{
			uint8_t			major;							//8
			uint8_t			minor;							//8

		} version;

		uint8_t			objectKey_length;					//8
														//	U8			objectKey_data_byte[4];				//8 x N2
		uint32_t			objectKey_data;

	} ObjectLocation_t;

	typedef struct _LiteComponent_s
	{
		uint32_t	componentId_tag;					//32
		uint8_t		component_data_length;				//8			N6
		uint8_t		component_data_byte[256];			// N6
	} LiteComponent_t;

	typedef struct _TAP_s
	{
		uint16_t		id;									//16
		uint16_t		use;								//16
		uint16_t		association_tag;					//16
		uint8_t			selector_length;					//8
		uint16_t		selector_type;						//16
		uint32_t		transactionId;						//32
		uint32_t		timeout;							//32

	} TAP_t;

	typedef struct _Name_s
	{
		uint8_t			nameComponents_count;						//8

		uint8_t			id_length[1];								//8		标准中规定是1			
		char			id_data_byte[1][65];						//
		uint8_t			kind_length[1];
		char			kind_data_byte[1][4];
	} NAME_t;
}

namespace DSM
{
	typedef struct _ConnBinder_s
	{
		uint32_t	componentId_tag;					//32
		int			component_data_length;				//8
		uint8_t		taps_count;							//8
		BIOP::TAP_t	Tap[16];							//为什么是16？  chendelin 2018.10.4

	} ConnBinder_t;
};

typedef struct _BIOPProfileBody_s
{
	uint32_t					profileId_tag;						//32
	int							profile_data_length;				//32
	uint8_t						profile_data_byte_order;			//8
	uint8_t						liteComponents_count;				//8

	BIOP::ObjectLocation_t		ObjectLocation;						//liteComponent 0
	DSM::ConnBinder_t			ConnBinder;							//liteComponent 1
	BIOP::LiteComponent_t		liteComponents[1];					//liteComponent 2 ~ N, 数组长度liteComponents_count - 2, 数组长度为什么是16？  chendelin  2018.10.4

} BIOPProfileBody_t;

typedef struct _LiteOptionsProfileBody_s
{
	uint32_t					profileId_tag;						//32
	int							profile_data_length;				//32
	uint8_t						profile_data_byte_order;			//8
	uint8_t						liteComponents_count;				//8

	BIOP::LiteComponent_t		liteComponents[1];					//数组长度为什么是16？  chendelin  2018.10.4

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
			int				profile_data_length;

			union
			{
				BIOPProfileBody_t			BIOPProfileBody;
				LiteOptionsProfileBody_t	LiteOptionsProfileBody;
			} u;

		}taggedProfile[4];

		BIOPProfileBody_t* pBIOPProfileBodyPortrait;

	} IOR_t;
};

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

_CDL_EXPORT int	MPEG2_DSMCC_DecodeIOR(BITS_t* pbs, IOP::IOR_t* pIOR);

#endif
