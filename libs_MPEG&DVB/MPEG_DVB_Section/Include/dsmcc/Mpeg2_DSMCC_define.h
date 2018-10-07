#ifndef _MPEG_DSMCC_DEFINE_H_
#define _MPEG_DSMCC_DEFINE_H_

#include <stdint.h>

/*------------------------------------------------------------
		common DSM-CC definition
-------------------------------------------------------------*/

//typedef struct DownloadCancel_s
//{
//} DownloadCancel_t, *pDownloadCancel_t;

//it is not propriateable to defined here
typedef struct compatibilityDescriptor_s
{
	uint16_t	compatibilityDescriptorLength;								//16		
	uint8_t*	compatibilityDescriptorBuf;
} compatibilityDescriptor_t, *pcompatibilityDescriptor_t;

#endif
