// Pane_muxinfoview.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation.h"
#include "NaviTree_PsiSiTables.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_table_id.h"
#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_table_id.h"

#include "..\Common\define.h"
#include "..\Magic_TS\TSMagic_GuiApi_MSG.h"
#include "..\Magic_TS\TSMagic_Trigger_Section.h"
#include "TSMagicView.h"
/////////////////////////////////////////////////////////////////////////////
// CPane_PsiSiTableTreeView

IMPLEMENT_DYNCREATE(CNaviTree_PsiSiTables, CTreeView)

CNaviTree_PsiSiTables::CNaviTree_PsiSiTables()
{
	m_hPatItem = NULL;
	m_hPmtItem = NULL;
	m_hCatItem = NULL;
	m_hTsdtItem = NULL;
	m_hNitItem = NULL;
	m_hBatItem = NULL;
	m_hSdtItem = NULL;
	m_hEitItem = NULL;
	m_hTdtItem = NULL;
	m_hTotItem = NULL;
	m_hRstItem = NULL;
	m_hIntItem = NULL;
	m_hAitItem = NULL;
	m_hDsmccItem = NULL;
	m_hMpeItem = NULL;
	m_hTsdtItem = NULL;
	m_hEcmItem = NULL;
	m_hEmmItem = NULL;

	m_hwndReceiver = NULL;
}

CNaviTree_PsiSiTables::~CNaviTree_PsiSiTables()
{
//	DeleteChildItems(m_hPsisiRootItem);
//	DeleteChildItems(m_hDataCastRootItem);
	m_hwndReceiver = NULL;
}


BEGIN_MESSAGE_MAP(CNaviTree_PsiSiTables, CTreeView)
	//{{AFX_MSG_MAP(CPane_PsiSiTableTreeView)
	ON_WM_CREATE()
	//ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &CNaviTree_PsiSiTables::OnNMDblclk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPane_PsiSiTableTreeView drawing

void CNaviTree_PsiSiTables::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPane_PsiSiTableTreeView diagnostics

#ifdef _DEBUG
void CNaviTree_PsiSiTables::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNaviTree_PsiSiTables::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPane_PsiSiTableTreeView message handlers

BOOL CNaviTree_PsiSiTables::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle = dwStyle | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CNaviTree_PsiSiTables::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	m_ImageList.Create(IDB_TREE_INFO, 20, 1, RGB(255, 255, 255));
	treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	TreeCtrlItem.hParent = TVI_ROOT;
#if CHINESE_VERSION
	strcpy_s(pszText, sizeof(pszText), "PSI/SI相关表");
#else
	strcpy_s(pszText, sizeof(pszText), "PSI/SI Tables");
#endif
	m_hPsisiRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hPsisiRootItem, 0);
	treeCtrl.SetItemImage(m_hPsisiRootItem, 0, 1);

#if CHINESE_VERSION
	strcpy_s(pszText, sizeof(pszText), "数据相关表");
#else
	strcpy_s(pszText, sizeof(pszText), "Data Tables");
#endif
	m_hDataCastRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hDataCastRootItem, 0);
	treeCtrl.SetItemImage(m_hDataCastRootItem, 0, 1);

	TreeCtrlItem.hParent = m_hDataCastRootItem;

	strcpy_s(pszText, sizeof(pszText), "AIT");
	m_hAitItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hAitItem, 0);
	treeCtrl.SetItemImage(m_hAitItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "INT");
	m_hIntItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hIntItem, 0);
	treeCtrl.SetItemImage(m_hIntItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "MPE");
	m_hMpeItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hMpeItem, 0);
	treeCtrl.SetItemImage(m_hMpeItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "DSM-CC");
	m_hDsmccItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hDsmccItem, 0);
	treeCtrl.SetItemImage(m_hDsmccItem, 0, 1);

	TreeCtrlItem.hParent = m_hPsisiRootItem;

	//PSI
	strcpy_s(pszText, sizeof(pszText), "TSDT");
	m_hTsdtItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hTsdtItem, 0);
	treeCtrl.SetItemImage(m_hTsdtItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "PAT");
	m_hPatItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hPatItem, 0);
	treeCtrl.SetItemImage(m_hPatItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "PMT");
	m_hPmtItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hPmtItem, 0);
	treeCtrl.SetItemImage(m_hPmtItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "CAT");
	m_hCatItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hCatItem, 0);
	treeCtrl.SetItemImage(m_hCatItem, 0, 1);

	//SI
	strcpy_s(pszText, sizeof(pszText), "NIT");
	m_hNitItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hNitItem, 0);
	treeCtrl.SetItemImage(m_hNitItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "BAT");
	m_hBatItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hBatItem, 0);
	treeCtrl.SetItemImage(m_hBatItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "SDT");
	m_hSdtItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hSdtItem, 0);
	treeCtrl.SetItemImage(m_hSdtItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "EIT");
	m_hEitItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hEitItem, 0);
	treeCtrl.SetItemImage(m_hEitItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "TDT");
	m_hTdtItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hTdtItem, 0);
	treeCtrl.SetItemImage(m_hTdtItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "TOT");
	m_hTotItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hTotItem, 0);
	treeCtrl.SetItemImage(m_hTotItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "RST");
	m_hRstItem = treeCtrl.InsertItem( &TreeCtrlItem );
	treeCtrl.SetItemData(m_hRstItem, 0);
	treeCtrl.SetItemImage(m_hRstItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "ECM");
	m_hEcmItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hEcmItem, 0);
	treeCtrl.SetItemImage(m_hEcmItem, 0, 1);

	strcpy_s(pszText, sizeof(pszText), "EMM");
	m_hEmmItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hEmmItem, 0);
	treeCtrl.SetItemImage(m_hEmmItem, 0, 1);

	treeCtrl.Expand(m_hPsisiRootItem, TVE_EXPAND);
	treeCtrl.Expand(m_hDataCastRootItem, TVE_EXPAND);

	return 0;
}

void CNaviTree_PsiSiTables::UpdatePAT(int section_number, CPAT* pPAT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[64];

	HTREEITEM		hSectionItem;
	HTREEITEM		hPatStreamItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pPAT != NULL)
	{
		TreeCtrlItem.hParent = m_hPatItem;

		uint16_t usTransportStreamID = pPAT->GetTransportStreamID();
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "流 0x%04X(%d)", usTransportStreamID, usTransportStreamID);
#else
		sprintf_s(pszText, sizeof(pszText), "STREAM 0x%04X(%d)", usTransportStreamID, usTransportStreamID);
#endif
		hPatStreamItem = treeCtrl.InsertItem(&TreeCtrlItem);

		treeCtrl.SetItemData(hPatStreamItem, 0);
		treeCtrl.SetItemImage(hPatStreamItem, 0, 1);

		TreeCtrlItem.hParent = hPatStreamItem;

//		for (section_index = 0; section_index < pPAT->m_nSectionCount; section_index ++)
//		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif

			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pPAT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pPAT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);

//		}

		treeCtrl.SetItemState(m_hPatItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdatePMT(int section_number, CPMT* pPMT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];

	HTREEITEM		hSectionItem;
	HTREEITEM		hPmtProgramItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pPMT != NULL)
	{
		uint16_t usPID = pPMT->GetPID();
		uint16_t usProgramNumber = pPMT->GetProgramNumber();

		TreeCtrlItem.hParent = m_hPmtItem;
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "节目 0x%04X(%5d)—PID=0x%04X(%d)", usProgramNumber, usProgramNumber, usPID, usPID);
#else
		sprintf_s(pszText, sizeof(pszText), "PROGRAM 0x%04X(%5d)—PID=0x%04X(%d)", usProgramNumber, usProgramNumber, usPID, usPID);
#endif
		hPmtProgramItem = treeCtrl.InsertItem(&TreeCtrlItem);

		treeCtrl.SetItemData(hPmtProgramItem, 0);
		treeCtrl.SetItemImage(hPmtProgramItem, 0, 1);

		TreeCtrlItem.hParent = hPmtProgramItem;

//		for (section_index = 0; section_index < pPMT->m_nSectionCount; section_index ++)
//		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pPMT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pPMT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);
//		}

		treeCtrl.SortChildren(m_hPmtItem);
		treeCtrl.SetItemState(m_hPmtItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdateTDT(CTDT* pTDT)
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pTDT != NULL)
	{
		uint32_t dwItemData = pTDT->GetKey();
		dwItemData <<= 8;
		dwItemData |= pTDT->GetTableID();
		dwItemData <<= 8;
		dwItemData |= 0x00;

		treeCtrl.SetItemData(m_hTdtItem, dwItemData);
		treeCtrl.SetItemState(m_hTdtItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdateTOT(CTOT* pTOT)
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pTOT != NULL)
	{
		uint32_t dwItemData = pTOT->GetKey();
		dwItemData <<= 8;
		dwItemData |= pTOT->GetTableID();
		dwItemData <<= 8;
		dwItemData |= 0x00;

		treeCtrl.SetItemData(m_hTotItem, dwItemData);
		treeCtrl.SetItemState(m_hTotItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdateRST(CRST* pRST)
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pRST != NULL)
	{
		uint32_t dwItemData = pRST->GetKey();
		dwItemData <<= 8;
		dwItemData |= pRST->GetTableID();
		dwItemData <<= 8;
		dwItemData |= 0x00;

		treeCtrl.SetItemData(m_hRstItem, dwItemData);
		treeCtrl.SetItemState(m_hRstItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdateCMT(CCMT* pCMT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];

	HTREEITEM		hParentItem = NULL;
	HTREEITEM		hTableItem;
	HTREEITEM		hPIDItem = NULL;
	HTREEITEM		hNextItem = NULL;
	CString			strTemp;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pCMT != NULL)
	{
		uint8_t ucTableID = pCMT->GetTableID();
		if ((ucTableID >= TABLE_ID_ECM_MIN) && (ucTableID <= TABLE_ID_ECM_MAX))
		{
			hParentItem = m_hEcmItem;
		}
		else if ((ucTableID >= TABLE_ID_EMM_MIN) && (ucTableID <= TABLE_ID_EMM_MAX))
		{
			hParentItem = m_hEmmItem;
		}

		uint16_t usPID = pCMT->GetPID();

#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "PID = 0x%04X(%d)", usPID, usPID);
#else
		sprintf_s(pszText, sizeof(pszText), "PID = 0x%04X(%d)", usPID, usPID);
#endif

		int find_pid_item = 0;
		if (treeCtrl.ItemHasChildren(hParentItem))
		{
			hPIDItem = treeCtrl.GetChildItem(hParentItem);
			while (hPIDItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hPIDItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_pid_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hPIDItem, TVGN_NEXT);
					hPIDItem = hNextItem;
				}
			}
		}

		if (find_pid_item == 0)
		{
			TreeCtrlItem.hParent = hParentItem;
			hPIDItem = treeCtrl.InsertItem(&TreeCtrlItem);

			treeCtrl.SetItemData(hPIDItem, 0);
			treeCtrl.SetItemImage(hPIDItem, 0, 1);

			treeCtrl.SortChildren(hParentItem);
		}

#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d)", ucTableID, ucTableID);
#else
		sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d)", ucTableID, ucTableID);
#endif

		int find_table_item = 0;
		if (treeCtrl.ItemHasChildren(hPIDItem))
		{
			hTableItem = treeCtrl.GetChildItem(hPIDItem);
			while (hTableItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hTableItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_table_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hTableItem, TVGN_NEXT);
					hTableItem = hNextItem;
				}
			}
		}

		if (find_table_item == 0)
		{
			TreeCtrlItem.hParent = hPIDItem;

			hTableItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pCMT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pCMT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= 0xff;

			treeCtrl.SetItemData(hTableItem, dwItemData);
			treeCtrl.SetItemImage(hTableItem, 2, 3);
			treeCtrl.SortChildren(hPIDItem);
		}

		treeCtrl.SetItemState(hParentItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdateCAT(int section_number, CCAT* pCAT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[64];

	HTREEITEM		hSectionItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pCAT != NULL)
	{
		if (!treeCtrl.ItemHasChildren(m_hCatItem))
		{
			TreeCtrlItem.hParent = m_hCatItem;

			//for (section_index = 0; section_index < pCAT->GetS; section_index ++)
			{
#if CHINESE_VERSION
				sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
				sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif
				hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

				uint32_t dwItemData = pCAT->GetKey();
				dwItemData <<= 8;
				dwItemData |= pCAT->GetTableID();
				dwItemData <<= 8;
				dwItemData |= section_number;

				treeCtrl.SetItemData(hSectionItem, dwItemData);
				treeCtrl.SetItemImage(hSectionItem, 2, 3);
			}
		}

		treeCtrl.SetItemState(m_hCatItem, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hCatItem))
		{
			DeleteChildItems(m_hCatItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateTSDT(int section_number, CTSDT* pTSDT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[64];
	//int				section_index;

	HTREEITEM		hSectionItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pTSDT != NULL)
	{
		if (!treeCtrl.ItemHasChildren(m_hTsdtItem))
		{
			TreeCtrlItem.hParent = m_hTsdtItem;

			//for (section_index = 0; section_index < pTSDT->m_nSectionCount; section_index ++)
			{
#if CHINESE_VERSION
				sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
				sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif
				hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

				uint32_t dwItemData = pTSDT->GetKey();
				dwItemData <<= 8;
				dwItemData |= pTSDT->GetTableID();
				dwItemData <<= 8;
				dwItemData |= section_number;

				treeCtrl.SetItemData(hSectionItem, dwItemData);
				treeCtrl.SetItemImage(hSectionItem, 2, 3);
			}
		}

		treeCtrl.SetItemState(m_hTsdtItem, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hTsdtItem))
		{
			DeleteChildItems(m_hTsdtItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateINT(int section_number, CINT* pINT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[64];

	HTREEITEM		hSectionItem;
	HTREEITEM		hIntPlatformItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pINT != NULL)
	{
		TreeCtrlItem.hParent = m_hIntItem;

		sprintf_s(pszText, sizeof(pszText), "platfomr_id = 0x%06X(%d)", pINT->m_uiPlatformID, pINT->m_uiPlatformID);

		hIntPlatformItem = treeCtrl.InsertItem(&TreeCtrlItem);
		treeCtrl.SetItemData(hIntPlatformItem, 0);
		treeCtrl.SetItemImage(hIntPlatformItem, 0, 1);

		TreeCtrlItem.hParent = hIntPlatformItem;

//		for (section_index = 0; section_index < pINT->m_nSectionCount; section_index ++)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_index, section_index);
#endif
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pINT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pINT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);
		}

		treeCtrl.SetItemState(m_hIntItem, TVIS_BOLD, TVIS_BOLD);
	}
}

void CNaviTree_PsiSiTables::UpdateAIT(int section_number, CAIT* pAIT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[64];

	HTREEITEM		hSectionItem = NULL;
	HTREEITEM		hAitItem = NULL;
	HTREEITEM		hSubAitItem = NULL;
	HTREEITEM		hNextItem = NULL;
	int				find_ait_item = 0;
	int				find_sub_ait_item = 0;

	CString			strTemp;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pAIT != NULL)
	{
		uint16_t usPID = pAIT->GetPID();

		TreeCtrlItem.hParent = m_hAitItem;

		sprintf_s(pszText, sizeof(pszText), "PID = 0x%04X(%4d)", usPID, usPID);
		
		find_ait_item = 0;
		if (treeCtrl.ItemHasChildren(m_hAitItem))
		{
			hAitItem = treeCtrl.GetChildItem(m_hAitItem);
			while (hAitItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hAitItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_ait_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hAitItem, TVGN_NEXT);
					hAitItem = hNextItem;
				}
			}
		}
		
		if (find_ait_item == 0)
		{
			TreeCtrlItem.hParent = m_hAitItem;

			hAitItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hAitItem, 0);
			treeCtrl.SetItemImage(hAitItem, 0, 1);
	
			TreeCtrlItem.hParent = hAitItem;
			
			treeCtrl.SortChildren(m_hAitItem);
			treeCtrl.SetItemState(m_hAitItem, TVIS_BOLD, TVIS_BOLD);
		}

		//按AIT subtable查找
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "应用类型 0x%04X(%d)", pAIT->m_usApplicationType, pAIT->m_usApplicationType);
#else
		sprintf_s(pszText, sizeof(pszText), "application_type 0x%04X(%d)", pAIT->m_usApplicationType, pAIT->m_usApplicationType);
#endif
		
		find_sub_ait_item = 0;
		if (treeCtrl.ItemHasChildren(hAitItem))
		{
			hSubAitItem = treeCtrl.GetChildItem(hAitItem);
			while (hSubAitItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hSubAitItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_sub_ait_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hSubAitItem, TVGN_NEXT);
					hSubAitItem = hNextItem;
				}
			}
		}

		if (find_sub_ait_item == 0)
		{
			TreeCtrlItem.hParent = hAitItem;

			hSubAitItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hSubAitItem, 0);
			treeCtrl.SetItemImage(hSubAitItem, 0, 1);

			treeCtrl.SortChildren(hAitItem);
	
//			for (section_index = 0; section_index < pAIT->m_nSectionCount; section_index ++)
			{
				TreeCtrlItem.hParent = hSubAitItem;
				
#if CHINESE_VERSION
				sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
				sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_index, section_index);
#endif
				hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);
				
				uint32_t dwItemData = pAIT->GetKey();
				dwItemData <<= 8;
				dwItemData |= pAIT->GetTableID();
				dwItemData <<= 8;
				dwItemData |= section_number;

				treeCtrl.SetItemData(hSectionItem, dwItemData);
				treeCtrl.SetItemImage(hSectionItem, 2, 3);
			}
		}
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hAitItem))
		{
			DeleteChildItems(m_hAitItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateNIT(int section_number, CNIT* pNIT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];
	CString			strTemp;
	int				find_network_item;

	HTREEITEM	hSectionItem = NULL;
	HTREEITEM	hNitNetworkItem = NULL;
	HTREEITEM	hNextItem = NULL;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pNIT != NULL)
	{
		uint8_t ucTableID = pNIT->GetTableID();
		uint16_t usNetworkID = pNIT->GetNetworkID();

		if (ucTableID == TABLE_ID_NIT_ACTUAL)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "当前网络 0x%04X(%d)", usNetworkID, usNetworkID);
#else
			sprintf_s(pszText, sizeof(pszText), "ACTUAL NETWORK 0x%04X(%d)", usNetworkID, usNetworkID);
#endif
		}
		else
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "其他网络 0x%04X(%d)", usNetworkID, usNetworkID);
#else
			sprintf_s(pszText, sizeof(pszText), "OTHER NETWORK 0x%04X(%d)", usNetworkID, usNetworkID);
#endif
		}
		
		find_network_item = 0;
		if (treeCtrl.ItemHasChildren(m_hNitItem))
		{
			hNitNetworkItem = treeCtrl.GetChildItem(m_hNitItem);
			while (hNitNetworkItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hNitNetworkItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_network_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hNitNetworkItem, TVGN_NEXT);
					hNitNetworkItem = hNextItem;
				}
			}
		}

		if (find_network_item == 0)
		{
			TreeCtrlItem.hParent = m_hNitItem;
	
			hNitNetworkItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hNitNetworkItem, 0);
			treeCtrl.SetItemImage(hNitNetworkItem, 0, 1);

			if (ucTableID == TABLE_ID_NIT_ACTUAL)
			{
				treeCtrl.SetItemState(hNitNetworkItem, TVIS_BOLD, TVIS_BOLD);
			}
		}

		TreeCtrlItem.hParent = hNitNetworkItem;

//		for (section_index = 0; section_index < pNIT->m_nSectionCount; section_index ++)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pNIT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pNIT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);

			treeCtrl.SortChildren(hNitNetworkItem);
			treeCtrl.SortChildren(m_hNitItem);
		}

		treeCtrl.SetItemState(m_hNitItem, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hNitItem))
		{
			DeleteChildItems(m_hNitItem);
		}
	}
}

void CNaviTree_PsiSiTables::Set(HWND hwndReceiver, int offline)
{
	m_hwndReceiver = hwndReceiver;
}

void CNaviTree_PsiSiTables::Reset(void)
{
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (treeCtrl.ItemHasChildren(m_hPatItem))
	{
		DeleteChildItems(m_hPatItem);
	}
	treeCtrl.SetItemState(m_hPatItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hPmtItem))
	{
		DeleteChildItems(m_hPmtItem);
	}
	treeCtrl.SetItemState(m_hPmtItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hCatItem))
	{
		DeleteChildItems(m_hCatItem);
	}
	treeCtrl.SetItemState(m_hCatItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hTsdtItem))
	{
		DeleteChildItems(m_hTsdtItem);
	}
	treeCtrl.SetItemState(m_hTsdtItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hIntItem))
	{
		DeleteChildItems(m_hIntItem);
	}
	treeCtrl.SetItemState(m_hIntItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hAitItem))
	{
		DeleteChildItems(m_hAitItem);
	}
	treeCtrl.SetItemState(m_hAitItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hNitItem))
	{
		DeleteChildItems(m_hNitItem);
	}
	treeCtrl.SetItemState(m_hNitItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hBatItem))
	{
		DeleteChildItems(m_hBatItem);
	}
	treeCtrl.SetItemState(m_hBatItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hSdtItem))
	{
		DeleteChildItems(m_hSdtItem);
	}
	treeCtrl.SetItemState(m_hSdtItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hEitItem))
	{
		DeleteChildItems(m_hEitItem);
	}
	treeCtrl.SetItemState(m_hEitItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hDsmccItem))
	{
		DeleteChildItems(m_hDsmccItem);
	}
	treeCtrl.SetItemState(m_hDsmccItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hMpeItem))
	{
		DeleteChildItems(m_hMpeItem);
	}
	treeCtrl.SetItemState(m_hMpeItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hIntItem))
	{
		DeleteChildItems(m_hIntItem);
	}
	treeCtrl.SetItemState(m_hIntItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hAitItem))
	{
		DeleteChildItems(m_hAitItem);
	}
	treeCtrl.SetItemState(m_hAitItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hEcmItem))
	{
		DeleteChildItems(m_hEcmItem);
	}
	treeCtrl.SetItemState(m_hEcmItem, ~TVIS_BOLD, TVIS_BOLD);

	if (treeCtrl.ItemHasChildren(m_hEmmItem))
	{
		DeleteChildItems(m_hEmmItem);
	}
	treeCtrl.SetItemState(m_hEmmItem, ~TVIS_BOLD, TVIS_BOLD);

	treeCtrl.SetItemState(m_hTdtItem, ~TVIS_BOLD, TVIS_BOLD);
	treeCtrl.SetItemState(m_hTotItem, ~TVIS_BOLD, TVIS_BOLD);
	treeCtrl.SetItemState(m_hRstItem, ~TVIS_BOLD, TVIS_BOLD);
}

void CNaviTree_PsiSiTables::UpdateBAT(int section_number, CBAT* pBAT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];
	CString			strTemp;

	HTREEITEM		hSectionItem = NULL;
	HTREEITEM		hBouquetItem = NULL;
	HTREEITEM		hNextItem = NULL;

	int				find_bouquet_item;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pBAT != NULL)
	{
		uint16_t bouquet_id = pBAT->GetBouquetID();
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "业务群 0x%04X(%d)", bouquet_id, bouquet_id);
#else
		sprintf_s(pszText, sizeof(pszText), "BOUQUET 0x%04X(%d)", bouquet_id, bouquet_id);
#endif

		find_bouquet_item = 0;
		if (treeCtrl.ItemHasChildren(m_hBatItem))
		{
			hBouquetItem = treeCtrl.GetChildItem(m_hBatItem);
			while (hBouquetItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hBouquetItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_bouquet_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hBouquetItem, TVGN_NEXT);
					hBouquetItem = hNextItem;
				}
			}
		}

		if (find_bouquet_item == 0)
		{
			TreeCtrlItem.hParent = m_hBatItem;

			hBouquetItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hBouquetItem, 0);
			treeCtrl.SetItemImage(hBouquetItem, 0, 1);
		}

		TreeCtrlItem.hParent = hBouquetItem;

#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
		sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_index, section_index);
#endif
		hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

		uint32_t dwItemData = pBAT->GetKey();
		dwItemData <<= 8;
		dwItemData |= pBAT->GetTableID();
		dwItemData <<= 8;
		dwItemData |= section_number;

		treeCtrl.SetItemData(hSectionItem, dwItemData);
		treeCtrl.SetItemImage(hSectionItem, 2, 3);

		treeCtrl.SortChildren(hBouquetItem);
		treeCtrl.SortChildren(m_hBatItem);

		treeCtrl.SetItemState(m_hBatItem, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hBatItem))
		{
			DeleteChildItems(m_hBatItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateSDT(int section_number, CSDT* pSDT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];

	HTREEITEM		hSectionItem;
	HTREEITEM		hSdtStreamItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pSDT != NULL)
	{
		uint16_t usTransportStreamID = pSDT->GetTransportStreamID();
		uint16_t usOriginalNetworkID = pSDT->GetOriginalNetworkID();
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "流 [ONetID=0x%04X(%d)-TSID=0x%04X(%d)]", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#else
		sprintf_s(pszText, sizeof(pszText), "STREAM [ONetID=0x%04X(%d)-TSID=0x%04X(%d)]", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#endif

		TreeCtrlItem.hParent = m_hSdtItem;
		hSdtStreamItem = treeCtrl.InsertItem(&TreeCtrlItem);
		treeCtrl.SetItemData(hSdtStreamItem, 0);
		treeCtrl.SetItemImage(hSdtStreamItem, 0, 1);

		uint16_t ucTableID = pSDT->GetTableID();
		if (ucTableID == TABLE_ID_SDT_ACTUAL)
		{
			treeCtrl.SetItemState(hSdtStreamItem, TVIS_BOLD, TVIS_BOLD);
		}

		TreeCtrlItem.hParent = hSdtStreamItem;

//		for (section_index = 0; section_index < pSDT->m_nSectionCount; section_index ++)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_index, section_index);
#endif
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pSDT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pSDT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);
		}

		treeCtrl.SortChildren(m_hSdtItem);
		treeCtrl.SetItemState(m_hSdtItem, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hSdtItem))
		{
			DeleteChildItems(m_hSdtItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateEIT(int section_number, CEIT* pEIT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];
	CString			strTemp;

	HTREEITEM		hSectionItem = NULL;
	HTREEITEM		hStreamItem = NULL;
	HTREEITEM		hServiceItem = NULL;
	HTREEITEM		hTableItem = NULL;
	HTREEITEM		hNextItem = NULL;
	int				find_stream_item;
	int				find_service_item;
	int				find_table_item;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pEIT != NULL)
	{
		uint16_t usOriginalNetworkID = pEIT->GetOriginalNetworkID();
		uint16_t usTransportStreamID = pEIT->GetTransportStreamID();
		uint16_t usServiceID = pEIT->GetServiceID();

		//按stream_id查找
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "流 [ONetID=0x%04X(%d)-TSID=0x%04X(%d)]", usOriginalNetworkID, usOriginalNetworkID, usTransportStreamID, usTransportStreamID);
#else
		sprintf_s(pszText, sizeof(pszText), "STREAM [ONetID=0x%04X(%d)-TSID=0x%04X(%d)", usOriginalNetworkID, usOriginalNetworkID， usTransportStreamID, usTransportStreamID);
#endif

		find_stream_item = 0;
		if (treeCtrl.ItemHasChildren(m_hEitItem))
		{
			hStreamItem = treeCtrl.GetChildItem(m_hEitItem);
			while (hStreamItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hStreamItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_stream_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hStreamItem, TVGN_NEXT);
					hStreamItem = hNextItem;
				}
			}
		}

		uint8_t ucTableID = pEIT->GetTableID();

		if (find_stream_item == 0)
		{
			TreeCtrlItem.hParent = m_hEitItem;

			hStreamItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hStreamItem, 0);
			treeCtrl.SetItemImage(hStreamItem, 0, 1);

			//现行EIT表
			if ((ucTableID == TABLE_ID_EIT_PF_ACTUAL) ||
				((ucTableID >= TABLE_ID_EIT_SCH_ACTUAL_0) && (ucTableID <= TABLE_ID_EIT_SCH_ACTUAL_F)))
			{
				treeCtrl.SetItemState(hStreamItem, TVIS_BOLD, TVIS_BOLD);
			}

			treeCtrl.SortChildren(m_hEitItem);
			treeCtrl.SetItemState(m_hEitItem, TVIS_BOLD, TVIS_BOLD);
		}

		//按service_id查找
#if CHINESE_VERSION
		sprintf_s(pszText, sizeof(pszText), "业务 0x%04X(%d)", usServiceID, usServiceID);
#else
		sprintf_s(pszText, sizeof(pszText), "SERVICE 0x%04X(%d)", usServiceID, usServiceID);
#endif

		find_service_item = 0;
		if (treeCtrl.ItemHasChildren(hStreamItem))
		{
			hServiceItem = treeCtrl.GetChildItem(hStreamItem);
			while (hServiceItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hServiceItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_service_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hServiceItem, TVGN_NEXT);
					hServiceItem = hNextItem;
				}
			}
		}

		if (find_service_item == 0)
		{
			TreeCtrlItem.hParent = hStreamItem;

			hServiceItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hServiceItem, 0);
			treeCtrl.SetItemImage(hServiceItem, 0, 1);

			treeCtrl.SortChildren(hStreamItem);
		}

		//按照table_id进行查询
		if (ucTableID == TABLE_ID_EIT_PF_ACTUAL)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - 现行传送流当前/后续事件", ucTableID, ucTableID);
#else
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - Present/Following for Actual Stream", table_id, table_id);
#endif
//			section_count = pEIT->m_nSectionCount_PF_A;
		}
		else if (ucTableID == TABLE_ID_EIT_PF_OTHER)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - 其他传送流当前/后续事件", ucTableID, ucTableID);
#else
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - Present/Following for Other Stream", table_id, table_id);
#endif
//			section_count = pEIT->m_nSectionCount_PF_O;
		}
		else if ((ucTableID >= TABLE_ID_EIT_SCH_ACTUAL_0) && (ucTableID <= TABLE_ID_EIT_SCH_ACTUAL_F))
		{
#ifdef CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - 现行传送流事件时间表", ucTableID, ucTableID);
#else
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - Schedule for Actual Stream", table_id, table_id);
#endif
		}
		else
		{
#ifdef CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - 其他传送流事件时间表", ucTableID, ucTableID);
#else
			sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d) - Schedule for Other Stream", table_id, table_id);
#endif
		}

		find_table_item = 0;
		if (treeCtrl.ItemHasChildren(hServiceItem))
		{
			hTableItem = treeCtrl.GetChildItem(hServiceItem);
			while (hTableItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hTableItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_table_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hTableItem, TVGN_NEXT);
					hTableItem = hNextItem;
				}
			}
		}

		if (find_table_item == 0)
		{
			TreeCtrlItem.hParent = hServiceItem;

			hTableItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hTableItem, 0);
			treeCtrl.SetItemImage(hTableItem, 0, 1);
		}

		TreeCtrlItem.hParent = hTableItem;

//		if ((pEIT->m_ucTableID == TABLE_ID_EIT_PF_ACTUAL) ||
//			(pEIT->m_ucTableID == TABLE_ID_EIT_PF_OTHER))
//		{
//			for (section_index = 0; section_index < pEIT->m_nSectionCount; section_index ++)
//			{
#if CHINESE_VERSION
//				sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_index, section_index);
#else
//				sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_index, section_index);
#endif
//				hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

//				ItemId.flag.uiClassIndex = GUI_PSISI_SYNTAX_EIT;
//				ItemId.flag.uiTableIndex = eit_index;
//				ItemId.flag.uiSectionNumber = section_index;
//				treeCtrl.SetItemData(hSectionItem, ItemId.uiValue);
//				treeCtrl.SetItemImage(hSectionItem, 2, 3);

//				ItemId.uiValue = 0x00000000;
//			}
//		}
//		else
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pEIT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pEIT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);
		}

		treeCtrl.SortChildren(hTableItem);
		treeCtrl.SortChildren(hServiceItem);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hEitItem))
		{
			DeleteChildItems(m_hEitItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateDSMCC(int section_number, CPVT* pPVT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];
	CString			strTemp;

	HTREEITEM		hSectionItem = NULL;
	HTREEITEM		hPIDItem = NULL;
	HTREEITEM		hTableItem = NULL;
	HTREEITEM		hExtensionItem = NULL;
	HTREEITEM		hNextItem = NULL;

	int				find_pid_item;
	int				find_table_item;
	int				find_extension_item;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pPVT != NULL)
	{
		uint16_t usPID = pPVT->GetPID();
		sprintf_s(pszText, sizeof(pszText), "PID = 0x%04X(%4d)", usPID, usPID);

		find_pid_item = 0;
		if (treeCtrl.ItemHasChildren(m_hDsmccItem))
		{
			hPIDItem = treeCtrl.GetChildItem(m_hDsmccItem);
			while (hPIDItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hPIDItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_pid_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hPIDItem, TVGN_NEXT);
					hPIDItem = hNextItem;
				}
			}
		}

		if (find_pid_item == 0)
		{
			TreeCtrlItem.hParent = m_hDsmccItem;

			hPIDItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hPIDItem, 0);

			treeCtrl.SetItemImage(hPIDItem, 0, 1);

			treeCtrl.SortChildren(m_hDsmccItem);
		}

		//按table_id查找
		//		GetDescriptionByTableID(pDSMCC->m_ucTableID, pszTemp);
		uint8_t ucTableID = pPVT->GetTableID();
		sprintf_s(pszText, sizeof(pszText), "table_id = 0x%02X(%d)", ucTableID, ucTableID);

		find_table_item = 0;
		if (treeCtrl.ItemHasChildren(hPIDItem))
		{
			hTableItem = treeCtrl.GetChildItem(hPIDItem);
			while (hTableItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hTableItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_table_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hTableItem, TVGN_NEXT);
					hTableItem = hNextItem;
				}
			}
		}

		if (find_table_item == 0)
		{
			TreeCtrlItem.hParent = hPIDItem;

			hTableItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hTableItem, 0);
			treeCtrl.SetItemImage(hTableItem, 0, 1);

			treeCtrl.SortChildren(hPIDItem);
		}

		//按table_id_extension查找
		uint16_t usTableIDExtension = pPVT->GetTableIDExtension();
		sprintf_s(pszText, sizeof(pszText), "table_id_extension = 0x%04X(%d)", usTableIDExtension, usTableIDExtension);

		find_extension_item = 0;
		if (treeCtrl.ItemHasChildren(hTableItem))
		{
			hExtensionItem = treeCtrl.GetChildItem(hTableItem);
			while (hExtensionItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hExtensionItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_extension_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hExtensionItem, TVGN_NEXT);
					hExtensionItem = hNextItem;
				}
			}
		}

		if (find_extension_item == 0)
		{
			TreeCtrlItem.hParent = hTableItem;

			hExtensionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			treeCtrl.SetItemData(hExtensionItem, 0);
			treeCtrl.SetItemImage(hExtensionItem, 0, 1);
			treeCtrl.SortChildren(hTableItem);
		}

		TreeCtrlItem.hParent = hExtensionItem;

		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%3d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%3d)", section_index, section_index);
#endif
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pPVT->GetKey();
			dwItemData <<= 8;
			dwItemData |= pPVT->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);
		}
		treeCtrl.SortChildren(hExtensionItem);

		treeCtrl.SetItemState(m_hDsmccItem, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		if (treeCtrl.ItemHasChildren(m_hDsmccItem))
		{
			DeleteChildItems(m_hDsmccItem);
		}
	}
}

void CNaviTree_PsiSiTables::UpdateMPE(int section_number, CMPE* pMPE)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[64];
	CString			strTemp;

	HTREEITEM		hSectionItem;
	HTREEITEM		hPIDItem = NULL;
	HTREEITEM		hNextItem = NULL;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pMPE != NULL)
	{
		uint16_t usPID = pMPE->GetPID();
		sprintf_s(pszText, sizeof(pszText), "PID = 0x%04X(%4d)", usPID, usPID);

		int find_pid_item = 0;
		if (treeCtrl.ItemHasChildren(m_hDsmccItem))
		{
			hPIDItem = treeCtrl.GetChildItem(m_hDsmccItem);
			while (hPIDItem != NULL)
			{
				strTemp = treeCtrl.GetItemText(hPIDItem);
				if (strcmp(pszText, strTemp.GetBuffer(MAX_TXT_CHARS)) == 0)
				{
					find_pid_item = 1;
					break;
				}
				else
				{
					hNextItem = treeCtrl.GetNextItem(hPIDItem, TVGN_NEXT);
					hPIDItem = hNextItem;
				}
			}
		}

		if (find_pid_item == 0)
		{
			TreeCtrlItem.hParent = m_hMpeItem;

			hPIDItem = treeCtrl.InsertItem(&TreeCtrlItem);
			treeCtrl.SetItemData(hPIDItem, 0);

			treeCtrl.SetItemImage(hPIDItem, 0, 1);

			treeCtrl.SortChildren(m_hMpeItem);
			treeCtrl.SetItemState(m_hMpeItem, TVIS_BOLD, TVIS_BOLD);
		}

		//for (section_index = 0; section_index < pMPE->m_nSectionCount; section_index ++)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "段 0x%02X(%d)", section_number, section_number);
#else
			sprintf_s(pszText, sizeof(pszText), "SECTION 0x%02X(%d)", section_number, section_number);
#endif
			TreeCtrlItem.hParent = hPIDItem;
			hSectionItem = treeCtrl.InsertItem(&TreeCtrlItem);

			uint32_t dwItemData = pMPE->GetKey();
			dwItemData <<= 8;
			dwItemData |= pMPE->GetTableID();
			dwItemData <<= 8;
			dwItemData |= section_number;

			treeCtrl.SetItemData(hSectionItem, dwItemData);
			treeCtrl.SetItemImage(hSectionItem, 2, 3);

			treeCtrl.SortChildren(hPIDItem);
		}
	}
}


void CNaviTree_PsiSiTables::DeleteChildItems(HTREEITEM hParentItem)
{
	HTREEITEM hChildItem;
	HTREEITEM hOldItem;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (treeCtrl.ItemHasChildren(hParentItem))
	{
		hChildItem = treeCtrl.GetChildItem(hParentItem);

		while (1)
		{
			hOldItem = hChildItem;
			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);

			treeCtrl.DeleteItem(hOldItem);

			if (hChildItem == NULL)
			{
				break;
			}
		}
	}
}

void CNaviTree_PsiSiTables::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//uint16_t	  usKeyID;
	uint32_t	  dwSelectItemData;
	//CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	//char		  pszText[MAX_TXT_CHARS];

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
	if (!treeCtrl.ItemHasChildren(hSelItem))
	{
		dwSelectItemData = (uint32_t)treeCtrl.GetItemData(hSelItem);
		if (dwSelectItemData > 0)
		{
			::SendMessage(m_hwndReceiver, WM_USER_PSISI_SEL_CHANGE, NULL, dwSelectItemData);

			//if (pTSMagicView->m_kThreadParams.main_thread_running)
			//{
			//	if ((pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1) ||
			//		(pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1) ||
			//		(pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1) ||
			//		(pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1) ||
			//		(pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1) ||
			//		(pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1))
			//	{
			//		if (pTSMagicView->m_kThreadParams.ts_trigger_thread_running == 1)
			//		{
			//			sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现TS捕捉线程尚未结束！");
			//			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//		}
			//		if (pTSMagicView->m_kThreadParams.pes_trigger_thread_running == 1)
			//		{
			//			sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现PES捕捉线程尚未结束！");
			//			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//		}
			//		if (pTSMagicView->m_kThreadParams.es_trigger_thread_running == 1)
			//		{
			//			sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现ES捕捉线程尚未结束！");
			//			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//		}
			//		if (pTSMagicView->m_kThreadParams.section_trigger_thread_running == 1)
			//		{
			//			sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现section捕捉线程尚未结束！");
			//			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//		}
			//		if (pTSMagicView->m_kThreadParams.dsmcc_download_thread_running == 1)
			//		{
			//			sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现DSMCC下载线程尚未结束！");
			//			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//		}
			//		if (pTSMagicView->m_kThreadParams.packet_decimate_thread_running == 1)
			//		{
			//			sprintf_s(pszText, sizeof(pszText), "section捕捉：未能启动，因为发现TS录制线程尚未结束！");
			//			::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			//		}
			//	}
			//	else
			//	{
			//		CTrigger_PsiSiSection* pSectionTrigger = pTSMagicView->GetSectionTrigger();
			//		CDB_PsiSiObjs* pDB_PsiSiObjs = pTSMagicView->GetPsiSiObjsDBase();
			//		CPVT* pPVT = NULL;

			//		usKeyID = (dwSelectItemData & 0xffff0000) >> 16;
			//		pPVT = pDB_PsiSiObjs->QueryByKey(usKeyID);
			//		if (pPVT != NULL)
			//		{
			//			if (pPVT->IsNormalSectionSyntax() == 1)
			//			{
			//				int section_number = (dwSelectItemData & 0x000000ff);
			//				pSectionTrigger->SetMatchParamsForNormalSection(pPVT->GetPID(), pPVT->GetTableID(), pPVT->GetTableIDExtension(), section_number);
			//			}
			//			else
			//			{
			//				pSectionTrigger->SetMatchParamsForOtherSection(pPVT->GetPID(), pPVT->GetTableID());
			//			}
			//		}

			//		::SendMessage(pTSMagicView->GetSafeHwnd(), WM_TSMAGIC_SECTION_TRIGGER_STATE, 1, 0);

			//		if (pTSMagicView->m_kThreadParams.offline == 1)
			//		{
			//			if (pTSMagicView->m_kThreadParams.main_thread_stopped == 1)
			//			{
			//				::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_section_trigger_thread, (LPVOID) & (pTSMagicView->m_kThreadParams), 0, 0);
			//			}
			//		}
			//	}
			//}
		}
	}

	*pResult = 0;
}
