#if !defined(AFX_YUVPLAYER_H__EB427D1E_7BDA_42B6_AB92_F71A784E1DAD__INCLUDED_)
#define AFX_YUVPLAYER_H__EB427D1E_7BDA_42B6_AB92_F71A784E1DAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// YUVPlayer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CYUVPlayer frame

class CYUVPlayer : public CFrameWnd
{
	DECLARE_DYNCREATE(CYUVPlayer)
public:
	CYUVPlayer();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CYUVPlayer)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CYUVPlayer();

	// Generated message map functions
	//{{AFX_MSG(CYUVPlayer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_YUVPLAYER_H__EB427D1E_7BDA_42B6_AB92_F71A784E1DAD__INCLUDED_)
