// ListView_PcrInfo.cpp: 实现文件
//
#include "stdafx.h"

//#include "pch.h"
#include "..\EverStationII.h"
#include "NaviList_PCRs.h"


// CListView_PcrInfo

IMPLEMENT_DYNCREATE(CNaviList_PCRs, CListView)

CNaviList_PCRs::CNaviList_PCRs()
{

}

CNaviList_PCRs::~CNaviList_PCRs()
{
}

BEGIN_MESSAGE_MAP(CNaviList_PCRs, CListView)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CListView_PcrInfo 诊断

#ifdef _DEBUG
void CNaviList_PCRs::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void CNaviList_PCRs::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG


// CListView_PcrInfo 消息处理程序


BOOL CNaviList_PCRs::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;

	return CListView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CNaviList_PCRs::Reset(void)
{
	CListCtrl& listCtrl = GetListCtrl();

	listCtrl.DeleteAllItems();
}

void CNaviList_PCRs::Set(int offline)
{
}



int CNaviList_PCRs::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CListCtrl& listCtrl = GetListCtrl();

	listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	//listCtrl.InsertColumn(0, "", LVCFMT_LEFT, 20, -1);
	//listCtrl.InsertColumn(1, "条目", LVCFMT_RIGHT, 240, -1);
	//listCtrl.InsertColumn(2, "描述", LVCFMT_LEFT, 400, -1);

	listCtrl.InsertColumn(0, "PCR_PID", LVCFMT_CENTER, 80, -1);				//icon
	listCtrl.InsertColumn(1, "科目", LVCFMT_RIGHT, 70, -1);
	listCtrl.InsertColumn(2, "检测项", LVCFMT_RIGHT, 90, -1);
	//width -= 240;
	listCtrl.InsertColumn(3, "检测结果", LVCFMT_LEFT, 100, -1);

	return 0;
}


void CNaviList_PCRs::OnSize(UINT nType, int cx, int cy)
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
