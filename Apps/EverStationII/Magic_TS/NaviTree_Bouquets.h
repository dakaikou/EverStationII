#if !defined(AFX_PANE_BOUQUETINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_BOUQUETINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Pane_BouquetInfoTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPane_BouquetInfoTreeView view
#include "..\Common\define.h"
#include "../Common/TreeView_XMLBrowser.h"
#include "..\Magic_TS\TSMagic_GuiApi.h"

#include <afxcview.h>

class CNaviTree_Bouquets : public CTreeView
{
public:
	CNaviTree_Bouquets();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_Bouquets)

public:
	CTreeView_PacketSyntax * m_pInfoTree;

// Attributes
protected:
	CImageList		m_ImageList;
	HTREEITEM		m_hRootItem;
// Operations
public:
	void UpdateBAT(CBAT* pBAT);
//	void Set(int offline);
	void Reset(void);

protected:
	void DeleteChildItems(HTREEITEM hParentItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPane_BouquetInfoTreeView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_Bouquets();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPane_BouquetInfoTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_BOUQUETINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
