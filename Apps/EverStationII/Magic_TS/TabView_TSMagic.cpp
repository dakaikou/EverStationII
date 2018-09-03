// TabView_TSMagic.cpp : 实现文件
//

#include "stdafx.h"
#include "TabView_TSMagic.h"


// CTabView_TSMagic

IMPLEMENT_DYNCREATE(CTabView_TSMagic, CTabView)

CTabView_TSMagic::CTabView_TSMagic()
{

}

CTabView_TSMagic::~CTabView_TSMagic()
{
}

BEGIN_MESSAGE_MAP(CTabView_TSMagic, CTabView)
END_MESSAGE_MAP()


// CTabView_TSMagic 诊断

#ifdef _DEBUG
void CTabView_TSMagic::AssertValid() const
{
	CTabView::AssertValid();
}

#ifndef _WIN32_WCE
void CTabView_TSMagic::Dump(CDumpContext& dc) const
{
	CTabView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTabView_TSMagic 消息处理程序
