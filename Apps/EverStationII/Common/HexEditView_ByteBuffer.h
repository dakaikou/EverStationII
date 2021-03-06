#if !defined(AFX_BYTE_BUFFER_HEX_EDIT_VIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
#define AFX_BYTE_BUFFER_HEX_EDIT_VIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

#pragma once

#include <afxcview.h>
#include <afxext.h>


// CHexEditView_ByteBuffer 视图

class CHexEditView_ByteBuffer : public CEditView
{
	DECLARE_DYNCREATE(CHexEditView_ByteBuffer)

	enum EDITMODE {
		EDIT_NONE,
		EDIT_ASCII,
		EDIT_HIGH,
		EDIT_LOW
	};
	// Attributes
protected:
	LPBYTE		m_pData;			// pointer to data
	int			m_length;			// length of data
	int			m_topindex;			// offset of first visible byte on screen

	int			m_currentAddress;	// address under cursor
	EDITMODE	m_currentMode;		// current editing mode: address/hex/ascii
	int			m_selStart;			// start address of selection
	int			m_selEnd;			// end address of selection

	int			m_bpr;				// byte per row 
	int			m_lpp;				// lines per page
	BOOL		m_bShowAddress;
	BOOL		m_bShowAscii;
	BOOL		m_bShowHex;
	BOOL		m_bAddressIsWide;

	BOOL		m_bNoAddressChange;	// internally used
	BOOL		m_bHalfPage;

	CFont		m_Font;
	int			m_lineHeight;
	int			m_nullWidth;
	BOOL		m_bUpdate;

	int			m_offHex,
				m_offAscii,
				m_offAddress;

	CPoint		m_editPos;

    int         m_AddOff;           //附加的偏移地址
	// Implementation
public:
	int GetData(LPBYTE p, int len);
	void SetData(LPBYTE p, int len);
	CSize GetSel(void);
	void SetSel(int s, int e);
	void SetBPR(int bpr);
	void SetOptions(BOOL a, BOOL h, BOOL c, BOOL w);

	void Reset(void);
	void DisplayByteBuffer(unsigned char* buf, int length);

protected:
	void		ScrollIntoView(int p);
	void		RepositionCaret(int p);
	void		Move(int x, int y);
	inline BOOL IsSelected(void);
	void		UpdateScrollbars(void);
	void		CreateEditCaret(void);
	void		CreateAddressCaret(void);
	CPoint		CalcPos(int x, int y);
	void		SelInsert(int s, int l);
	void		SelDelete(int s, int e);
	inline void NormalizeSel(void);

protected:
	CHexEditView_ByteBuffer();           // 动态创建所使用的受保护的构造函数
	virtual ~CHexEditView_ByteBuffer();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};

#endif //AFX_BYTE_BUFFER_HEX_EDIT_VIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

