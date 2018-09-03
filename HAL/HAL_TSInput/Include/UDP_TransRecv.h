#ifndef _UDP_TRANSRECV_H_
#define _UDP_TRANSRECV_H_

#include "../compile.h"

//ÍøÂç´íÎó´úÂë
#define NETWORK_NO_DEVICES					0xEA010001
#define NETWORK_OPEN_ERROR					0xEA010002
#define NETWORK_LINK_LAYER_TYPE_ERROR		0xEA010003
#define NETWORK_COMPILE_FILTER_ERROR		0xEA010004
#define NETWORK_SET_FILTER_ERROR			0xEA010005

_CDL_EXPORT	ULONG udp_transmit_init(void);
_CDL_EXPORT	ULONG udp_start_transmit(int bitrate);
_CDL_EXPORT	ULONG udp_transmit(unsigned char *buffer, int count);
_CDL_EXPORT	ULONG udp_stop_transmit(void);
_CDL_EXPORT	ULONG udp_transmit_fini(void);

_CDL_EXPORT	ULONG udp_receive_init(char* pszDstIP, int dst_port);
_CDL_EXPORT	ULONG udp_start_receive(void);
_CDL_EXPORT	ULONG udp_receive(unsigned char *buffer, int count);
_CDL_EXPORT	ULONG udp_stop_receive(void);
_CDL_EXPORT	ULONG udp_receive_fini(void);

_CDL_EXPORT	ULONG udp_get_bitrate(void);

#endif
