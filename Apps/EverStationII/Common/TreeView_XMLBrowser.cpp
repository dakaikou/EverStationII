// View_TSPacketSyntaxTree.cpp : implementation file
//

#include "stdafx.h"
//#include "EverStation.h"
#include "TreeView_XMLBrowser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeView_PsisiSyntax

//#include "MiddleWare/MiddleWare_Tools\Include\MiddleWare_Tools.h"
//#include "..\common\define.h"
//#include "..\Common\GuiCommon_TreeFunction.h"

IMPLEMENT_DYNCREATE(CTreeView_PacketSyntax, CTreeView)

CTreeView_PacketSyntax::CTreeView_PacketSyntax()
{
	m_strTitle = "Unknown Tree";
	m_hRootItem = NULL;
}

CTreeView_PacketSyntax::~CTreeView_PacketSyntax()
{
}


BEGIN_MESSAGE_MAP(CTreeView_PacketSyntax, CTreeView)
	//{{AFX_MSG_MAP(CTreeView_PesEsSyntax)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, &CTreeView_PacketSyntax::OnTvnKeydown)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &CTreeView_PacketSyntax::OnTvnSelchanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeView_PesEsSyntax drawing

void CTreeView_PacketSyntax::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeView_PesEsSyntax diagnostics

#ifdef _DEBUG
void CTreeView_PacketSyntax::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeView_PacketSyntax::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeView_PsisiSyntax message handlers

BOOL CTreeView_PacketSyntax::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CTreeView_PacketSyntax::Init(char* pszTitle)
{
	m_strTitle = pszTitle;
}

//void CTreeView_PacketSyntax::DeleteChildItems(HTREEITEM hParentItem)
//{
//	CTreeCtrl&		treeCtrl = GetTreeCtrl();
//	HTREEITEM hChildItem;
//	HTREEITEM hOldItem;
//
//	if (treeCtrl.ItemHasChildren(hParentItem))
//	{
//		hChildItem = treeCtrl.GetChildItem(hParentItem);
//
//		while (1)
//		{
//			hOldItem = hChildItem;
//			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
//
//			treeCtrl.DeleteItem(hOldItem);
//
//			if (hChildItem == NULL)
//			{
//				break;
//			}
//		}
//	}
//}

void CTreeView_PacketSyntax::Reset(void)
{
	CTreeCtrl&		treeCtrl = GetTreeCtrl();
	treeCtrl.DeleteAllItems();

	char			pszText[256];
	TV_INSERTSTRUCT TreeCtrlItem;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	sprintf_s(pszText, sizeof(pszText), "%s\0", m_strTitle.GetBuffer());
	TreeCtrlItem.hParent = TVI_ROOT;
	TreeCtrlItem.hInsertAfter = TVI_FIRST;
	m_hRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	treeCtrl.SetItemData(m_hRootItem, 0xFFFFFFFF);

}

int CTreeView_PacketSyntax::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	//char			pszText[256];
	//CTreeCtrl&		treeCtrl = GetTreeCtrl();
	//TV_INSERTSTRUCT TreeCtrlItem;

	//TreeCtrlItem.item.pszText = pszText;
	//TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	//strcpy_s(pszText, sizeof(pszText), "TS包语法分析");
	//TreeCtrlItem.hParent = TVI_ROOT;
	//m_hRootItem = treeCtrl.InsertItem(&TreeCtrlItem);
	//treeCtrl.SetItemData(m_hRootItem, 0xFFFFFFFF);
	
	return 0;
}

void CTreeView_PacketSyntax::ShowXMLDoc(tinyxml2::XMLDocument* pxmlDoc)
{
	CTreeCtrl&		treeCtrl = GetTreeCtrl();
	treeCtrl.DeleteAllItems();

	Preorder(TVI_ROOT, pxmlDoc->RootElement());

	m_hRootItem = treeCtrl.GetRootItem();
	treeCtrl.Expand(m_hRootItem, TVE_EXPAND);
}

HTREEITEM CTreeView_PacketSyntax::DisplaySingleNode(HTREEITEM hParent, tinyxml2::XMLElement* pxmlNode)
{
	CTreeCtrl&		treeCtrl = GetTreeCtrl();
	char			pszText[256];
	char			pszElement[256];
	TV_INSERTSTRUCT TreeCtrlItem;
	DWORD			dwParam;

	TreeCtrlItem.item.pszText = pszText;
	TreeCtrlItem.item.mask = TVIF_TEXT | TVIF_PARAM;

	TreeCtrlItem.hParent = hParent;
	TreeCtrlItem.hInsertAfter = TVI_LAST;

	int bits = pxmlNode->IntAttribute("bits", -1);

	if ((bits > 0) && (bits <= 64))
	{
		const char* pszMnemonic = pxmlNode->Attribute("mnemonic");
		if (bits > 1)
		{
			if (pszMnemonic != NULL)
			{
				sprintf_s(pszElement, sizeof(pszElement), "%s <%d bits | '%s'>", pxmlNode->Name(), bits, pszMnemonic);
			}
			else
			{
				sprintf_s(pszElement, sizeof(pszElement), "%s <%d bits>", pxmlNode->Name(), bits);
			}
		}
		else
		{
			if (pszMnemonic != NULL)
			{
				sprintf_s(pszElement, sizeof(pszElement), "%s <%d bit | '%s'>", pxmlNode->Name(), bits, pszMnemonic);
			}
			else
			{
				sprintf_s(pszElement, sizeof(pszElement), "%s <%d bit>", pxmlNode->Name(), bits);
			}
		}

		unsigned int value = pxmlNode->UnsignedAttribute("value", 0xffffffff);
		if (bits == 1)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = %d", pszElement, value);
		}
		else if (bits == 2)
		{
			char pszTemp[16];
			_itoa_s(value, pszTemp, 2);

			if (strlen(pszTemp) == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "%s = b0%s(%d)", pszElement, pszTemp, value);
			}
			else
			{
				sprintf_s(pszText, sizeof(pszText), "%s = b%s(%d)", pszElement, pszTemp, value);
			}
		}
		else if (bits <= 4)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%X(%d)", pszElement, value, value);
			//char pszTemp[16];
			//_itoa_s(value, pszTemp, 2);
			//sprintf_s(pszText, sizeof(pszText), "%s = 0b%s(%d)", pNode->data.name, pszTemp, value);
		}
		else if (bits <= 8)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%02X(%d)", pszElement, value, value);
		}
		else if (bits <= 12)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%03X(%d)", pszElement, value, value);
		}
		else if (bits <= 16)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%04X(%d)", pszElement, value, value);
		}
		else if (bits <= 20)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%05X(%d)", pszElement, value, value);
		}
		else if (bits <= 24)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%06X(%d)", pszElement, value, value);
		}
		else if (bits <= 28)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%07X(%d)", pszElement, value, value);
		}
		else if (bits <= 32)
		{
			sprintf_s(pszText, sizeof(pszText), "%s = 0x%08X(%u)", pszElement, value, value);
		}
		else if (bits <= 64)
		{
			int64_t ull_value = pxmlNode->Int64Attribute("value", 0xffffffffffffffff);
			sprintf_s(pszText, sizeof(pszText), "%s = %#llX(%I64d)", pszElement, ull_value, ull_value);
		}
	}
	else
	{
		int bytes = pxmlNode->IntAttribute("bytes", -1);
		if ((bytes > 0) && (bytes <= 4))	//added by chendelin 2018.7.26
		{
			if (bytes > 1)
			{
				sprintf_s(pszElement, sizeof(pszElement), "%s <%d bytes>", pxmlNode->Name(), bytes);
			}
			else
			{
				sprintf_s(pszElement, sizeof(pszElement), "%s <%d byte>", pxmlNode->Name(), bytes);
			}

			unsigned int value = pxmlNode->UnsignedAttribute("value", 0xffffffff);
			if (bytes == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "%s = 0x%02X(%d)", pszElement, value, value);
			}
			else if (bytes == 2)
			{
				sprintf_s(pszText, sizeof(pszText), "%s = 0x%04X(%d)", pszElement, value, value);
			}
			else if (bytes == 3)
			{
				sprintf_s(pszText, sizeof(pszText), "%s = 0x%06X(%d)", pszElement, value, value);
			}
			else if (bytes == 4)
			{
				sprintf_s(pszText, sizeof(pszText), "%s = 0x%08X(%u)", pszElement, value, value);
			}
		}
		else
		{
			const char* pszValue = pxmlNode->Attribute("value");
			if (pszValue != NULL)
			{
				sprintf_s(pszText, sizeof(pszText), "%s = %s", pxmlNode->Name(), pszValue);
			}
			else
			{
				unsigned int value = pxmlNode->UnsignedAttribute("value", 0xffffffff);
				if (value != 0xffffffff)
				{
					sprintf_s(pszText, sizeof(pszText), "%s = %d", pxmlNode->Name(), value);
				}
				else
				{
					sprintf_s(pszText, sizeof(pszText), "%s", pxmlNode->Name());
				}
			}
		}
	}

	const char* pszComment = pxmlNode->Attribute("comment");

	if (pszComment != NULL)
	{
		int len = (int)(strlen(pszText));

		sprintf_s(pszText + len, sizeof(pszText) - len - 1, " : [%s]", pszComment);
	}

	int field_length = pxmlNode->IntAttribute("field_length", -1);
	if (field_length > 0)
	{
		int len = (int)(strlen(pszText));

		sprintf_s(pszText + len, sizeof(pszText) - len - 1, " <-%d字节->", field_length);
	}

	const char* pszError = pxmlNode->Attribute("error");

	if (pszError != NULL)
	{
		int len = (int)(strlen(pszText));

		sprintf_s(pszText + len, sizeof(pszText) - len - 1, " *** %s ***", pszError);
	}

	HTREEITEM hItem = treeCtrl.InsertItem(&TreeCtrlItem);

	if (pxmlNode->FirstChildElement() != NULL)
	{
		//说明是个复合节点
		treeCtrl.SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);
	}

	if (pszError != NULL)
	{
		treeCtrl.SetItemState(hItem, TVIS_BOLD, TVIS_BOLD);
	}

	int offset = pxmlNode->IntAttribute("offset", -1);
	int length = pxmlNode->IntAttribute("length", -1);

	if ((offset >= 0) && (length > 0))
	{
		dwParam = offset;
		dwParam <<= 16;
		dwParam |= length;
		treeCtrl.SetItemData(hItem, dwParam);
	}

	return hItem;
}

void CTreeView_PacketSyntax::Preorder(HTREEITEM hParent, tinyxml2::XMLElement* pxmlNode)
{
	if (pxmlNode != NULL)
	{
		//cout << " " << pNode->data << " ";
		HTREEITEM hItem = DisplaySingleNode(hParent, pxmlNode);

		Preorder(hItem, pxmlNode->FirstChildElement());
		Preorder(hParent, pxmlNode->NextSiblingElement());
	}
}

void CTreeView_PacketSyntax::OnTvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//CTreeCtrl& treeCtrl = GetTreeCtrl();

	//HTREEITEM hSelItem = treeCtrl.GetSelectedItem();
	//DWORD uiValue = treeCtrl.GetItemData(hSelItem);

	*pResult = 0;
}

void CTreeView_PacketSyntax::OnTvnSelchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	//TVITEM item = pNMTreeView->itemNew;

	//HTREEITEM hSelItem = item.hItem;
	//if (item.mask & TVIF_PARAM)
	//{
	//	DWORD uiValue = item.lParam;
	//	char pszTemp[64];
	//	sprintf_s(pszTemp, 64, "CHILD: OnTvnSelchanged(), uiValue=0x%08X\0", uiValue);
	//	AfxMessageBox(pszTemp);
	//}

	::SendMessage(m_hNotifyParent, WM_NOTIFY, NULL, (LPARAM)pNMHDR);

	*pResult = 0;
}
