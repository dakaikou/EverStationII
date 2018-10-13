#ifndef _TSMAGIC_DECIMATE_DSMCC_
#define _TSMAGIC_DECIMATE_DSMCC_

#include <stdint.h>

uint32_t TSMagic_dsmcc_download_thread(LPVOID lpParam);

//int ts_dsmcc_download_packet_filter(CDB_OCDCs* pDB_OCDCs, transport_packet_t* ptransport_packet);
//int ts_dsmcc_download_section_filter(CDB_OCDCs* pDB_OCDCs, uint16_t nPID, uint8_t* section_buf, int section_length);

#endif		//_TSMAGIC_DECIMATE_DSMCC_
