#if !defined(AFX_PANE_PSISISYNTAXTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_PSISISYNTAXTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NaviTree_PsiSiTables.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_PsiSiTables view
//#include "..\Common\define.h"

#include <afxcview.h>
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_DSMCC_Table.h"

//要求实现为信息展示的独立功能控件，不依赖其他兄弟窗口，不应该知道父窗口是谁，只要知道父窗口句柄即可
//若确实需要与其他兄弟窗口交互，应将消息中继回父窗口
//chendelin 2019.4.29

//输入：PSI/SI表，见中间件层PsiSiTable的定义
//      数据库
//      接受消息的（父）窗口句柄
//输出：通过用户自定义消息的形式告知调用者当前所选中业务的bouquet_id

#define WM_USER_PSISI_SEL_CHANGE		 WM_USER + 0x2c30

class CNaviTree_PsiSiSectionGuide : public CTreeView
{
public:
	CNaviTree_PsiSiSectionGuide();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_PsiSiSectionGuide)

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

	HWND			m_hwndReceiver;

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
	void Set(HWND hwndReceiver, int offline = 1);

protected:
	//void SetTriggerParams(CTrigger_PsiSiSection* pSectionTrigger, uint32_t dwID);


private:
	void DeleteChildItems(HTREEITEM hParentItem);

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNaviTree_PsiSiSectionGuide)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_PsiSiSectionGuide();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNaviTree_PsiSiSectionGuide)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_MUXINFOVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
