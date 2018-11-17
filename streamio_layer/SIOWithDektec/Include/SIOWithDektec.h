#ifndef _DEKTEC_TRANSRECV_H_
#define _DEKTEC_TRANSRECV_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _SIO_DEKTEC_EXPORT
#       define SIO_DEKTEC_LIB __declspec(dllexport)
#   elif defined(_SIO_DEKTEC_IMPORT)
#       define SIO_DEKTEC_LIB __declspec(dllimport)
#   else
#       define SIO_DEKTEC_LIB
#   endif
#elif __GNUC__ >= 4
#   define SIO_DEKTEC_LIB __attribute__((visibility("default")))
#else
#   define SIO_DEKTEC_LIB
#endif

#include <stdint.h>


//Scan for available DekTec devices
#define DTDV_SCAN_FAILED		-1;
// Did we find our device?
#define FIND_DEVICE_FAILED		-2;
// Attach to the device
#define ATTACH_DEVICE_FAILED	-3;
// Attach to the output channel
#define ATTACH_PORT_FAILED		-4;
// Init channel to initial 'safe' state
#define SET_RTXCONTROL_FAILED	-5;
// Set the receive mode
#define SET_RECEIVEMODE_FAILED	-6;


SIO_DEKTEC_LIB	int dektec_transmit_init(void);
SIO_DEKTEC_LIB	int dektec_start_transmit(int bitrate);
SIO_DEKTEC_LIB	int dektec_transmit(unsigned char *buffer, int count);
SIO_DEKTEC_LIB	int dektec_stop_transmit(void);
SIO_DEKTEC_LIB	int dektec_transmit_fini(void);

SIO_DEKTEC_LIB	int dektec_receive_init(void);
SIO_DEKTEC_LIB	int dektec_start_receive(void);
SIO_DEKTEC_LIB	int dektec_receive(unsigned char *buffer, int count);
SIO_DEKTEC_LIB	int dektec_stop_receive(void);
SIO_DEKTEC_LIB	int dektec_receive_fini(void);

SIO_DEKTEC_LIB	int dektec_get_asiin_bitrate(void);

#endif
