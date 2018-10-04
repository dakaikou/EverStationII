#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_DSMCC_section_UNM.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	S32		reserved_count;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		move_length;
	U8		moduleInfoLength;
	S32		copy_length;
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
			copy_length = min(pcompatibilityDescriptor->compatibilityDescriptorLength, sizeof(pcompatibilityDescriptor->compatibilityDescriptorBuf));
			memcpy(pcompatibilityDescriptor->compatibilityDescriptorBuf, bytes.p_cur, copy_length);
			BYTES_skip(&bytes, pcompatibilityDescriptor->compatibilityDescriptorLength);
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
							copy_length = min(sizeof(pBIOP_moduleInfo->userInfo_data_byte), pBIOP_moduleInfo->userInfoLength);
							memcpy(pBIOP_moduleInfo->userInfo_data_byte, bytes.p_cur, copy_length);
							BYTES_skip(&bytes, pBIOP_moduleInfo->userInfoLength);
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
				copy_length = min(sizeof(pDownloadInfoIndication->privateDataByte), pDownloadInfoIndication->privateDataLength);
				memcpy(pDownloadInfoIndication->privateDataByte, bytes.p_cur, copy_length);
				BYTES_skip(&bytes, pDownloadInfoIndication->privateDataLength);
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeGroupInfoIndication(uint8_t *buf, int length, GroupInfoIndication_t* pGroupInfoIndication)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*	ptemp;
	uint8_t*	pend;
	S32		remain_length;
	S32		n;
	S32		i;
	S32		reserved_count;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		move_length;
	S32		copy_length;
	S32		GroupInfoLength;
	S32		compatibilityDescriptorLength;

	if ((buf != NULL) && (length > 0) && (pGroupInfoIndication != NULL))
	{
		memset(pGroupInfoIndication, 0x00, sizeof(GroupInfoIndication_t));

		ptemp = buf;

		pGroupInfoIndication->NumberOfGroups = *ptemp ++;
		pGroupInfoIndication->NumberOfGroups <<= 8;
		pGroupInfoIndication->NumberOfGroups |= *ptemp ++;

		n = 0;
		for (i = 0; i < pGroupInfoIndication->NumberOfGroups; i++)
		{
			remain_length = length - (int)(ptemp - buf);
			if (remain_length >= 12)
			{
				if (n < MAX_GROUPS)
				{
					pGroupInfoIndication->GroupInfo[n].GroupId = *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupId <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupId |= *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupId <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupId |= *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupId <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupId |= *ptemp ++;

					pGroupInfoIndication->GroupInfo[n].GroupSize = *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupSize <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupSize |= *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupSize <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupSize |= *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupSize <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupSize |= *ptemp ++;

					pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength = *ptemp ++;
					pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength <<= 8;
					pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength |= *ptemp ++;

					if (pGroupInfoIndication->GroupInfo[n].GroupCompatibility.compatibilityDescriptorLength == 0)
					{
						pGroupInfoIndication->GroupInfo[n].GroupInfoLength = *ptemp ++;
						pGroupInfoIndication->GroupInfo[n].GroupInfoLength <<= 8;
						pGroupInfoIndication->GroupInfo[n].GroupInfoLength |= *ptemp ++;

						remain_length = length - (int)(ptemp - buf);

						if (remain_length >= pGroupInfoIndication->GroupInfo[n].GroupInfoLength)
						{
							pend = ptemp + pGroupInfoIndication->GroupInfo[n].GroupInfoLength;

							memset(&(pGroupInfoIndication->GroupInfo[n].name_descriptor), 0, sizeof(name_descriptor_t));
							memset(&(pGroupInfoIndication->GroupInfo[n].location_descriptor), 0, sizeof(location_descriptor_t));
				
							reserved_count = 0;
				
							while (ptemp < pend)
							{
								descriptor_tag = (ptemp[0] | 0x3000);
								descriptor_length = ptemp[1];
				
								move_length = descriptor_length + 2;

								switch (descriptor_tag)
								{
								case MPEG2_DSMCC_NAME_DESCRIPTOR:
									MPEG2_DSMCC_decode_name_descriptor(ptemp, move_length, &(pGroupInfoIndication->GroupInfo[n].name_descriptor));
									break;
								case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
									MPEG2_DSMCC_decode_location_descriptor(ptemp, move_length, &(pGroupInfoIndication->GroupInfo[n].location_descriptor));
									break;
								default:
									if (descriptor_tag == 0x3081)
									{
										if (pGroupInfoIndication->GroupInfo[n].name_descriptor.descriptor_tag == 0x00)
										{
											MPEG2_DSMCC_decode_name_descriptor(ptemp, move_length, &(pGroupInfoIndication->GroupInfo[n].name_descriptor));
										}
									}
									else
									{
										if (reserved_count < MAX_RESERVED_DESCRIPTORS)
										{
											pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
											pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
											pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_buf = ptemp;
											pGroupInfoIndication->GroupInfo[n].reserved_descriptor[reserved_count].descriptor_size = move_length;
											reserved_count ++;
										}
									}

									break;
								}
				
								ptemp += move_length;
							}

							assert(ptemp == pend);
				
							pGroupInfoIndication->GroupInfo[n].reserved_count = reserved_count;

							n ++;
						}
						else
						{
							rtcode = SECTION_PARSE_SYNTAX_ERROR;
							break;
						}
					}
					else
					{
						rtcode = SECTION_PARSE_SYNTAX_ERROR;
						break;
					}
				}
				else
				{
					//skip GroupId
					ptemp += 4;

					//skip GroupSize
					ptemp += 4;

					//skip compatibilityDescriptorLength
					compatibilityDescriptorLength = *ptemp ++;
					compatibilityDescriptorLength <<= 8;
					compatibilityDescriptorLength |= *ptemp ++;

					if (compatibilityDescriptorLength == 0)
					{
						//skip GroupInfoLength
						GroupInfoLength = *ptemp ++;
						GroupInfoLength <<= 8;
						GroupInfoLength |= *ptemp ++;

						remain_length = length - (int)(ptemp - buf);

						if (remain_length >= GroupInfoLength)
						{
							ptemp += GroupInfoLength;
						}
						else
						{
							rtcode = SECTION_PARSE_SYNTAX_ERROR;
							break;
						}
					}
					else
					{
						rtcode = SECTION_PARSE_SYNTAX_ERROR;
						break;
					}
				}
			}
			else
			{
				rtcode = SECTION_PARSE_SYNTAX_ERROR;
				break;
			}
		}
		pGroupInfoIndication->N = n;

		remain_length = length - (int)(ptemp - buf);

		if (remain_length >= 2)
		{
			pGroupInfoIndication->PrivateDataLength = *ptemp ++;
			pGroupInfoIndication->PrivateDataLength <<= 8;
			pGroupInfoIndication->PrivateDataLength |= *ptemp ++;

			remain_length = length - (int)(ptemp - buf);
			if (remain_length >= pGroupInfoIndication->PrivateDataLength)
			{
				copy_length = min(64, pGroupInfoIndication->PrivateDataLength);
				if (copy_length > 0)
				{
					memcpy(pGroupInfoIndication->privateDataByte, ptemp, copy_length);
					ptemp += pGroupInfoIndication->PrivateDataLength;

					remain_length = length - (int)(ptemp - buf);
					if (remain_length > 0)
					{
						//说明解析有问题
						rtcode = SECTION_PARSE_SYNTAX_ERROR;
					}
				}
			}
			else
			{
				rtcode = SECTION_PARSE_SYNTAX_ERROR;
			}
		}
		else
		{
			rtcode = SECTION_PARSE_SYNTAX_ERROR;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeServiceGatewayInfo(uint8_t *buf, int length, ServiceGatewayInfo_t* pServiceGatewayInfo)
{
	int		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		i;
	S32		component_index;
	S32		remain_length;

	BIOPProfileBody_t*			pBIOPProfileBody;
	LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
	BIOP::ObjectLocation_t*		pObjectLocation;
	DSM::ConnBinder_t*			pConnBinder;
	U32							tag;

	if ((buf != NULL) && (length > 4) && (pServiceGatewayInfo != NULL))
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

int	MPEG2_DSMCC_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;

	if ((buf != NULL) && (length > 24) && (pDownloadServerInitiate != NULL))
	{
		memset(pDownloadServerInitiate, 0x00, sizeof(DownloadServerInitiate_t));

		BYTES_t		bytes;
		BYTES_map(&bytes, buf, length);

		BYTES_copy(pDownloadServerInitiate->serverId, &bytes, 20);

		pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength = BYTES_get(&bytes, 2);

		if (pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength > 0)
		{
			int copy_length = min(pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength, sizeof(pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorBuf));
			memcpy(pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorBuf, bytes.p_cur, copy_length);
			BYTES_skip(&bytes, pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength);
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

					rtcode = MPEG2_DSMCC_DecodeGroupInfoIndication(ptemp, pDownloadServerInitiate->privateDataLength, pGroupInfoIndication);
				}
				else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)			//OC
				{
					///////////////////////////////////////////////////////////////////////////
					ServiceGatewayInfo_t* pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);

					rtcode = MPEG2_DSMCC_DecodeServiceGatewayInfo(ptemp, pDownloadServerInitiate->privateDataLength, pServiceGatewayInfo);
				}
				else
				{
					int copy_length = min(pDownloadServerInitiate->privateDataLength, sizeof(pDownloadServerInitiate->u.privateDataByte));
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

int MPEG2_DSMCC_UNM_DecodeSection(uint8_t *section_buf, int section_size, dsmcc_unm_section_t* pdsmcc_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;

	int						copy_length;
	uint8_t*				ptemp;
	int						stream_error;

	uint8_t					section_syntax_indicator;

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	DownloadServerInitiate_t*			pDownloadServerInitiate;
	DownloadInfoIndication_t*			pDownloadInfoIndication;
	//DownloadDataBlock_t*				pDownloadDataBlock;

	if ((section_buf != NULL) &&
		((section_size >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (section_size <= MPEG2_DSMCC_SECTION_MAX_SIZE)) &&
		(pdsmcc_section != NULL))
	{
		memset(pdsmcc_section, 0x00, sizeof(dsmcc_unm_section_t));

		section_syntax_indicator = (section_buf[1] & 0x80) >> 7;

		stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			pdsmcc_section->recalculated.CRC_32 = Encode_CRC_32(section_buf, section_size - 4);
			uint32_t CRC_32_encoded = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculated.CRC_32 != CRC_32_encoded)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加
			pdsmcc_section->recalculated.checksum = Encode_CRC_32(section_buf, section_size - 4);
			uint32_t checksum_encoded = ((section_buf[section_size - 4] << 24) | (section_buf[section_size - 3] << 16) | (section_buf[section_size - 2] << 8) | section_buf[section_size - 1]);

			if (pdsmcc_section->recalculated.checksum != checksum_encoded)
			{
				stream_error = 1;
			}
		}

		//if (stream_error == 0)
		{
			BITS_t bs;
			BITS_map(&bs, section_buf, section_size);

			pdsmcc_section->table_id = BITS_get(&bs, 8);
			assert(pdsmcc_section->table_id == TABLE_ID_DSMCC_UNM);

			pdsmcc_section->section_syntax_indicator = BITS_get(&bs, 1);
			pdsmcc_section->private_indicator = BITS_get(&bs, 1);
			pdsmcc_section->reserved0 = BITS_get(&bs, 2);
			pdsmcc_section->dsmcc_section_length = BITS_get(&bs, 12);

			if ((pdsmcc_section->dsmcc_section_length + 3) == section_size)
			{
				pdsmcc_section->table_id_extension = BITS_get(&bs, 16);

				pdsmcc_section->reserved1 = BITS_get(&bs, 2);
				pdsmcc_section->version_number = BITS_get(&bs, 5);
				pdsmcc_section->current_next_indicator = BITS_get(&bs, 1);

				pdsmcc_section->section_number = BITS_get(&bs, 8);
				pdsmcc_section->last_section_number = BITS_get(&bs, 8);

				int section_payload_length = pdsmcc_section->dsmcc_section_length - 4 - 5;
				if (section_payload_length > 0)
				{
					ptemp = bs.p_cur;
					BITS_byteSkip(&bs, section_payload_length);

					pdsmccMessageHeader = &(pdsmcc_section->dsmccMessageHeader);

					BYTES_t bytes;
					BYTES_map(&bytes, ptemp, section_payload_length);

					pdsmccMessageHeader->protocolDiscriminator = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->dsmccType = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->messageId = BYTES_get(&bytes, 2);
					pdsmccMessageHeader->transactionId = BYTES_get(&bytes, 4);
					pdsmccMessageHeader->reserved = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->adaptationLength = BYTES_get(&bytes, 1);
					pdsmccMessageHeader->messageLength = BYTES_get(&bytes, 2);

					assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
					//messageLength解析错误，将是灾难性的

					if (pdsmccMessageHeader->adaptationLength > 0)
					{
						//解析adaptation
						pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType = BYTES_get(&bytes, 1);

						pdsmccMessageHeader->dsmccAdaptationHeader.N = pdsmccMessageHeader->adaptationLength - 1;
						copy_length = min(64, pdsmccMessageHeader->dsmccAdaptationHeader.N);
						if (copy_length > 0)
						{
							memcpy(pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, bytes.p_cur, copy_length);
							BYTES_skip(&bytes, pdsmccMessageHeader->dsmccAdaptationHeader.N);
						}
					}

					int msg_payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
					if (msg_payload_length > 0)
					{
						if (pdsmccMessageHeader->messageId == 0x1002)			//DII
						{
							pDownloadInfoIndication = &(pdsmcc_section->u.DownloadInfoIndication);

							rtcode = MPEG2_DSMCC_DecodeDownloadInfoIndication(bytes.p_cur, msg_payload_length, pDownloadInfoIndication);
						}
						else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
						{
							pDownloadServerInitiate = &(pdsmcc_section->u.DownloadServerInitiate);

							rtcode = MPEG2_DSMCC_DecodeDownloadServerInitiate(bytes.p_cur, msg_payload_length, pDownloadServerInitiate);
						}
						else
						{
							assert(0);
							rtcode = SECTION_PARSE_SYNTAX_ERROR;
						}

						BYTES_skip(&bytes, msg_payload_length);
					}

					assert(bytes.p_cur == bs.p_cur);
				}

				uint32_t checksum_or_crc32 = BITS_get(&bs, 32);
				if (pdsmcc_section->section_syntax_indicator == 1)
				{
					pdsmcc_section->encode.CRC_32 = checksum_or_crc32;
				}
				else
				{
					pdsmcc_section->encode.checksum = checksum_or_crc32;
				}
			}
			else
			{
				rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
			}
		}

		//else
		//{
		//	rtcode = SECTION_PARSE_CRC_ERROR;						//校验错误
		//}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

