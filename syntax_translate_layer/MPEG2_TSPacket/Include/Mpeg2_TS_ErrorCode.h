#ifndef _MPEG_TS_ERROR_H_
#define _MPEG_TS_ERROR_H_

#ifdef TSPACKET_PARSE_NO_ERROR
#undef TSPACKET_PARSE_NO_ERROR
#endif

#define TSPACKET_PARSE_NO_ERROR						0x00000000

//TS_packet�����������
#define TSPACKET_PARSE_PARAMETER_ERROR			0xE1010001
#define TSPACKET_PARSE_SYNC_ERROR				0xE1010002
#define TSPACKET_PARSE_SYNTAX_ERROR				0xE1010003

#define TSPACKET_PARSE_UNKNOWN_ERROR			0xFFFFFFFF

#endif