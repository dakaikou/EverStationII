#ifndef __CDL_INET_H__
#define __CDL_INET_H__

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../compile.h"

#define IPv4_HEAD_LENGTH_MIN				20
#define IPv4_HEAD_LENGTH_MAX				60

#define TCP_HEAD_LENGTH_MIN					20
#define TCP_HEAD_LENGTH_MAX					60

#define UDP_HEAD_LENGTH						8		

typedef struct
{
	U8			Version;
	U8			IHL;
	U8			TypeofService;
	U16			TotalLength;
	U16			Identification;
	U8			Flags;
	U16			FragmentOffset;
	U8			TimetoLive;
	U8			Protocol;
	U16			HeaderChecksum;
	U32			SourceAddress;
	U32			DestinationAddress;
	U32			Options;
	U32			Padding;

	U8*			data_buf;
	S32			data_length;
	
} CDL_IPv4_t; 

typedef struct
{
	U16			SourcePort;
	U16			DestinationPort;
	U16			Length;
	U16			Checksum;

	U8*			data_buf;
	S32			data_length;

} CDL_UDP_t;

typedef struct
{
	U8			version;						//2
	U8			padding;						//1
	U8			extension;						//1
	U8			CSRC_count;						//4

	U8			marker;							//1
	U8			payload_type;					//7

	U16			sequence_number;				//16

	U32			timestamp;						//32
	U32			SSRC;							//32
	U32			CSRC[16];						//32 x 16

	U8*			data_buf;
	S32			data_length;
	
} CDL_RTP_t;

_CDL_EXPORT S32	CDL_IPv4_Parse(U8* buf, S32 length, CDL_IPv4_t* pIPv4);
_CDL_EXPORT S32	CDL_IPv4_Encode(U8* buf, S32* plength, CDL_IPv4_t* pIPv4);

_CDL_EXPORT S32	CDL_UDP_Parse(U8* buf, S32 length, CDL_UDP_t* pUDP);
_CDL_EXPORT S32	CDL_UDP_Encode(U8* buf, S32* plength, CDL_UDP_t* pUDP);

_CDL_EXPORT S32	CDL_RTP_Parse(U8* buf, S32 length, CDL_RTP_t* pRTP);
_CDL_EXPORT S32	CDL_RTP_Encode(U8* buf, S32* plength, CDL_RTP_t* pRTP);

#endif
