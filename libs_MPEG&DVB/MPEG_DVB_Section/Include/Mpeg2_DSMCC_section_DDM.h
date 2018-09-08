#ifndef _MPEG_DSMCC_SECTION_DDM_H_
#define _MPEG_DSMCC_SECTION_DDM_H_

#include "../compile.h"
#include "Mpeg2_DSMCC_section.h"
#include "Mpeg2_DSMCC_section_UNM.h"

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/
typedef struct _Bindings_s
{
	BIOP::Name_t	Name;
	uint8_t			bindingType;							//8
	IOP::IOR_t		IOR;

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

	uint8_t						serviceContextList_count;					//8
	serviceContextList_t	serviceContextList[2];

	uint32_t			messageBody_length;							//32
	uint16_t			bindings_count;								//16

	Bindings_t	bindings[1024];

} DirectoryMessage_t;

typedef struct _FileMessage_s
{
	S8			magic[5];									//4 x 8				0x42494F50       "BIOP"

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
	S8				objectKind_data[4];							//4x8				0x64697200		dir

	uint16_t		objectInfo_length;							//16
	uint8_t			ContentSize[8];								//64
	uint8_t			objectInfo_data_byte[1];					//8 x N

	uint8_t					serviceContextList_count;					//8
	serviceContextList_t	serviceContextList[2];

	uint32_t			messageBody_length;							//32
	uint32_t			content_length;								//32
	uint8_t*			content_data_byte;

} FileMessage_t;

//DDB的封装格式只有一种，DDB的净荷可能有多种情况
typedef struct DownloadDataBlock_s
{
	uint16_t		moduleId;								//16
	uint8_t			moduleVersion;							//8
	uint8_t			reserved;								//8
	uint16_t		blockNumber;							//16

	uint16_t		N;
	uint8_t*		blockDataByte;

} DownloadDataBlock_t, *pDownloadDataBlock_t;

typedef struct DownloadDataRequest_s
{
	uint8_t		reserved;
} DownloadDataRequest_t, *pDownloadDataRequest_t;

typedef struct dsmccDownloadDataHeader_s
{
	uint8_t			protocolDiscriminator;					//8
	uint8_t			dsmccType;								//8
	uint16_t		messageId;								//16
	uint32_t		downloadId;								//32

	uint8_t			reserved;								//8
	uint8_t			adaptationLength;						//8
	uint16_t		messageLength;							//16

	dsmccAdaptationHeader_t		dsmccAdaptationHeader;

} dsmccDownloadDataHeader_t;

typedef struct DSMCC_DDM_section_s
{
	uint8_t		table_id;									//8

	uint8_t		section_syntax_indicator;					//1
	uint8_t		private_indicator;							//1
	uint8_t		reserved0;									//2
	uint16_t	dsmcc_section_length;						//12
	
	uint16_t	table_id_extension;							//16

	uint8_t		reserved1;									//2
	uint8_t		version_number;								//5
	uint8_t		current_next_indicator;						//1

	uint8_t		section_number;								//8
	uint8_t		last_section_number;						//8

	dsmccDownloadDataHeader_t		dsmccDownloadDataHeader;
	DownloadDataBlock_t				DownloadDataBlock;

	uint32_t		CRC_32;										//32
	uint32_t		CRC_32_verify;								//32

} dsmcc_ddm_section_t;

_CDL_EXPORT	int	MPEG2_DSMCC_DDM_DecodeSection(uint8_t *buf, int length, dsmcc_ddm_section_t* pDSMCCSection);

_CDL_EXPORT	int	MPEG2_DSMCC_DecodeDirectoryMessage(uint8_t *buf, int length, DirectoryMessage_t* pDirectoryMessage);
_CDL_EXPORT	int	MPEG2_DSMCC_DecodeFileMessage(uint8_t *buf, int length, FileMessage_t* pFileMessage);

#endif
