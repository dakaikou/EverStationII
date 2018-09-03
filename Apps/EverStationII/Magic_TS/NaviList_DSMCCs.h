#if !defined(AFX_TSMAGIC_NAVIPANE_DSMCCLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
#define AFX_TSMAGIC_NAVIPANE_DSMCCLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mpeg2_ServiceInfoListView.h : header file
//
#include <stdint.h>

/////////////////////////////////////////////////////////////////////////////
// CListView_ServiceInfo view
#include <afxcview.h>

#include "../Common/TreeView_XMLBrowser.h"
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_Table.h"
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_DSMCC_Table.h"

#define LISTITEM_COL_INDEX_MARK				0
#define LISTITEM_COL_INDEX_TS_PID			1
#define LISTITEM_COL_INDEX_SERVICE_ID		2
#define LISTITEM_COL_INDEX_SERVICE_NAME		3
#define LISTITEM_COL_INDEX_COMPONENT_TAG	4
#define LISTITEM_COL_INDEX_ASSOCIATION_TAG	5
#define LISTITEM_COL_INDEX_TRANSACTION_ID	6
#define LISTITEM_COL_INDEX_CAROUSEL_ID		7
#define LISTITEM_COL_INDEX_BROADCAST_ID		8
#define LISTITEM_COL_INDEX_CAROUSEL_TYPE_ID	9
#define LISTITEM_COL_COUNT					10

typedef struct
{
	CString strTitle;
	int		nWidth;
	UINT	uiAlign;
} CAROUSEL_COL_ITEM_t;

class CNaviList_DSMCCs : public CListView
{
public:
	CNaviList_DSMCCs();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CNaviList_DSMCCs)

protected:
	CAROUSEL_COL_ITEM_t  m_stColHeader[LISTITEM_COL_COUNT];

// Attributes
public:
	CTreeView_PacketSyntax * m_pInfoTree;

// Operations
protected:
	void OC_BuildDownloadTable(uint16_t PID, uint32_t downloadId, uint16_t moduleId, uint32_t objectKey_data, char* pszDir);

public:
	void Reset(void);
	void Set(int offline);
	void UpdatePMT(CPMT* pPMT);
	void UpdateSDT(CSDT* pSDT);
	void UpdateDSMCC(CPVT* pPVT);

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
	virtual ~CNaviList_DSMCCs();
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
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnOcdcDownload();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TSMAGIC_NAVIPANE_DSMCCLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
