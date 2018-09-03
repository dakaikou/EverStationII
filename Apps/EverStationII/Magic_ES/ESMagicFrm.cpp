// TSMagicFrm.cpp : implementation of the CESMagicFrame class
//

#include "stdafx.h"
//#include "EverStation.h"

#include "ESMagicFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CESMagicFrame

IMPLEMENT_DYNCREATE(CESMagicFrame, CMDIChildWndEx)

BEGIN_MESSAGE_MAP(CESMagicFrame, CMDIChildWndEx)
	//{{AFX_MSG_MAP(CESMagicFrame)
		//ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
		ON_WM_SETFOCUS()
		ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CESMagicFrame construction/destruction

CESMagicFrame::CESMagicFrame()
{
	// TODO: add member initialization code here
	m_pwndView = NULL;
}

CESMagicFrame::~CESMagicFrame()
{
//	if (m_pwndView != NULL)
//	{
//		delete m_pwndView;
//		m_pwndView = NULL;
//	}
}

BOOL CESMagicFrame::PreCreateWindow(CREATESTRUCT& cs)
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

void CESMagicFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Modify this function to change how the frame is activated.

	nCmdShow = SW_SHOWMAXIMIZED;
	CMDIChildWndEx::ActivateFrame(nCmdShow);
}


/////////////////////////////////////////////////////////////////////////////
// CESMagicFrame diagnostics

#ifdef _DEBUG
void CESMagicFrame::AssertValid() const
{
	CMDIChildWndEx::AssertValid();
}

void CESMagicFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWndEx::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CESMagicFrame message handlers
//void CESMagicFrame::OnFileClose() 
//{
//	// To close the frame, just send a WM_CLOSE, which is the equivalent
//	// choosing close from the system menu.
//
//	SendMessage(WM_CLOSE);
//}

int CESMagicFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// create a view to occupy the client area of the frame
//	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
//		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
//	{
//		TRACE0("Failed to create view window\n");
//		return -1;
//	}

	if (m_pwndView == NULL)
	{
		m_pwndView = new CESMagicView;
		m_pwndView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
	}

	return 0;
}

void CESMagicFrame::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWndEx::OnSetFocus(pOldWnd);

//	m_wndView.SetFocus();
	if (m_pwndView != NULL)
	{
		m_pwndView->SetFocus();
	}
}

BOOL CESMagicFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// let the view have first crack at the command
//	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
//		return TRUE;

	if (m_pwndView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	
	// otherwise, do default handling
	return CMDIChildWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CESMagicFrame::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
//	if (gkThreadParams.analyzing == 1)
//	{
//		AfxMessageBox("分析程序仍在运行，强行关闭会破坏系统!");
//	}
//	else
//	{
//		CMDIChildWnd::OnClose();
//	}
	
	CMDIChildWndEx::OnClose();
}
