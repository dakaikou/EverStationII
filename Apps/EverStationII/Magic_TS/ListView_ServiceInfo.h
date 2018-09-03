#if !defined(AFX_MPEG2_SERVICEINFOLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
#define AFX_MPEG2_SERVICEINFOLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mpeg2_ServiceInfoListView.h : header file
//
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo view
#include <afxcview.h>

class CListView_ServiceInfo : public CListView
{
public:
	CListView_ServiceInfo();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListView_ServiceInfo)

// Attributes
public:

// Operations
public:
	void Reset(void);
	void Set(int offline);
	void Update(uint16_t network_id, uint16_t stream_id, uint16_t service_id);
	//void UpdateNetworkInfo(int network_index);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListView_ServiceInfo)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListView_ServiceInfo();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CListView_ServiceInfo)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG2_SERVICEINFOLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
