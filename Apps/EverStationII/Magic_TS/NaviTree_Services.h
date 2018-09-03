#if !defined(AFX_PANE_EPGINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_EPGINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Pane_EpgInfoTreeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPane_EpgInfoTreeView view
#include "..\Common\define.h"
#include "..\Magic_TS\TSMagic_GuiApi.h"

#include "View_EpgContainer.h"
//#include "ListView_EpgSchedule.h"
//#include "ListView_ServiceInfo.h"

#include <afxcview.h>

class CNaviTree_Services : public CTreeView
{
public:
	CNaviTree_Services();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_Services)

// Attributes
protected:

	HTREEITEM		m_hRootItem;
	CImageList		m_ImageList;

public:
	CView_EpgContainer*			m_pViewEpg;

// Operations
public:
	void Reset(void);
//	void Set(int offline);
	void UpdateSDT(CSDT* pSDT);
	void UpdateNIT(CNIT* pNIT);

protected:
	void DeleteChildItems(HTREEITEM hParentItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPane_EpgInfoTreeView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_Services();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPane_EpgInfoTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_EPGINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
