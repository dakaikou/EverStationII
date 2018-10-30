#ifndef _TSMAGIC_CALLBACKS_TO_GUI_H_
#define _TSMAGIC_CALLBACKS_TO_GUI_H_

#include "MiddleWare/MiddleWare_TransportStream\Include\MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_TSPackets.h"
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"

void CALLBACK_REPORT_PCR_Record(RECORD_PCR_t* pPcrRecord);
void CALLBACK_REPORT_PCR_Attribute(int ID, int curInterval, int curJitter, PCR_INTERVAL_ATTRIBUTE_t* pIntervalAttr=NULL, PCR_JITTER_ATTRIBUTE_t* pJitterAttr=NULL);
void CALLBACK_REPORT_bitrates(int cur, BITRATE_ATTRIBUTE_t* pattr=NULL);

void CALLBACK_REPORT_pat(int section_number, CPAT* pPAT);
void CALLBACK_REPORT_cat(int section_number, CCAT* pCAT);
void CALLBACK_REPORT_pmt(int section_number, CPMT* pPMT);
void CALLBACK_REPORT_nit(int section_number, CNIT* pNIT);
void CALLBACK_REPORT_bat(int section_number, CBAT* pBAT);
void CALLBACK_REPORT_sdt(int section_number, CSDT* pSDT);
void CALLBACK_REPORT_eit(int section_number, CEIT* pEIT);
void CALLBACK_REPORT_int(int section_number, CINT* pINT);
void CALLBACK_REPORT_ait(int section_number, CAIT* pAIT);
void CALLBACK_REPORT_mpe(int section_number, CMPE* pMPE);
void CALLBACK_REPORT_dsmcc(int section_number, CPVT* pPVT);
void CALLBACK_REPORT_tsdt(int section_number, CTSDT* pTSDT);

void CALLBACK_REPORT_cmt(CCMT* pCMT);
void CALLBACK_REPORT_tot(CTOT* pTOT);
void CALLBACK_REPORT_tdt(CTDT* pTDT);
void CALLBACK_REPORT_rst(CRST* pRST);

void REPORT_TS_packet_statistic(WPARAM wParam, LPARAM lParam);

#endif	//_TSMAGIC_CALLBACKS_TO_GUI_H_
