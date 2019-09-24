#if !defined(AFX_NAVITREE_ESPIDS_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)
#define AFX_NAVITREE_ESPIDS_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NaviTree_ESPIDs.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNaviTree_PesEsGuide view

#include <afxcview.h>
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"

//Ҫ��ʵ��Ϊ��Ϣչʾ�Ķ������ܿؼ��������������ֵܴ��ڣ���Ӧ��֪����������˭��ֻҪ֪�������ھ������
//��ȷʵ��Ҫ�������ֵܴ��ڽ�����Ӧ����Ϣ�м̻ظ�����
//chendelin 2019.4.29

//���룺PSI/SI�������м����PsiSiTable�Ķ���
//      ���ݿ�
//      ������Ϣ�ģ��������ھ��
//�����ͨ���û��Զ�����Ϣ����ʽ��֪�����ߵ�ǰ��ѡ��ҵ���ES_id

#define WM_USER_ES_PID_SEL_CHANGE		 WM_USER + 0x031F

class CNaviTree_PesEsGuide : public CTreeView
{
public:
	CNaviTree_PesEsGuide();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviTree_PesEsGuide)

// Attributes
protected:
	HTREEITEM		m_hPesEsRootItem;
	CImageList		m_ImageList;

	HWND			m_hwndReceiver;
// Operations
public:
	void UpdatePAT(CPAT* pPAT);
	void UpdatePMT(CPMT* pPMT);

	void Reset(void);
	void Set(HWND hwndReceiver, int offline = 1);
private:
	void DeleteChildItems(HTREEITEM hParentItem);
	//void FiringCatchThread(void);

private:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNaviTree_PesEsGuide)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNaviTree_PesEsGuide();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CNaviTree_PesEsGuide)
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

#endif // !defined(AFX_NAVITREE_ESPIDS_H__BB4F1884_8E1B_11D6_B5D2_000629D3759C__INCLUDED_)