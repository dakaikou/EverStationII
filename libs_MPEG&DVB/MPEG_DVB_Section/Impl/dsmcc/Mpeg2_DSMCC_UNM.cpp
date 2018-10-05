#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DSI.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_UNM_DII.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

//#ifndef min
//#define min(a,b)  (((a)<(b))?(a):(b))
//#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_UNM_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	S32		reserved_count;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		move_length;
	S32		tap_index;

	DC_moduleInfo_t*	pDC_moduleInfo;
	BIOP_moduleInfo_t*	pBIOP_moduleInfo;

	if ((buf != NULL) && (length > 0) && (pDownloadInfoIndication != NULL))
	{
		memset(pDownloadInfoIndication, 0x00, sizeof(DownloadInfoIndication_t));

		BYTES_t bytes;
		BYTES_map(&bytes, buf, length);

		pDownloadInfoIndication->downloadId = BYTES_get(&bytes, 4);

		pDownloadInfoIndication->blockSize = BYTES_get(&bytes, 2);

		pDownloadInfoIndication->windowSize = BYTES_get(&bytes, 1);

		pDownloadInfoIndication->ackPeriod = BYTES_get(&bytes, 1);

		pDownloadInfoIndication->tCDownloadWindow = BYTES_get(&bytes, 4);

		pDownloadInfoIndication->tCDownloadScenario = BYTES_get(&bytes, 4);

		compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadInfoIndication->compatibilityDescriptor);
		pcompatibilityDescriptor->compatibilityDescriptorLength = BYTES_get(&bytes, 2);
		if (pcompatibilityDescriptor->compatibilityDescriptorLength > 0)
		{
			BYTES_copy(pcompatibilityDescriptor->compatibilityDescriptorBuf, sizeof(pcompatibilityDescriptor), &bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
		}

		//if (pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength == 0x0000)
		{
			//defined in EN 301192
			pDownloadInfoIndication->numberOfModules = BYTES_get(&bytes, 2);
			assert(pDownloadInfoIndication->numberOfModules <= MAX_MODULES_PER_GROUP);

			for (int moduleIndex = 0; moduleIndex < pDownloadInfoIndication->numberOfModules; moduleIndex++)
			{
				pDownloadInfoIndication->moduleInfo[moduleIndex].moduleId = BYTES_get(&bytes, 2);

				pDownloadInfoIndication->moduleInfo[moduleIndex].moduleSize = BYTES_get(&bytes, 4);

				pDownloadInfoIndication->moduleInfo[moduleIndex].moduleVersion = BYTES_get(&bytes, 1);
				pDownloadInfoIndication->moduleInfo[moduleIndex].moduleInfoLength = BYTES_get(&bytes, 1);

				if (pDownloadInfoIndication->moduleInfo[moduleIndex].moduleInfoLength > 0)
				{
					uint8_t* moduleInfoByte = bytes.p_cur;

					//判断moduleInfo payload的类型
					pDownloadInfoIndication->moduleInfo[moduleIndex].data_broadcast_type = 0x0006;			//默认假设为DC
					if (pDownloadInfoIndication->moduleInfo[moduleIndex].moduleInfoLength >= 21)
					{
						int taps_count = moduleInfoByte[12];
						if (taps_count >= 1)
						{
							uint16_t ID = (moduleInfoByte[13] << 8) | moduleInfoByte[14];
							uint16_t use = (moduleInfoByte[15] << 8) | moduleInfoByte[16];
							uint8_t selector_length = moduleInfoByte[19];
							if ((ID == 0x0000) && (use == 0x0017) && (selector_length == 0x00))
							{
								pDownloadInfoIndication->moduleInfo[moduleIndex].data_broadcast_type = 0x0007;	//假设为OC
							}
						}
					}

					if (pDownloadInfoIndication->moduleInfo[moduleIndex].data_broadcast_type == 0x0006)			//DC
					{
						pDC_moduleInfo = &(pDownloadInfoIndication->moduleInfo[moduleIndex].u.DC_moduleInfo);

						reserved_count = 0;
						int loop_length = pDownloadInfoIndication->moduleInfo[moduleIndex].moduleInfoLength;
						while ((loop_length >= 2) && (reserved_count < MAX_RESERVED_DESCRIPTORS))
						{
							descriptor_tag = (moduleInfoByte[0] | 0x3000);
							descriptor_length = moduleInfoByte[1];
							move_length = descriptor_length + 2;

							switch (descriptor_tag)
							{
							case MPEG2_DSMCC_NAME_DESCRIPTOR:
								rtcode = MPEG2_DSMCC_decode_name_descriptor(moduleInfoByte, move_length, &(pDC_moduleInfo->name_descriptor));
								break;
							case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
								rtcode = MPEG2_DSMCC_decode_location_descriptor(moduleInfoByte, move_length, &(pDC_moduleInfo->location_descriptor));
								break;
							default:

								if (descriptor_tag == 0x3081)
								{
									if (pDC_moduleInfo->name_descriptor.descriptor_tag == 0x00)
									{
										rtcode = MPEG2_DSMCC_decode_name_descriptor(moduleInfoByte, move_length, &(pDC_moduleInfo->name_descriptor));
									}
								}
								else
								{
									if (reserved_count < MAX_RESERVED_DESCRIPTORS)
									{
										pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
										pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
										pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_buf = moduleInfoByte;
										pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_size = move_length;

										reserved_count ++;
									}
								}

								break;
							}

							moduleInfoByte += move_length;
						}

						pDC_moduleInfo->reserved_count = reserved_count;
					}
					else if (pDownloadInfoIndication->moduleInfo[moduleIndex].data_broadcast_type == 0x0007)
					{
						//假设是OC，尝试解析一遍
						pBIOP_moduleInfo = &(pDownloadInfoIndication->moduleInfo[moduleIndex].u.BIOP_moduleInfo);

						pBIOP_moduleInfo->moduleTimeOut = BYTES_get(&bytes, 4);

						pBIOP_moduleInfo->blockTimeOut = BYTES_get(&bytes, 4);

						pBIOP_moduleInfo->minBlockTime = BYTES_get(&bytes, 4);

						pBIOP_moduleInfo->taps_count = BYTES_get(&bytes, 1);
						assert(pBIOP_moduleInfo->taps_count <= 4);

						for (tap_index = 0; tap_index < pBIOP_moduleInfo->taps_count; tap_index++)
						{
							pBIOP_moduleInfo->TAP[tap_index].id = BYTES_get(&bytes, 2);

							pBIOP_moduleInfo->TAP[tap_index].use = BYTES_get(&bytes, 2);

							pBIOP_moduleInfo->TAP[tap_index].association_tag = BYTES_get(&bytes, 2);

							pBIOP_moduleInfo->TAP[tap_index].selector_length = BYTES_get(&bytes, 1);
							if (pBIOP_moduleInfo->TAP[tap_index].selector_length > 0)
							{
								BYTES_skip(&bytes, pBIOP_moduleInfo->TAP[tap_index].selector_length);
							}
						}

						pBIOP_moduleInfo->userInfoLength = BYTES_get(&bytes, 1);
						if (pBIOP_moduleInfo->userInfoLength > 0)
						{
							BYTES_copy(pBIOP_moduleInfo->userInfo_data_byte, sizeof(pBIOP_moduleInfo->userInfo_data_byte), &bytes, pBIOP_moduleInfo->userInfoLength);
						}
					}
					else
					{
						BYTES_skip(&bytes, pDownloadInfoIndication->moduleInfo[moduleIndex].moduleInfoLength);
					}
				}
			}

			pDownloadInfoIndication->privateDataLength = BYTES_get(&bytes, 2);
			if (pDownloadInfoIndication->privateDataLength > 0)
			{
				BYTES_copy(pDownloadInfoIndication->privateDataByte, sizeof(pDownloadInfoIndication->privateDataByte), &bytes, pDownloadInfoIndication->privateDataLength);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_UNM_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length > 24) && (pDownloadServerInitiate != NULL))
	{
		memset(pDownloadServerInitiate, 0x00, sizeof(DownloadServerInitiate_t));

		BYTES_t		bytes;
		BYTES_map(&bytes, buf, length);

		BYTES_copy(pDownloadServerInitiate->serverId, sizeof(pDownloadServerInitiate->serverId), &bytes, 20);

		pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength = BYTES_get(&bytes, 2);

		if (pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength > 0)
		{
			compatibilityDescriptor_t* pcompatibilityDescriptor = &(pDownloadServerInitiate->compatibilityDescriptor);
			BYTES_copy(pcompatibilityDescriptor->compatibilityDescriptorBuf, sizeof(pcompatibilityDescriptor->compatibilityDescriptorBuf), &bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
		}

		//if (pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength == 0)
		{
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

				if (pDownloadServerInitiate->data_broadcast_type == 0x0006)		//DC
				{
					///////////////////////////////////////////////////////////////////////////
					GroupInfoIndication_t* pGroupInfoIndication = &(pDownloadServerInitiate->u.GroupInfoIndication);

					rtcode = MPEG2_DSMCC_DSI_DC_DecodeGroupInfoIndication(ptemp, pDownloadServerInitiate->privateDataLength, pGroupInfoIndication);
				}
				else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)			//OC
				{
					///////////////////////////////////////////////////////////////////////////
					ServiceGatewayInfo_t* pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);

					rtcode = MPEG2_DSMCC_DSI_OC_DecodeServiceGatewayInfo(ptemp, pDownloadServerInitiate->privateDataLength, pServiceGatewayInfo);
				}
				else
				{
					int copy_length = MIN(pDownloadServerInitiate->privateDataLength, sizeof(pDownloadServerInitiate->u.privateDataByte));
					memcpy(pDownloadServerInitiate->u.privateDataByte, ptemp, copy_length);
				}

				BYTES_skip(&bytes, pDownloadServerInitiate->privateDataLength);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


