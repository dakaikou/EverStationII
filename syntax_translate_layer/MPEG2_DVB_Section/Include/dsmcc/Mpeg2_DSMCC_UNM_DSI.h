#ifndef _MPEG_DSMCC_UNM_DSI_H_
#define _MPEG_DSMCC_UNM_DSI_H_

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

#include "Mpeg2_DSMCC_UNM_DSI_DC.h"
#include "Mpeg2_DSMCC_UNM_DSI_OC.h"

typedef struct DownloadServerInitiate_s
{
	uint8_t							serverId[20];								//20x8
	compatibilityDescriptor_t		compatibilityDescriptor;
	uint16_t						privateDataLength;							//2 bytes
	uint8_t*						privateDataByte;							//lifecycle is the same as the section buf

	int								data_broadcast_type;						//0x0006		-- DC - GroupInfoIndication

} DownloadServerInitiate_t, *pDownloadServerInitiate_t;

DSMCC_UNM_DSI_LIB	int	MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate);

#endif
