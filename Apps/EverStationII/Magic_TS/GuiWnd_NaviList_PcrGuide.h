#pragma once


// CNaviList_PcrGuide 视图
#include <afxcview.h>

class CNaviList_PcrGuide : public CListView
{
	DECLARE_DYNCREATE(CNaviList_PcrGuide)

protected:
	CNaviList_PcrGuide();           // 动态创建所使用的受保护的构造函数
	virtual ~CNaviList_PcrGuide();
public:
	void Reset(void);
	void Set(int offline = 1);
	//void Update(uint16_t network_id, uint16_t stream_id, uint16_t service_id);

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


