#ifndef _MPEG_DSMCC_UNM_DSI_DC_H_
#define _MPEG_DSMCC_UNM_DSI_DC_H_

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

#include "Mpeg2_DSMCC_define.h"

#include "../MPEG2_DVB_SysCapability.h"
#include "../MPEG2_DVB_Common.h"
#include "../Mpeg2_DSMCC_Descriptor.h"

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

DSMCC_UNM_DSI_LIB int	MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication);

#endif
