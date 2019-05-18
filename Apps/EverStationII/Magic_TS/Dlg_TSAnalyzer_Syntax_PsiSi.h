#if !defined(AFX_DLG_PSISISYNTAX_H__C0C4E0C4_E409_474B_BB0B_436359D87F94__INCLUDED_)
#define AFX_DLG_PSISISYNTAX_H__C0C4E0C4_E409_474B_BB0B_436359D87F94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_PsiSiSyntax.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_PsiSi dialog

#include "GuiWnd_NaviTree_PsiSiSectionGuide.h"
#include "..\Common\TreeView_XMLBrowser.h"

#if BYTE_BUFFER_USE_LISTCTRL_VIEW
#include "..\Common\ListView_ByteBuffer.h"
#else
#include "..\Common\HexEditView_ByteBuffer.h"
#endif
#include "..\resource.h"

class CDlg_TSAnalyzer_Syntax_PsiSi : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_Syntax_PsiSi(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_Syntax_PsiSi)
	enum { IDD = IDD_TS_ANALYZER_PSISI };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_Syntax_PsiSi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

public:
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

protected:
	CSplitterWnd				m_wndSplitter;
	//CSplitterWnd_PsiSiAnalyzer	m_wndSplitter;
	CNaviTree_PsiSiSectionGuide*		m_pNaviTree;
	CTreeView_XMLBrowser*		m_pSyntaxTree;
	CHexEditView_ByteBuffer*	m_pHexList;

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_PsiSi)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	afx_msg LRESULT OnReportPsiSiSelChange(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_PSISISYNTAX_H__C0C4E0C4_E409_474B_BB0B_436359D87F94__INCLUDED_)
