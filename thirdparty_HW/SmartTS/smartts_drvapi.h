#ifndef _TSSB_DRVAPI_H_
#define _TSSB_DRVAPI_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _SMART_TS_EXPORT
#       define SMART_TS_LIB __declspec(dllexport)
#   elif defined(_SMART_TS_IMPORT)
#       define SMART_TS_LIB __declspec(dllimport)
#   else
#       define SMART_TS_LIB
#   endif
#elif __GNUC__ >= 4
#   define SMART_TS_LIB __attribute__((visibility("default")))
#else
#   define SMART_TS_LIB
#endif

#include <stdint.h>

#include <windows.h>
#include <windef.h>

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

SMART_TS_LIB ULONG smartts_init(void);

SMART_TS_LIB ULONG smartts_start_transmit(int ext_sync_clk, int bitrate);
SMART_TS_LIB ULONG smartts_transmit( PUCHAR buffer, ULONG count );
SMART_TS_LIB ULONG smartts_stop_transmit( void );

SMART_TS_LIB ULONG smartts_checklocked(void);

SMART_TS_LIB ULONG smartts_start_receive(int loopout = 0);
SMART_TS_LIB ULONG smartts_receive( PUCHAR buffer, ULONG count );
SMART_TS_LIB ULONG smartts_stop_receive( void );

SMART_TS_LIB ULONG smartts_pause( void );
SMART_TS_LIB ULONG smartts_resume( UCHAR );

SMART_TS_LIB ULONG smartts_fini(void);

SMART_TS_LIB ULONG smartts_lreset( void );

SMART_TS_LIB ULONG smartts_get_asiin_status(void);

SMART_TS_LIB ULONG smartts_get_asiin_bitrate_map(ULONG pbitrate_map[], int count);
SMART_TS_LIB ULONG smartts_get_asiin_bitrate(void);

SMART_TS_LIB ULONG smartts_set_timeslice_count(int timeslice_count);
SMART_TS_LIB ULONG smartts_get_timeslice_count(void);

SMART_TS_LIB void smartts_config_timer(void);
SMART_TS_LIB void smartts_start_timer(void);
SMART_TS_LIB void smartts_stop_timer(void);

SMART_TS_LIB ULONG smartts_enable_timer(int index);
SMART_TS_LIB ULONG smartts_disable_timer(int index);

SMART_TS_LIB double smartts_set_timer(int index, double frequency);
SMART_TS_LIB double smartts_get_timer(int index);

SMART_TS_LIB ULONG smartts_enable_int(int index);
SMART_TS_LIB ULONG smartts_disable_int(int index);
SMART_TS_LIB ULONG smartts_clear_int(int index);
SMART_TS_LIB BOOL  smartts_check_int(int index);

SMART_TS_LIB ULONG smartts_select_1Hz_clk(int bExternal);
SMART_TS_LIB ULONG smartts_select_ref_clk(int bExternal);

//__declspec(dllexport) ULONG smartts_enable_reframe(void);
//__declspec(dllexport) ULONG smartts_disable_reframe(void);

SMART_TS_LIB ULONG smartts_enable_asiin(void);
SMART_TS_LIB ULONG smartts_disable_asiin(void);

//__declspec(dllexport) ULONG smartts_enable_asiout(void);
//__declspec(dllexport) ULONG smartts_disable_asiout(void);

SMART_TS_LIB ULONG smartts_set_iomode(ULONG mode);
SMART_TS_LIB ULONG smartts_get_iomode(void);

SMART_TS_LIB ULONG smartts_bist_set_params(int ext_sync_clk, int bitrate, ULONG mode = BIST_MODE_188_0);
SMART_TS_LIB ULONG smartts_bist_get_mode(void);

SMART_TS_LIB ULONG smartts_get_sendfifo_error_count(void);
SMART_TS_LIB ULONG smartts_get_recvfifo_error_count(void);

SMART_TS_LIB ULONG ReadFpgaInnerReg32(ULONG offset, PULONG buffer);
SMART_TS_LIB ULONG ReadFpgaInnerReg16(ULONG offset, PUSHORT buffer);
SMART_TS_LIB ULONG ReadFpgaInnerReg8(ULONG offset, PUCHAR buffer);

SMART_TS_LIB ULONG ReadLocalConfigReg32(ULONG offset, PULONG buffer);
SMART_TS_LIB ULONG ReadLocalConfigReg16(ULONG offset, PUSHORT buffer);
SMART_TS_LIB ULONG ReadLocalConfigReg8(ULONG offset, PUCHAR buffer);

SMART_TS_LIB ULONG ReadPciConfigReg32(ULONG offset, PULONG buffer);
SMART_TS_LIB ULONG ReadPciConfigReg16(ULONG offset, PUSHORT buffer);
SMART_TS_LIB ULONG ReadPciConfigReg8(ULONG offset, PUCHAR buffer);

#endif
