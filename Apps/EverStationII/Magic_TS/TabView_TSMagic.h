#if !defined(AFX_TABVIEW_TSMAGIC_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)
#define AFX_TABVIEW_TSMAGIC_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CTabView_TSMagic 视图
#include <afxcview.h>
#include <afxTabView.h>

class CTabView_TSMagic : public CTabView
{
	DECLARE_DYNCREATE(CTabView_TSMagic)

protected:
	CTabView_TSMagic();           // 动态创建所使用的受保护的构造函数
	virtual ~CTabView_TSMagic();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};

#endif  //!defined(AFX_TABVIEW_TSMAGIC_H__144A6533_0DA3_4FAE_A2D3_A9BD12BB1DDA__INCLUDED_)

