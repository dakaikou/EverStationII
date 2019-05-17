#pragma once


// CNaviList_ESFrames 视图
#include <afxcview.h>

class CNaviList_ESFrames : public CListView
{
	DECLARE_DYNCREATE(CNaviList_ESFrames)

protected:
	CNaviList_ESFrames();           // 动态创建所使用的受保护的构造函数
	virtual ~CNaviList_ESFrames();
public:
	void Reset(void);
	void Set(int offline = 1);

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


