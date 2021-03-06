#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI_OC.h"
#include "../../Include/MPEG2_DVB_ErrorCode.h"

#include "../../../Common/BitStream.h"
#include "../../../Common/ByteStream.h"

/////////////////////////////////////////////

int	MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length >= 12) && (pServiceGatewayInfo != NULL))
	{
		memset(pServiceGatewayInfo, 0x00, sizeof(ServiceGatewayInfo_t));

		BITS_t bs;
		BITS_map(&bs, buf, length);

		BIOP::IOR_t*	pIOR = &(pServiceGatewayInfo->IOR);
		MPEG2_DSMCC_BIOP_DecodeIOR(&bs, pIOR);

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



