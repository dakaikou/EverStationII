// Pane_BouquetInfoTreeView.cpp : implementation file
//

#include "stdafx.h"
#include "NaviTree_Bouquets.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"
#include "MiddleWare\MiddleWare_PsiSiTable\Include\MiddleWare_PSISI_ErrorCode.h"
#include "MiddleWare/MiddleWare_TS_DBases/Include/MiddleWare_DB_PsiSiObjs.h"

#include "libs_MPEG&DVB\MPEG_DVB_Section\Include\DVB_SI_Utilities.h"
#include "libs_Utilities\Include\XStream_Utilities.h"

#include "..\Common\define.h"

#include "TSMagicView.h"

/////////////////////////////////////////////////////////////////////////////
// CPane_BouquetInfoTreeView

IMPLEMENT_DYNCREATE(CNaviTree_Bouquets, CTreeView)

CNaviTree_Bouquets::CNaviTree_Bouquets()
{
	m_hRootItem = NULL;
}

CNaviTree_Bouquets::~CNaviTree_Bouquets()
{
	m_hRootItem = NULL;
}

BEGIN_MESSAGE_MAP(CNaviTree_Bouquets, CTreeView)
	//{{AFX_MSG_MAP(CPane_BouquetInfoTreeView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_UPDATE_NIT, OnUpdateNIT)
//	ON_MESSAGE(WM_UPDATE_SDT, OnUpdateSDT)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPane_BouquetInfoTreeView drawing

void CNaviTree_Bouquets::OnDraw(CDC* pDC)
{
//	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CPane_BouquetInfoTreeView diagnostics

#ifdef _DEBUG
void CNaviTree_Bouquets::AssertValid() const
{
	CTreeView::AssertValid();
}

void CNaviTree_Bouquets::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPane_BouquetInfoTreeView message handlers

BOOL CNaviTree_Bouquets::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle = dwStyle | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CNaviTree_Bouquets::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	TV_INSERTSTRUCT		TreeCtrlItem;
	char				pszText[64];

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	m_ImageList.Create(IDB_BOUQUET, 20, 1, RGB(255, 255, 255));
	treeCtrl.SetImageList(&m_ImageList, TVSIL_NORMAL);

	TreeCtrlItem.hParent = TVI_ROOT;

#if CHINESE_VERSION
	strcpy_s(pszText, sizeof(pszText), "业务群信息");
#else
	strcpy_s(pszText, sizeof(pszText), "Bouquet Information");
#endif
	m_hRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hRootItem, 0);
	treeCtrl.SetItemImage(m_hRootItem, 0, 0);

	treeCtrl.Expand(m_hRootItem, TVE_EXPAND);

	return 0;
}

void CNaviTree_Bouquets::UpdateBAT(CBAT* pBAT)
{
	TV_INSERTSTRUCT TreeCtrlItem;
	char			pszText[MAX_TXT_CHARS];
	char			pszTemp[48];
	int				find_bouquet_item;
	DWORD			item_data;

	HTREEITEM				hChildItem;
	HTREEITEM				hBouquetItem;
	HTREEITEM				hNextItem;

	uint16_t bouquet_id;

	CTreeCtrl& treeCtrl = GetTreeCtrl();

	if (pBAT != NULL)
	{
		bouquet_id = pBAT->GetBouquetID();

		int rtcode = pBAT->GetBouquetName(pszTemp, sizeof(pszTemp));

		if (rtcode == MIDDLEWARE_PSISI_NO_ERROR)
		{
#if CHINESE_VERSION
			sprintf_s(pszText, sizeof(pszText), "业务群 0x%04X(%d) [%s]", bouquet_id, bouquet_id, pszTemp);
#else
			sprintf_s(pszText, sizeof(pszText), "BOUQUET 0x%04X(%d) [%s]", bouquet_id, bouquet_id, pszTemp);
#endif

			item_data = pBAT->GetKey();
			item_data <<= 16;
			item_data |= bouquet_id;

			TreeCtrlItem.item.pszText = pszText;
			TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

			find_bouquet_item = 0;
			if (treeCtrl.ItemHasChildren(m_hRootItem))
			{
				hBouquetItem = treeCtrl.GetChildItem(m_hRootItem);
				while (hBouquetItem != NULL)
				{
					uint32_t temp = (uint32_t)treeCtrl.GetItemData(hBouquetItem);
					if (item_data == temp)
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
				TreeCtrlItem.hParent = m_hRootItem;

				hChildItem = treeCtrl.InsertItem(&TreeCtrlItem);
				treeCtrl.SetItemData(hChildItem, item_data);
				treeCtrl.SetItemImage(hChildItem, 1, 1);

				treeCtrl.SortChildren(m_hRootItem);

				treeCtrl.Expand(m_hRootItem, TVE_EXPAND);
			}
			else
			{
				//xiaowangtest.ts，section_number=1的section先传，section_number=0的section后传，将导致bouquet_name在界面上不显示
				//为修订此BUG，在此强行再设置一下bouquet名称。
				//这么做不是最好的方法，容待以后再优化
				//chendelin  2018.8.25
				treeCtrl.SetItemText(hBouquetItem, pszText);
			}
		}
	}
}

void CNaviTree_Bouquets::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
#if DEBUG_BAT
	CTreeCtrl& treeCtrl = GetTreeCtrl();

	HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
	DWORD code = (DWORD)treeCtrl.GetItemData(hSelItem);

	if (code > 0)
	{
		m_pInfoTree->Reset();

		//uint16_t usKey = (code & 0xffff0000) >> 16;

		CTSMagicView* pTSMagicView = CTSMagicView::GetView();
		CDB_PsiSiObjs* pDB_PsiSiOjbs = pTSMagicView->GetPsiSiObjsDBase();

		HALForXMLDoc xmlDoc;
		pDB_PsiSiOjbs->BuildBouquetTree(code, &xmlDoc);
		m_pInfoTree->ShowXMLDoc(&xmlDoc);

#ifdef _DEBUG
		char	pszExeFile[MAX_PATH];
		char	exeDrive[3];
		char	pszXmlDir[MAX_PATH];
		char	pszFilePath[MAX_PATH];
		GetModuleFileName(NULL, pszExeFile, MAX_PATH);
		exeDrive[0] = pszExeFile[0];
		exeDrive[1] = pszExeFile[1];
		exeDrive[2] = '\0';

		sprintf_s(pszXmlDir, sizeof(pszXmlDir), "%s\\~EverStationII\\xml", exeDrive);
		BuildDirectory(pszXmlDir);

		sprintf_s(pszFilePath, sizeof(pszFilePath), "%s\\BOUQUET_sematics_0x%04X.xml", pszXmlDir, code & 0xffff);
		xmlDoc.SaveFile(pszFilePath);
#endif
	}
#endif

	*pResult = 0;
}

void CNaviTree_Bouquets::DeleteChildItems(HTREEITEM hParentItem)
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

void CNaviTree_Bouquets::Reset(void)
{
	DeleteChildItems(m_hRootItem);
}

//void CPane_BouquetInfoTreeView::Set(int offline)
//{
//}