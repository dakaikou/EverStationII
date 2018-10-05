#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../nativeInclude/dsmcc/Mpeg2_DSMCC_UNM_DSI_OC.h"
#include "../../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

//#ifndef min
//#define min(a,b)  (((a)<(b))?(a):(b))
//#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length >= 12) && (pServiceGatewayInfo != NULL))
	{
		memset(pServiceGatewayInfo, 0x00, sizeof(ServiceGatewayInfo_t));

		BITS_t bs;
		BITS_map(&bs, buf, length);

		IOP::IOR_t*	pIOR = &(pServiceGatewayInfo->IOR);
		MPEG2_DSMCC_DecodeIOR(&bs, pIOR);

		pServiceGatewayInfo->downloadTaps_count = BITS_get(&bs, 8);
		if (pServiceGatewayInfo->downloadTaps_count > 0)
		{
			assert(0);
		}

		pServiceGatewayInfo->serviceContextList_count = BITS_get(&bs, 8);
		if (pServiceGatewayInfo->serviceContextList_count > 0)
		{
			assert(0);
		}

		pServiceGatewayInfo->userInfoLength = BITS_get(&bs, 16);
		if (pServiceGatewayInfo->userInfoLength > 0)
		{
//				assert(0);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}



