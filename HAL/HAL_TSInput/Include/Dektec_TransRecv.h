#ifndef _DEKTEC_TRANSRECV_H_
#define _DEKTEC_TRANSRECV_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"
#include "../compile.h"

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


_CDL_EXPORT	ULONG dektec_transmit_init(void);
_CDL_EXPORT	ULONG dektec_start_transmit(int bitrate);
_CDL_EXPORT	ULONG dektec_transmit(unsigned char *buffer, int count);
_CDL_EXPORT	ULONG dektec_stop_transmit(void);
_CDL_EXPORT	ULONG dektec_transmit_fini(void);

_CDL_EXPORT	ULONG dektec_receive_init(void);
_CDL_EXPORT	ULONG dektec_start_receive(void);
_CDL_EXPORT	ULONG dektec_receive(unsigned char *buffer, int count);
_CDL_EXPORT	ULONG dektec_stop_receive(void);
_CDL_EXPORT	ULONG dektec_receive_fini(void);

_CDL_EXPORT	ULONG dektec_get_asiin_bitrate(void);

#endif
