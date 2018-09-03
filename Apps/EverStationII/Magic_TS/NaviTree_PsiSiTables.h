#if !defined(AFX_PANE_PSISISYNTAXTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_PSISISYNTAXTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Pane_PsisiSyntaxTreeview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPane_PsiSiTableTreeView view
#include "..\Common\define.h"
#include "..\Magic_TS\TSMagic_GuiApi.h"

#include <afxcview.h>

class CNaviTree_PsiSiTables : public CTreeView
{
public:
	CNaviTree_PsiSiTables();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_PsiSiTables)

// Attributes
protected:
	HTREEITEM		m_hPsisiRootItem;
	HTREEITEM		m_hDataCastRootItem;

	HTREEITEM		m_hPatItem;
	HTREEITEM		m_hPmtItem;
	HTREEITEM		m_hCatItem;
	HTREEITEM		m_hTsdtItem;
	HTREEITEM		m_hNitItem;
	HTREEITEM		m_hBatItem;
	HTREEITEM		m_hSdtItem;
	HTREEITEM		m_hEitItem;
	HTREEITEM		m_hTdtItem;
	HTREEITEM		m_hTotItem;
	HTREEITEM		m_hRstItem;
	HTREEITEM		m_hEcmItem;
	HTREEITEM		m_hEmmItem;

	HTREEITEM		m_hIntItem;
	HTREEITEM		m_hAitItem;
	HTREEITEM		m_hDsmccItem;
	HTREEITEM		m_hMpeItem;

	CImageList		m_ImageList;

// Operations
public:
	void UpdateCMT(CCMT* pCMT);
	void UpdateTDT(CTDT* pTDT);
	void UpdateTOT(CTOT* pTOT);
	void UpdateRST(CRST* pRST);

	void UpdateTSDT(int section_number, CTSDT* pTSDT);
	void UpdatePAT(int section_number, CPAT* pPAT);
	void UpdateCAT(int section_number, CCAT* pCAT);
	void UpdatePMT(int section_number, CPMT* pPMT);
	void UpdateBAT(int section_number, CBAT* pBAT);
	void UpdateSDT(int section_number, CSDT* pSDT);
	void UpdateNIT(int section_number, CNIT* pNIT);
	void UpdateAIT(int section_number, CAIT* pAIT);
	void UpdateINT(int section_number, CINT* pINT);
	void UpdateEIT(int section_number, CEIT* pEIT);

	void UpdateDSMCC(int section_number, CPVT* pPVT);
	void UpdateMPE(int section_number, CMPE* pMPE);

	void Reset(void);
	//void Set(int offline);

protected:
	//void SetTriggerParams(CTrigger_PsiSiSection* pSectionTrigger, uint32_t dwID);


private:
	void DeleteChildItems(HTREEITEM hParentItem);

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPane_PsiSiTableTreeView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_PsiSiTables();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPane_PsiSiTableTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_MUXINFOVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
