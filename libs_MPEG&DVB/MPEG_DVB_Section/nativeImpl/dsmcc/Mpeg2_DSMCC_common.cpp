#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../../nativeInclude/dsmcc/Mpeg2_DSMCC_common.h"
#include "../../nativeInclude/MPEG_DVB_ErrorCode.h"

#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"

#ifndef min
#define min(a,b)  (((a)<(b))?(a):(b))
#endif
/////////////////////////////////////////////

int	MPEG2_DSMCC_DecodeIOR(BITS_t* pbs, IOP::IOR_t* pIOR)
{
	int			rtcode = SECTION_PARSE_NO_ERROR;

	if ((pbs != NULL) && (pIOR != NULL))
	{
		//½âÎöIOP::IOR()
		pIOR->type_id_length = BITS_get(pbs, 32);

		assert(pIOR->type_id_length == 4);
		
		{
			BITS_byteCopy(pIOR->type_id_byte, sizeof(pIOR->type_id_byte), pbs, pIOR->type_id_length);

			if ((pIOR->type_id_length % 4) != 0)
			{
				for (int i = 0; i < (int)(4 - (pIOR->type_id_length % 4)); i++)
				{
					pIOR->alignment_gap[i] = BITS_get(pbs, 8);
				}
			}

			pIOR->taggedProfiles_count = BITS_get(pbs, 32);

			if (pIOR->taggedProfiles_count > 0)
			{
				for (int profile_index = 0; profile_index < (int)(pIOR->taggedProfiles_count); profile_index++)
				{
					uint32_t tag = BITS_get(pbs, 32);

					pIOR->taggedProfile[profile_index].profileId_tag = tag;

					if (tag == 0x49534F06)					//TAG_BIOP(BIOP Profile Body)
					{
						BIOPProfileBody_t* pBIOPProfileBody = &(pIOR->taggedProfile[profile_index].u.BIOPProfileBody);

						pBIOPProfileBody->profileId_tag = tag;

						pBIOPProfileBody->profile_data_length = BITS_get(pbs, 32);

						pBIOPProfileBody->profile_data_byte_order = BITS_get(pbs, 8);
						pBIOPProfileBody->liteComponents_count = BITS_get(pbs, 8);

						//BIOP Object Location
						BIOP::ObjectLocation_t* pObjectLocation = &(pBIOPProfileBody->ObjectLocation);

						pObjectLocation->componentId_tag = BITS_get(pbs, 32);
						assert(pObjectLocation->componentId_tag == 0x49534F50);

						pObjectLocation->component_data_length = BITS_get(pbs, 8);

						pObjectLocation->carouselId = BITS_get(pbs, 32);

						pObjectLocation->moduleId = BITS_get(pbs, 16);

						pObjectLocation->version.major = BITS_get(pbs, 8);
						pObjectLocation->version.minor = BITS_get(pbs, 8);

						pObjectLocation->objectKey_length = BITS_get(pbs, 8);
						assert(pObjectLocation->objectKey_length == 4);

						pObjectLocation->objectKey_data = BITS_get(pbs, 32);

						//DSM Conn Binder
						DSM::ConnBinder_t* pConnBinder = &(pBIOPProfileBody->ConnBinder);

						pConnBinder->componentId_tag = BITS_get(pbs, 32);
						assert(pConnBinder->componentId_tag == 0x49534F40);

						pConnBinder->component_data_length = BITS_get(pbs, 8);
						pConnBinder->taps_count = BITS_get(pbs, 8);

						for (int tap_index = 0; tap_index < pConnBinder->taps_count; tap_index++)
						{
							pConnBinder->Tap[tap_index].id = BITS_get(pbs, 16);

							pConnBinder->Tap[tap_index].use = BITS_get(pbs, 16);

							pConnBinder->Tap[tap_index].association_tag = BITS_get(pbs, 16);

							pConnBinder->Tap[tap_index].selector_length = BITS_get(pbs, 8);

							pConnBinder->Tap[tap_index].selector_type = BITS_get(pbs, 16);

							pConnBinder->Tap[tap_index].transactionId = BITS_get(pbs, 32);

							pConnBinder->Tap[tap_index].timeout = BITS_get(pbs, 32);
						}

						//normal components
						for (int component_index = 0; component_index < pBIOPProfileBody->liteComponents_count - 2; component_index++)
						{
							BIOP::LiteComponent_t* pLiteComponent = pBIOPProfileBody->liteComponents + component_index;

							pLiteComponent->componentId_tag = BITS_get(pbs, 32);

							pLiteComponent->component_data_length = BITS_get(pbs, 8);

							BITS_byteCopy(pLiteComponent->component_data_byte, sizeof(pLiteComponent->component_data_byte), pbs, pLiteComponent->component_data_length);
						}

						pIOR->pBIOPProfileBodyPortrait = pBIOPProfileBody;

					}
					else if (tag == 0x49534F05)				//TAG_LITE_OPTION(LiteOptionsProfileBody)
					{
						//assert(0);

						LiteOptionsProfileBody_t* pLiteOptionsProfileBody = &(pIOR->taggedProfile[profile_index].u.LiteOptionsProfileBody);

						pLiteOptionsProfileBody->profileId_tag = tag;

						pLiteOptionsProfileBody->profile_data_length = BITS_get(pbs, 32);
						pLiteOptionsProfileBody->profile_data_byte_order = BITS_get(pbs, 8);
						pLiteOptionsProfileBody->liteComponents_count = BITS_get(pbs, 8);

						for (int component_index = 0; component_index < pLiteOptionsProfileBody->liteComponents_count; component_index++)
						{
							BIOP::LiteComponent_t* pLiteComponent = pLiteOptionsProfileBody->liteComponents + component_index;

							pLiteComponent->componentId_tag = BITS_get(pbs, 32);

							pLiteComponent->component_data_length = BITS_get(pbs, 8);

							BITS_byteCopy(pLiteComponent->component_data_byte, sizeof(pLiteComponent->component_data_byte), pbs, pLiteComponent->component_data_length);
						}
					}
					else
					{
						assert(0);
					}
				}
			}
		}
	}
	else
	{
		rtcode = SECTION_PARSE_PARAMETER_ERROR;
	}

	return rtcode;
}




