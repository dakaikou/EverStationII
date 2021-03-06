#pragma once


// CBitmapButton_HoverEffect

class CBitmapButton_HoverEffect : public CBitmapButton
{
	DECLARE_DYNAMIC(CBitmapButton_HoverEffect)

public:
	CBitmapButton_HoverEffect();
	virtual ~CBitmapButton_HoverEffect();

protected:
	BOOL m_bTracking;
	BOOL m_bHovering;
	//BOOL m_bLBtnDown;

	//WINDOWPLACEMENT	m_stOldWndPlacement;		//结构中包含了有关窗口在屏幕上位置的信息

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
};


