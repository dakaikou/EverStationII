// Dlg_Epg.cpp : implementation file
//

#include "stdafx.h"
//#include "everstation.h"
#include "Dlg_TSAnalyzer_Semantic_Epg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Semantic_Epg dialog


CDlg_TSAnalyzer_Semantic_Epg::CDlg_TSAnalyzer_Semantic_Epg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Semantic_Epg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Semantic_Epg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_Semantic_Epg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Semantic_Epg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Semantic_Epg, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Semantic_Epg)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_SERVICE_SEL_CHANGE, OnReportServiceSelChange)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Semantic_Epg message handlers

void CDlg_TSAnalyzer_Semantic_Epg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if (m_wndSplitter.GetSafeHwnd() != NULL)
	{
		m_wndSplitter.SetWindowPos(NULL, 5, 5, cx - 10, cy - 10, 0);
		m_wndSplitter.SetColumnInfo(0, 580, 0);			//设置列宽度
		m_wndSplitter.RecalcLayout();

		m_wndRightSplitter.SetRowInfo(0, 150, 100);		//设置行的高度，最小高度
		m_wndRightSplitter.RecalcLayout();
	}
}

BOOL CDlg_TSAnalyzer_Semantic_Epg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	if (m_wndSplitter.GetSafeHwnd() == NULL)
	{
		m_wndSplitter.CreateStatic(this, 1, 2);
		m_wndSplitter.CreateView(0, 0,RUNTIME_CLASS(CNaviTree_ServiceGuide), CSize(100, 0), NULL);
		//m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CView_EpgContainer), CSize(0, 0), NULL);

		m_wndRightSplitter.CreateStatic(&m_wndSplitter, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter.IdFromRowCol(0, 1));
		m_wndRightSplitter.CreateView(0, 0, RUNTIME_CLASS(CListView_ServiceDescription), CSize(0, 100), NULL);
		m_wndRightSplitter.CreateView(1, 0, RUNTIME_CLASS(CListView_EpgSchedule), CSize(0, 0), NULL);

		CRect rect;

		GetClientRect(&rect);
		rect.left += 5;
		rect.right -= 5;
		rect.top += 5;
		rect.bottom -= 5;

		m_wndSplitter.MoveWindow(&rect);

		m_pServiceListPane = (CNaviTree_ServiceGuide*)m_wndSplitter.GetPane(0, 0);
		m_pServiceListPane->Set(this->GetSafeHwnd());

		//m_pServiceListPane->m_pViewEpg = (CView_EpgContainer*)m_wndSplitter.GetPane(0, 1);
		m_pServiceDescription = (CListView_ServiceDescription*)m_wndRightSplitter.GetPane(0, 0);
		m_pEpgScheduleList = (CListView_EpgSchedule*)m_wndRightSplitter.GetPane(1, 0);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Semantic_Epg::Reset(void)
{
	m_pServiceListPane->Reset();

	m_pServiceDescription->Reset();
	m_pEpgScheduleList->Reset();
}

//void CDlg_TSAnalyzer_Epg::Set(int offline)
//{
//	m_pPane->Set(offline);
//	m_pList->Set(offline);
//}

void CDlg_TSAnalyzer_Semantic_Epg::UpdateSDT(CSDT* pSDT)
{
	m_pServiceListPane->UpdateSDT(pSDT);
}

void CDlg_TSAnalyzer_Semantic_Epg::UpdateNIT(CNIT* pNIT)
{
	m_pServiceListPane->UpdateNIT(pNIT);
}



void CDlg_TSAnalyzer_Semantic_Epg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

LRESULT CDlg_TSAnalyzer_Semantic_Epg::OnReportServiceSelChange(WPARAM wParam, LPARAM lParam)
{
	uint16_t network_id = ((wParam & 0xffff0000) >> 16);
	uint16_t transport_stream_id = (wParam & 0x0000ffff);
	uint32_t service_id = (uint32_t)lParam;

	m_pServiceDescription->Reset();
	m_pServiceDescription->Update(network_id, transport_stream_id, service_id);

	m_pEpgScheduleList->Reset();
	m_pEpgScheduleList->Update(network_id, transport_stream_id, service_id);

	return 0;
}

//BOOL CDlg_TSAnalyzer_Epg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: 在此添加专用代码和/或调用基类
//	NMHDR* nmhdr = (LPNMHDR)lParam;
//
//	//typedef struct _nmhdr {
//	//	HWND hwndFrom;		//A window handle to the control sending the message.
//	//	UINT idFrom;		//An identifier of the control sending the message.
//	//	UINT code;			//A notification code. This member can be one of the common notification codes (see Notifications under General Control Reference), or it can be a control-specific notification code.
//	//} NMHDR;
//
//	if (m_pServiceListPane != NULL)
//	{
//		//语法树和16进制字节表显示联动
//		if (nmhdr->hwndFrom == m_pServiceListPane->GetSafeHwnd())
//		{
//			if (nmhdr->code == NM_DBLCLK)
//			{
//				LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(nmhdr);
//
//				//typedef struct tagNMTREEVIEWA {
//				//	NMHDR   hdr;				//contains information about this notification message.
//				//	UINT    action;				//Notification-specific action flag. This member is used with the following notification codes.
//												//TVN_ITEMEXPANDING
//												//TVN_ITEMEXPANDED
//												//TVN_SELCHANGING
//												//TVN_SELCHANGED
//				//	TVITEMA itemOld;			//contains information about the old item state. This member is zero for notification messages that do not use it.
//				//	TVITEMA itemNew;			//information about the new item state. This member is zero for notification messages that do not use it.
//				//	POINT   ptDrag;				//contains the client coordinates of the mouse at the time the event occurred that caused the notification message to be sent.
//				//} NMTREEVIEWA, * LPNMTREEVIEWA;
//
//
//				TVITEM item = pNMTreeView->itemNew;
//
//				//typedef struct tagTVITEMA {
//				//	UINT      mask;
//				//	HTREEITEM hItem;			//Handle to the item.
//				//	UINT      state;
//				//	UINT      stateMask;
//				//	LPSTR     pszText;
//				//	int       cchTextMax;
//				//	int       iImage;
//				//	int       iSelectedImage;
//				//	int       cChildren;
//				//	LPARAM    lParam;			//A value to associate with the item.
//				//} TVITEMA, * LPTVITEMA;
//
//
//				HTREEITEM hSelItem = item.hItem;
//
//				if (item.mask & TVIF_PARAM)
//				{
//					DWORD uiValue = (DWORD)(item.lParam);
//
//					uint32_t	item_code;
//					uint8_t		item_style;
//					uint16_t	item_param_id;
//						
//					uint16_t	network_id;
//					uint16_t	transport_stream_id;
//					uint16_t	service_id;
//						
//					m_pServiceDescription->Reset();
//					m_pEpgScheduleList->Reset();
//						
//					item_code = uiValue;
//					item_style = (item_code & 0xf0000000) >> 28;
//					item_param_id = (item_code & 0x0000ffff);
//						
//					if (item_style == TREEITEM_STYLE_SERVICE)
//					{
//						service_id = item_param_id;
//						//HTREEITEM hParentItem = treeCtrl.GetParentItem(hSelItem);
//						
//						//uint32_t parent_item_code = (DWORD)treeCtrl.GetItemData(hParentItem);
//						//uint8_t parent_item_style = (parent_item_code & 0xf0000000) >> 28;
//						//uint16_t parent_item_param_id = (parent_item_code & 0x0000ffff);
//						
//						//assert(parent_item_style == TREEITEM_STYLE_STREAM);
//						//transport_stream_id = parent_item_param_id;
//						
//						//HTREEITEM hGrandParentItem = treeCtrl.GetParentItem(hParentItem);
//						
//						//uint32_t grandparent_item_code = (DWORD)treeCtrl.GetItemData(hGrandParentItem);
//						//uint8_t grandparent_item_style = (grandparent_item_code & 0xf0000000) >> 28;
//						//uint16_t grandparent_item_param_id = (grandparent_item_code & 0x0000ffff);
//						
//						//assert(grandparent_item_style == TREEITEM_STYLE_NETWORK);
//						//network_id = grandparent_item_param_id;
//						
//						//m_pViewEpg->Update(network_id, transport_stream_id, service_id);
//					}
//					else if (item_style == TREEITEM_STYLE_STREAM)
//					{
//						transport_stream_id = item_param_id;
//						//HTREEITEM hParentItem = treeCtrl.GetParentItem(hSelItem);
//						
//						//uint32_t parent_item_code = (DWORD)treeCtrl.GetItemData(hParentItem);
//						//uint8_t parent_item_style = (parent_item_code & 0xf0000000) >> 28;
//						//uint16_t parent_item_param_id = (parent_item_code & 0x0000ffff);
//						
//						//assert(parent_item_style == TREEITEM_STYLE_NETWORK);
//						//network_id = parent_item_param_id;
//						
//						//m_pViewEpg->Update(network_id, transport_stream_id, 0xFFFF);
//					}
//					else if (item_style == TREEITEM_STYLE_NETWORK)
//					{
//						network_id = item_param_id;
//						//m_pViewEpg->Update(network_id, 0xFFFF, 0xFFFF);
//					}
//				}
//
//				return false;	////如果不返回false，会将TVN_SELECTED消息反射回m_pSyntaxTree控件，然后又弹射回来，如此反复，无限循环
//			}
//			else if(nmhdr->code == TVN_SELCHANGED)
//			{
//				LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(nmhdr);
//
//				TVITEM item = pNMTreeView->itemNew;
//				HTREEITEM hSelItem = item.hItem;
//
//				if (hSelItem != NULL)
//				{
//					if (item.mask & TVIF_PARAM)
//					{
//						DWORD uiValue = (DWORD)(item.lParam);
//
//						uint32_t	item_code;
//						uint8_t		item_style;
//						uint16_t	item_param_id;
//
//						uint16_t	network_id;
//						uint16_t	transport_stream_id;
//						uint16_t	service_id;
//
//						m_pServiceDescription->Reset();
//						m_pEpgScheduleList->Reset();
//
//						item_code = uiValue;
//						item_style = (item_code & 0xf0000000) >> 28;
//						item_param_id = (item_code & 0x0000ffff);
//
//						if (item_style == TREEITEM_STYLE_SERVICE)
//						{
//							service_id = item_param_id;
//							//HTREEITEM hParentItem = treeCtrl.GetParentItem(hSelItem);
//
//							//uint32_t parent_item_code = (DWORD)treeCtrl.GetItemData(hParentItem);
//							//uint8_t parent_item_style = (parent_item_code & 0xf0000000) >> 28;
//							//uint16_t parent_item_param_id = (parent_item_code & 0x0000ffff);
//
//							//assert(parent_item_style == TREEITEM_STYLE_STREAM);
//							//transport_stream_id = parent_item_param_id;
//
//							//HTREEITEM hGrandParentItem = treeCtrl.GetParentItem(hParentItem);
//
//							//uint32_t grandparent_item_code = (DWORD)treeCtrl.GetItemData(hGrandParentItem);
//							//uint8_t grandparent_item_style = (grandparent_item_code & 0xf0000000) >> 28;
//							//uint16_t grandparent_item_param_id = (grandparent_item_code & 0x0000ffff);
//
//							//assert(grandparent_item_style == TREEITEM_STYLE_NETWORK);
//							//network_id = grandparent_item_param_id;
//
//							//m_pViewEpg->Update(network_id, transport_stream_id, service_id);
//						}
//						else if (item_style == TREEITEM_STYLE_STREAM)
//						{
//							transport_stream_id = item_param_id;
//							//HTREEITEM hParentItem = treeCtrl.GetParentItem(hSelItem);
//
//							//uint32_t parent_item_code = (DWORD)treeCtrl.GetItemData(hParentItem);
//							//uint8_t parent_item_style = (parent_item_code & 0xf0000000) >> 28;
//							//uint16_t parent_item_param_id = (parent_item_code & 0x0000ffff);
//
//							//assert(parent_item_style == TREEITEM_STYLE_NETWORK);
//							//network_id = parent_item_param_id;
//
//							//m_pViewEpg->Update(network_id, transport_stream_id, 0xFFFF);
//						}
//						else if (item_style == TREEITEM_STYLE_NETWORK)
//						{
//							network_id = item_param_id;
//							//m_pViewEpg->Update(network_id, 0xFFFF, 0xFFFF);
//						}
//					}
//				}
//
//				return false;	////如果不返回false，会将TVN_SELECTED消息反射回m_pSyntaxTree控件，然后又弹射回来，如此反复，无限循环
//			}
//		}
//	}
//
//	return CDialog::OnNotify(wParam, lParam, pResult);
//}
