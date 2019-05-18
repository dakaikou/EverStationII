#if !defined(AFX_PANE_EPGINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_PANE_EPGINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NaviTree_ServiceGuide.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_ServiceGuide view
#include <afxcview.h>

#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"

//Ҫ��ʵ��Ϊ��Ϣչʾ�Ķ������ܿؼ��������������ֵܴ��ڣ���Ӧ��֪����������˭��ֻҪ֪�������ھ������
//��ȷʵ��Ҫ�������ֵܴ��ڽ�����Ӧ����Ϣ�м̻ظ�����
//chendelin 2019.4.28

//���룺SDT��NIT�����м����PsiSiTable�Ķ���
//      ���ݿ�
//      ������Ϣ�ģ��������ھ��
//�����ͨ���û��Զ�����Ϣ����ʽ��֪�����ߵ�ǰ��ѡ��ҵ���network_id,transport_stream_id��service_id
#define WM_USER_SERVICE_SEL_CHANGE		 WM_USER + 0x7D6E

class CNaviTree_ServiceGuide : public CTreeView
{
public:
	CNaviTree_ServiceGuide();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_ServiceGuide)

// Attributes
protected:

	HTREEITEM		m_hRootItem;
	CImageList		m_ImageList;

	HWND			m_hwndReceiver;

public:

// Operations
public:
	void Reset(void);
	void Set(HWND hwndReceiver, int offline = 1);
	void UpdateSDT(CSDT* pSDT);
	void UpdateNIT(CNIT* pNIT);

protected:
	void DeleteChildItems(HTREEITEM hParentItem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNaviTree_ServiceGuide)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_ServiceGuide();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNaviTree_ServiceGuide)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTvnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PANE_EPGINFOTREEVIEW_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
