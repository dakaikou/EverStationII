#ifndef _MPEG_DVB_ERROR_H_
#define _MPEG_DVB_ERROR_H_

#ifdef SECTION_PARSE_NO_ERROR
#undef SECTION_PARSE_NO_ERROR
#endif

#define SECTION_PARSE_NO_ERROR					0x00000000

//section�����������
#define SECTION_PARSE_PARAMETER_ERROR		0xE1030001
#define SECTION_PARSE_CRC_ERROR				0xE1030002
#define SECTION_PARSE_SYNTAX_ERROR			0xE1030003
//#define SECTION_PARSE_VERSION_CHANGE		0xE1030004
//#define SECTION_PARSE_SECTION_NUMBER_ERROR	0xE1030005
//#define SECTION_PARSE_SECTION_NOT_COMPLETED	0xE1030006
//#define SECTION_PARSE_MEMORY_ERROR			0xE1030007
#define SECTION_PARSE_LENGTH_ERROR			0xE1030008
//#define SECTION_PARSE_DUPLICATED_SECTION	0xE1030009						//�ظ���section
//#define SECTION_PARSE_NEW_SECTION			0xE103000A						//��section
#define SECTION_PARSE_NOT_SUPPORTED_TABLE	0xE103000B

#define SECTION_PARSE_UNKNOWN_ERROR			0xffffffff


#endif