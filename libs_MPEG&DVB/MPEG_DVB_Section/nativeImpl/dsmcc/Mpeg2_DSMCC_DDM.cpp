#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../nativeInclude/MPEG_DVB_ErrorCode.h"
#include "../../nativeInclude/dsmcc/Mpeg2_DSMCC_DDM.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DDM_DecodeDirectoryMessage(uint8_t *buf, int length, DirectoryMessage_t* pDirectoryMessage)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;
	int				copy_length;
	int				i;
	int				j;
	int				profile_index;
	int				component_index;
	int				binding_index;
	int				remain_length;
	uint32_t		tag;

	BIOPProfileBody_t*			pBIOPProfileBody;
	LiteOptionsProfileBody_t*	pLiteOptionsProfileBody;
	IOP::IOR_t*					pIOR;
	BIOP::ObjectLocation_t*		pObjectLocation;
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

		copy_length = min(sizeof(pDirectoryMessage->objectInfo_data_byte), pDirectoryMessage->objectInfo_length);
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
					for (profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
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

							//BIOP Object Location
							pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

							pObjectLocation->componentId_tag = *ptemp++;
							pObjectLocation->componentId_tag <<= 8;
							pObjectLocation->componentId_tag |= *ptemp++;
							pObjectLocation->componentId_tag <<= 8;
							pObjectLocation->componentId_tag |= *ptemp++;
							pObjectLocation->componentId_tag <<= 8;
							pObjectLocation->componentId_tag |= *ptemp++;
							assert(pObjectLocation->componentId_tag == 0x49534F50);

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

							pObjectLocation->objectKey_data = *ptemp++;
							pObjectLocation->objectKey_data <<= 8;
							pObjectLocation->objectKey_data |= *ptemp++;
							pObjectLocation->objectKey_data <<= 8;
							pObjectLocation->objectKey_data |= *ptemp++;
							pObjectLocation->objectKey_data <<= 8;
							pObjectLocation->objectKey_data |= *ptemp++;

							//DSM Conn Binder
							pConnBinder = &(pBIOPProfileBody->ConnBinder);

							pConnBinder->componentId_tag = *ptemp++;
							pConnBinder->componentId_tag <<= 8;
							pConnBinder->componentId_tag |= *ptemp++;
							pConnBinder->componentId_tag <<= 8;
							pConnBinder->componentId_tag |= *ptemp++;
							pConnBinder->componentId_tag <<= 8;
							pConnBinder->componentId_tag |= *ptemp++;
							assert(pConnBinder->componentId_tag == 0x49534F40);

							pConnBinder->component_data_length = *ptemp++;
							pConnBinder->taps_count = *ptemp++;

							for (int tap_index = 0; tap_index < pConnBinder->taps_count; tap_index++)
							{
								pConnBinder->Tap[tap_index].id = *ptemp++;
								pConnBinder->Tap[tap_index].id <<= 8;
								pConnBinder->Tap[tap_index].id |= *ptemp++;
								pConnBinder->Tap[tap_index].id <<= 8;

								pConnBinder->Tap[tap_index].use = *ptemp++;
								pConnBinder->Tap[tap_index].use <<= 8;
								pConnBinder->Tap[tap_index].use |= *ptemp++;

								pConnBinder->Tap[tap_index].association_tag = *ptemp++;
								pConnBinder->Tap[tap_index].association_tag <<= 8;
								pConnBinder->Tap[tap_index].association_tag |= *ptemp++;

								pConnBinder->Tap[tap_index].selector_length = *ptemp++;

								pConnBinder->Tap[tap_index].selector_type = *ptemp++;
								pConnBinder->Tap[tap_index].selector_type <<= 8;
								pConnBinder->Tap[tap_index].selector_type |= *ptemp++;

								pConnBinder->Tap[tap_index].transactionId = *ptemp++;
								pConnBinder->Tap[tap_index].transactionId <<= 8;
								pConnBinder->Tap[tap_index].transactionId |= *ptemp++;
								pConnBinder->Tap[tap_index].transactionId <<= 8;
								pConnBinder->Tap[tap_index].transactionId |= *ptemp++;
								pConnBinder->Tap[tap_index].transactionId <<= 8;
								pConnBinder->Tap[tap_index].transactionId |= *ptemp++;

								pConnBinder->Tap[tap_index].timeout = *ptemp++;
								pConnBinder->Tap[tap_index].timeout <<= 8;
								pConnBinder->Tap[tap_index].timeout |= *ptemp++;
								pConnBinder->Tap[tap_index].timeout <<= 8;
								pConnBinder->Tap[tap_index].timeout |= *ptemp++;
								pConnBinder->Tap[tap_index].timeout <<= 8;
								pConnBinder->Tap[tap_index].timeout |= *ptemp++;
							}

							for (component_index = 0; component_index < pBIOPProfileBody->liteComponents_count - 2; component_index++)
							{
								BIOP::LiteComponent_t* pLiteComponent = pBIOPProfileBody->liteComponents + component_index;

								pLiteComponent->componentId_tag = *ptemp++;
								pLiteComponent->componentId_tag <<= 8;
								pLiteComponent->componentId_tag |= *ptemp++;
								pLiteComponent->componentId_tag <<= 8;
								pLiteComponent->componentId_tag |= *ptemp++;
								pLiteComponent->componentId_tag <<= 8;
								pLiteComponent->componentId_tag |= *ptemp++;

								pLiteComponent->component_data_length = *ptemp++;

								copy_length = min(pLiteComponent->component_data_length, sizeof(pLiteComponent->component_data_byte));
								memcpy(pLiteComponent->component_data_byte, ptemp, copy_length);
								ptemp += pLiteComponent->component_data_length;
							}

							pIOR->pBIOPProfileBodyPortrait = pBIOPProfileBody;
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

			copy_length = min(sizeof(pBindings->objectInfo_data_byte), pBindings->objectInfo_length);
			if (copy_length > 0)
			{
				memcpy(pBindings->objectInfo_data_byte, ptemp, copy_length);
				ptemp += pBindings->objectInfo_length;
			}

			//remain_length = length - (int)(ptemp - buf);
		}

		//remain_length = length - (int)(ptemp - buf);
		//assert(remain_length == 0);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_DDM_DecodeFileMessage(uint8_t *buf, int length, FileMessage_t* pFileMessage)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;
	int				copy_length;
	int				i;
	int				remain_length;

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
