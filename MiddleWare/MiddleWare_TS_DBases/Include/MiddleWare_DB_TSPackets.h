#ifndef _MIDDLEWARE_DBASE_TSPACKETS_H_
#define _MIDDLEWARE_DBASE_TSPACKETS_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _MW_DB_EXPORT
#       define MW_DB_LIB __declspec(dllexport)
#   elif defined(_MW_DB_IMPORT)
#       define MW_DB_LIB __declspec(dllimport)
#   else
#       define MW_DB_LIB
#   endif
#elif __GNUC__ >= 4
#   define MW_DB_LIB __attribute__((visibility("default")))
#else
#   define MW_DB_LIB
#endif

#include <stdint.h>

#include <Windows.h>

#include "translate_layer/Mpeg2_TSPacket/Include/Mpeg2_TS_packet.h"

#define MAX_PID_COUNT						8192
#define PID_STYLE_VERIFY_COUNTDOWNS			3

typedef struct _TS_packet_info_s
{
	//31-19：TS PID
	//18-16：TS payload class
	//15- 8: main_type, stream_id or table_id
	// 7- 0: sub type
	uint8_t			ucClass;			//3bits
	uint16_t		usPID;				//13bits
	uint8_t			ucMainType;			//8bits, stream_id or table_id
	uint8_t			ucStreamType;		//8bits, stream_type						//0				UNKNOWN
	//uint8_t			ucSubType;			

	uint32_t		uiTempCheckedType[20];					//临时检测出的类型
	int				nCheckedCount;

	int			count;				//可以根据count属性判断该PID包是否真正出现过
	
	float		ratio;

	float		datarate_min;
	float		datarate_cur;
	float		datarate_max;

	int			continuity_counter;
	int			continuity_counter_error;
	int			continuity_counter_need_update;

	int			ts_scramble;
	char		ts_scramble_style[32];
	int			ts_scramble_need_update;

	char		pszPacketStyle[256];
	int			packet_style_need_update;

	//与流抽选有关
	int			bDecimate;

} RECORD_TSPacket_t;

class MW_DB_LIB CDB_TSPackets
{
public:
	CDB_TSPackets(void);
	~CDB_TSPackets(void);
public:

	int						m_total_packet_count;
	int						m_total_bitrate_from_software;
	int						m_total_bitrate_from_hardware;
	int						m_packet_length;
//	S8						m_filename[MAX_PATH];

protected:
	int						m_nPIDCount;
	RECORD_TSPacket_t*		m_pRecords[MAX_PID_COUNT];

public:
	void(*callback_gui_update)(WPARAM wParam, LPARAM lParam);

public:
	int GetRecord(uint16_t usPID, RECORD_TSPacket_t *pRecord);

	int	AddPacket(transport_packet_t* pTS_packet);

	int UpdateStreamType(uint16_t usPID, uint8_t stream_type);
	int UpdateDescription(uint16_t usPID, char* pszMsg, int size);
	int SetDecimateFlag(uint16_t usPID, int bDecimate);
	int	AddDummyRecord(uint16_t usPID, uint8_t payload_class, uint8_t stream_type, uint8_t stream_subtype, char* pszMsg, int size);

	void Reset(void);
	void PartialReset(void);
};

#endif		//_TSMAGIC_DBASE_TSPACKETS_H_
