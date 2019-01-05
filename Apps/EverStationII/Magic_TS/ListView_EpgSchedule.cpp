// Mpeg2_EpgScheduleListView.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation.h"
#include "ListView_EpgSchedule.h"
#include "..\common\define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListView_EpgSchedule
#include "TSMagicView.h"
#include "MiddleWare/MiddleWare_PsiSiTable/Include/MiddleWare_PSISI_Table.h"
#include "syntax_translate_layer\MPEG2_DVB_Section\Include\DVB_SI_Utilities.h"

IMPLEMENT_DYNCREATE(CListView_EpgSchedule, CListView)

CListView_EpgSchedule::CListView_EpgSchedule()
{
}

CListView_EpgSchedule::~CListView_EpgSchedule()
{
}


BEGIN_MESSAGE_MAP(CListView_EpgSchedule, CListView)
	//{{AFX_MSG_MAP(CListView_EpgSchedule)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListView_EpgSchedule drawing

void CListView_EpgSchedule::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CListView_EpgSchedule diagnostics

#ifdef _DEBUG
void CListView_EpgSchedule::AssertValid() const
{
	CListView::AssertValid();
}

void CListView_EpgSchedule::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListView_EpgSchedule message handlers

BOOL CListView_EpgSchedule::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CListView_EpgSchedule::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CListCtrl&	listCtrl = GetListCtrl();

	int			 i;
	int			 nColWidth[] = {80, 170, 80, 260, 560};
//	CString		 str[] = {"event_id", "start_time", "duration", "running_status", "free_CA_mode", "event_name"};
//	UINT		 uAlign[] = {LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT};

	CString		 str[] = {"event_id", "start_time", "duration", "event_name", "event_text"};
	UINT		 uAlign[] = {LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT, LVCFMT_LEFT};

	listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (i = 0; i < 5; i ++)
	{
		listCtrl.InsertColumn(i, str[i], uAlign[i], nColWidth[i], -1);
	}
	
	return 0;
}

void CListView_EpgSchedule::Set(int offline)
{
}

void CListView_EpgSchedule::Reset(void)
{
	CListCtrl&	listCtrl = GetListCtrl();
	listCtrl.DeleteAllItems();
}

void CListView_EpgSchedule::Update(uint16_t original_network_id, uint16_t transport_stream_id, uint16_t service_id)
{
	CEIT* pEIT = NULL;
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();

	pEIT = pDB_PsiSiObjs->QueryEITSchBy3ID(original_network_id, transport_stream_id, service_id);
	if (pEIT != NULL)
	{
		int				i, j;
		int				count;
		int				item_index;
		char			pszText[MAX_TXT_CHARS];
		unsigned int	data;
		int				bFound;
		CListCtrl&		listCtrl = GetListCtrl();
		EVENT_INFO_t	stEventInfo;

		int nEventCount = pEIT->GetEventCount();
		for (i = 0; i < nEventCount; i++)
		{
			pEIT->GetEventInfoByIndex(i, &stEventInfo);

			count = listCtrl.GetItemCount();

			bFound = 0;

			//step 1: check existence
			for (j = 0; j < count; j++)
			{
				data = (DWORD)listCtrl.GetItemData(j);
				if (data == stEventInfo.event_id)
				{
					bFound = 1;
					break;
				}
			}

			//step 2: sort by event_id and insert an item
			if (!bFound)
			{
				for (j = 0; j < count; j++)
				{
					data = (DWORD)listCtrl.GetItemData(j);
					if (data > stEventInfo.event_id)
					{
						break;
					}
				}

				item_index = j;

				sprintf_s(pszText, sizeof(pszText), "0x%04X", stEventInfo.event_id);
				listCtrl.InsertItem(item_index, pszText);
				listCtrl.SetItemData(item_index, stEventInfo.event_id);

				DVB_SI_NumericCoding2Text_UTCTime(stEventInfo.start_time, pszText, sizeof(pszText));
				listCtrl.SetItemText(item_index, 1, pszText);

				DVB_SI_NumericCoding2Text_BCDTime(stEventInfo.duration, pszText, sizeof(pszText));
				listCtrl.SetItemText(item_index, 2, pszText);

				//				DecodeRunningStatusCodetoText(pEvent[i].running_status, pszText);
				//				listCtrl.SetItemText(item_index, 3, pszText);

				//				sprintf_s(pszText, sizeof(pszText), "%d", pEvent[i].free_CA_mode);
				//				listCtrl.SetItemText(item_index, 4, pszText);

				if (strlen(stEventInfo.pszEventName) > 0)
				{
					listCtrl.SetItemText(item_index, 3, stEventInfo.pszEventName);
				}

				if (strlen(stEventInfo.pszEventText) > 0)
				{
					listCtrl.SetItemText(item_index, 4, stEventInfo.pszEventText);
				}
			}
		}
	}
}

void CListView_EpgSchedule::OnSize(UINT nType, int cx, int cy)
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
