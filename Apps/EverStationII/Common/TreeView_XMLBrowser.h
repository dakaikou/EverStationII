#if !defined(AFX_VIEW_PACKET_SYNTAX_TREEVIEW_H__36A0A914_61A7_4C6D_876F_98011CEFE911__INCLUDED_)
#define AFX_VIEW_PACKET_SYNTAX_TREEVIEW_H__36A0A914_61A7_4C6D_876F_98011CEFE911__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Mpeg2_PsisiSyntaxTreeView.h : header file
//
#include <afxcview.h>

/////////////////////////////////////////////////////////////////////////////
// CTreeView_PesEsSyntax view
#include "thirdparty_libs\tinyxml2\Include\tinyxml2.h"

class CTreeView_XMLBrowser : public CTreeView
{
public:
	CTreeView_XMLBrowser();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeView_XMLBrowser)

protected:

	CString m_strTitle;
// Attributes
public:
	HTREEITEM		m_hRootItem;
	HWND			m_hNotifyParent;

// Operations
protected:
	//void DeleteChildItems(HTREEITEM hParentItem);

	//void Preorder(HTREEITEM hParent, STreeNode* pNode);      //  前序遍历
	void Preorder(HTREEITEM hParent, tinyxml2::XMLElement* pxmlNode);      //  前序遍历

	//HTREEITEM DisplaySingleNode(HTREEITEM hParent, STreeNode* pNode);
	HTREEITEM DisplaySingleNode(HTREEITEM hParent, tinyxml2::XMLElement* pxmlNode);

public:
	void ShowXMLDoc(tinyxml2::XMLDocument* pxmlDoc);
	void Reset(void);
	void Init(char* pszTitle);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeView_PesEsSyntax)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeView_XMLBrowser();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeView_PesEsSyntax)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VIEW_PACKET_SYNTAX_TREEVIEW_H__36A0A914_61A7_4C6D_876F_98011CEFE911__INCLUDED_)
