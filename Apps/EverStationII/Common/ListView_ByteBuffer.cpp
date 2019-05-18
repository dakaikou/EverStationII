// Mpeg2_PsisiSyntaxListView.cpp : implementation file
//

#include "stdafx.h"
#include <assert.h>
//#include "EverStation.h"
#include "ListView_ByteBuffer.h"
#include "..\common\Define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListView_ByteBuffer
#define MAX_COLUMN_IN_LISTVIEW					18

IMPLEMENT_DYNCREATE(CListView_ByteBuffer, CListView)

unsigned int ListView_ByteBuffer_idle_operation(LPVOID lpParam)
{
	CListView_ByteBuffer* pfatherView = (CListView_ByteBuffer*)lpParam;

	pfatherView->PrepareRemainedRow();

	return 0;
}


CListView_ByteBuffer::CListView_ByteBuffer()
{
	m_pucTempBuffer = NULL;
	m_nTempLength = 0;

	m_nTotalRowCount = 0;
	m_nShowedCount = 0;
}

CListView_ByteBuffer::~CListView_ByteBuffer()
{
	if (m_pucTempBuffer != NULL)
	{
		free(m_pucTempBuffer);
		m_pucTempBuffer = NULL;
	}
	m_nTempLength = 0;
}


BEGIN_MESSAGE_MAP(CListView_ByteBuffer, CListView)
	//{{AFX_MSG_MAP(CListView_ByteBuffer)
	ON_WM_CREATE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListView_ByteBuffer drawing

void CListView_ByteBuffer::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CListView_ByteBuffer diagnostics

#ifdef _DEBUG
void CListView_ByteBuffer::AssertValid() const
{
	CListView::AssertValid();
}

void CListView_ByteBuffer::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CListView_ByteBuffer message handlers

BOOL CListView_ByteBuffer::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	dwStyle |= LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL;
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

//一次性显示完所有Buf里面的二进制数，速度有点跟不上，采取先显示可视范围内的数据，待用户滚动鼠标时，显示剩余部分
void CListView_ByteBuffer::DisplayByteBuffer(unsigned char* buf, int length)
{
	if (buf != NULL)
	{
		if (length > 0)
		{
			::WaitForSingleObject(m_hAccessEvent, INFINITE);

			if (m_pucTempBuffer != NULL)
			{
				free(m_pucTempBuffer);
				m_pucTempBuffer = NULL;
			}
			m_pucTempBuffer = (unsigned char*)malloc(length);
			m_nTempLength = length;
			if (m_pucTempBuffer != NULL)
			{
				memcpy(m_pucTempBuffer, buf, length);

				m_nTotalRowCount = (length + (m_nColCount - 1)) / m_nColCount;
				m_nShowedCount = 0;

				DisplayFirstViewedBuffer(m_pucTempBuffer, m_nTempLength);
				//::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ListView_ByteBuffer_idle_operation, (LPVOID)this, 0, 0);
			}

			::SetEvent(m_hAccessEvent);
		}
	}
}

void CListView_ByteBuffer::PrepareRemainedRow(void)
{
	CListCtrl&	listCtrl = GetListCtrl();

	char			 pszItem[256];
	//int				 left_len;
	//int				 cols;
	//unsigned char*	rdptr;

	//unsigned char* remain_buf = m_pucTempBuffer;
	//int remain_length = m_nTempLength;

	//if ((remain_buf != NULL) && (remain_length > 0))
	//{
	//	left_len = remain_length;
	//	rdptr = remain_buf;
	DWORD state = ::WaitForSingleObject(m_hAccessEvent, INFINITE);
	if (state == WAIT_OBJECT_0)
	{
		int nItemCount = listCtrl.GetItemCount();
		if (nItemCount < m_nTotalRowCount)
		{
			//增加行
			for (int item = nItemCount; item < m_nTotalRowCount; item++)
			{
				if (m_nNewComing == 1)
				{
					break;
				}
				sprintf_s(pszItem, sizeof(pszItem), "%08X\0", item * m_nColCount);
				listCtrl.InsertItem(item, pszItem);

				//if (left_len < m_nColCount)
				//{
				//	cols = left_len;
				//}
				//else
				//{
				//	cols = m_nColCount;
				//}

				//for (int j = 0; j < cols; j++)
				//{
				//	if (m_nNewComing == 1)
				//	{
				//		break;
				//	}
				//	sprintf_s(pszItem, sizeof(pszItem), "%02X", rdptr[j]);
				//	//listCtrl.SetItemText(item, j + 1, pszItem);
				//}

				//memset(pszItem, 0x00, sizeof(pszItem));

				//for (int j = 0; j < cols; j++)
				//{
				//	if (m_nNewComing == 1)
				//	{
				//		break;
				//	}
				//	if (rdptr[j] < 0x20)
				//	{
				//		sprintf_s(pszItem + j, 2, "%c", '..');
				//	}
				//	else
				//	{
				//		sprintf_s(pszItem + j, 2, "%c", rdptr[j]);
				//	}
				//}
				//pszItem[cols] = '\0';
				//listCtrl.SetItemText(item, (m_nColCount + 1), pszItem);

				//rdptr += cols;
				//left_len -= cols;

				//m_nShowedCount++;

			}
		}
		else if (nItemCount > m_nTotalRowCount)
		{
			//删除行
			for (int item = m_nTotalRowCount - 1; item >= nItemCount; item--)
			{
				listCtrl.DeleteItem(item);
			}
		}
	}
	::SetEvent(m_hAccessEvent);
}

void CListView_ByteBuffer::DisplayFirstViewedBuffer(unsigned char* remain_buf, int remain_length)
{
	CListCtrl&	listCtrl = GetListCtrl();

	int				 count = listCtrl.GetItemCount();
	char			 pszItem[256];
	int				 left_len;
	int				 cols;
	unsigned char*	rdptr;
	RECT			rectContainer;

	listCtrl.GetClientRect(&rectContainer);

	assert(count == m_nShowedCount);
	assert(m_nShowedCount == 0);

	if ((remain_buf != NULL) && (remain_length > 0))
	{
		left_len = remain_length;
		rdptr = remain_buf;
		for (int item = count; item < m_nTotalRowCount; item++)
		{
			sprintf_s(pszItem, sizeof(pszItem), "%08X", item * m_nColCount);
			listCtrl.InsertItem(item, pszItem);

			if (left_len < m_nColCount)
			{
				cols = left_len;
			}
			else
			{
				cols = m_nColCount;
			}

			for (int j = 0; j < cols; j++)
			{
				sprintf_s(pszItem, sizeof(pszItem), "%02X", rdptr[j]);
				listCtrl.SetItemText(item, j + 1, pszItem);
			}

			memset(pszItem, 0x00, sizeof(pszItem));
			for (int j = 0; j < cols; j++)
			{
				if (rdptr[j] < 0x20)
				{
					sprintf_s(pszItem + j, 2, "%c", '..');
				}
				else
				{
					sprintf_s(pszItem + j, 2, "%c", rdptr[j]);
				}
			}
			pszItem[cols] = '\n';
			listCtrl.SetItemText(item, (m_nColCount + 1), pszItem);

			rdptr += cols;
			left_len -= cols;

			m_nShowedCount++;

			POINT ptItem;
			listCtrl.GetItemPosition(item, &ptItem);

			if (ptItem.y > rectContainer.bottom)
			{
				break;
			}
		}
	}
}

//一次性显示完所有Buf里面的二进制数，速度有点跟不上，采取先显示可视范围内的数据，待用户滚动鼠标时，显示剩余部分
void CListView_ByteBuffer::DisplayRemainBuffer(unsigned char* remain_buf, int remain_length)
{
	CListCtrl&	listCtrl = GetListCtrl();

	int				 count = listCtrl.GetItemCount();
	char			 pszItem[256];
	int				 left_len;
	int				 cols;
	unsigned char*	rdptr;
	RECT			rectContainer;

	listCtrl.GetClientRect(&rectContainer);

	assert(count == m_nShowedCount);

	if ((remain_buf != NULL) && (remain_length > 0))
	{
		left_len = remain_length;
		rdptr = remain_buf;
		for (int item = count; item < m_nTotalRowCount; item++)
		{
			sprintf_s(pszItem, sizeof(pszItem), "%08X", item * m_nColCount);
			listCtrl.InsertItem(item, pszItem);

			if (left_len < m_nColCount)
			{
				cols = left_len;
			}
			else
			{
				cols = m_nColCount;
			}

			for (int j = 0; j < cols; j++)
			{
				sprintf_s(pszItem, sizeof(pszItem), "%02X", rdptr[j]);
				listCtrl.SetItemText(item, j + 1, pszItem);
			}

			memset(pszItem, 0x00, sizeof(pszItem));
			for (int j = 0; j < cols; j++)
			{
				if (rdptr[j] < 0x20)
				{
					sprintf_s(pszItem + j, 2, "%c", '..');
				}
				else
				{
					sprintf_s(pszItem + j, 2, "%c", rdptr[j]);
				}
			}
			pszItem[cols] = '\n';
			listCtrl.SetItemText(item, (m_nColCount + 1), pszItem);

			rdptr += cols;
			left_len -= cols;

			m_nShowedCount++;

			POINT ptItem;
			listCtrl.GetItemPosition(item, &ptItem);

			if (ptItem.y > rectContainer.bottom)
			{
				break;
			}
		}
	}
}

void CListView_ByteBuffer::Reset(void)
{
	CListCtrl&		listCtrl = GetListCtrl();

	//m_nNewComing = 1;			//新到了一批数据，准备下一次的显示
	//Sleep(100);					//睡100ms，等待线程退出
	::WaitForSingleObject(m_hAccessEvent, INFINITE);
	listCtrl.DeleteAllItems();
	::SetEvent(m_hAccessEvent);
}

int CListView_ByteBuffer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	CListCtrl& listCtrl = GetListCtrl();
	
	listCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	m_nColCount = 16;						//需要根据窗口的大小调整

	char pszText[16];
	int i = 0;
		
	listCtrl.InsertColumn(i, "地址(HEX)", LVCFMT_LEFT, 100, -1);

	for (i = 1; i < (m_nColCount + 1); i ++)
	{
		sprintf_s(pszText, sizeof(pszText), "%X", i - 1);

		listCtrl.InsertColumn(i, pszText, LVCFMT_CENTER, 38, -1);
	}

	listCtrl.InsertColumn(i, "", LVCFMT_LEFT, m_nColCount * 22, -1);

	m_hAccessEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	m_nNewComing = 0;
	
	return 0;
}

void CListView_ByteBuffer::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	 //TODO: 在此添加消息处理程序代码和/或调用默认值
	unsigned char* remain_buf;
	int			   remain_length;

	if ((nSBCode == SB_LINEDOWN) || (nSBCode == SB_PAGEDOWN))
	{
		if (m_nShowedCount < m_nTotalRowCount)
		{
			remain_buf = m_pucTempBuffer + m_nShowedCount * m_nColCount;
			remain_length = m_nTempLength - m_nShowedCount * m_nColCount;

			DisplayRemainBuffer(remain_buf, remain_length);
		}
	}

	CListView::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL CListView_ByteBuffer::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	 //TODO: 在此添加消息处理程序代码和/或调用默认值
	unsigned char* remain_buf;
	int			   remain_length;

	if (zDelta < 0)
	{
		if (m_nShowedCount < m_nTotalRowCount)
		{
			remain_buf = m_pucTempBuffer + m_nShowedCount * m_nColCount;
			remain_length = m_nTempLength - m_nShowedCount * m_nColCount;

			DisplayRemainBuffer(remain_buf, remain_length);
		}
	}

	return CListView::OnMouseWheel(nFlags, zDelta, pt);
}


void CListView_ByteBuffer::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	 //TODO: 在此添加消息处理程序代码和/或调用默认值
	unsigned char* remain_buf;
	int			   remain_length;

	if ((nChar == VK_DOWN) || (nChar == VK_NEXT))
	{
		if (m_nShowedCount < m_nTotalRowCount)
		{
			remain_buf = m_pucTempBuffer + m_nShowedCount * m_nColCount;
			remain_length = m_nTempLength - m_nShowedCount * m_nColCount;

			DisplayRemainBuffer(remain_buf, remain_length);
		}
	}

	CListView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CListView_ByteBuffer::OnSize(UINT nType, int cx, int cy)
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
		listwidth -= 1;
	}
	listCtrl.SetColumnWidth(columns - 1, listwidth);
}

