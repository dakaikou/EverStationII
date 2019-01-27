#if !defined(AFX_MPEG2_APPLOGLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
#define AFX_MPEG2_APPLOGLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
/////////////////////////////////////////////////////////////////////////////
// CListView_RunningLog view
typedef struct
{
	int				type;
	COleDateTime	time;
	char			pszText[256];

} LOGITEM_t;

#define LOG_ITEM_BUF_COUNT			1024
#define TIMER_SHOW_LOG				2341
#define RUNNING_LOG_MAX_COUNT		4096

class CListCtrl_RunningLog : public CListCtrl
{
	// 构造
public:
	CListCtrl_RunningLog();

	// 实现
public:
	virtual ~CListCtrl_RunningLog();

protected:
	CImageList	m_imageDebug;

	LOGITEM_t	m_logItem[LOG_ITEM_BUF_COUNT];
	int			m_nLogCount;
	HANDLE		m_hLogAccess;
	int			m_nDebugCount;

public:
	void AppendLog(int type, char* pszItem);
	//void AppendLogW(int type, wchar_t* pszItem);
	void Open(void);
	void Close(void);
	void ShowLog(void);

protected:

protected:
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//afx_msg void OnEditCopy();
	//afx_msg void OnEditClear();
	afx_msg void OnViewRunningLog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG2_APPLOGLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
