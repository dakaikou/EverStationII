#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../Include/dsmcc/Mpeg2_DSMCC_BIOP.h"
#include "../../Include/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
/////////////////////////////////////////////

//I don't known the length of IOR befor parsing, this may cause some trouble   chendelin 2018.10.8
int	MPEG2_DSMCC_IOP_DecodeIOR(BITS_t* pbs, IOP::IOR_t* pIOR)
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
					IOP::TaggedProfile_t* pTaggedProfile = pIOR->taggedProfiles + profile_index;

					pTaggedProfile->profileId_tag = BITS_get(pbs, 32);
					pTaggedProfile->profile_data_length = BITS_get(pbs, 32);

					pTaggedProfile->profile_data_byte = pbs->p_cur;
					BITS_byteSkip(pbs, pTaggedProfile->profile_data_length);

					pIOR->taggedProfilesLength += (8 + pTaggedProfile->profile_data_length);

					if (pTaggedProfile->profileId_tag == 0x49534F06)
					{
						MPEG2_DSMCC_IOP_DecodeBIOPProfileBody(pTaggedProfile->profileId_tag, pTaggedProfile->profile_data_length, pTaggedProfile->profile_data_byte, &(pIOR->BIOPProfileBody));
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

int	MPEG2_DSMCC_IOP_DecodeBIOPProfileBody(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, BIOPProfileBody_t* pBIOPProfileBody)
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
		DSM::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);

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

int	MPEG2_DSMCC_IOP_DecodeLiteOptionsProfileBody(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, LiteOptionsProfileBody_t* pLiteOptionsProfileBody)
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



