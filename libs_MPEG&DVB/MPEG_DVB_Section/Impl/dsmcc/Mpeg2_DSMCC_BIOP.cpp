#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
/////////////////////////////////////////////

//I don't known the length of IOR befor parsing, this may cause some trouble   chendelin 2018.10.8
int	MPEG2_DSMCC_BIOP_DecodeIOR(BITS_t* pbs, BIOP::IOR_t* pIOR)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;

	if ((pbs != NULL) && (pIOR != NULL))
	{
		//½âÎöIOP::IOR()
		pIOR->type_id_length = BITS_get(pbs, 32);
		pIOR->total_length = 4;
		assert(pIOR->type_id_length == 4);
		
		{
			BITS_byteCopy(pIOR->type_id_byte, sizeof(pIOR->type_id_byte), pbs, pIOR->type_id_length);
			pIOR->total_length += pIOR->type_id_length;

			if ((pIOR->type_id_length % 4) != 0)
			{
				for (int i = 0; i < (int)(4 - (pIOR->type_id_length % 4)); i++)
				{
					pIOR->alignment_gap[i] = BITS_get(pbs, 8);
					pIOR->total_length++;
				}
			}

			pIOR->taggedProfiles_count = BITS_get(pbs, 32);
			pIOR->total_length += 4;

			if (pIOR->taggedProfiles_count > 0)
			{
				assert(pIOR->taggedProfiles_count <= 4);

				pIOR->taggedProfilesLength = 0;
				for (int profile_index = 0; profile_index < pIOR->taggedProfiles_count; profile_index++)
				{
					BIOP::TaggedProfile_t* pTaggedProfile = pIOR->taggedProfiles + profile_index;

					pTaggedProfile->profileId_tag = BITS_get(pbs, 32);
					pTaggedProfile->profile_data_length = BITS_get(pbs, 32);

					pTaggedProfile->profile_data_byte = pbs->p_cur;
					BITS_byteSkip(pbs, pTaggedProfile->profile_data_length);

					pIOR->taggedProfilesLength += (8 + pTaggedProfile->profile_data_length);

					if (pTaggedProfile->profileId_tag == 0x49534F06)
					{
						MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody(pTaggedProfile->profileId_tag, pTaggedProfile->profile_data_length, pTaggedProfile->profile_data_byte, &(pIOR->BIOPProfileBody));
					}
				}
			}

			pIOR->total_length += pIOR->taggedProfilesLength;
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, BIOP::BIOPProfileBody_t* pBIOPProfileBody)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	assert(profileId_tag == 0x49534F06);					//TAG_BIOP(BIOP Profile Body)

	if ((profile_data_byte != NULL) && (profile_data_length > 0) && (pBIOPProfileBody != NULL))
	{
		pBIOPProfileBody->profileId_tag = profileId_tag;

		pBIOPProfileBody->profile_data_length = profile_data_length;

		BITS_t bs;
		BITS_map(&bs, profile_data_byte, profile_data_length);

		pBIOPProfileBody->profile_data_byte_order = BITS_get(&bs, 8);
		pBIOPProfileBody->liteComponents_count = BITS_get(&bs, 8);

		//BIOP Object Location
		BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

		pObjectLocation->componentId_tag = BITS_get(&bs, 32);
		assert(pObjectLocation->componentId_tag == 0x49534F50);

		pObjectLocation->component_data_length = BITS_get(&bs, 8);

		pObjectLocation->carouselId = BITS_get(&bs, 32);

		pObjectLocation->moduleId = BITS_get(&bs, 16);

		pObjectLocation->version.major = BITS_get(&bs, 8);
		pObjectLocation->version.minor = BITS_get(&bs, 8);

		pObjectLocation->objectKey_length = BITS_get(&bs, 8);
		assert(pObjectLocation->objectKey_length == 4);

		pObjectLocation->objectKey_data = BITS_get(&bs, 32);

		//DSM Conn Binder
		BIOP::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);

		pConnBinder->componentId_tag = BITS_get(&bs, 32);
		assert(pConnBinder->componentId_tag == 0x49534F40);

		pConnBinder->component_data_length = BITS_get(&bs, 8);
		pConnBinder->taps_count = BITS_get(&bs, 8);

		for (int tap_index = 0; tap_index < pConnBinder->taps_count; tap_index++)
		{
			pConnBinder->Tap[tap_index].id = BITS_get(&bs, 16);

			pConnBinder->Tap[tap_index].use = BITS_get(&bs, 16);

			pConnBinder->Tap[tap_index].association_tag = BITS_get(&bs, 16);

			pConnBinder->Tap[tap_index].selector_length = BITS_get(&bs, 8);

			pConnBinder->Tap[tap_index].selector_type = BITS_get(&bs, 16);

			pConnBinder->Tap[tap_index].transactionId = BITS_get(&bs, 32);

			pConnBinder->Tap[tap_index].timeout = BITS_get(&bs, 32);
		}

		//normal components
		for (int component_index = 0; component_index < pBIOPProfileBody->liteComponents_count - 2; component_index++)
		{
			BIOP::LiteComponent_t* pLiteComponent = pBIOPProfileBody->liteComponents + component_index;

			pLiteComponent->componentId_tag = BITS_get(&bs, 32);

			pLiteComponent->component_data_length = BITS_get(&bs, 8);

			BITS_byteCopy(pLiteComponent->component_data_byte, sizeof(pLiteComponent->component_data_byte), &bs, pLiteComponent->component_data_length);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_BIOP_DecodeLiteOptionsProfileBody(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, BIOP::LiteOptionsProfileBody_t* pLiteOptionsProfileBody)
{
	int rtcode = SECTION_PARSE_NO_ERROR;

	assert(profileId_tag == 0x49534F05);				//TAG_LITE_OPTION(LiteOptionsProfileBody)

	if ((profile_data_byte != NULL) && (profile_data_length > 0) && (pLiteOptionsProfileBody != NULL))
	{
		pLiteOptionsProfileBody->profileId_tag = profileId_tag;

		pLiteOptionsProfileBody->profile_data_length = profile_data_length;

		BITS_t bs;
		BITS_map(&bs, profile_data_byte, profile_data_length);

		pLiteOptionsProfileBody->profile_data_byte_order = BITS_get(&bs, 8);
		pLiteOptionsProfileBody->liteComponents_count = BITS_get(&bs, 8);

		for (int component_index = 0; component_index < pLiteOptionsProfileBody->liteComponents_count; component_index++)
		{
			BIOP::LiteComponent_t* pLiteComponent = pLiteOptionsProfileBody->liteComponents + component_index;

			pLiteComponent->componentId_tag = BITS_get(&bs, 32);

			pLiteComponent->component_data_length = BITS_get(&bs, 8);

			BITS_byteCopy(pLiteComponent->component_data_byte, sizeof(pLiteComponent->component_data_byte), &bs, pLiteComponent->component_data_length);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}


//DDM
int	MPEG2_DSMCC_BIOP_DecodeDirectoryMessage(uint8_t *msg_buf, int msg_length, BIOP::DirectoryMessage_t* pDirectoryMessage)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;

	if ((msg_buf != NULL) && (msg_length > 0) && (pDirectoryMessage != NULL))
	{
		memset(pDirectoryMessage, 0x00, sizeof(BIOP::DirectoryMessage_t));

		BITS_t bs;
		BITS_map(&bs, msg_buf, msg_length);

		BITS_byteCopy(pDirectoryMessage->magic, sizeof(pDirectoryMessage->magic), &bs, 4);
		pDirectoryMessage->magic[4] = '\0';

		pDirectoryMessage->biop_version.major = BITS_get(&bs, 8);
		pDirectoryMessage->biop_version.minor = BITS_get(&bs, 8);

		pDirectoryMessage->byte_order = BITS_get(&bs, 8);
		pDirectoryMessage->message_type = BITS_get(&bs, 8);

		pDirectoryMessage->message_size = BITS_get(&bs, 32);

		pDirectoryMessage->objectKey_length = BITS_get(&bs, 8);
		assert(pDirectoryMessage->objectKey_length == 4);

		pDirectoryMessage->objectKey_data = BITS_get(&bs, 32);

		pDirectoryMessage->objectKind_length = BITS_get(&bs, 32);
		assert(pDirectoryMessage->objectKind_length == 4);

		BITS_byteCopy(pDirectoryMessage->objectKind_data, sizeof(pDirectoryMessage->objectKind_data), &bs, pDirectoryMessage->objectKind_length);

		pDirectoryMessage->objectInfo_length = BITS_get(&bs, 16);

		BITS_byteCopy(pDirectoryMessage->objectInfo_data_byte, sizeof(pDirectoryMessage->objectInfo_data_byte), &bs, pDirectoryMessage->objectInfo_length);

		pDirectoryMessage->serviceContextList_count = BITS_get(&bs, 8);
		assert(pDirectoryMessage->serviceContextList_count <= 2);

		for (int context_index = 0; context_index < pDirectoryMessage->serviceContextList_count; context_index++)
		{
			serviceContextList_t* pserviceContext = pDirectoryMessage->serviceContextList + context_index;
			pserviceContext->context_id = BITS_get(&bs, 32);

			pserviceContext->context_data_length = BITS_get(&bs, 16);

			BITS_byteCopy(pserviceContext->context_data_byte, sizeof(pserviceContext->context_data_byte), &bs, pserviceContext->context_data_length);
		}

		pDirectoryMessage->messageBody_length = BITS_get(&bs, 32);

		pDirectoryMessage->bindings_count = BITS_get(&bs, 16);

		assert(pDirectoryMessage->bindings_count <= 1024);
		for (int binding_index = 0; binding_index < pDirectoryMessage->bindings_count; binding_index++)
		{
			BIOP::Binding_t* pstBinding = pDirectoryMessage->bindings + binding_index;

			pstBinding->Name.nameComponents_count = BITS_get(&bs, 8);
			assert(pstBinding->Name.nameComponents_count <= 1);

			for (int component_index = 0; component_index < pstBinding->Name.nameComponents_count; component_index++)
			{
				pstBinding->Name.nameComponents[component_index].id_length = BITS_get(&bs, 8);
				BITS_byteCopy(pstBinding->Name.nameComponents[component_index].id_data_byte, sizeof(pstBinding->Name.nameComponents[component_index].id_data_byte), &bs, pstBinding->Name.nameComponents[component_index].id_length);

				pstBinding->Name.nameComponents[component_index].kind_length = BITS_get(&bs, 8);
				BITS_byteCopy(pstBinding->Name.nameComponents[component_index].kind_data_byte, sizeof(pstBinding->Name.nameComponents[component_index].kind_data_byte), &bs, pstBinding->Name.nameComponents[component_index].kind_length);
			}

			pstBinding->bindingType = BITS_get(&bs, 8);

			BIOP::IOR_t* pIOR = &(pstBinding->IOR);
			MPEG2_DSMCC_BIOP_DecodeIOR(&bs, pIOR);

			pstBinding->objectInfo_length = BITS_get(&bs, 16);
			BITS_byteCopy(pstBinding->objectInfo_data_byte, sizeof(pstBinding->objectInfo_data_byte), &bs, pstBinding->objectInfo_length);
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

int	MPEG2_DSMCC_BIOP_DecodeFileMessage(uint8_t *msg_buf, int msg_length, BIOP::FileMessage_t* pFileMessage)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;

	if ((msg_buf != NULL) && (msg_length > 0) && (pFileMessage != NULL))
	{
		memset(pFileMessage, 0x00, sizeof(BIOP::FileMessage_t));

		BITS_t bs;
		BITS_map(&bs, msg_buf, msg_length);

		BITS_byteCopy(pFileMessage->magic, sizeof(pFileMessage->magic), &bs, 4);
		pFileMessage->magic[4] = '\0';

		pFileMessage->biop_version.major = BITS_get(&bs, 8);
		pFileMessage->biop_version.minor = BITS_get(&bs, 8);

		pFileMessage->byte_order = BITS_get(&bs, 8);
		pFileMessage->message_type = BITS_get(&bs, 8);

		pFileMessage->message_size = BITS_get(&bs, 32);

		pFileMessage->objectKey_length = BITS_get(&bs, 8);
		assert(pFileMessage->objectKey_length == 4);

		pFileMessage->objectKey_data = BITS_get(&bs, 32);

		pFileMessage->objectKind_length = BITS_get(&bs, 32);
		assert(pFileMessage->objectKind_length == 4);

		BITS_byteCopy(pFileMessage->objectKind_data, sizeof(pFileMessage->objectKind_data), &bs, pFileMessage->objectKind_length);

		pFileMessage->objectInfo_length = BITS_get(&bs, 16);
		assert(pFileMessage->objectInfo_length == 8);

		BITS_byteCopy(pFileMessage->ContentSize, sizeof(pFileMessage->ContentSize), &bs, 8);

		int remain_length = pFileMessage->objectInfo_length - 8;
		if (remain_length > 0)
		{
			BITS_byteCopy(pFileMessage->objectInfo_data_byte, sizeof(pFileMessage->objectInfo_data_byte), &bs, remain_length);
		}

		pFileMessage->serviceContextList_count = BITS_get(&bs, 8);
		assert(pFileMessage->serviceContextList_count <= 2);

		for (int context_index = 0; context_index < pFileMessage->serviceContextList_count; context_index++)
		{
			serviceContextList_t* pserviceContext = pFileMessage->serviceContextList + context_index;

			pserviceContext->context_id = BITS_get(&bs, 32);

			pserviceContext->context_data_length = BITS_get(&bs, 16);

			BITS_byteCopy(pserviceContext->context_data_byte, sizeof(pserviceContext->context_data_byte), &bs, pserviceContext->context_data_length);
		}

		pFileMessage->messageBody_length = BITS_get(&bs, 32);

		pFileMessage->content_length = BITS_get(&bs, 32);

		pFileMessage->content_data_byte = (uint8_t*)malloc(pFileMessage->content_length);
		BITS_byteCopy(pFileMessage->content_data_byte, pFileMessage->content_length, &bs, pFileMessage->content_length);
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}

