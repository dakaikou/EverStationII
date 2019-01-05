#ifndef __CDL_INET_H__
#define __CDL_INET_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _SYNTAX_IP_EXPORT
#       define SYNTAX_IP_LIB __declspec(dllexport)
#   elif defined(_SYNTAX_IP_IMPORT)
#       define SYNTAX_IP_LIB __declspec(dllimport)
#   else
#       define SYNTAX_IP_LIB
#   endif
#elif __GNUC__ >= 4
#   define SYNTAX_IP_LIB __attribute__((visibility("default")))
#else
#   define SYNTAX_IP_LIB
#endif

#include <stdint.h>

#define IPv4_HEAD_LENGTH_MIN				20
#define IPv4_HEAD_LENGTH_MAX				60

#define TCP_HEAD_LENGTH_MIN					20
#define TCP_HEAD_LENGTH_MAX					60

#define UDP_HEAD_LENGTH						8		

typedef struct
{
	uint8_t			Version;
	uint8_t			IHL;
	uint8_t			TypeofService;
	uint16_t			TotalLength;
	uint16_t			Identification;
	uint8_t			Flags;
	uint16_t			FragmentOffset;
	uint8_t			TimetoLive;
	uint8_t			Protocol;
	uint16_t			HeaderChecksum;
	uint32_t			SourceAddress;
	uint32_t			DestinationAddress;
	uint32_t			Options;
	uint32_t			Padding;

	uint8_t*			data_buf;
	int			data_length;
	
} INET_IPv4_t; 

typedef struct
{
	uint16_t			SourcePort;
	uint16_t			DestinationPort;
	uint16_t			Length;
	uint16_t			Checksum;

	uint8_t*			data_buf;
	int			data_length;

} INET_UDP_t;

typedef struct
{
	uint8_t			version;						//2
	uint8_t			padding;						//1
	uint8_t			extension;						//1
	uint8_t			CSRC_count;						//4

	uint8_t			marker;							//1
	uint8_t			payload_type;					//7

	uint16_t			sequence_number;				//16

	uint32_t			timestamp;						//32
	uint32_t			SSRC;							//32
	uint32_t			CSRC[16];						//32 x 16

	uint8_t*			data_buf;
	int			data_length;
	
} INET_RTP_t;

SYNTAX_IP_LIB int	INET_IPv4_Parse(uint8_t* buf, int length, INET_IPv4_t* pIPv4);
SYNTAX_IP_LIB int	INET_IPv4_Encode(uint8_t* buf, int* plength, INET_IPv4_t* pIPv4);

SYNTAX_IP_LIB int	INET_UDP_Parse(uint8_t* buf, int length, INET_UDP_t* pUDP);
SYNTAX_IP_LIB int	INET_UDP_Encode(uint8_t* buf, int* plength, INET_UDP_t* pUDP);

SYNTAX_IP_LIB int	INET_RTP_Parse(uint8_t* buf, int length, INET_RTP_t* pRTP);
SYNTAX_IP_LIB int	INET_RTP_Encode(uint8_t* buf, int* plength, INET_RTP_t* pRTP);

#endif
