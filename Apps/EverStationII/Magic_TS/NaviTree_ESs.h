#if !defined(AFX_PANE_PESESPIDTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_PESESPIDTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

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

class CNaviTree_ESs : public CTreeView
{
public:
	CNaviTree_ESs();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_ESs)

// Attributes
protected:
	HTREEITEM		m_hPesEsRootItem;
	CImageList		m_ImageList;

// Operations
public:
	void UpdatePAT(CPAT* pPAT);
	void UpdatePMT(CPMT* pPMT);

	void Reset(void);

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
	virtual ~CNaviTree_ESs();
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
public:
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEsVideoPreview();
	afx_msg void OnEsAudioPreview();
	afx_msg void OnEsSyntaxAnalyse();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_PESESPIDTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
