// TSMagicFrm.cpp : implementation of the CTSMagicFrame class
//

#include "stdafx.h"
//#include "EverStation.h"

#include "TSMagicFrm.h"
#include ".\tsmagicfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTSMagicFrame

IMPLEMENT_DYNCREATE(CTSMagicFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CTSMagicFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CTSMagicFrame)
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	//ON_WM_CLOSE()
	//}}AFX_MSG_MAP
//	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTSMagicFrame construction/destruction

CTSMagicFrame::CTSMagicFrame()
{
	// TODO: add member initialization code here
	//m_pwndView = NULL;
}

CTSMagicFrame::~CTSMagicFrame()
{
}

BOOL CTSMagicFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if( !CMDIChildWndEx::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}

void CTSMagicFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Modify this function to change how the frame is activated.

	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWndEx::ActivateFrame(nCmdShow);
}


/////////////////////////////////////////////////////////////////////////////
// CTSMagicFrame diagnostics

#ifdef _DEBUG
void CTSMagicFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CTSMagicFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTSMagicFrame message handlers
int CTSMagicFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (m_pwndView == NULL)
	//{
	//	m_pwndView = new CTSMagicView;
	//	m_pwndView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
	//}

	return 0;
}

void CTSMagicFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWndEx::OnSetFocus(pOldWnd);

	//if (m_pwndView != NULL)
	//{
	//	m_pwndView->SetFocus();
	//}
}

BOOL CTSMagicFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// let the view have first crack at the command
	//if (m_pwndView != NULL)
	//{
	//	if (m_pwndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
	//		return TRUE;
	//}

	// otherwise, do default handling
	return CMDIChildWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CTSMagicFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	//if (m_pwndView != NULL)
	//{
	//	if (m_pwndView->m_kThreadParams.main_thread_running == 1)
	//	{
	//		AfxMessageBox("TSMagicFrame仍在运行，强行关闭会破坏系统!");
	//	}
	//	else
	//	{
	//		CMDIChildWndEx::OnClose();
	//	}
	//}
	//else
	//{
	//	CMDIChildWndEx::OnClose();
	//}

	CMDIChildWndEx::OnClose();
}

