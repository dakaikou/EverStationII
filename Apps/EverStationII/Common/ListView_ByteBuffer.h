#if !defined(AFX_MPEG2_PSISISYNTAXLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
#define AFX_MPEG2_PSISISYNTAXLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mpeg2_PsisiSyntaxListView.h : header file
//

#include <afxcview.h>
/////////////////////////////////////////////////////////////////////////////
// CListView_ByteBuffer view

class CListView_ByteBuffer : public CListView
{
public:
	CListView_ByteBuffer();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CListView_ByteBuffer)

// Attributes
protected:
	int m_nColCount;

	int	m_nTotalRowCount;
	int m_nShowedCount;

	unsigned char* m_pucTempBuffer;
	int			   m_nTempLength;
	int				m_nNewComing;

	HANDLE		m_hAccessEvent;

// Operations
public:
	void Reset(void);
	void DisplayByteBuffer(unsigned char* buf, int length);
	void PrepareRemainedRow(void);

protected:
	void DisplayRemainBuffer(unsigned char* buf, int length);
	void DisplayFirstViewedBuffer(unsigned char* buf, int length);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListView_ByteBuffer)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CListView_ByteBuffer();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CListView_ByteBuffer)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MPEG2_PSISISYNTAXLISTVIEW_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
