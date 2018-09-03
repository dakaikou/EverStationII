#if !defined(AFX_MPEG2_EPGSCHEDULELISTVIEW_H__18294B90_88A4_4081_A149_92320DB867CB__INCLUDED_)
#define AFX_MPEG2_EPGSCHEDULELISTVIEW_H__18294B90_88A4_4081_A149_92320DB867CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mpeg2_EpgScheduleListView.h : header file
//
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
/////////////////////////////////////////////////////////////////////////////
// CListView_EpgSchedule view
#include <afxcview.h>

class CListView_EpgSchedule : public CListView
{
public:
	CListView_EpgSchedule();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListView_EpgSchedule)

// Attributes
public:

// Operations
public:
	void Reset(void);
	void Set(int offline);

	void Update(uint16_t original_network_id, uint16_t transport_stream_id, uint16_t service_id);
protected:
	//void AddScheduleInfo(EVENT_INFO_t* pastEvent, unsigned short usEventCount);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListView_EpgSchedule)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListView_EpgSchedule();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CListView_EpgSchedule)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG2_EPGSCHEDULELISTVIEW_H__18294B90_88A4_4081_A149_92320DB867CB__INCLUDED_)
