#if !defined(AFX_PANE_BOUQUETINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_BOUQUETINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NaviTree_Bouquets.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_Bouquets view
//#include "..\Common\define.h"

#include <afxcview.h>
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"

//要求实现为信息展示的独立功能控件，不依赖其他兄弟窗口，不应该知道父窗口是谁，只要知道父窗口句柄即可
//若确实需要与其他兄弟窗口交互，应将消息中继回父窗口
//chendelin 2019.4.28

//输入：BAT表，见中间件层PsiSiTable的定义
//      数据库
//      接受消息的（父）窗口句柄
//输出：通过用户自定义消息的形式告知调用者当前所选中业务的bouquet_id

#define WM_USER_BOUQUET_SEL_CHANGE		 WM_USER + 0x8911

class CNaviTree_BouquetGuide : public CTreeView
{
public:
	CNaviTree_BouquetGuide();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_BouquetGuide)

public:

// Attributes
protected:
	CImageList		m_ImageList;
	HTREEITEM		m_hRootItem;

	HWND			m_hwndReceiver;
// Operations
public:
	void UpdateBAT(CBAT* pBAT);
	void Set(HWND hwndReceiver, int offline = 1);
	void Reset(void);

protected:
	void DeleteChildItems(HTREEITEM hParentItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNaviTree_BouquetGuide)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_BouquetGuide();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNaviTree_BouquetGuide)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_BOUQUETINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
