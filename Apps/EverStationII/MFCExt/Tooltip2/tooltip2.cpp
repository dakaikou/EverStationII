//File: ToolTip2.cpp
//Implementation file for class CToolTip2
//Copyright (C) 2000  Dhandapani Ammasai( dammasai@hotmail.com )
//Mail your comments, criticism, bugs to the above e-mail address.
#include "stdafx.h"
#include "tooltip2.h"

IMPLEMENT_DYNAMIC( CToolTip2, CWnd );

BEGIN_MESSAGE_MAP(CToolTip2, CWnd)
	//{{AFX_MSG_MAP(CToolTip2)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//Default Constructor
CToolTip2::CToolTip2()
{
//	m_szText = "";
	m_nItemCount = 0;
	m_pParentWnd = NULL;
	m_bShowStatus = FALSE;
}

//Destructor
CToolTip2::~CToolTip2()
{
}

//Create Tool Tip
BOOL CToolTip2::Create(CWnd* pParentWnd)
{
	ASSERT(this != NULL );
	ASSERT(pParentWnd != NULL);

	m_pParentWnd = pParentWnd;
	//Create font
	m_font.CreateFont(15, 0, 0, 0, FW_REGULAR, 0, 0, 0, 0, 0, 0, 0, 0, "MS SansSerif");
	CRect rectInitialSize(0,0,0,0);//Initial Window size. Will be dynamically changed later.
	return CreateEx(NULL, NULL, NULL,WS_POPUP |  WS_CHILD | WS_CLIPSIBLINGS, rectInitialSize,pParentWnd, NULL, NULL);
}

//Set tooltip text
void CToolTip2::SetText(const CString& rsText)
{
	ASSERT(this != NULL );

	m_szText[0] = rsText;
	m_szText[0].TrimRight();
	m_nItemCount = 1;
}

void CToolTip2::ClearText(void)
{
	m_nItemCount = 0;
}

void CToolTip2::AppendText(const CString& rsText)
{
	if (m_nItemCount < MAX_TOOLTIP_LINES)
	{
		m_szText[m_nItemCount] = rsText;
		m_nItemCount ++;
	}
}

//Show tooltip
//rCurrentPoint specifies current mouse position and it is in client coordinates of parent window(Not in screen coordinates).
BOOL CToolTip2::Show(const CPoint& rCurrentPoint)
{
	ASSERT(this != NULL );
	ASSERT(m_hWnd != NULL );

	//Is text empty or tool tip already displayed?
//	if ( m_szText.IsEmpty() || m_bShowStatus)
//		return FALSE;
	if (m_nItemCount == 0)
	{
		return FALSE;
	}

	m_ptCurrent = rCurrentPoint;
	m_bShowStatus = TRUE;
	//Show tool tip
	DisplayToolTip(rCurrentPoint);
	return TRUE;
}

//Close the tooltip
void CToolTip2::Close()
{
	ASSERT(this != NULL );
	ASSERT(m_hWnd != NULL );

	ShowWindow(SW_HIDE); //Hide tooltip
	m_bShowStatus = FALSE;
}

void CToolTip2::OnPaint()
{
	CPaintDC dc(this);

	DisplayToolTip(m_ptCurrent);
}

//Display tooltip
void CToolTip2::DisplayToolTip(const CPoint& rCurrentPoint)
{
	int		width = 0;
	int		i;
	int		item = 0;
	CSize	size;

	CDC* pDC = GetDC();

	CBrush		*pOldBrush;

	CFont *pOldFont;
	pOldFont = pDC->SelectObject(&m_font);

	for (i = 0; i < m_nItemCount; i++)
	{
		size = pDC->GetTextExtent(m_szText[i]);
		if (size.cx > width)
		{
			width = size.cx;
			item = 0;
		}
	}

	size = pDC->GetTextExtent(m_szText[item]);
	pDC->LPtoDP(&size);
	//Form tooltip rectangle
	CRect rectToolTip(rCurrentPoint.x, rCurrentPoint.y, rCurrentPoint.x+size.cx+7, rCurrentPoint.y+size.cy * m_nItemCount + 2);

	//Draw Tooltip Rect and Text
	//
	pDC->SetBkMode(TRANSPARENT);
	CBrush brushToolTip(GetSysColor(COLOR_INFOBK));
	pOldBrush = pDC->SelectObject(&brushToolTip);

	//Create and select thick black pen
	CPen penBlack(PS_SOLID, 0, COLORREF(RGB(0, 0, 0)));
	CPen* pOldPen = pDC->SelectObject(&penBlack);

	//Draw rectangle filled with COLOR_INFOBK
	pDC->Rectangle(0,0,rectToolTip.Width(),rectToolTip.Height());

	//Draw tooltip text
   	pDC->SetTextColor( GetSysColor(COLOR_INFOTEXT) );//Tool Tip color set in control panel settings
	pDC->SetTextAlign(TA_LEFT);

	int x = 3;
	int y = 1;
	for (i = 0; i < m_nItemCount; i++)
	{
		pDC->TextOut(x, y, m_szText[i]);
		y += size.cy;
	}

	CRect rectWnd = rectToolTip;
	m_pParentWnd->ClientToScreen(rectWnd); //Convert from client to screen coordinates
	CPoint ptToolTipLeft = rectWnd.TopLeft();

	//Now display tooltip
	SetWindowPos(&wndTop,ptToolTipLeft.x+1, ptToolTipLeft.y+1, rectWnd.Width(), rectWnd.Height(),SWP_SHOWWINDOW|SWP_NOOWNERZORDER|SWP_NOACTIVATE);

	//put back old objects
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldFont);

	ReleaseDC(pDC);
}


