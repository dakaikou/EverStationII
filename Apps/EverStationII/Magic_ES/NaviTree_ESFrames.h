#if !defined(AFX_NAVITREE_ESGRAMES_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_NAVITREE_ESGRAMES_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NaviTree_ESFrames.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_ESFrames view

#include <afxcview.h>

//要求实现为信息展示的独立功能控件，不依赖其他兄弟窗口，不应该知道父窗口是谁，只要知道父窗口句柄即可
//若确实需要与其他兄弟窗口交互，应将消息中继回父窗口
//chendelin 2019.4.29

//输入：PSI/SI表，见中间件层PsiSiTable的定义
//      数据库
//      接受消息的（父）窗口句柄
//输出：通过用户自定义消息的形式告知调用者当前所选中业务的ES_id

#define WM_USER_ES_FRAME_SEL_CHANGE		 WM_USER + 0x8F01

class CNaviTree_ESFrames : public CTreeView
{
public:
	CNaviTree_ESFrames();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_ESFrames)

// Attributes
protected:
	HTREEITEM		m_hPesEsRootItem;
	CImageList		m_ImageList;

	HWND			m_hwndReceiver;
// Operations
public:
	//void UpdatePAT(CPAT* pPAT);
	//void UpdatePMT(CPMT* pPMT);

	void Reset(void);
	void Set(HWND hwndReceiver, int offline = 1);
private:
	void DeleteChildItems(HTREEITEM hParentItem);
	//void FiringCatchThread(void);

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
	virtual ~CNaviTree_ESFrames();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CPane_PsiSiTableTreeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
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

#endif // !defined(AFX_NAVITREE_ESGRAMES_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
