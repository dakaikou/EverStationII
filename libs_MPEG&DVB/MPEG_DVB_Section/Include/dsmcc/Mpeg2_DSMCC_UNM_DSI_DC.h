#ifndef _MPEG_DSMCC_UNM_DSI_DC_H_
#define _MPEG_DSMCC_UNM_DSI_DC_H_

#include "../../compile.h"

#include "../MPEG_DVB_SysCapability.h"
#include "../MPEG_DVB_Common.h"
#include "../Mpeg2_DSMCC_Descriptor.h"

//it is not propriateable to defined here
typedef struct compatibilityDescriptor_s
{
	uint16_t	compatibilityDescriptorLength;								//16		
	uint8_t		compatibilityDescriptorBuf[64];
} compatibilityDescriptor_t, *pcompatibilityDescriptor_t;

//for DC
typedef struct GroupInfo_s
{
	uint32_t					GroupId;
	uint32_t					GroupSize;
	compatibilityDescriptor_t	GroupCompatibility;
	int							GroupInfoLength;

	name_descriptor_t			name_descriptor;
	location_descriptor_t		location_descriptor;

	int							group_descriptor_count;
	reserved_descriptor_t		group_descriptors[MAX_RESERVED_DESCRIPTORS];

} GroupInfo_t, *pGroupInfo_t;

typedef struct GroupInfoIndication_s
{
	int							NumberOfGroups;							//16
	GroupInfo_t					GroupInfo[MAX_GROUPS];

	int							PrivateDataLength;
	uint8_t						privateDataByte[64];

} GroupInfoIndication_t, *pGroupInfoIndication_t;

_CDL_EXPORT int	MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication);

#endif
