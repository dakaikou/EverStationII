#pragma once
#include <afxext.h>
#include <stdint.h>

#include "NaviTree_PsiSiTables.h"
#include "..\Common\TreeView_XMLBrowser.h"

#if BYTE_BUFFER_USE_LISTCTRL_VIEW
#include "..\Common\ListView_ByteBuffer.h"
#else
#include "..\Common\HexEditView_ByteBuffer.h"
#endif

class CSplitterWnd_PsiSiAnalyzer : public CSplitterWnd
{
public:
	CSplitterWnd_PsiSiAnalyzer();
	~CSplitterWnd_PsiSiAnalyzer();

	void Reset(void);
	//	void Set(int offline);
	void DisplaySection(uint8_t* section_buf, int length);

	void UpdatePAT(int section_number, CPAT* pPAT);
	void UpdatePMT(int section_number, CPMT* pPMT);
	void UpdateBAT(int section_number, CBAT* pBAT);
	void UpdateSDT(int section_number, CSDT* pSDT);
	void UpdateNIT(int section_number, CNIT* pNIT);
	void UpdateAIT(int section_number, CAIT* pAIT);
	void UpdateINT(int section_number, CINT* pINT);
	void UpdateEIT(int section_number, CEIT* pEIT);
	void UpdateDSMCC(int section_number, CPVT* pPVT);
	void UpdateMPE(int section_number, CMPE* pMPE);
	void UpdateCAT(int section_number, CCAT* pCAT);
	void UpdateTSDT(int section_number, CTSDT* pTSDT);
	void UpdateTDT(CTDT* pTDT);
	void UpdateTOT(CTOT* pTOT);
	void UpdateRST(CRST* pRST);
	void UpdateCMT(CCMT* pCMT);

	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

