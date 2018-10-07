#ifndef _MPEG_DSMCC_DDM_DDB_OC_H_
#define _MPEG_DSMCC_DDM_DDB_OC_H_

#include "../../compile.h"
#include "Mpeg2_DSMCC_BIOP.h"

typedef struct _Bindings_s
{
	BIOP::Name_t		Name;
	uint8_t				bindingType;							//8
	IOP::IOR_t			IOR;

	int			objectInfo_length;						//16
	uint8_t		objectInfo_data_byte[16];

} Bindings_t;

typedef struct serviceContextList_s
{
	uint32_t	context_id;									//32
	int			context_data_length;						//16
	uint8_t		context_data_byte[4];						//8 x N
} serviceContextList_t;

typedef struct _DirectoryMessage_s
{
	char		magic[5];									//4 x 8				0x42494F50       "BIOP"

	struct
	{
		uint8_t		major;										//8					0x01
		uint8_t		minor;										//8					0x00
	}biop_version;

	uint8_t			byte_order;									//8					0x00
	uint8_t			message_type;								//8					0x00

	uint32_t		message_size;								//32

	int				objectKey_length;							//8
															//	U8			objectKey_data_byte[4];						//8 x N				//DVB规定长度小于等于4个字节
	uint32_t		objectKey_data;

	uint32_t		objectKind_length;							//32				0x00000004
	char			objectKind_data[4];							//4x8				0x64697200		dir

	uint16_t		objectInfo_length;							//16
	uint8_t			objectInfo_data_byte[8];					//8 x N

	uint8_t					serviceContextList_count;					//8
	serviceContextList_t	serviceContextList[2];

	uint32_t			messageBody_length;							//32
	uint16_t			bindings_count;								//16

	Bindings_t	bindings[1024];

} DirectoryMessage_t;

typedef struct _FileMessage_s
{
	char	magic[5];									//4 x 8				0x42494F50       "BIOP"

	struct
	{
		uint8_t		major;										//8					0x01
		uint8_t		minor;										//8					0x00
	}biop_version;

	uint8_t			byte_order;									//8					0x00
	uint8_t			message_type;								//8					0x00

	uint32_t		message_size;								//32

	uint8_t			objectKey_length;							//8
															//	U8			objectKey_data_byte[4];						//8 x N				//DVB规定长度小于等于4个字节
	uint32_t		objectKey_data;

	uint32_t		objectKind_length;							//32				0x00000004
	char			objectKind_data[4];							//4x8				0x64697200		dir

	uint16_t		objectInfo_length;							//16
	uint8_t			ContentSize[8];								//64
	uint8_t			objectInfo_data_byte[1];					//8 x N

	uint8_t					serviceContextList_count;					//8
	serviceContextList_t	serviceContextList[2];

	uint32_t			messageBody_length;							//32
	uint32_t			content_length;								//32
	uint8_t*			content_data_byte;

} FileMessage_t;

_CDL_EXPORT	int	MPEG2_DSMCC_DDM_DecodeDirectoryMessage(uint8_t *buf, int length, DirectoryMessage_t* pDirectoryMessage);
_CDL_EXPORT	int	MPEG2_DSMCC_DDM_DecodeFileMessage(uint8_t *buf, int length, FileMessage_t* pFileMessage);

#endif
