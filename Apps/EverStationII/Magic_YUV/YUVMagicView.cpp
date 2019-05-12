// YUVMagicView.cpp : implementation file
//

#include "stdafx.h"
#include "..\EverStationII.h"
#include "YUVMagicView.h"
#include "YUVMagicDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CYUVMagicView
//#include "..\Magic_YUV\GuiApi_YUV.h"
#include "..\resource.h"

#include "thirdparty_libs\glog\glog\logging.h"
#include "..\common\define.h"
#include "..\common\GuiApi_MSG.h"

IMPLEMENT_DYNCREATE(CYUVMagicView, CFormView)

CYUVMagicView::CYUVMagicView()
	: CFormView(CYUVMagicView::IDD)
{
	//{{AFX_DATA_INIT(CYUVMagicView)
	//}}AFX_DATA_INIT

	m_bInitDone = 0;

	TAB_YUV_PREVIEW = -1;
	TAB_YUV_EXCHANGE = -1;
	TAB_YUV_SPLITTE = -1;
	TAB_YUV_COMBINE = -1;
}

CYUVMagicView::~CYUVMagicView()
{
}

void CYUVMagicView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CYUVMagicView)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CYUVMagicView, CFormView)
	//{{AFX_MSG_MAP(CYUVMagicView)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_YUVMAGIC, OnSelchangeTabConsole)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_TSMAGIC_OFFLINE_THREAD, OnTSMagicOfflineThreadMsg)
//	ON_MESSAGE(WM_TSMAGIC_REALTIME_THREAD, OnTSMagicRealtimeThreadMsg)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CYUVMagicView diagnostics

#ifdef _DEBUG
void CYUVMagicView::AssertValid() const
{
	CFormView::AssertValid();
}

void CYUVMagicView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CYUVMagicView message handlers

BOOL CYUVMagicView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CYUVMagicView::AdjustLayout(int cx, int cy)
{
	CWnd*	pWnd;
	CRect	rect;
//	int		xoffset;
	int		yoffset;
//	int		width;
	int		height;

	yoffset = 0;

	pWnd = GetDlgItem(IDC_TAB_YUVMAGIC);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		height = cy;
		pWnd->SetWindowPos(NULL, 5, 5, cx - 10, height - 10, 0);

		yoffset = height + 5; 

		CRect		rectTab;
		CRect		rectItem;

		((CTabCtrl*)pWnd)->GetItemRect(0, &rectItem);

		pWnd->GetClientRect(&rectTab);
		rectTab.top += rectItem.Height() + 5;
		rectTab.bottom -= 1;
		rectTab.left += 1;
		rectTab.right -= 1;

#if GUI_YUV_PREVIEW
		m_dlgPreview.MoveWindow(&rectTab);
#endif

#if GUI_YUV_EXCHANGE
		m_dlgExchange.MoveWindow(&rectTab);
#endif

#if GUI_YUV_COMBINE
		m_dlgCombine.MoveWindow(&rectTab);
#endif
	}
}

void CYUVMagicView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	
	// TODO: Add your specialized code here and/or call the base class
	InitConsoleTab();
	
	m_bInitDone = 1;

	CRect	rect;
	GetClientRect(&rect);
	AdjustLayout(rect.Width(), rect.Height());
}

void CYUVMagicView::InitConsoleTab(void)
{
	int i;
	int	item;
	CTabCtrl*	 pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_YUVMAGIC);

	m_imageTab.Create(IDB_CONSOLE_YUV_MAGIC, 36, 1, RGB(0xab,0xcd,0xef));

	pTabCtrl->SetImageList(&m_imageTab);
	pTabCtrl->SetItemSize(CSize(200, 42));

#if GUI_YUV_PREVIEW
	item = pTabCtrl->GetItemCount();
	TAB_YUV_PREVIEW = item;
	pTabCtrl->InsertItem(item, "预览", 0);
#endif

#if GUI_YUV_EXCHANGE
	item = pTabCtrl->GetItemCount();
	TAB_YUV_EXCHANGE = item;
	pTabCtrl->InsertItem(item, "转换", 1);
#endif

#if GUI_YUV_COMBINE
	item = pTabCtrl->GetItemCount();
	TAB_YUV_COMBINE = item;
	pTabCtrl->InsertItem(item, "合并", 1);
#endif

	pTabCtrl->SetCurSel(0);

	for (i = 0; i < pTabCtrl->GetItemCount(); i++)
	{
		m_pdlgWnd[i] = NULL;
	}

#if GUI_YUV_PREVIEW
	m_dlgPreview.Create(IDD_YUV_PREVIEW, GetDlgItem(IDC_TAB_YUVMAGIC));
	m_pdlgWnd[TAB_YUV_PREVIEW] = (CWnd*)&m_dlgPreview;
#endif

#if GUI_YUV_EXCHANGE
	m_dlgExchange.Create(IDD_YUV_EXCHANGE, GetDlgItem(IDC_TAB_YUVMAGIC));
	m_pdlgWnd[TAB_YUV_EXCHANGE] = (CWnd*)&m_dlgExchange;
#endif

#if GUI_YUV_COMBINE
	m_dlgCombine.Create(IDD_YUV_COMBINE, GetDlgItem(IDC_TAB_YUVMAGIC));
	m_pdlgWnd[TAB_YUV_COMBINE] = (CWnd*)&m_dlgCombine;
#endif

	ActivateTabPage(pTabCtrl->GetCurSel());
}

void CYUVMagicView::ActivateTabPage(int nSel)
{
	int		  i;
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_YUVMAGIC);
	CWnd*	  pWnd;

	for (i = 0; i < pTabCtrl->GetItemCount(); i++)
	{
		pWnd = m_pdlgWnd[i];

		if (pWnd != NULL)
		{
			if (i == nSel)
			{
				pWnd->ShowWindow(SW_SHOW);
			}
			else
			{
				pWnd->ShowWindow(SW_HIDE);
			}
		}
	}
}

void CYUVMagicView::OnSize(UINT nType, int cx, int cy) 
{
	CFormView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW)
	{
		return;		// arrangement not needed
	}

	if (m_bInitDone)
	{
		AdjustLayout(cx, cy);
	}
}

void CYUVMagicView::OnSelchangeTabConsole(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_TAB_YUVMAGIC);
	int nSel = pTabCtrl->GetCurSel();

	ActivateTabPage(nSel);
	
	*pResult = 0;
}

void CYUVMagicView::GUIReset(void)
{
	CWnd* pWnd;

	pWnd = GetDlgItem(IDC_EDIT_FILE);			//说明窗口已经撤销
	pWnd->SetWindowText("");

#if GUI_YUV_PREVIEW
	m_dlgPreview.Reset();
#endif

#if GUI_YUV_EXCHANGE
	m_dlgExchange.Reset();
#endif

#if GUI_YUV_COMBINE
	m_dlgCombine.Reset();
#endif
}

CYUVMagicView* CYUVMagicView::GetView(void)
{
	CYUVMagicView* pView = NULL;

	CEverStationIIApp* pApp = (CEverStationIIApp*)AfxGetApp();
	CMultiDocTemplate* pDocTemplate = pApp->m_pDocTemplateForYUV;

	POSITION posDoc = pDocTemplate->GetFirstDocPosition();
	while (posDoc != NULL)
	{
		CYUVMagicDoc* pDoc = (CYUVMagicDoc*)pDocTemplate->GetNextDoc(posDoc);
		POSITION posView = pDoc->GetFirstViewPosition();
		while (posView != NULL)
		{
			pView = (CYUVMagicView*)pDoc->GetNextView(posView);
			break;
		}
	}

	return pView;
}

void CYUVMagicView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
#if GUI_YUV_PREVIEW
	m_dlgPreview.DestroyWindow();
#endif

#if GUI_YUV_EXCHANGE
	m_dlgExchange.DestroyWindow();
#endif

#if GUI_YUV_COMBINE
	m_dlgCombine.DestroyWindow();
#endif
}

