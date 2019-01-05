#ifndef __MPEG_DSMCC_DESCRIPTOR_H__
#define __MPEG_DSMCC_DESCRIPTOR_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MPEG2_DVB_SECTION_EXPORT
#       define MPEG2_DSMCC_DESCRIPTOR_LIB __declspec(dllexport)
#   elif defined(_MPEG2_DVB_SECTION_IMPORT)
#       define MPEG2_DSMCC_DESCRIPTOR_LIB __declspec(dllimport)
#   else
#       define MPEG2_DSMCC_DESCRIPTOR_LIB
#   endif
#elif __GNUC__ >= 4
#   define MPEG2_DSMCC_DESCRIPTOR_LIB __attribute__((visibility("default")))
#else
#   define MPEG2_DSMCC_DESCRIPTOR_LIB
#endif

#include <stdint.h>

//DSMCC 定义的描述子		EN 301 192
#define MPEG2_DSMCC_NAME_DESCRIPTOR									0x3002
#define MPEG2_DSMCC_LOCATION_DESCRIPTOR								0x3006

/*-----------------------DSM-CC---------------------------------------------------*/

/*DSM-CC	name_descriptor			EN301192			0x02*/
typedef struct _name_descriptor_s
{
	uint16_t	descriptor_tag;						//8
	uint8_t		descriptor_length;					//8

	char		text_char[255];							//8 x N

} name_descriptor_t, *pname_descriptor_t;

MPEG2_DSMCC_DESCRIPTOR_LIB int MPEG2_DSMCC_decode_name_descriptor(uint8_t* buf, int length, name_descriptor_t* pname_descriptor);

/*DSM-CC	location_descriptor			EN301192			0x06*/
typedef struct _location_descriptor_s
{
	uint16_t		descriptor_tag;						//8
	uint8_t			descriptor_length;					//8

	uint8_t		location_tag;							//8

} location_descriptor_t, *plocation_descriptor_t;

MPEG2_DSMCC_DESCRIPTOR_LIB int MPEG2_DSMCC_decode_location_descriptor(uint8_t* buf, int length, location_descriptor_t* plocation_descriptor);

#endif