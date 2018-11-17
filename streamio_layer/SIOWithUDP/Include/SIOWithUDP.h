#ifndef _TAL_WINPCAP_UDP_TRANSRECV_H_
#define _TAL_WINPCAP_UDP_TRANSRECV_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _SIO_UDP_EXPORT
#       define SIO_UDP_LIB __declspec(dllexport)
#   elif defined(_SIO_UDP_IMPORT)
#       define SIO_UDP_LIB __declspec(dllimport)
#   else
#       define SIO_UDP_LIB
#   endif
#elif __GNUC__ >= 4
#   define SIO_UDP_LIB __attribute__((visibility("default")))
#else
#   define SIO_UDP_LIB
#endif

#include <stdint.h>

//ÍøÂç´íÎó´úÂë
#define NETWORK_NO_ERROR					0x00000000
#define NETWORK_NO_DEVICES					0xEA010001
#define NETWORK_OPEN_ERROR					0xEA010002
#define NETWORK_LINK_LAYER_TYPE_ERROR		0xEA010003
#define NETWORK_COMPILE_FILTER_ERROR		0xEA010004
#define NETWORK_SET_FILTER_ERROR			0xEA010005
#define NETWORK_UNKNOWN_ERROR				0xFFFFFFFF

SIO_UDP_LIB	int udp_transmit_init(void);
SIO_UDP_LIB	int udp_start_transmit(int bitrate);
SIO_UDP_LIB	int udp_transmit(unsigned char *buffer, int count);
SIO_UDP_LIB	int udp_stop_transmit(void);
SIO_UDP_LIB	int udp_transmit_fini(void);

SIO_UDP_LIB	int udp_receive_init(char* pszDstIP, int dst_port);
SIO_UDP_LIB	int udp_start_receive(void);
SIO_UDP_LIB	int udp_receive(unsigned char *buffer, int count);
SIO_UDP_LIB	int udp_stop_receive(void);
SIO_UDP_LIB	int udp_receive_fini(void);

SIO_UDP_LIB	int udp_get_bitrate(void);

#endif
