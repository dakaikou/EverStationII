#ifndef _MPEG_PES_ERROR_H_
#define _MPEG_PES_ERROR_H_

#ifdef PES_PACKET_NO_ERROR
#undef PES_PACKET_NO_ERROR
#endif

#define PES_PACKET_NO_ERROR						0x00000000

//ϵͳ�������
#define PES_PACKET_PARAMETER_ERROR				0xE1040001

#define PES_PACKET_UNKNOWN_ERROR				0xFFFFFFFF

#endif