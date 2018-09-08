#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

//#include "../Include/Mpeg2_DSMCC_Descriptor.h"
//#include "../Include/Mpeg2_PSI_section.h"
#include "../Include/Mpeg2_DSMCC_section.h"
#include "../Include/Mpeg2_DSMCC_section_DDM.h"
#include "../Include/Mpeg2_table_id.h"
#include "../Include/MPEG_DVB_ErrorCode.h"

#include "libs_Math/Include/CRC_32.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DecodeDownloadDataBlock(uint8_t *buf, int length, DownloadDataBlock_t* pDownloadDataBlock)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;

	if ((buf != NULL) && (length > 0) && (pDownloadDataBlock != NULL))
	{
		memset(pDownloadDataBlock, 0x00, sizeof(DownloadDataBlock_t));

		ptemp = buf;

		pDownloadDataBlock->moduleId = *ptemp ++;
		pDownloadDataBlock->moduleId <<= 8;
		pDownloadDataBlock->moduleId |= *ptemp ++;

		pDownloadDataBlock->moduleVersion = *ptemp ++;

		pDownloadDataBlock->reserved = *ptemp ++;

		pDownloadDataBlock->blockNumber = *ptemp ++;
		pDownloadDataBlock->blockNumber <<= 8;
		pDownloadDataBlock->blockNumber |= *ptemp ++;

		pDownloadDataBlock->N = length - (unsigned short)(ptemp - buf);
		if (pDownloadDataBlock->N > 0)
		{
//			memcpy(pDownloadDataBlock->blockDataByte, ptemp, pDownloadDataBlock->N);
			pDownloadDataBlock->blockDataByte = ptemp;
			ptemp += pDownloadDataBlock->N;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int MPEG2_DSMCC_DDM_DecodeSection(uint8_t *buf, int length, dsmcc_ddm_section_t* pDSMCC_section)
{
	int						rtcode = SECTION_PARSE_NO_ERROR;
	int						payload_length;
	int						copy_length;
	uint8_t*				ptemp;
	uint32_t				tempId;
	int						stream_error;

	uint8_t					section_syntax_indicator;

	dsmccDownloadDataHeader_t*			pdsmccDownloadDataHeader;
	DownloadDataBlock_t*				pDownloadDataBlock;

	if ((buf != NULL) && (length >= MPEG2_DSMCC_SECTION_MIN_SIZE) && (pDSMCC_section != NULL))
	{
		memset(pDSMCC_section, 0x00, sizeof(dsmcc_ddm_section_t));

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
			assert(pDSMCC_section->table_id == TABLE_ID_DSMCC_DDM);

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

				pdsmccDownloadDataHeader = &(pDSMCC_section->dsmccDownloadDataHeader);

				pdsmccDownloadDataHeader->protocolDiscriminator = *ptemp++;
				pdsmccDownloadDataHeader->dsmccType = *ptemp ++;

				pdsmccDownloadDataHeader->messageId = *ptemp ++;
				pdsmccDownloadDataHeader->messageId <<= 8;
				pdsmccDownloadDataHeader->messageId |= *ptemp ++;

				tempId = *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;
				tempId <<= 8;
				tempId |= *ptemp ++;

				pdsmccDownloadDataHeader->downloadId = tempId;

				pdsmccDownloadDataHeader->reserved = *ptemp ++;

				pdsmccDownloadDataHeader->adaptationLength = *ptemp ++;

				pdsmccDownloadDataHeader->messageLength = *ptemp ++;
				pdsmccDownloadDataHeader->messageLength <<= 8;
				pdsmccDownloadDataHeader->messageLength |= *ptemp ++;

				assert(pdsmccDownloadDataHeader->adaptationLength <= pdsmccDownloadDataHeader->messageLength);
				//messageLength解析错误，将是灾难性的

				if (pdsmccDownloadDataHeader->adaptationLength > 0)
				{
					//解析adaptation
					pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationType = *ptemp ++;

					copy_length = min(64, pdsmccDownloadDataHeader->adaptationLength - 1);
					pdsmccDownloadDataHeader->dsmccAdaptationHeader.N = copy_length;
					if (copy_length > 0)
					{
						memcpy(pdsmccDownloadDataHeader->dsmccAdaptationHeader.adaptationDataByte, ptemp, copy_length);
						ptemp += (pdsmccDownloadDataHeader->adaptationLength - 1);
					}
				}

				payload_length = (int)(buf - ptemp);

				if (payload_length >= pdsmccDownloadDataHeader->messageLength)
				{
					if (pdsmccDownloadDataHeader->messageId == 0x1002)			//DII
					{
						assert(0);
					}
					else if (pdsmccDownloadDataHeader->messageId == 0x1003)					//DDB
					{
						pDownloadDataBlock = &(pDSMCC_section->DownloadDataBlock);
						rtcode = MPEG2_DSMCC_DecodeDownloadDataBlock(ptemp, payload_length, pDownloadDataBlock);

					}
					else if (pdsmccDownloadDataHeader->messageId == 0x1006)							//DSI
					{
						//assert(0);
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

int	MPEG2_DSMCC_DecodeDirectoryMessage(uint8_t *buf, int length, DirectoryMessage_t* pDirectoryMessage)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		copy_length;
	S32		i;
	S32		j;
	S32		profile_index;
	S32		component_index;
	S32		binding_index;
	S32		remain_length;
	U32		tag;

	//BIOP::DirectoryMessage()
	BIOPProfileBody_t*			pBIOPProfileBody;
	LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
	IOP::IOR_t*					pIOR;
	BIOP::ObjectLocation_t*		pObjectLocation;
	//BIOP::Name_t*				pName;
	Bindings_t*					pBindings;
	DSM::ConnBinder_t*			pConnBinder;

	if ((buf != NULL) && (length > 0) && (pDirectoryMessage != NULL))
	{
		//		memset(pBIOP_DirectoryMessage, 0x00, sizeof(BIOP_DirectoryMessage_t));
		ptemp = buf;

		memcpy(pDirectoryMessage->magic, ptemp, 4);
		pDirectoryMessage->magic[4] = '\0';
		ptemp += 4;

		pDirectoryMessage->biop_version.major = *ptemp++;
		pDirectoryMessage->biop_version.minor = *ptemp++;

		pDirectoryMessage->byte_order = *ptemp++;
		pDirectoryMessage->message_type = *ptemp++;

		pDirectoryMessage->message_size = *ptemp++;
		pDirectoryMessage->message_size <<= 8;
		pDirectoryMessage->message_size |= *ptemp++;
		pDirectoryMessage->message_size <<= 8;
		pDirectoryMessage->message_size |= *ptemp++;
		pDirectoryMessage->message_size <<= 8;
		pDirectoryMessage->message_size |= *ptemp++;

		pDirectoryMessage->objectKey_length = *ptemp++;
		assert(pDirectoryMessage->objectKey_length == 4);

		pDirectoryMessage->objectKey_data = *ptemp++;
		pDirectoryMessage->objectKey_data <<= 8;
		pDirectoryMessage->objectKey_data |= *ptemp++;
		pDirectoryMessage->objectKey_data <<= 8;
		pDirectoryMessage->objectKey_data |= *ptemp++;
		pDirectoryMessage->objectKey_data <<= 8;
		pDirectoryMessage->objectKey_data |= *ptemp++;

		pDirectoryMessage->objectKind_length = *ptemp++;
		pDirectoryMessage->objectKind_length <<= 8;
		pDirectoryMessage->objectKind_length |= *ptemp++;
		pDirectoryMessage->objectKind_length <<= 8;
		pDirectoryMessage->objectKind_length |= *ptemp++;
		pDirectoryMessage->objectKind_length <<= 8;
		pDirectoryMessage->objectKind_length |= *ptemp++;

		assert(pDirectoryMessage->objectKind_length == 4);
		memcpy(pDirectoryMessage->objectKind_data, ptemp, 4);
		ptemp += 4;

		pDirectoryMessage->objectInfo_length = *ptemp++;
		pDirectoryMessage->objectInfo_length <<= 8;
		pDirectoryMessage->objectInfo_length |= *ptemp++;

		copy_length = min(8, pDirectoryMessage->objectInfo_length);
		if (copy_length > 0)
		{
			memcpy(pDirectoryMessage->objectInfo_data_byte, ptemp, copy_length);
			ptemp += pDirectoryMessage->objectInfo_length;
		}

		pDirectoryMessage->serviceContextList_count = *ptemp++;
		assert(pDirectoryMessage->serviceContextList_count <= 2);

		for (i = 0; i < pDirectoryMessage->serviceContextList_count; i++)
		{
			pDirectoryMessage->serviceContextList[i].context_id = *ptemp++;
			pDirectoryMessage->serviceContextList[i].context_id <<= 8;
			pDirectoryMessage->serviceContextList[i].context_id |= *ptemp++;
			pDirectoryMessage->serviceContextList[i].context_id <<= 8;
			pDirectoryMessage->serviceContextList[i].context_id |= *ptemp++;
			pDirectoryMessage->serviceContextList[i].context_id <<= 8;
			pDirectoryMessage->serviceContextList[i].context_id |= *ptemp++;

			pDirectoryMessage->serviceContextList[i].context_data_length = *ptemp++;
			pDirectoryMessage->serviceContextList[i].context_data_length <<= 8;
			pDirectoryMessage->serviceContextList[i].context_data_length |= *ptemp++;

			copy_length = min(4, pDirectoryMessage->serviceContextList[i].context_data_length);
			if (copy_length > 0)
			{
				memcpy(pDirectoryMessage->serviceContextList[i].context_data_byte, ptemp, copy_length);
				ptemp += pDirectoryMessage->serviceContextList[i].context_data_length;
			}
		}

		pDirectoryMessage->messageBody_length = *ptemp++;
		pDirectoryMessage->messageBody_length <<= 8;
		pDirectoryMessage->messageBody_length |= *ptemp++;
		pDirectoryMessage->messageBody_length <<= 8;
		pDirectoryMessage->messageBody_length |= *ptemp++;
		pDirectoryMessage->messageBody_length <<= 8;
		pDirectoryMessage->messageBody_length |= *ptemp++;

		pDirectoryMessage->bindings_count = *ptemp++;
		pDirectoryMessage->bindings_count <<= 8;
		pDirectoryMessage->bindings_count |= *ptemp++;

		assert(pDirectoryMessage->bindings_count <= 1024);
		for (binding_index = 0; binding_index < pDirectoryMessage->bindings_count; binding_index++)
		{
			pBindings = pDirectoryMessage->bindings + binding_index;

			//pName = &(pBindings->Name);

			pBindings->Name.nameComponents_count = *ptemp++;
			assert(pBindings->Name.nameComponents_count <= 1);

			for (j = 0; j < pBindings->Name.nameComponents_count; j++)
			{
				pBindings->Name.id_length[j] = *ptemp++;
				copy_length = min(64, pBindings->Name.id_length[j]);
				if (copy_length > 0)
				{
					memcpy(pBindings->Name.id_data_byte[j], ptemp, copy_length);
					pBindings->Name.id_data_byte[j][copy_length] = '\0';

					ptemp += pBindings->Name.id_length[j];
				}

				pBindings->Name.kind_length[j] = *ptemp++;
				//				assert(pName->kind_length[j] <= 4);
				copy_length = min(4, pBindings->Name.kind_length[j]);
				if (copy_length > 0)
				{
					memcpy(pBindings->Name.kind_data_byte[j], ptemp, copy_length);
					ptemp += pBindings->Name.kind_length[j];
				}
			}

			pBindings->bindingType = *ptemp++;

			pIOR = &(pBindings->IOR);

			pIOR->type_id_length = *ptemp++;
			pIOR->type_id_length <<= 8;
			pIOR->type_id_length |= *ptemp++;
			pIOR->type_id_length <<= 8;
			pIOR->type_id_length |= *ptemp++;
			pIOR->type_id_length <<= 8;
			pIOR->type_id_length |= *ptemp++;

			if (pIOR->type_id_length == 4)
			{
				memcpy(pIOR->type_id_byte, ptemp, pIOR->type_id_length);
				ptemp += pIOR->type_id_length;

				pIOR->taggedProfiles_count = *ptemp++;
				pIOR->taggedProfiles_count <<= 8;
				pIOR->taggedProfiles_count |= *ptemp++;
				pIOR->taggedProfiles_count <<= 8;
				pIOR->taggedProfiles_count |= *ptemp++;
				pIOR->taggedProfiles_count <<= 8;
				pIOR->taggedProfiles_count |= *ptemp++;

				if (pIOR->taggedProfiles_count > 0)
				{
					for (profile_index = 0; profile_index < (S32)(pIOR->taggedProfiles_count); profile_index++)
					{
						tag = *ptemp++;
						tag <<= 8;
						tag |= *ptemp++;
						tag <<= 8;
						tag |= *ptemp++;
						tag <<= 8;
						tag |= *ptemp++;

						pIOR->taggedProfile[profile_index].profileId_tag = tag;

						if (tag == 0x49534F06)				//TAG_BIOP(BIOP Profile Body)
						{
							pBIOPProfileBody = &(pIOR->taggedProfile[profile_index].u.BIOPProfileBody);

							pBIOPProfileBody->profileId_tag = tag;

							pBIOPProfileBody->profile_data_length = *ptemp++;
							pBIOPProfileBody->profile_data_length <<= 8;
							pBIOPProfileBody->profile_data_length |= *ptemp++;
							pBIOPProfileBody->profile_data_length <<= 8;
							pBIOPProfileBody->profile_data_length |= *ptemp++;
							pBIOPProfileBody->profile_data_length <<= 8;
							pBIOPProfileBody->profile_data_length |= *ptemp++;

							pBIOPProfileBody->profile_data_byte_order = *ptemp++;
							pBIOPProfileBody->liteComponents_count = *ptemp++;

							for (component_index = 0; component_index < pBIOPProfileBody->liteComponents_count; component_index++)
							{
								tag = *ptemp++;
								tag <<= 8;
								tag |= *ptemp++;
								tag <<= 8;
								tag |= *ptemp++;
								tag <<= 8;
								tag |= *ptemp++;

								if (tag == 0x49534F50)
								{
									pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

									pObjectLocation->componentId_tag = tag;
									pObjectLocation->component_data_length = *ptemp++;

									pObjectLocation->carouselId = *ptemp++;
									pObjectLocation->carouselId <<= 8;
									pObjectLocation->carouselId |= *ptemp++;
									pObjectLocation->carouselId <<= 8;
									pObjectLocation->carouselId |= *ptemp++;
									pObjectLocation->carouselId <<= 8;
									pObjectLocation->carouselId |= *ptemp++;

									pObjectLocation->moduleId = *ptemp++;
									pObjectLocation->moduleId <<= 8;
									pObjectLocation->moduleId |= *ptemp++;

									pObjectLocation->version.major = *ptemp++;
									pObjectLocation->version.minor = *ptemp++;

									pObjectLocation->objectKey_length = *ptemp++;
									assert(pObjectLocation->objectKey_length == 4);

									//									if (pBIOP_ObjectLocation->objectKey_length > 0)
									//									{
									//										assert(pBIOP_ObjectLocation->objectKey_length <= 4);
									//										memcpy(pBIOP_ObjectLocation->objectKey_data_byte, ptemp, pBIOP_ObjectLocation->objectKey_length);
									//										ptemp += pBIOP_ObjectLocation->objectKey_length;
									//									}

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
									pConnBinder->component_data_length = *ptemp++;
									pConnBinder->taps_count = *ptemp++;

									pConnBinder->Tap.id = *ptemp++;
									pConnBinder->Tap.id <<= 8;
									pConnBinder->Tap.id |= *ptemp++;
									pConnBinder->Tap.id <<= 8;

									pConnBinder->Tap.use = *ptemp++;
									pConnBinder->Tap.use <<= 8;
									pConnBinder->Tap.use |= *ptemp++;

									pConnBinder->Tap.association_tag = *ptemp++;
									pConnBinder->Tap.association_tag <<= 8;
									pConnBinder->Tap.association_tag |= *ptemp++;

									pConnBinder->Tap.selector_length = *ptemp++;

									pConnBinder->Tap.selector_type = *ptemp++;
									pConnBinder->Tap.selector_type <<= 8;
									pConnBinder->Tap.selector_type |= *ptemp++;

									pConnBinder->Tap.transactionId = *ptemp++;
									pConnBinder->Tap.transactionId <<= 8;
									pConnBinder->Tap.transactionId |= *ptemp++;
									pConnBinder->Tap.transactionId <<= 8;
									pConnBinder->Tap.transactionId |= *ptemp++;
									pConnBinder->Tap.transactionId <<= 8;
									pConnBinder->Tap.transactionId |= *ptemp++;

									pConnBinder->Tap.timeout = *ptemp++;
									pConnBinder->Tap.timeout <<= 8;
									pConnBinder->Tap.timeout |= *ptemp++;
									pConnBinder->Tap.timeout <<= 8;
									pConnBinder->Tap.timeout |= *ptemp++;
									pConnBinder->Tap.timeout <<= 8;
									pConnBinder->Tap.timeout |= *ptemp++;
								}
							}
						}
						else if (tag == 0x49534F05)			//TAG_LITE_OPTIONS(LiteOptionsProfileBody)
						{
							pLiteOptionsProfileBody = &(pIOR->taggedProfile[profile_index].u.LiteOptionsProfileBody);

							pLiteOptionsProfileBody->profileId_tag = tag;

							pLiteOptionsProfileBody->profile_data_length = *ptemp++;
							pLiteOptionsProfileBody->profile_data_length <<= 8;
							pLiteOptionsProfileBody->profile_data_length |= *ptemp++;
							pLiteOptionsProfileBody->profile_data_length <<= 8;
							pLiteOptionsProfileBody->profile_data_length |= *ptemp++;
							pLiteOptionsProfileBody->profile_data_length <<= 8;
							pLiteOptionsProfileBody->profile_data_length |= *ptemp++;

							pLiteOptionsProfileBody->profile_data_byte_order = *ptemp++;
						}

						remain_length = length - (int)(ptemp - buf);

						if (remain_length <= 0)
						{
							break;
						}
					}
				}
			}

			pBindings->objectInfo_length = *ptemp++;
			pBindings->objectInfo_length <<= 8;
			pBindings->objectInfo_length |= *ptemp++;

			copy_length = min(16, pBindings->objectInfo_length);
			if (copy_length > 0)
			{
				memcpy(pBindings->objectInfo_data_byte, ptemp, copy_length);
				ptemp += pBindings->objectInfo_length;
			}

			remain_length = length - (int)(ptemp - buf);
		}

		remain_length = length - (int)(ptemp - buf);
		assert(remain_length == 0);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DecodeFileMessage(uint8_t *buf, int length, FileMessage_t* pFileMessage)
{
	S32		rtcode = SECTION_PARSE_NO_ERROR;
	U8*		ptemp;
	S32		copy_length;
	S32		i;
	S32		remain_length;

	if ((buf != NULL) && (length > 0) && (pFileMessage != NULL))
	{
		memset(pFileMessage, 0x00, sizeof(FileMessage_t));
		ptemp = buf;

		memcpy(pFileMessage->magic, ptemp, 4);
		pFileMessage->magic[4] = '\0';
		ptemp += 4;

		pFileMessage->biop_version.major = *ptemp++;
		pFileMessage->biop_version.minor = *ptemp++;

		pFileMessage->byte_order = *ptemp++;
		pFileMessage->message_type = *ptemp++;

		pFileMessage->message_size = *ptemp++;
		pFileMessage->message_size <<= 8;
		pFileMessage->message_size |= *ptemp++;
		pFileMessage->message_size <<= 8;
		pFileMessage->message_size |= *ptemp++;
		pFileMessage->message_size <<= 8;
		pFileMessage->message_size |= *ptemp++;

		pFileMessage->objectKey_length = *ptemp++;
		assert(pFileMessage->objectKey_length == 4);

		pFileMessage->objectKey_data = *ptemp++;
		pFileMessage->objectKey_data <<= 8;
		pFileMessage->objectKey_data |= *ptemp++;
		pFileMessage->objectKey_data <<= 8;
		pFileMessage->objectKey_data |= *ptemp++;
		pFileMessage->objectKey_data <<= 8;
		pFileMessage->objectKey_data |= *ptemp++;

		pFileMessage->objectKind_length = *ptemp++;
		pFileMessage->objectKind_length <<= 8;
		pFileMessage->objectKind_length |= *ptemp++;
		pFileMessage->objectKind_length <<= 8;
		pFileMessage->objectKind_length |= *ptemp++;
		pFileMessage->objectKind_length <<= 8;
		pFileMessage->objectKind_length |= *ptemp++;

		assert(pFileMessage->objectKind_length == 4);
		memcpy(pFileMessage->objectKind_data, ptemp, 4);
		ptemp += 4;

		pFileMessage->objectInfo_length = *ptemp++;
		pFileMessage->objectInfo_length <<= 8;
		pFileMessage->objectInfo_length |= *ptemp++;

		assert(pFileMessage->objectInfo_length == 8);

		memcpy(pFileMessage->ContentSize, ptemp, 8);
		ptemp += 8;

		copy_length = min(1, pFileMessage->objectInfo_length - 8);
		if (copy_length > 0)
		{
			memcpy(pFileMessage->objectInfo_data_byte, ptemp, copy_length);
			ptemp += (pFileMessage->objectInfo_length - 8);
		}

		pFileMessage->serviceContextList_count = *ptemp++;
		assert(pFileMessage->serviceContextList_count <= 2);

		for (i = 0; i < pFileMessage->serviceContextList_count; i++)
		{
			pFileMessage->serviceContextList[i].context_id = *ptemp++;
			pFileMessage->serviceContextList[i].context_id <<= 8;
			pFileMessage->serviceContextList[i].context_id |= *ptemp++;
			pFileMessage->serviceContextList[i].context_id <<= 8;
			pFileMessage->serviceContextList[i].context_id |= *ptemp++;
			pFileMessage->serviceContextList[i].context_id <<= 8;
			pFileMessage->serviceContextList[i].context_id |= *ptemp++;

			pFileMessage->serviceContextList[i].context_data_length = *ptemp++;
			pFileMessage->serviceContextList[i].context_data_length <<= 8;
			pFileMessage->serviceContextList[i].context_data_length |= *ptemp++;

			copy_length = min(4, pFileMessage->serviceContextList[i].context_data_length);
			if (copy_length > 0)
			{
				memcpy(pFileMessage->serviceContextList[i].context_data_byte, ptemp, copy_length);
				ptemp += pFileMessage->serviceContextList[i].context_data_length;
			}
		}

		pFileMessage->messageBody_length = *ptemp++;
		pFileMessage->messageBody_length <<= 8;
		pFileMessage->messageBody_length |= *ptemp++;
		pFileMessage->messageBody_length <<= 8;
		pFileMessage->messageBody_length |= *ptemp++;
		pFileMessage->messageBody_length <<= 8;
		pFileMessage->messageBody_length |= *ptemp++;

		pFileMessage->content_length = *ptemp++;
		pFileMessage->content_length <<= 8;
		pFileMessage->content_length |= *ptemp++;
		pFileMessage->content_length <<= 8;
		pFileMessage->content_length |= *ptemp++;
		pFileMessage->content_length <<= 8;
		pFileMessage->content_length |= *ptemp++;

		pFileMessage->content_data_byte = (uint8_t*)malloc(pFileMessage->content_length);
		memcpy(pFileMessage->content_data_byte, ptemp, pFileMessage->content_length);
		ptemp += pFileMessage->content_length;

		remain_length = length - (int)(ptemp - buf);
		assert(remain_length == 0);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}
