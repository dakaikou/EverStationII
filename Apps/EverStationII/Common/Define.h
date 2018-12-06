#ifndef _DEFINE_H_
#define _DEFINE_H_

#define CHINESE_VERSION					1
#define OFFLINE_ANALYZE					0

#define TS_FILE_READ_INTEGRITY_DIAGNOSIS	0

#define GUI_REPORT_FILE_ANALYSE_RATIO	1
#define GUI_REPORT_TS_STATISTIC			1 & GUI_REPORT_FILE_ANALYSE_RATIO
#define GUI_REPORT_TS_BITRATE			1
#define GUI_REPORT_PCR_DIAGNOSIS		1
#define GUI_REPORT_PCR_OBSERVATION		1

#define OPEN_TS_PACKET_ANALYZER			1

#define OPEN_PCR_ANALYZER				1 & OPEN_TS_PACKET_ANALYZER 
#define OPEN_PACKET_STATISTIC			1 & OPEN_TS_PACKET_ANALYZER
#define OPEN_SECTION_SPLICER			1 & OPEN_TS_PACKET_ANALYZER & OPEN_PACKET_STATISTIC

#define OPEN_SECTION_TRIGGER			1 & OPEN_TS_PACKET_ANALYZER & OPEN_SECTION_SPLICER
#define OPEN_TS_PACKET_TRIGGER			1 & OPEN_TS_PACKET_ANALYZER
#define OPEN_PES_PACKET_TRIGGER			1 & OPEN_TS_PACKET_ANALYZER


/*------------------------------------------------------------
					GUI interface definition
-------------------------------------------------------------*/
#define BYTE_BUFFER_USE_LISTCTRL_VIEW				0

/*------------------------------------------------------------
					Message definition
-------------------------------------------------------------*/
#define MAX_TXT_CHARS						256
#define MAX_LOG_COUNT						4096

typedef enum _DEBUG_STAGE
{
	DEBUG_OPEN			= 0,
	DEBUG_INFO,
	DEBUG_WARNING,
	DEBUG_ERROR,
	DEBUG_IDLE,
	DEBUG_SAVE,
	DEBUG_CLOSE
} DEBUG_TYPE_e;

#endif
