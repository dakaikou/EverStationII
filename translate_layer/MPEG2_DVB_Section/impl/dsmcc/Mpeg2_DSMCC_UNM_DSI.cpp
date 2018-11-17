#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "..\..\Include\dsmcc\Mpeg2_DSMCC_UNM_DSI.h"
#include "..\..\Include\MPEG2_DVB_ErrorCode.h"

#include "../../../Stream_AL/ByteStream.h"

int	MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length >= 24) && (pDownloadServerInitiate != NULL))
	{
		memset(pDownloadServerInitiate, 0x00, sizeof(DownloadServerInitiate_t));

		BYTES_t		bytes;
		BYTES_map(&bytes, buf, length);

		BYTES_copy(pDownloadServerInitiate->serverId, sizeof(pDownloadServerInitiate->serverId), &bytes, 20);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadServerInitiate->compatibilityDescriptor);
		pcompatibilityDescriptor->compatibilityDescriptorLength = BYTES_get(&bytes, 2);
		if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
		{
			pcompatibilityDescriptor->compatibilityDescriptorBuf = bytes.p_cur;
			BYTES_skip(&bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
			//BYTES_copy(pcompatibilityDescriptor->compatibilityDescriptorBuf, sizeof(pcompatibilityDescriptor->compatibilityDescriptorBuf), &bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
		}

		//defined in EN 301 192
		pDownloadServerInitiate->privateDataLength = BYTES_get(&bytes, 2);

		//若privateDataLength解析错误，将导致灾难性错误
		if (pDownloadServerInitiate->privateDataLength > 0)
		{
			//检查privateDataByte的类型
			pDownloadServerInitiate->data_broadcast_type = -1;

			uint8_t* ptemp = bytes.p_cur;
			int value_hi = (ptemp[0] << 8) | ptemp[1];
			if (value_hi == 0)
			{
				int value_lo = (ptemp[2] << 8) | ptemp[3];
				if (value_lo == 4)
				{
					//判断的依据为 IOP::IOR()的type_id_length 一般为4
					pDownloadServerInitiate->data_broadcast_type = 0x0007;
				}
			}
			else
			{
				pDownloadServerInitiate->data_broadcast_type = 0x0006;
			}

			//if (pDownloadServerInitiate->data_broadcast_type == 0x0006)		//DC
			//{
			//	///////////////////////////////////////////////////////////////////////////
			//	GroupInfoIndication_t* pGroupInfoIndication = &(pDownloadServerInitiate->u.GroupInfoIndication);

			//	rtcode = MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(ptemp, pDownloadServerInitiate->privateDataLength, pGroupInfoIndication);
			//}
			//else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)			//OC
			//{
			//	///////////////////////////////////////////////////////////////////////////
			//	ServiceGatewayInfo_t* pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);

			//	rtcode = MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(ptemp, pDownloadServerInitiate->privateDataLength, pServiceGatewayInfo);
			//}
			//else
			//{
			//	int copy_length = MIN(pDownloadServerInitiate->privateDataLength, sizeof(pDownloadServerInitiate->u.privateDataByte));
			//	memcpy(pDownloadServerInitiate->u.privateDataByte, ptemp, copy_length);
			//}

			pDownloadServerInitiate->privateDataByte = bytes.p_cur;
			BYTES_skip(&bytes, pDownloadServerInitiate->privateDataLength);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




