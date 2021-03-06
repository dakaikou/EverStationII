// BitmapButton_HoverEffect.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "BitmapButton_HoverEffect.h"


// CBitmapButton_HoverEffect

IMPLEMENT_DYNAMIC(CBitmapButton_HoverEffect, CBitmapButton)

CBitmapButton_HoverEffect::CBitmapButton_HoverEffect()
{
	m_bTracking = FALSE;
	m_bHovering = FALSE;
	//m_bLBtnDown = FALSE;
}

CBitmapButton_HoverEffect::~CBitmapButton_HoverEffect()
{
}


BEGIN_MESSAGE_MAP(CBitmapButton_HoverEffect, CBitmapButton)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()
//	ON_WM_KILLFOCUS()
	//ON_WM_SHOWWINDOW()
	//ON_WM_CREATE()
	//ON_WM_SIZE()
END_MESSAGE_MAP()



// CBitmapButton_HoverEffect 消息处理程序




void CBitmapButton_HoverEffect::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (!m_bTracking)
	{
		//   鼠标移入窗时，请求WM_MOUSEHOVER和WM_MOUSELEAVE 消息
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;

		m_bTracking = _TrackMouseEvent(&tme);
	}

	CBitmapButton::OnMouseMove(nFlags, point);
}


void CBitmapButton_HoverEffect::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bTracking = FALSE;
	m_bHovering = TRUE;

	CDC* pBtnDC = GetDC();
	CDC memDC;
	memDC.CreateCompatibleDC(pBtnDC);
	CBitmap* pOld = memDC.SelectObject(&m_bitmapFocus);
	if (pOld == NULL)
		return;     // destructors will clean up 

	CRect rect;
	GetClientRect(&rect);
	pBtnDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);

	ReleaseDC(pBtnDC);

	Invalidate(FALSE);

	CBitmapButton::OnMouseHover(nFlags, point);
}


void CBitmapButton_HoverEffect::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bTracking = FALSE;
	m_bHovering = FALSE;

	CDC* pBtnDC = GetDC();
	CDC memDC;
	memDC.CreateCompatibleDC(pBtnDC);
	CBitmap* pOld = memDC.SelectObject(&m_bitmap);
	if (pOld == NULL)
		return;     // destructors will clean up 

	CRect rect;
	GetClientRect(&rect);
	pBtnDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);

	ReleaseDC(pBtnDC);

	Invalidate(FALSE);

	CBitmapButton::OnMouseLeave();
}



void CBitmapButton_HoverEffect::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	// TODO:  添加您的代码以绘制指定项
	if (lpDIS != NULL)
	{
		UINT state = lpDIS->itemState;
		UINT action = lpDIS->itemAction;

		CBitmap* pBitmap = NULL;

		if (m_bHovering == FALSE)
		{
			if (action == ODA_DRAWENTIRE)
			{
				if (state & ODS_DISABLED)
				{
					pBitmap = &m_bitmapDisabled;
				}
				//else if (state & ODS_FOCUS)
				//{
				//	if (state & ODS_SELECTED)
				//	{
				//		pBitmap = &m_bitmapSel;
				//	}
				//	else
				//	{
				//		pBitmap = &m_bitmapFocus;
				//	}
				//}
				else if (state & ODS_NOFOCUSRECT)
				{
					pBitmap = &m_bitmap;

					//DWORD dwStyle = GetStyle();
					//if ((dwStyle & BS_AUTOCHECKBOX) == BS_AUTOCHECKBOX)
					//{
					//	UINT nState = GetState();
					//	if ((nState & 0x00000003) == 0x00000000)
					//	{
					//		pBitmap = &m_bitmapFocus;
					//	}
					//	else
					//	{
					//		pBitmap = &m_bitmap;
					//	}
					//}
					//else
					//{
						//pBitmap = &m_bitmap;
					//}
				}
				else
				{
					pBitmap = NULL;
				}
			}
		}
		else
		{
			if (action == ODA_SELECT)
			{
				if (state & ODS_FOCUS)
				{
					if (state & ODS_SELECTED)
					{
						pBitmap = &m_bitmapSel;
					}
					else
					{
						pBitmap = &m_bitmapFocus;
					}
				}
			}
			else if (action == ODA_FOCUS)
			{
				if (state & ODS_FOCUS)
				{
					if (state & ODS_SELECTED)
					{
						pBitmap = &m_bitmapSel;
					}
					else
					{
						pBitmap = &m_bitmapFocus;
					}
				}
			}
		}

		if (pBitmap != NULL)
		{
			CDC* pBtnDC = CDC::FromHandle(lpDIS->hDC);
			CDC memDC;
			memDC.CreateCompatibleDC(pBtnDC);
			CBitmap* pOld = memDC.SelectObject(pBitmap);
			if (pOld == NULL)
				return;     // destructors will clean up 

			CRect rect;
			rect.CopyRect(&lpDIS->rcItem);

			pBtnDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
			memDC.SelectObject(pOld);

			ReleaseDC(pBtnDC);
		}
	}
}


//void CBitmapButton_HoverEffect::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	//m_bLBtnDown = TRUE;
//
//	//CDC* pBtnDC = GetDC();
//	//CDC memDC;
//	//memDC.CreateCompatibleDC(pBtnDC);
//	//CBitmap* pOld = memDC.SelectObject(&m_bitmapSel);
//	//if (pOld == NULL)
//	//	return;     // destructors will clean up 
//
//	//CRect rect;
//	//GetClientRect(&rect);
//	//pBtnDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
//	//memDC.SelectObject(pOld);
//
//	//ReleaseDC(pBtnDC);
//
//	//Invalidate(FALSE);
//
//	CBitmapButton::OnLButtonDown(nFlags, point);
//}


//void CBitmapButton_HoverEffect::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	//if (m_bLBtnDown)
//	//{
//	//	CDC* pBtnDC = GetDC();
//	//	CDC memDC;
//	//	memDC.CreateCompatibleDC(pBtnDC);
//	//	CBitmap* pOld = memDC.SelectObject(&m_bitmapFocus);
//	//	if (pOld == NULL)
//	//		return;     // destructors will clean up 
//
//	//	CRect rect;
//	//	GetClientRect(&rect);
//	//	pBtnDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
//	//	memDC.SelectObject(pOld);
//
//	//	ReleaseDC(pBtnDC);
//
//	//	Invalidate(FALSE);
//
//	//	m_bLBtnDown = FALSE;
//	//}
//
//	CBitmapButton::OnLButtonUp(nFlags, point);
//}


//void CBitmapButton_HoverEffect::OnKillFocus(CWnd* pNewWnd)
//{
//	CBitmapButton::OnKillFocus(pNewWnd);
//
//	// TODO: 在此处添加消息处理程序代码
//}


//void CBitmapButton_HoverEffect::OnShowWindow(BOOL bShow, UINT nStatus)
//{
//	CBitmapButton::OnShowWindow(bShow, nStatus);
//
//	// TODO: 在此处添加消息处理程序代码
//	//GetWindowPlacement(&m_stOldWndPlacement);
//	
//	//	CDC* pBtnDC = GetDC();
////	CDC memDC;
////	memDC.CreateCompatibleDC(pBtnDC);
////	CBitmap* pOld = memDC.SelectObject(&m_bitmap);
////	if (pOld == NULL)
////		return;     // destructors will clean up 
////
////	CRect rect;
////	GetClientRect(&rect);
////	pBtnDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);
////	memDC.SelectObject(pOld);
////
////	ReleaseDC(pBtnDC);
//}


//int CBitmapButton_HoverEffect::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CBitmapButton::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  在此添加您专用的创建代码
//	//CRect rect;
//	//GetWindowRect(&rect);
//	//rect.MoveToXY(0, 0);
//	////注意, 这里一定要加1,否则会导致最后一个像素被裁剪掉.
//	//HRGN rgn = CreateRoundRectRgn(rect.left, rect.top, rect.right + 1, rect.bottom + 1, 10, 10);//圆角半径
//	//SetWindowRgn(rgn, TRUE);
//
//	return 0;
//}


//void CBitmapButton_HoverEffect::OnSize(UINT nType, int cx, int cy)
//{
//	CBitmapButton::OnSize(nType, cx, cy);
//
//	// TODO: 在此处添加消息处理程序代码
//	//CRect rect;
//	//GetWindowRect(&rect);
//	//rect.MoveToXY(0, 0);
//	////注意, 这里一定要加1,否则会导致最后一个像素被裁剪掉.
//	//HRGN rgn = CreateRoundRectRgn(rect.left, rect.top, rect.right + 1, rect.bottom + 1, 10, 10);//圆角半径
//	//SetWindowRgn(rgn, TRUE);
//
//	return;
//}
