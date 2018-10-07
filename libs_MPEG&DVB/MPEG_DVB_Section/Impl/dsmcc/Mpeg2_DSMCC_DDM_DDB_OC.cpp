#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "../../Include/MPEG_DVB_ErrorCode.h"
#include "../../Include/dsmcc/Mpeg2_DSMCC_DDM_DDB_OC.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"

//DDM
int	MPEG2_DSMCC_DDM_DecodeDirectoryMessage(uint8_t *buf, int length, DirectoryMessage_t* pDirectoryMessage)
{
	int				rtcode = SECTION_PARSE_NO_ERROR;
	uint8_t*		ptemp;
	int				copy_length;
	int				i;
	int				j;
	int				binding_index;
	int				remain_length;

	IOP::IOR_t*					pIOR;
	Bindings_t*					pBindings;

	if ((buf != NULL) && (length > 0) && (pDirectoryMessage != NULL))
	{
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

		copy_length = MIN(sizeof(pDirectoryMessage->objectInfo_data_byte), pDirectoryMessage->objectInfo_length);
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

			copy_length = MIN(4, pDirectoryMessage->serviceContextList[i].context_data_length);
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
				copy_length = MIN(64, pBindings->Name.id_length[j]);
				if (copy_length > 0)
				{
					memcpy(pBindings->Name.id_data_byte[j], ptemp, copy_length);
					pBindings->Name.id_data_byte[j][copy_length] = '\0';

					ptemp += pBindings->Name.id_length[j];
				}

				pBindings->Name.kind_length[j] = *ptemp++;
				//				assert(pName->kind_length[j] <= 4);
				copy_length = MIN(4, pBindings->Name.kind_length[j]);
				if (copy_length > 0)
				{
					memcpy(pBindings->Name.kind_data_byte[j], ptemp, copy_length);
					ptemp += pBindings->Name.kind_length[j];
				}
			}

			pBindings->bindingType = *ptemp++;

			pIOR = &(pBindings->IOR);

			BITS_t bs;
			remain_length = length - (int)(ptemp - buf);
			BITS_map(&bs, ptemp, remain_length);

			MPEG2_DSMCC_IOP_DecodeIOR(&bs, pIOR);
			ptemp += pIOR->total_length;
			assert(ptemp == bs.p_cur);

			pBindings->objectInfo_length = *ptemp++;
			pBindings->objectInfo_length <<= 8;
			pBindings->objectInfo_length |= *ptemp++;

			copy_length = MIN(sizeof(pBindings->objectInfo_data_byte), pBindings->objectInfo_length);
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

		copy_length = MIN(1, pFileMessage->objectInfo_length - 8);
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

			copy_length = MIN(4, pFileMessage->serviceContextList[i].context_data_length);
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
