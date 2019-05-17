// NaviList_ESFrames.cpp: 实现文件
//
#include "stdafx.h"

//#include "pch.h"
#include "NaviList_ESFrames.h"


// CNaviList_ESFrames

IMPLEMENT_DYNCREATE(CNaviList_ESFrames, CListView)

CNaviList_ESFrames::CNaviList_ESFrames()
{

}

CNaviList_ESFrames::~CNaviList_ESFrames()
{
}

BEGIN_MESSAGE_MAP(CNaviList_ESFrames, CListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CNaviList_ESFrames 诊断

#ifdef _DEBUG
void CNaviList_ESFrames::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CNaviList_ESFrames::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNaviList_ESFrames 消息处理程序


BOOL CNaviList_ESFrames::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	return CListView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CNaviList_ESFrames::Reset(void)
{
	CListCtrl& listCtrl = GetListCtrl();

	listCtrl.DeleteAllItems();
}

void CNaviList_ESFrames::Set(int offline)
{
}



int CNaviList_ESFrames::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CListCtrl& listCtrl = GetListCtrl();

	listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	listCtrl.InsertColumn(0, "帧序号", LVCFMT_RIGHT, 70, -1);				//icon
	listCtrl.InsertColumn(1, "帧类型", LVCFMT_CENTER, 70, -1);
	listCtrl.InsertColumn(2, "起始位置", LVCFMT_RIGHT, 120, -1);
	listCtrl.InsertColumn(3, "结束位置", LVCFMT_RIGHT, 120, -1);
	listCtrl.InsertColumn(4, "长度", LVCFMT_RIGHT, 120, -1);
	listCtrl.InsertColumn(5, "备注", LVCFMT_CENTER, 120, -1);

	return 0;
}


void CNaviList_ESFrames::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CListCtrl& listCtrl = GetListCtrl();
	CRect rectList;
	listCtrl.GetWindowRect(&rectList);
	int listwidth = rectList.Width();

	CHeaderCtrl* pHeaderCtrl = listCtrl.GetHeaderCtrl();
	int columns = pHeaderCtrl->GetItemCount();

	for (int i = 0; i < columns - 1; i++)
	{
		listwidth -= listCtrl.GetColumnWidth(i);
		listwidth -= 3;
	}
	listCtrl.SetColumnWidth(columns - 1, listwidth);
}
