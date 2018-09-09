#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_DSMCC_section_UNM.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DecodeDownloadInfoIndication(uint8_t *buf, int length, DownloadInfoIndication_t* pDownloadInfoIndication)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	U8*		pstart;
	U8*		pend;
	S32		i;
	S32		n;
	S32		remain_length;
	S32		reserved_count;
	U16		descriptor_tag;
	U8		descriptor_length;
	S32		move_length;
	U8		moduleInfoLength;
	S32		copy_length;
	S32		parse_payload;
	S32		tap_index;

	DC_moduleInfo_t*	pDC_moduleInfo;
	BIOP_moduleInfo_t*	pBIOP_moduleInfo;

	if ((buf != NULL) && (length > 0) && (pDownloadInfoIndication != NULL))
	{
		memset(pDownloadInfoIndication, 0x00, sizeof(DownloadInfoIndication_t));

		ptemp = buf;

		pDownloadInfoIndication->downloadId = *ptemp ++;
		pDownloadInfoIndication->downloadId <<= 8;
		pDownloadInfoIndication->downloadId |= *ptemp ++;
		pDownloadInfoIndication->downloadId <<= 8;
		pDownloadInfoIndication->downloadId |= *ptemp ++;
		pDownloadInfoIndication->downloadId <<= 8;
		pDownloadInfoIndication->downloadId |= *ptemp ++;

		pDownloadInfoIndication->blockSize = *ptemp ++;
		pDownloadInfoIndication->blockSize <<= 8;
		pDownloadInfoIndication->blockSize |= *ptemp ++;

		pDownloadInfoIndication->windowSize = *ptemp ++;

		pDownloadInfoIndication->ackPeriod = *ptemp ++;

		pDownloadInfoIndication->tCDownloadWindow = *ptemp ++;
		pDownloadInfoIndication->tCDownloadWindow <<= 8;
		pDownloadInfoIndication->tCDownloadWindow |= *ptemp ++;
		pDownloadInfoIndication->tCDownloadWindow <<= 8;
		pDownloadInfoIndication->tCDownloadWindow |= *ptemp ++;
		pDownloadInfoIndication->tCDownloadWindow <<= 8;
		pDownloadInfoIndication->tCDownloadWindow |= *ptemp ++;

		pDownloadInfoIndication->tCDownloadScenario = *ptemp ++;
		pDownloadInfoIndication->tCDownloadScenario <<= 8;
		pDownloadInfoIndication->tCDownloadScenario |= *ptemp ++;
		pDownloadInfoIndication->tCDownloadScenario <<= 8;
		pDownloadInfoIndication->tCDownloadScenario |= *ptemp ++;
		pDownloadInfoIndication->tCDownloadScenario <<= 8;
		pDownloadInfoIndication->tCDownloadScenario |= *ptemp ++;

		pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength = *ptemp ++;
		pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength <<= 8;
		pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength |= *ptemp ++;

		if (pDownloadInfoIndication->compatibilityDescriptor.compatibilityDescriptorLength == 0x0000)
		{
			//defined in EN 301192
			pDownloadInfoIndication->numberOfModules = *ptemp ++;
			pDownloadInfoIndication->numberOfModules <<= 8;
			pDownloadInfoIndication->numberOfModules |= *ptemp ++;

			n = 0;
			for (i = 0; i < pDownloadInfoIndication->numberOfModules; i++)
			{
				remain_length = length - (int)(ptemp - buf);
				if (remain_length >= 8)
				{
					if (n < MAX_MODULES_PER_GROUP)
					{
						pDownloadInfoIndication->moduleInfo[n].moduleId = *ptemp ++;
						pDownloadInfoIndication->moduleInfo[n].moduleId <<= 8;
						pDownloadInfoIndication->moduleInfo[n].moduleId |= *ptemp ++;

						pDownloadInfoIndication->moduleInfo[n].moduleSize = *ptemp ++;
						pDownloadInfoIndication->moduleInfo[n].moduleSize <<= 8;
						pDownloadInfoIndication->moduleInfo[n].moduleSize |= *ptemp ++;
						pDownloadInfoIndication->moduleInfo[n].moduleSize <<= 8;
						pDownloadInfoIndication->moduleInfo[n].moduleSize |= *ptemp ++;
						pDownloadInfoIndication->moduleInfo[n].moduleSize <<= 8;
						pDownloadInfoIndication->moduleInfo[n].moduleSize |= *ptemp ++;

						pDownloadInfoIndication->moduleInfo[n].moduleVersion = *ptemp ++;
						pDownloadInfoIndication->moduleInfo[n].moduleInfoLength = *ptemp ++;

						pDownloadInfoIndication->moduleInfo[n].data_broadcast_type = 0x0006;			//默认假设为DC

						remain_length = length - (int)(ptemp - buf);

						//如果moduleInfoLength解析错误,则会导致指针越界
						if (remain_length >= pDownloadInfoIndication->moduleInfo[n].moduleInfoLength)
						{
							pstart = ptemp;
							pend = ptemp + pDownloadInfoIndication->moduleInfo[n].moduleInfoLength;

							parse_payload = 1;
							while (parse_payload)
							{
								ptemp = pstart;

								if (pDownloadInfoIndication->moduleInfo[n].data_broadcast_type == 0x0006)			//DC
								{
									//不管三七二十一，先按DC解析一遍
									pDC_moduleInfo = &(pDownloadInfoIndication->moduleInfo[n].u.DC_moduleInfo);

									reserved_count = 0;
									while (ptemp < pend)
									{
										descriptor_tag = (ptemp[0] | 0x3000);
										descriptor_length = ptemp[1];
										move_length = descriptor_length + 2;

										switch (descriptor_tag)
										{
										case MPEG2_DSMCC_NAME_DESCRIPTOR:
											rtcode = MPEG2_DSMCC_decode_name_descriptor(ptemp, move_length, &(pDC_moduleInfo->name_descriptor));
											break;
										case MPEG2_DSMCC_LOCATION_DESCRIPTOR:
											rtcode = MPEG2_DSMCC_decode_location_descriptor(ptemp, move_length, &(pDC_moduleInfo->location_descriptor));
											break;
										default:

											if (descriptor_tag == 0x3081)
											{
												if (pDC_moduleInfo->name_descriptor.descriptor_tag == 0x00)
												{
													rtcode = MPEG2_DSMCC_decode_name_descriptor(ptemp, move_length, &(pDC_moduleInfo->name_descriptor));
												}
											}
											else
											{
												if (reserved_count < MAX_RESERVED_DESCRIPTORS)
												{
													pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_tag = descriptor_tag;
													pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_length = descriptor_length;
													pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_buf = ptemp;
													pDC_moduleInfo->reserved_descriptor[reserved_count].descriptor_size = move_length;

													reserved_count ++;
												}
											}

											break;
										}

										ptemp += move_length;
									}

									pDC_moduleInfo->reserved_count = reserved_count;

									if (ptemp != pend)
									{
										rtcode = SECTION_PARSE_SYNTAX_ERROR;
									}

									if (rtcode == SECTION_PARSE_NO_ERROR)
									{
										parse_payload = 0;
									}
									else
									{
										rtcode = SECTION_PARSE_NO_ERROR;
										pDownloadInfoIndication->moduleInfo[n].data_broadcast_type = 0x0007;
									}
								}
								else if (pDownloadInfoIndication->moduleInfo[n].data_broadcast_type == 0x0007)
								{
									//假设是OC，尝试解析一遍
									pBIOP_moduleInfo = &(pDownloadInfoIndication->moduleInfo[n].u.BIOP_moduleInfo);

									pBIOP_moduleInfo->moduleTimeOut = *ptemp ++;
									pBIOP_moduleInfo->moduleTimeOut <<= 8;
									pBIOP_moduleInfo->moduleTimeOut |= *ptemp ++;
									pBIOP_moduleInfo->moduleTimeOut <<= 8;
									pBIOP_moduleInfo->moduleTimeOut |= *ptemp ++;
									pBIOP_moduleInfo->moduleTimeOut <<= 8;
									pBIOP_moduleInfo->moduleTimeOut |= *ptemp ++;

									pBIOP_moduleInfo->blockTimeOut = *ptemp ++;
									pBIOP_moduleInfo->blockTimeOut <<= 8;
									pBIOP_moduleInfo->blockTimeOut |= *ptemp ++;
									pBIOP_moduleInfo->blockTimeOut <<= 8;
									pBIOP_moduleInfo->blockTimeOut |= *ptemp ++;
									pBIOP_moduleInfo->blockTimeOut <<= 8;
									pBIOP_moduleInfo->blockTimeOut |= *ptemp ++;

									pBIOP_moduleInfo->minBlockTime = *ptemp ++;
									pBIOP_moduleInfo->minBlockTime <<= 8;
									pBIOP_moduleInfo->minBlockTime |= *ptemp ++;
									pBIOP_moduleInfo->minBlockTime <<= 8;
									pBIOP_moduleInfo->minBlockTime |= *ptemp ++;
									pBIOP_moduleInfo->minBlockTime <<= 8;
									pBIOP_moduleInfo->minBlockTime |= *ptemp ++;

									pBIOP_moduleInfo->taps_count = *ptemp ++;

									assert(pBIOP_moduleInfo->taps_count <= 4);
									for (tap_index = 0; tap_index < pBIOP_moduleInfo->taps_count; tap_index++)
									{
										pBIOP_moduleInfo->TAP[tap_index].id = *ptemp ++;
										pBIOP_moduleInfo->TAP[tap_index].id <<= 8;
										pBIOP_moduleInfo->TAP[tap_index].id |= *ptemp ++;

										pBIOP_moduleInfo->TAP[tap_index].use = *ptemp ++;
										pBIOP_moduleInfo->TAP[tap_index].use <<= 8;
										pBIOP_moduleInfo->TAP[tap_index].use |= *ptemp ++;

										pBIOP_moduleInfo->TAP[tap_index].association_tag = *ptemp ++;
										pBIOP_moduleInfo->TAP[tap_index].association_tag <<= 8;
										pBIOP_moduleInfo->TAP[tap_index].association_tag |= *ptemp ++;

										pBIOP_moduleInfo->TAP[tap_index].selector_length = *ptemp ++;
										if (pBIOP_moduleInfo->TAP[tap_index].selector_length > 0)
										{
//											assert(0);
											ptemp += pBIOP_moduleInfo->TAP[tap_index].selector_length;
										}
									}

									pBIOP_moduleInfo->userInfoLength = *ptemp ++;

									if (pBIOP_moduleInfo->userInfoLength > 0)
									{
										memcpy(pBIOP_moduleInfo->userInfo_data_byte, ptemp, pBIOP_moduleInfo->userInfoLength);
										ptemp += pBIOP_moduleInfo->userInfoLength;
									}

									if (ptemp != pend)
									{
										rtcode = SECTION_PARSE_SYNTAX_ERROR;
									}

									if (rtcode == SECTION_PARSE_NO_ERROR)
									{
										parse_payload = 0;
									}
									else
									{
										rtcode = SECTION_PARSE_NO_ERROR;
										pDownloadInfoIndication->moduleInfo[n].data_broadcast_type = 0xffff;
									}
								}
								else
								{
									parse_payload = 0;
								}
							}

							ptemp = pend;

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
						//skip moduleId
						ptemp += 2;

						//skip moduleSize
						ptemp += 4;

						//skip moduleVersion
						ptemp += 1;

						moduleInfoLength = *ptemp ++;

						remain_length = length - (int)(ptemp - buf);
//							assert(remain_length >= pDownloadInfoIndication->moduleInfoLength[i]);

						//如果moduleInfoLength解析错误,则会导致指针越界
						if (remain_length >= moduleInfoLength)
						{
							ptemp += moduleInfoLength;
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

			pDownloadInfoIndication->N = n;

			remain_length = length - (int)(ptemp - buf);
			if (remain_length >= 2)
			{
				pDownloadInfoIndication->privateDataLength = *ptemp ++;
				pDownloadInfoIndication->privateDataLength <<= 8;
				pDownloadInfoIndication->privateDataLength |= *ptemp ++;

				remain_length = length - (int)(ptemp - buf);

				if (remain_length >= pDownloadInfoIndication->privateDataLength)
				{
					copy_length = min(64, pDownloadInfoIndication->privateDataLength);
					if (copy_length > 0)
					{
						memcpy(pDownloadInfoIndication->privateDataByte, ptemp, copy_length);
						ptemp += pDownloadInfoIndication->privateDataLength;
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
			rtcode = SECTION_PARSE_SYNTAX_ERROR;
			assert(0);
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
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		i;
	S32		component_index;
	S32		remain_length;

	IOP::IOR_t*					pIOR;
	BIOPProfileBody_t*			pBIOPProfileBody;
	LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
	BIOP::ObjectLocation_t*		pObjectLocation;
	DSM::ConnBinder_t*			pConnBinder;
	U32							tag;

	if ((buf != NULL) && (length > 4) && (pServiceGatewayInfo != NULL))
	{
		memset(pServiceGatewayInfo, 0x00, sizeof(ServiceGatewayInfo_t));

		ptemp = buf;

		//解析IOP::IOR()
		pServiceGatewayInfo->IOR.type_id_length = *ptemp ++;
		pServiceGatewayInfo->IOR.type_id_length <<= 8;
		pServiceGatewayInfo->IOR.type_id_length |= *ptemp ++;
		pServiceGatewayInfo->IOR.type_id_length <<= 8;
		pServiceGatewayInfo->IOR.type_id_length |= *ptemp ++;
		pServiceGatewayInfo->IOR.type_id_length <<= 8;
		pServiceGatewayInfo->IOR.type_id_length |= *ptemp ++;

		if (pServiceGatewayInfo->IOR.type_id_length == 4)
		{
			memcpy(pServiceGatewayInfo->IOR.type_id_byte, ptemp, pServiceGatewayInfo->IOR.type_id_length);
			ptemp += pServiceGatewayInfo->IOR.type_id_length;

			if ((pServiceGatewayInfo->IOR.type_id_length % 4) != 0)
			{
				for (i = 0; i < (S32)(4 - (pServiceGatewayInfo->IOR.type_id_length % 4)); i++)
				{
					pServiceGatewayInfo->IOR.alignment_gap[i] = *ptemp ++;
				}
			}

			pServiceGatewayInfo->IOR.taggedProfiles_count = *ptemp ++;
			pServiceGatewayInfo->IOR.taggedProfiles_count <<= 8;
			pServiceGatewayInfo->IOR.taggedProfiles_count |= *ptemp ++;
			pServiceGatewayInfo->IOR.taggedProfiles_count <<= 8;
			pServiceGatewayInfo->IOR.taggedProfiles_count |= *ptemp ++;
			pServiceGatewayInfo->IOR.taggedProfiles_count <<= 8;
			pServiceGatewayInfo->IOR.taggedProfiles_count |= *ptemp ++;

			if (pServiceGatewayInfo->IOR.taggedProfiles_count > 0)
			{
				for (i = 0; i < (S32)(pServiceGatewayInfo->IOR.taggedProfiles_count); i++)
				{
					pIOR = &(pServiceGatewayInfo->IOR);

					tag = *ptemp ++;
					tag <<= 8;
					tag |= *ptemp ++;
					tag <<= 8;
					tag |= *ptemp ++;
					tag <<= 8;
					tag |= *ptemp ++;

					pIOR->taggedProfile[i].profileId_tag = tag;

					if (tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
					{
						pBIOPProfileBody = &(pIOR->taggedProfile[i].u.BIOPProfileBody);

						pBIOPProfileBody->profileId_tag = tag;

						pBIOPProfileBody->profile_data_length = *ptemp ++;
						pBIOPProfileBody->profile_data_length <<= 8;
						pBIOPProfileBody->profile_data_length |= *ptemp ++;
						pBIOPProfileBody->profile_data_length <<= 8;
						pBIOPProfileBody->profile_data_length |= *ptemp ++;
						pBIOPProfileBody->profile_data_length <<= 8;
						pBIOPProfileBody->profile_data_length |= *ptemp ++;

						pBIOPProfileBody->profile_data_byte_order = *ptemp ++;
						pBIOPProfileBody->liteComponents_count = *ptemp ++;

						for (component_index = 0; component_index < pBIOPProfileBody->liteComponents_count; component_index ++)
						{
							tag = *ptemp ++;
							tag <<= 8;
							tag |= *ptemp ++;
							tag <<= 8;
							tag |= *ptemp ++;
							tag <<= 8;
							tag |= *ptemp ++;

							if (tag == 0x49534F50)
							{
								pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

								pObjectLocation->componentId_tag = tag;
								pObjectLocation->component_data_length = *ptemp ++;

								pObjectLocation->carouselId = *ptemp ++;
								pObjectLocation->carouselId <<= 8;
								pObjectLocation->carouselId |= *ptemp ++;
								pObjectLocation->carouselId <<= 8;
								pObjectLocation->carouselId |= *ptemp ++;
								pObjectLocation->carouselId <<= 8;
								pObjectLocation->carouselId |= *ptemp ++;

								pObjectLocation->moduleId = *ptemp ++;
								pObjectLocation->moduleId <<= 8;
								pObjectLocation->moduleId |= *ptemp ++;

								pObjectLocation->version.major = *ptemp ++;
								pObjectLocation->version.minor = *ptemp ++;

								pObjectLocation->objectKey_length = *ptemp ++;
//								if (pBIOP_ObjectLocation->objectKey_length > 0)
//								{
//									assert(pBIOP_ObjectLocation->objectKey_length <= 4);
//									memcpy(pBIOP_ObjectLocation->objectKey_data_byte, ptemp, pBIOP_ObjectLocation->objectKey_length);
//									ptemp += pBIOP_ObjectLocation->objectKey_length;
//								}
								pObjectLocation->objectKey_data = *ptemp++;
								pObjectLocation->objectKey_data <<= 8;
								pObjectLocation->objectKey_data |= *ptemp++;
								pObjectLocation->objectKey_data <<= 8;
								pObjectLocation->objectKey_data |= *ptemp++;
								pObjectLocation->objectKey_data <<= 8;
								pObjectLocation->objectKey_data |= *ptemp++;
							}
							else if (tag == 0x49534F40)
							{
								pConnBinder = &(pBIOPProfileBody->ConnBinder);

								pConnBinder->componentId_tag = tag;
								pConnBinder->component_data_length = *ptemp ++;
								pConnBinder->taps_count = *ptemp ++;

								pConnBinder->Tap.id = *ptemp ++;
								pConnBinder->Tap.id <<= 8;
								pConnBinder->Tap.id |= *ptemp ++;
								pConnBinder->Tap.id <<= 8;

								pConnBinder->Tap.use = *ptemp ++;
								pConnBinder->Tap.use <<= 8;
								pConnBinder->Tap.use |= *ptemp ++;

								pConnBinder->Tap.association_tag = *ptemp ++;
								pConnBinder->Tap.association_tag <<= 8;
								pConnBinder->Tap.association_tag |= *ptemp ++;

								pConnBinder->Tap.selector_length = *ptemp ++;

								pConnBinder->Tap.selector_type = *ptemp ++;
								pConnBinder->Tap.selector_type <<= 8;
								pConnBinder->Tap.selector_type |= *ptemp ++;

								pConnBinder->Tap.transactionId = *ptemp ++;
								pConnBinder->Tap.transactionId <<= 8;
								pConnBinder->Tap.transactionId |= *ptemp ++;
								pConnBinder->Tap.transactionId <<= 8;
								pConnBinder->Tap.transactionId |= *ptemp ++;
								pConnBinder->Tap.transactionId <<= 8;
								pConnBinder->Tap.transactionId |= *ptemp ++;

								pConnBinder->Tap.timeout = *ptemp ++;
								pConnBinder->Tap.timeout <<= 8;
								pConnBinder->Tap.timeout |= *ptemp ++;
								pConnBinder->Tap.timeout <<= 8;
								pConnBinder->Tap.timeout |= *ptemp ++;
								pConnBinder->Tap.timeout <<= 8;
								pConnBinder->Tap.timeout |= *ptemp ++;
							}
						}
					}
					else if (tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
					{
						pLiteOptionsProfileBody = &(pIOR->taggedProfile[i].u.LiteOptionsProfileBody);

						pLiteOptionsProfileBody->profileId_tag = tag;

						pLiteOptionsProfileBody->profile_data_length = *ptemp ++;
						pLiteOptionsProfileBody->profile_data_length <<= 8;
						pLiteOptionsProfileBody->profile_data_length |= *ptemp ++;
						pLiteOptionsProfileBody->profile_data_length <<= 8;
						pLiteOptionsProfileBody->profile_data_length |= *ptemp ++;
						pLiteOptionsProfileBody->profile_data_length <<= 8;
						pLiteOptionsProfileBody->profile_data_length |= *ptemp ++;

						pLiteOptionsProfileBody->profile_data_byte_order = *ptemp ++;
					}

					remain_length = length - (int)(ptemp - buf);
				}
			}

			pServiceGatewayInfo->downloadTaps_count = *ptemp ++;
			if (pServiceGatewayInfo->downloadTaps_count > 0)
			{
				assert(0);
			}

			pServiceGatewayInfo->serviceContextList_count = *ptemp ++;
			if (pServiceGatewayInfo->serviceContextList_count > 0)
			{
				assert(0);
			}

			pServiceGatewayInfo->userInfoLength = *ptemp ++;
			pServiceGatewayInfo->userInfoLength <<= 8;
			pServiceGatewayInfo->userInfoLength |= *ptemp ++;
			if (pServiceGatewayInfo->userInfoLength > 0)
			{
//				assert(0);
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

int	MPEG2_DSMCC_DecodeDownloadServerInitiate(uint8_t *buf, int length, DownloadServerInitiate_t* pDownloadServerInitiate)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	U8*		pend;
	S32		remain_length;

	GroupInfoIndication_t*			pGroupInfoIndication;
	ServiceGatewayInfo_t*			pServiceGatewayInfo;

	if ((buf != NULL) && (length > 24) && (pDownloadServerInitiate != NULL))
	{
		memset(pDownloadServerInitiate, 0x00, sizeof(DownloadServerInitiate_t));

		ptemp = buf;

		memcpy(pDownloadServerInitiate->serverId, ptemp, 20);
		ptemp += 20;

		pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength = *ptemp ++;
		pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength <<= 8;
		pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength |= *ptemp ++;

		if (pDownloadServerInitiate->compatibilityDescriptor.compatibilityDescriptorLength == 0)
		{
			//defined in EN 301 192
			pDownloadServerInitiate->privateDataLength = *ptemp ++;
			pDownloadServerInitiate->privateDataLength <<= 8;
			pDownloadServerInitiate->privateDataLength |= *ptemp ++;

			//若privateDataLength解析错误，将导致灾难性错误
			remain_length = length - (int)(ptemp - buf);
			if (remain_length >= pDownloadServerInitiate->privateDataLength)
			{
				pend = ptemp + pDownloadServerInitiate->privateDataLength;	//指出下一处指针的位置

				//检查privateDataByte的类型
				pDownloadServerInitiate->data_broadcast_type = -1;

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
					pGroupInfoIndication = &(pDownloadServerInitiate->u.GroupInfoIndication);

					rtcode = MPEG2_DSMCC_DecodeGroupInfoIndication(ptemp, pDownloadServerInitiate->privateDataLength, pGroupInfoIndication);
				}
				else if (pDownloadServerInitiate->data_broadcast_type == 0x0007)			//OC
				{
					///////////////////////////////////////////////////////////////////////////
					pServiceGatewayInfo = &(pDownloadServerInitiate->u.ServiceGatewayInfo);

					rtcode = MPEG2_DSMCC_DecodeServiceGatewayInfo(ptemp, pDownloadServerInitiate->privateDataLength, pServiceGatewayInfo);
				}
				else
				{
				}

				ptemp = pend;
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

int MPEG2_DSMCC_UNM_DecodeSection(uint8_t *buf, int length, dsmcc_unm_section_t* pDSMCC_section)
{
	S32						rtcode = SECTION_PARSE_NO_ERROR;
	S32						payload_length;
	S32						copy_length;
	U8*						ptemp;
	U32						tempId;
	S32						stream_error;

	U8						section_syntax_indicator;

	dsmccMessageHeader_t*				pdsmccMessageHeader;

	DownloadServerInitiate_t*			pDownloadServerInitiate;
	DownloadInfoIndication_t*			pDownloadInfoIndication;
	//DownloadDataBlock_t*				pDownloadDataBlock;

	if ((buf != NULL) && (length >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (pDSMCC_section != NULL))
	{
		memset(pDSMCC_section, 0x00, sizeof(dsmcc_unm_section_t));

		pDSMCC_section->CRC_32_verify = Encode_CRC_32(buf, length - 4);
		pDSMCC_section->CRC_32 = ((buf[length - 4] << 24) | (buf[length - 3] << 16) | (buf[length - 2] << 8) | buf[length - 1]);

		section_syntax_indicator = (buf[1] & 0x80) >> 7;

		stream_error = 0;
		if (section_syntax_indicator == 1)
		{
			if (pDSMCC_section->CRC_32_verify != pDSMCC_section->CRC_32)
			{
				stream_error = 1;
			}
		}
		else
		{
			//没有校验checksum, 以后增加

//			assert((length % 4) == 0);
		}

		if (stream_error == 0)
		{
			pDSMCC_section->table_id = *buf++;
			assert(pDSMCC_section->table_id == TABLE_ID_DSMCC_UNM);

			pDSMCC_section->section_syntax_indicator = (*buf & 0x80) >> 7;

			pDSMCC_section->dsmcc_section_length = (*buf++ & 0x0f) << 8;
			pDSMCC_section->dsmcc_section_length |= *buf++;

			if ((pDSMCC_section->dsmcc_section_length + 3) == length)
			{
				pDSMCC_section->table_id_extension = (*buf++) << 8;
				pDSMCC_section->table_id_extension |= *buf++;

				pDSMCC_section->version_number = (*buf & 0x3E) >> 1;
				pDSMCC_section->current_next_indicator = (*buf++ & 0x01);

				pDSMCC_section->section_number = *buf++;
				pDSMCC_section->last_section_number = *buf++;

				ptemp = buf;
				buf += (pDSMCC_section->dsmcc_section_length - 9);

				pdsmccMessageHeader = &(pDSMCC_section->dsmccMessageHeader);

				pdsmccMessageHeader->protocolDiscriminator = *ptemp++;
				pdsmccMessageHeader->dsmccType = *ptemp ++;

				pdsmccMessageHeader->messageId = *ptemp ++;
				pdsmccMessageHeader->messageId <<= 8;
				pdsmccMessageHeader->messageId |= *ptemp ++;

				tempId = *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;

				pdsmccMessageHeader->transactionId = tempId;

				pdsmccMessageHeader->reserved = *ptemp ++;

				pdsmccMessageHeader->adaptationLength = *ptemp ++;

				pdsmccMessageHeader->messageLength = *ptemp ++;
				pdsmccMessageHeader->messageLength <<= 8;
				pdsmccMessageHeader->messageLength |= *ptemp ++;

				assert(pdsmccMessageHeader->adaptationLength <= pdsmccMessageHeader->messageLength);
				//messageLength解析错误，将是灾难性的

				if (pdsmccMessageHeader->adaptationLength > 0)
				{
					//解析adaptation
					pdsmccMessageHeader->dsmccAdaptationHeader.adaptationType = *ptemp ++;

					copy_length = min(64, pdsmccMessageHeader->adaptationLength - 1);
					pdsmccMessageHeader->dsmccAdaptationHeader.N = copy_length;
					if (copy_length > 0)
					{
						memcpy(pdsmccMessageHeader->dsmccAdaptationHeader.adaptationDataByte, ptemp, copy_length);
						ptemp += (pdsmccMessageHeader->adaptationLength - 1);
					}
				}

//					payload_length = pdsmccMessageHeader->messageLength - pdsmccMessageHeader->adaptationLength;
				payload_length = (int)(buf - ptemp);

				if (payload_length >= pdsmccMessageHeader->messageLength)
				{
					if (pdsmccMessageHeader->messageId == 0x1002)			//DII
					{
						pDownloadInfoIndication = &(pDSMCC_section->u.DownloadInfoIndication);

						rtcode = MPEG2_DSMCC_DecodeDownloadInfoIndication(ptemp, payload_length, pDownloadInfoIndication);
					}
					else if (pdsmccMessageHeader->messageId == 0x1003)					//DDB
					{
						assert(0);
					}
					else if (pdsmccMessageHeader->messageId == 0x1006)							//DSI
					{
						pDownloadServerInitiate = &(pDSMCC_section->u.DownloadServerInitiate);

						rtcode = MPEG2_DSMCC_DecodeDownloadServerInitiate(ptemp, payload_length, pDownloadServerInitiate);
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

				buf += 4;
			}
			else
			{
				rtcode = SECTION_PARSE_LENGTH_ERROR;				//section长度错误
			}
		}
		else
		{
			rtcode = SECTION_PARSE_CRC_ERROR;						//校验错误
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

