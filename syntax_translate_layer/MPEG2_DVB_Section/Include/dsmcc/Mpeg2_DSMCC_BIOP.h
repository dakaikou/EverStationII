#ifndef _MPEG_DSMCC_BIOP_H_
#define _MPEG_DSMCC_BIOP_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define DSMCC_BIOP_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define DSMCC_BIOP_LIB __declspec(dllimport)
#   else
#       define DSMCC_BIOP_LIB
#   endif
#elif __GNUC__ >= 4
#   define DSMCC_BIOP_LIB __attribute__((visibility("default")))
#else
#   define DSMCC_BIOP_LIB
#endif

#include <stdint.h>

#include "../../../Common/BitStream.h"
#include "Mpeg2_DSMCC_define.h"

/*------------------------------------------------------------
		DSM-CC definition 
-------------------------------------------------------------*/

namespace BIOP
{
	typedef struct _TAP_s
	{
		uint16_t		id;									//16
		uint16_t		use;								//16
		uint16_t		association_tag;					//16
		uint8_t			selector_length;					//8
		uint16_t		selector_type;						//16
		uint32_t		transactionId;						//32
		uint32_t		timeout;							//32

	} TAP_t;

	typedef struct _Name_s
	{
		uint8_t			nameComponents_count;						//8

		struct
		{
			uint8_t			id_length;								//8		标准中规定是1			
			char			id_data_byte[65];						//
			uint8_t			kind_length;
			char			kind_data_byte[4];

		} nameComponents[1];
	} Name_t;

	typedef struct _ObjectLocation_s
	{
		uint32_t	componentId_tag;					//32
		uint8_t		component_data_length;				//8
		uint32_t	carouselId;							//32
		uint16_t	moduleId;							//16
		struct
		{
			uint8_t			major;							//8
			uint8_t			minor;							//8

		} version;

		uint8_t			objectKey_length;					//8
		uint32_t		objectKey_data;

	} ObjectLocation_t;

	typedef struct _ConnBinder_s
	{
		uint32_t	componentId_tag;					//32
		int			component_data_length;				//8
		uint8_t		taps_count;							//8
		TAP_t		Tap[16];							//为什么是16？  chendelin 2018.10.4

	} ConnBinder_t;

	typedef struct _LiteComponent_s
	{
		uint32_t	componentId_tag;					//32
		uint8_t		component_data_length;				//8			N6
		uint8_t		component_data_byte[256];			// N6
	} LiteComponent_t;

	typedef struct
	{
		uint32_t		profileId_tag;						//32
		int				profile_data_length;				//32
		uint8_t*		profile_data_byte;

	} TaggedProfile_t;

	typedef struct _BIOPProfileBody_s
	{
		uint32_t					profileId_tag;						//32
		int							profile_data_length;				//32
		uint8_t						profile_data_byte_order;			//8
		uint8_t						liteComponents_count;				//8

		ObjectLocation_t		ObjectLocation;						//liteComponent 0
		ConnBinder_t			ConnBinder;							//liteComponent 1
		LiteComponent_t		liteComponents[1];					//liteComponent 2 ~ N, 数组长度liteComponents_count - 2, 数组长度为什么是16？  chendelin  2018.10.4

	} BIOPProfileBody_t;

	typedef struct _LiteOptionsProfileBody_s
	{
		uint32_t					profileId_tag;						//32
		int							profile_data_length;				//32
		uint8_t						profile_data_byte_order;			//8
		uint8_t						liteComponents_count;				//8

		LiteComponent_t			liteComponents[1];					//数组长度为什么是16？  chendelin  2018.10.4

	} LiteOptionsProfileBody_t;

	typedef struct _IOR_s
	{
		uint32_t				type_id_length;						//32
		char					type_id_byte[4];					//8 x N1
		uint8_t					alignment_gap[4];					//8 x ???

		int					taggedProfiles_count;				//32
		TaggedProfile_t		taggedProfiles[4];					//array count why 4?	chendelin 2018.10.6

		int					taggedProfilesLength;				//just for check chendelin  2018.10.6
		int					total_length;					//just for check chendelin  2018.10.6

		BIOPProfileBody_t	BIOPProfileBody;

	} IOR_t;

	typedef struct _Bindings_s
	{
		Name_t			Name;
		uint8_t			bindingType;							//8
		IOR_t			IOR;

		int			objectInfo_length;						//16
		uint8_t		objectInfo_data_byte[16];

	} Binding_t;

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
																	//	uint8_t			objectKey_data_byte[4];						//8 x N				//DVB规定长度小于等于4个字节
		uint32_t		objectKey_data;

		uint32_t		objectKind_length;							//32				0x00000004
		char			objectKind_data[4];							//4x8				0x64697200		dir

		uint16_t		objectInfo_length;							//16
		uint8_t			objectInfo_data_byte[8];					//8 x N

		uint8_t					serviceContextList_count;					//8
		serviceContextList_t	serviceContextList[2];

		uint32_t			messageBody_length;							//32
		uint16_t			bindings_count;								//16
		Binding_t			bindings[1024];

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

	typedef struct _ModuleInfo_s
	{
		uint32_t		moduleTimeOut;				//32
		uint32_t		blockTimeOut;				//32
		uint32_t		minBlockTime;				//32

		uint8_t			taps_count;					//8
		TAP_t			TAP[4];

		uint8_t			userInfoLength;				//8
		uint8_t*		userInfo_data_byte;			//8

	} ModuleInfo_t;

};


DSMCC_BIOP_LIB int	MPEG2_DSMCC_BIOP_DecodeIOR(BITS_t* pbs, BIOP::IOR_t* pIOR);
DSMCC_BIOP_LIB int	MPEG2_DSMCC_BIOP_DecodeBIOPProfileBody(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, BIOP::BIOPProfileBody_t* pBIOPProfileBody);
DSMCC_BIOP_LIB int	MPEG2_DSMCC_BIOP_DecodeLiteOptionsProfileBody(uint32_t profileId_tag, uint32_t profile_data_length, uint8_t* profile_data_byte, BIOP::LiteOptionsProfileBody_t* pLiteOptionsProfileBody);

DSMCC_BIOP_LIB	int	MPEG2_DSMCC_BIOP_DecodeDirectoryMessage(uint8_t *msg_buf, int msg_length, BIOP::DirectoryMessage_t* pDirectoryMessage);
DSMCC_BIOP_LIB	int	MPEG2_DSMCC_BIOP_DecodeFileMessage(uint8_t *msg_buf, int msg_length, BIOP::FileMessage_t* pFileMessage);

#endif
