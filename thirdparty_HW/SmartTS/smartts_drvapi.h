#ifndef _TSSB_DRVAPI_H_
#define _TSSB_DRVAPI_H_

#include <windows.h>

#define SMARTTS_NULL			0x43444c00
#define SMARTTS_ASI_IN			0x43444c01
#define SMARTTS_ASI_OUT			0x43444c02
#define SMARTTS_ASI_INOUT		0x43444c03

typedef enum
{
	SMARTTS_IOMODE_NULL			= 0,
	SMARTTS_IOMODE_SEPARATE		= 1,
	SMARTTS_IOMODE_INLOOP		= 2,
	SMARTTS_IOMODE_OUTLOOP		= 3,
	SMARTTS_IOMODE_BIST			= 4,
	SMARTTS_IOMODE_SEND			= 5,
	SMARTTS_IOMODE_RECEIVE		= 6,
	SMARTTS_IOMODE_LOCAL		= 7,
} SMARTTS_IOMODE_TYPE_e;

typedef enum
{
	SMARTTS_TIMER0				= 0,
	SMARTTS_TIMER1,
	SMARTTS_TIMER2,
	SMARTTS_TIMER3,
} SMARTTS_TIMER_e;

typedef enum
{
	BIST_MODE_188_0				= 0,
	BIST_MODE_188_16NULL		= 2,
	BIST_MODE_188_16RS			= 3,
} BIST_MODE_e;

#define TIMER_REF_CLK_12M			12000000			//Hz
#define TIMER_REF_CLK_27M			27000000			//Hz
#define TIMER_REF_CLK_36M			36000000			//Hz

#define CLK_COEFF_12M				(((1<<30) / (double)TIMER_REF_CLK_12M) * (1<<2))
#define CLK_COEFF_27M				(((1<<30) / (double)TIMER_REF_CLK_27M) * (1<<2))
#define CLK_COEFF_36M				(((1<<30) / (double)TIMER_REF_CLK_36M) * (1<<2))

#define TIME_RESOLUTION				50						//ms
#define CALCU_TIMESLICE_COUNT		(1000 / TIME_RESOLUTION / 2)

_CDL_EXPORT ULONG smartts_init(void);

_CDL_EXPORT ULONG smartts_start_transmit(int ext_sync_clk, int bitrate);
_CDL_EXPORT ULONG smartts_transmit( PUCHAR buffer, ULONG count );
_CDL_EXPORT ULONG smartts_stop_transmit( void );

_CDL_EXPORT ULONG smartts_checklocked(void);

_CDL_EXPORT ULONG smartts_start_receive(int loopout = 0);
_CDL_EXPORT ULONG smartts_receive( PUCHAR buffer, ULONG count );
_CDL_EXPORT ULONG smartts_stop_receive( void );

_CDL_EXPORT ULONG smartts_pause( void );
_CDL_EXPORT ULONG smartts_resume( UCHAR );

_CDL_EXPORT ULONG smartts_fini(void);

_CDL_EXPORT ULONG smartts_lreset( void );

_CDL_EXPORT ULONG smartts_get_asiin_status(void);

_CDL_EXPORT ULONG smartts_get_asiin_bitrate_map(ULONG pbitrate_map[], int count);
_CDL_EXPORT ULONG smartts_get_asiin_bitrate(void);

_CDL_EXPORT ULONG smartts_set_timeslice_count(int timeslice_count);
_CDL_EXPORT ULONG smartts_get_timeslice_count(void);

_CDL_EXPORT void smartts_config_timer(void);
_CDL_EXPORT void smartts_start_timer(void);
_CDL_EXPORT void smartts_stop_timer(void);

_CDL_EXPORT ULONG smartts_enable_timer(int index);
_CDL_EXPORT ULONG smartts_disable_timer(int index);

_CDL_EXPORT double smartts_set_timer(int index, double frequency);
_CDL_EXPORT double smartts_get_timer(int index);

_CDL_EXPORT ULONG smartts_enable_int(int index);
_CDL_EXPORT ULONG smartts_disable_int(int index);
_CDL_EXPORT ULONG smartts_clear_int(int index);
_CDL_EXPORT BOOL  smartts_check_int(int index);

_CDL_EXPORT ULONG smartts_select_1Hz_clk(int bExternal);
_CDL_EXPORT ULONG smartts_select_ref_clk(int bExternal);

//__declspec(dllexport) ULONG smartts_enable_reframe(void);
//__declspec(dllexport) ULONG smartts_disable_reframe(void);

_CDL_EXPORT ULONG smartts_enable_asiin(void);
_CDL_EXPORT ULONG smartts_disable_asiin(void);

//__declspec(dllexport) ULONG smartts_enable_asiout(void);
//__declspec(dllexport) ULONG smartts_disable_asiout(void);

_CDL_EXPORT ULONG smartts_set_iomode(ULONG mode);
_CDL_EXPORT ULONG smartts_get_iomode(void);

_CDL_EXPORT ULONG smartts_bist_set_params(int ext_sync_clk, int bitrate, ULONG mode = BIST_MODE_188_0);
_CDL_EXPORT ULONG smartts_bist_get_mode(void);

_CDL_EXPORT ULONG smartts_get_sendfifo_error_count(void);
_CDL_EXPORT ULONG smartts_get_recvfifo_error_count(void);

_CDL_EXPORT ULONG ReadFpgaInnerReg32(ULONG offset, PULONG buffer);
_CDL_EXPORT ULONG ReadFpgaInnerReg16(ULONG offset, PUSHORT buffer);
_CDL_EXPORT ULONG ReadFpgaInnerReg8(ULONG offset, PUCHAR buffer);

_CDL_EXPORT ULONG ReadLocalConfigReg32(ULONG offset, PULONG buffer);
_CDL_EXPORT ULONG ReadLocalConfigReg16(ULONG offset, PUSHORT buffer);
_CDL_EXPORT ULONG ReadLocalConfigReg8(ULONG offset, PUCHAR buffer);

_CDL_EXPORT ULONG ReadPciConfigReg32(ULONG offset, PULONG buffer);
_CDL_EXPORT ULONG ReadPciConfigReg16(ULONG offset, PUSHORT buffer);
_CDL_EXPORT ULONG ReadPciConfigReg8(ULONG offset, PUCHAR buffer);

#endif
