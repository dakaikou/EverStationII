#ifndef _DEFINE_H_
#define _DEFINE_H_

#define CHINESE_VERSION					1
#define OFFLINE_ANALYZE					0

#define OPEN_PACKET_STATISTIC			1
#define OPEN_PCR_ANALYZER				1

//#define OPEN_PAT_ANALYZER				1
//#define OPEN_PMT_ANALYZER				1
//#define OPEN_CAT_ANALYZER				1
//#define OPEN_NIT_ANALYZER				1
//#define OPEN_INT_ANALYZER				1
//#define OPEN_BAT_ANALYZER				1
//#define OPEN_SDT_ANALYZER				1
//#define OPEN_EIT_ANALYZER				1
//#define OPEN_RST_ANALYZER				1
//#define OPEN_TDT_ANALYZER				1
//#define OPEN_TOT_ANALYZER				1
//#define OPEN_TSDT_ANALYZER				1

//#define OPEN_DSMCC_ANALYZER				1
//#define OPEN_MPE_ANALYZER				1
//#define OPEN_AIT_ANALYZER				1

//#define OPEN_DSMCC_DOWNLOAD				(0 & OPEN_DSMCC_ANALYZER)
#define OPEN_SECTION_SPLICER			1
#define OPEN_SECTION_TRIGGER			1
#define OPEN_TS_PACKET_TRIGGER			1
#define OPEN_PES_PACKET_TRIGGER			1

#define OPEN_AC3_AUDIO_ANALYZER			1
#define OPEN_MPEG_AUDIO_ANALYZER		1
#define OPEN_MPEG_VIDEO_ANALYZER		1
#define OPEN_H264_VIDEO_ANALYZER		1
#define OPEN_AVS_VIDEO_ANALYZER			1
#define OPEN_AAC_AUDIO_ANALYZER			1
#define OPEN_MPEG_PS_ANALYZER			1
#define OPEN_YUV_VIDEO_ANALYZER			1
/*------------------------------------------------------------
					GUI interface definition
-------------------------------------------------------------*/
#define BYTE_BUFFER_USE_LISTCTRL_VIEW				0

/*------------------------------------------------------------
					Message definition
-------------------------------------------------------------*/

//#define MAX_TSPACKET_TRIGGER_COUNT			100
//#define MAX_PCR_FIELD_TRIGGER_COUNT		100
//#define MAX_SECTION_TRIGGER_COUNT			100

#define MAX_TXT_CHARS						256
#define MAX_LOG_COUNT						4096

//typedef enum _GUI_PSISI_SYNTAX
//{
//	GUI_PSISI_SYNTAX_NONE		= 0,
//	GUI_PSISI_SYNTAX_PAT,
//	GUI_PSISI_SYNTAX_PMT,
//	GUI_PSISI_SYNTAX_NIT,
//	GUI_PSISI_SYNTAX_CAT,
//	GUI_PSISI_SYNTAX_BAT,
//	GUI_PSISI_SYNTAX_SDT,
//	GUI_PSISI_SYNTAX_EIT,
//	GUI_PSISI_SYNTAX_TDT,
//	GUI_PSISI_SYNTAX_TOT,
//	GUI_PSISI_SYNTAX_RST,
//	GUI_PSISI_SYNTAX_MPE,
//	GUI_PSISI_SYNTAX_DSMCC_UNM,
//	GUI_PSISI_SYNTAX_DSMCC_DDM,
//	GUI_PSISI_SYNTAX_DSMCC_PVT,
//	GUI_PSISI_SYNTAX_INT,
//	GUI_PSISI_SYNTAX_AIT,
//	GUI_PSISI_SYNTAX_TSDT,
//
//} GUI_PSISI_SYNTAX_CLASS_e;
//
//typedef union
//{
//	unsigned int	uiValue;
//	struct
//	{
//		unsigned int uiClassIndex		:8;
//		unsigned int uiSectionNumber	:8;
//		unsigned int uiTableIndex		:16;
//	} flag;
//
//} GUI_PSISI_SYNTAX_ITEM_ID_u;

//typedef enum _GUI_EPG_TYPE
//{
//	GUI_EPG_NONE		= 0,
//	GUI_EPG_STREAM,
//	GUI_EPG_SERVICE,
//
//} GUI_EPG_CLASS_e;
//
//typedef union
//{
//	unsigned int	 uiValue;
//	struct
//	{
//		unsigned int	uiServiceIndex	:16;
//		unsigned int	uiStreamIndex	:8;
//		unsigned int	uiClass			:3;
//
//	} flag;
//
//} GUI_EPG_ITEM_ID_u;


//typedef enum
//{
//	GUI_BOUQUET_NONE		= 0,
//	GUI_BOUQUET_BOUQUET,
//
//} GUI_BOUQUET_CLASS_e;
//
//typedef union
//{
//	unsigned int	 uiValue;
//	struct
//	{
//		unsigned int	uiBouquetIndex	:5;
//		unsigned int	uiClass			:3;
//
//	} flag;
//
//} GUI_BOUQUET_ITEM_ID_u;

typedef enum _FILTER_PSISI_SYNTAX
{
	FILTER_PSISI_ANALYZER		= 0,
	FILTER_PSISI_SYNTAX_PAT,
	FILTER_PSISI_SYNTAX_PMT,
	FILTER_PSISI_SYNTAX_CAT,
	FILTER_PSISI_SYNTAX_TSDT,
	FILTER_PSISI_SYNTAX_NIT,
	FILTER_PSISI_SYNTAX_SDT,
	FILTER_PSISI_SYNTAX_EIT,
	FILTER_PSISI_SYNTAX_BAT,
	FILTER_PSISI_SYNTAX_TDT,
	FILTER_PSISI_SYNTAX_TOT,
	FILTER_PSISI_SYNTAX_RST,
	FILTER_PSISI_SYNTAX_AIT,
	FILTER_PSISI_SYNTAX_INT,
	FILTER_PSISI_SYNTAX_MPE,
	FILTER_PSISI_SYNTAX_DSMCC,
} FILTER_PSISI_SYNTAX_e;

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
