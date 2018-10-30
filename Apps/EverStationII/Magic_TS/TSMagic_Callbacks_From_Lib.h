#ifndef _TSMAGIC_CALLBACKS_FROM_LIB_H_
#define _TSMAGIC_CALLBACKS_FROM_LIB_H_

#include <stdint.h>

#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_PsiSiObjs.h"

//int SectionSplice_update(CDB_PsiSiTables* pDB_PsiSiTables, HWND hWnd, uint16_t PID, uint8_t* buf, int length);
int REPORT_PSISI_section(CDB_PsiSiObjs* pDB_PsiSiObjs, HWND hWnd, uint16_t PID, uint8_t* buf, int length);

#endif	//_TSMAGIC_CALLBACKS_FROM_LIB_H_
