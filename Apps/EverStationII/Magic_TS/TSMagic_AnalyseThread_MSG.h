#ifndef _MPEG2_GUIAPI_MSG_H_
#define _MPEG2_GUIAPI_MSG_H_

#define WM_TSMAGIC_OFFLINE_THREAD				WM_USER + 1000
#define WM_TSMAGIC_REALTIME_THREAD				WM_USER + 1001
#define WM_TSMAGIC_DOWNLOAD_THREAD				WM_USER + 1002
#define WM_TSMAGIC_DECIMATE_THREAD				WM_USER + 1003
#define WM_TSMAGIC_RECORD_THREAD				WM_USER + 1004

#define WM_TSMAGIC_APPEND_LOG					WM_USER + 1100

#define WM_TSMAGIC_REPORT_RATIO					WM_USER + 1101
#define WM_TSMAGIC_REPORT_FILESIZE				WM_USER + 1102

#define WM_TSMAGIC_DOWNLOAD_LOG					WM_USER + 1103
#define WM_TSMAGIC_DOWNLOAD_RATIO				WM_USER + 1104

#define WM_TSMAGIC_DECIMATE_LOG					WM_USER + 1105
#define WM_TSMAGIC_DECIMATE_RATIO				WM_USER + 1106

#define WM_TSMAGIC_RECORD_LOG					WM_USER + 1107
#define WM_TSMAGIC_RECORD_RATIO					WM_USER + 1108

#define WM_TSMAGIC_ETR290_LOG					WM_USER + 1110

#define WM_TSMAGIC_REPORT_BITRATE				WM_USER + 1111
#define WM_TSMAGIC_REPORT_BITRATE_MAP			WM_USER + 1112
#define WM_TSMAGIC_REPORT_PACKET_LENGTH			WM_USER + 1113

#define WM_TSMAGIC_TS_TRIGGER_STATE				WM_USER + 1115
#define WM_TSMAGIC_PES_TRIGGER_STATE			WM_USER + 1116
#define WM_TSMAGIC_ES_TRIGGER_STATE				WM_USER + 1117
#define WM_TSMAGIC_SECTION_TRIGGER_STATE		WM_USER + 1118

#define WM_TSMAGIC_REPORT_SIGNAL_STATUS			WM_USER + 1120

#define WM_ESDEMUXTHREAD_EXIT					WM_USER + 1200

#define WM_UPDATE_PAT						WM_USER + 2000
#define WM_UPDATE_PMT						WM_USER + 2001
#define WM_UPDATE_CAT						WM_USER + 2002
#define WM_UPDATE_BAT						WM_USER + 2003
#define WM_UPDATE_NIT						WM_USER + 2004
#define WM_UPDATE_SDT						WM_USER + 2005
#define WM_UPDATE_EIT						WM_USER + 2006
#define WM_UPDATE_RST						WM_USER + 2007
#define WM_UPDATE_TDT						WM_USER + 2008
#define WM_UPDATE_TOT						WM_USER + 2009

#define WM_UPDATE_TS_PACKET_STATISTIC			WM_USER + 2020

#define WM_UPDATE_TRIGGER_TS_PACKET				WM_USER + 2030
#define WM_UPDATE_TRIGGER_TS_PID				WM_USER + 2031
#define WM_UPDATE_TRIGGER_PSISI_SECTION			WM_USER + 2032

#define WM_UPDATE_PCR_STATISTIC					WM_USER + 2040
#define WM_UPDATE_EPG_INFO						WM_USER + 2045

#define WM_UPDATE_WAVE_HEADER					WM_USER + 2421

#endif		//_MPEG2_GUIAPI_MSG_H_
