// Dlg_PacketDecimate.cpp : implementation file
//

#include "stdafx.h"

#include <assert.h>

//#include "EverStation.h"
#include "Dlg_TSAnalyzer_TSPacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Packets dialog
#include "TSMagic_GuiApi.h"
#include "TSMagic_GuiApi_MSG.h"
#include "TSMagic_Decimate_TSPacket.h"

#include "TSMagicView.h"
#include "Dlg_TSAnalyzer_TSPacket.h"

#include "MiddleWare/MiddleWare_TransportStream/Include/MiddleWare_TransportStream.h"
#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities.h"

CDlg_TSAnalyzer_Packets::CDlg_TSAnalyzer_Packets(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Packets::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Packets)
	m_nStart = 0;
	m_nEnd = 0;
	m_nLength = 0;
	m_nTotal = 0;
	//}}AFX_DATA_INIT
}


void CDlg_TSAnalyzer_Packets::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Packets)
	DDX_Control(pDX, IDC_SLIDER_FILE, m_sliderFile);
	DDX_Control(pDX, IDC_LIST_PID, m_listPID);
	DDX_Text(pDX, IDC_EDIT_START, m_nStart);
	DDX_Text(pDX, IDC_EDIT_END, m_nEnd);
	DDX_Text(pDX, IDC_EDIT_LENGTH, m_nLength);
	DDX_Text(pDX, IDC_EDIT_TOTAL, m_nTotal);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Packets, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Packets)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_DECIMATE_TS2TS, OnBtnDecimateTS2TS)
	ON_BN_CLICKED(IDC_BTN_DECIMATE_TS2PES, OnBtnDecimateTS2PES)
	ON_BN_CLICKED(IDC_BTN_DECIMATE_TS2ES, OnBtnDecimateTS2ES)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_LEFTMARK, OnBtnLeftmark)
	ON_BN_CLICKED(IDC_BTN_RIGHTMARK, OnBtnRightmark)
	ON_EN_CHANGE(IDC_EDIT_START, OnChangeEditStart)
	ON_EN_CHANGE(IDC_EDIT_LENGTH, OnChangeEditLength)
	ON_EN_CHANGE(IDC_EDIT_END, OnChangeEditEnd)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_DECIMATE_STOP, &CDlg_TSAnalyzer_Packets::OnBnClickedBtnDecimateStop)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PID, &CDlg_TSAnalyzer_Packets::OnRclickListPid)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Packets message handlers

void CDlg_TSAnalyzer_Packets::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	int		width;
	int		height;
	CRect	rectCtrl;

	int		xoffset = 0;
	int		yoffset = 0;

	width = cx - 180;

	pWnd = GetDlgItem(IDC_SLIDER_FILE);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetClientRect(&rectCtrl);
		pWnd->SetWindowPos(NULL, 0, cy - rectCtrl.Height() - 5, width, rectCtrl.Height(), 0);

		height = cy - rectCtrl.Height() - 10;
		xoffset = width;
		yoffset = cy - rectCtrl.Height() - 5;
	}

	pWnd = GetDlgItem(IDC_BTN_LEFTMARK);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetClientRect(&rectCtrl);
//		ScreenToClient(&rectCtrl);
		pWnd->SetWindowPos(NULL, xoffset + 5, yoffset + 5, rectCtrl.Width(), rectCtrl.Height(), 0);
		xoffset += rectCtrl.Width() + 5;
	}

	pWnd = GetDlgItem(IDC_BTN_RIGHTMARK);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetClientRect(&rectCtrl);
//		ScreenToClient(&rectCtrl);
		pWnd->SetWindowPos(NULL, xoffset + 5, yoffset + 5, rectCtrl.Width(), rectCtrl.Height(), 0);
//		xoffset += rectCtrl.Width();
	}

	pWnd = GetDlgItem(IDC_LIST_PID);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->SetWindowPos(NULL, 5, 5, width - 10, height - 10, 0);

		CRect rectList;
		pWnd->GetWindowRect(&rectList);
		int listwidth = rectList.Width();

		CHeaderCtrl* pHeaderCtrl = m_listPID.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			listwidth -= m_listPID.GetColumnWidth(i);
			listwidth -= 1;
		}
		m_listPID.SetColumnWidth(columns - 1, listwidth);
	}

	xoffset = width;

	pWnd = GetDlgItem(IDC_STATIC_START);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_STATIC_END);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_STATIC_LENGTH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	
//		xoffset += rectCtrl.Width() + 10;
	}

	pWnd = GetDlgItem(IDC_STATIC_TOTAL);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	
		xoffset += rectCtrl.Width() + 20;
	}

	pWnd = GetDlgItem(IDC_EDIT_START);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_EDIT_END);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_EDIT_LENGTH);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_EDIT_TOTAL);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectCtrl);
		ScreenToClient(&rectCtrl);

		pWnd->SetWindowPos(NULL, xoffset + 5, rectCtrl.top, rectCtrl.Width(), rectCtrl.Height(), 0);
	}
}

void CDlg_TSAnalyzer_Packets::InitPIDList(void)
{
	int	nColWidth[] = {20, 120, 80, 100, 100, 100, 74, 100, 80, 560};
	CString str[] = {"", "PID", "比例(%)", "最小(Kbps)", "当前(Kbps)", "最大(Kbps)", "包数量", "连续计数错", "加扰指示", "类型描述"};
	UINT	uiAlign[] = {LVCFMT_CENTER, LVCFMT_LEFT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_RIGHT, LVCFMT_LEFT};

	m_listPID.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	for (int i = 0; i < sizeof(nColWidth) / sizeof(int); i++)
	{
		m_listPID.InsertColumn(i, str[i], uiAlign[i], nColWidth[i], -1);
	}
}

BOOL CDlg_TSAnalyzer_Packets::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	m_dlgProgress.Create(IDD_ANALYSE_PROGRESS, this);
//	m_dlgProgress.ShowWindow(SW_HIDE);

	InitPIDList();
	Reset();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Packets::UpdatePIDList(void)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();
	CDB_TSPackets* pDBTSPackets = pTSMagicView->GetTSPacketsDBase();

	int						nTotalPkCount = pDBTSPackets->m_total_packet_count;
	RECORD_TSPacket_t		TSPacketInfo;
	S32						dTotalDataRate;
	char pszTemp[256];
	U32  pid_style_code;
	S32	 rtcode;

	if (nTotalPkCount > 0)
	{
		if (pDBTSPackets->m_total_bitrate_from_hardware >= 1000000000)
		{
			dTotalDataRate = pDBTSPackets->m_total_bitrate_from_software;
		}
		else
		{
			dTotalDataRate = pDBTSPackets->m_total_bitrate_from_hardware;
		}

		int nItemCount = m_listPID.GetItemCount();
		int nItemIndex = 0;
		CString str = _T("");

		for( int pid = 0; pid < MAX_PID_COUNT; pid ++ )
		{
//			pPacketInfo = pDBTSPackets->m_pRecords[i];
			rtcode = pDBTSPackets->GetRecord(pid, &TSPacketInfo);
			if (rtcode == NO_ERROR)
			{
				pid_style_code = TSPacketInfo.ucClass;
				pid_style_code <<= 13;
				pid_style_code |= TSPacketInfo.usPID;
				pid_style_code <<= 8;
				pid_style_code |= TSPacketInfo.ucMainType;
				pid_style_code <<= 8;
				pid_style_code |= TSPacketInfo.ucStreamType;

				if (nItemIndex >= nItemCount)//need to insert an item
				{
					str.Format( "0x%04X (%d)", pid, pid );

					m_listPID.InsertItem(nItemIndex, "");
					m_listPID.SetItemData(nItemIndex, (DWORD)pid);

					m_listPID.SetItemText(nItemIndex, 1, str);										//PID
					//m_listPID.SetItemState(nItemIndex, LVIS_SELECTED, LVIS_SELECTED);

					TSPacketInfo.ratio = (float)TSPacketInfo.count / nTotalPkCount;
					str.Format( "%7.4f", 100* TSPacketInfo.ratio);
					m_listPID.SetItemText( nItemIndex, 2, str );									//比例

					TSPacketInfo.datarate_cur = TSPacketInfo.ratio * dTotalDataRate;

					if (TSPacketInfo.datarate_cur > TSPacketInfo.datarate_max)
					{
						TSPacketInfo.datarate_max = TSPacketInfo.datarate_cur;
					}
					if (TSPacketInfo.datarate_cur < TSPacketInfo.datarate_min)
					{
						TSPacketInfo.datarate_min = TSPacketInfo.datarate_cur;
					}

					str.Format( "%7.4f", TSPacketInfo.datarate_min / 1000);
					m_listPID.SetItemText( nItemIndex, 3, str );									//最小

					str.Format( "%7.4f", TSPacketInfo.datarate_cur / 1000);
					m_listPID.SetItemText( nItemIndex, 4, str );									//当前

					str.Format( "%7.4f", TSPacketInfo.datarate_max / 1000);
					m_listPID.SetItemText( nItemIndex, 5, str );									//最大

					str.Format( "%d", TSPacketInfo.count);
					m_listPID.SetItemText(nItemIndex, 6, str );										//包数量

					str.Format( "%d", TSPacketInfo.continuity_counter_error);
					m_listPID.SetItemText(nItemIndex, 7, str );										//连续计数

					m_listPID.SetItemText(nItemIndex, 8, TSPacketInfo.ts_scramble_style);			//加扰指示
					TSPacketInfo.ts_scramble_need_update = 0;

					//if (pPacketInfo->ucClass != PID_CLASS_UNKNOWN)
					if (TSPacketInfo.packet_style_need_update)
					{
//						m_listPID.SetItemText(nItemIndex, 9, pPacketInfo->pszPacketStyle);			//类型描述

						sprintf_s(pszTemp, "[0x%08X]-%s\n", pid_style_code, TSPacketInfo.pszPacketStyle);
						m_listPID.SetItemText(nItemIndex, 9, pszTemp);			//类型描述

						TSPacketInfo.packet_style_need_update = 0;
					}

					nItemCount ++;
				}
				else
				{
					int nPid = (int)m_listPID.GetItemData(nItemIndex);
					if (pid < nPid)
					{
						str.Format( "0x%04X (%d)", pid, pid );

						m_listPID.InsertItem(nItemIndex, "");
						m_listPID.SetItemData(nItemIndex, (DWORD)pid);

						m_listPID.SetItemText(nItemIndex, 1, str);									//PID

						TSPacketInfo.ratio = (float)TSPacketInfo.count / nTotalPkCount;
						str.Format( "%7.4f", 100* TSPacketInfo.ratio);
						m_listPID.SetItemText( nItemIndex, 2, str );								//比例
						
						TSPacketInfo.datarate_cur = TSPacketInfo.ratio * dTotalDataRate;
						if (TSPacketInfo.datarate_cur > TSPacketInfo.datarate_max)
						{
							TSPacketInfo.datarate_max = TSPacketInfo.datarate_cur;
						}
						if (TSPacketInfo.datarate_cur < TSPacketInfo.datarate_min)
						{
							TSPacketInfo.datarate_min = TSPacketInfo.datarate_cur;
						}

						str.Format( "%7.4f", TSPacketInfo.datarate_min / 1000);
						m_listPID.SetItemText( nItemIndex, 3, str );									//最小

						str.Format( "%7.4f", TSPacketInfo.datarate_cur / 1000);
						m_listPID.SetItemText( nItemIndex, 4, str );								//当前

						str.Format( "%7.4f", TSPacketInfo.datarate_max / 1000);
						m_listPID.SetItemText( nItemIndex, 5, str );									//最大

						str.Format( "%d", TSPacketInfo.count);
						m_listPID.SetItemText(nItemIndex, 6, str );										//包数量

						str.Format( "%d", TSPacketInfo.continuity_counter_error);
						m_listPID.SetItemText(nItemIndex, 7, str );									//连续计数

						m_listPID.SetItemText(nItemIndex, 8, TSPacketInfo.ts_scramble_style);		//加扰类型
						TSPacketInfo.ts_scramble_need_update = 0;

//						if (pPacketInfo->ucClass != PID_CLASS_UNKNOWN)
						if (TSPacketInfo.packet_style_need_update)
						{
							sprintf_s(pszTemp, "[0x%08X]-%s\n", pid_style_code, TSPacketInfo.pszPacketStyle);
							m_listPID.SetItemText(nItemIndex, 9, pszTemp);			//类型描述

							TSPacketInfo.packet_style_need_update = 0;
						}

						nItemCount ++;
					}
					else if (pid > nPid)
					{
						for(int m = nPid; m < pid; m ++)
						{
							m_listPID.DeleteItem( nItemIndex );
							nItemCount --;
						}
					}
					else
					{
						TSPacketInfo.ratio = (float)TSPacketInfo.count / nTotalPkCount;
						str.Format( "%7.4f", 100* TSPacketInfo.ratio);
						m_listPID.SetItemText( nItemIndex, 2, str );									//比例

						TSPacketInfo.datarate_cur = TSPacketInfo.ratio * dTotalDataRate;
						if (TSPacketInfo.datarate_cur > TSPacketInfo.datarate_max)
						{
							TSPacketInfo.datarate_max = TSPacketInfo.datarate_cur;
						}
						if (TSPacketInfo.datarate_cur < TSPacketInfo.datarate_min)
						{
							TSPacketInfo.datarate_min = TSPacketInfo.datarate_cur;
						}

						str.Format( "%7.4f", TSPacketInfo.datarate_min / 1000);
						m_listPID.SetItemText( nItemIndex, 3, str );									//最小

						str.Format( "%7.4f", TSPacketInfo.datarate_cur / 1000);
						m_listPID.SetItemText( nItemIndex, 4, str );									//当前

						str.Format( "%7.4f", TSPacketInfo.datarate_max / 1000);
						m_listPID.SetItemText( nItemIndex, 5, str );									//最大

						str.Format( "%d", TSPacketInfo.count);
						m_listPID.SetItemText(nItemIndex, 6, str );										//包数量

						str.Format( "%d", TSPacketInfo.continuity_counter_error);
						m_listPID.SetItemText(nItemIndex, 7, str);										//连续计数

						if (TSPacketInfo.ts_scramble_need_update)
						{
							m_listPID.SetItemText(nItemIndex, 8, TSPacketInfo.ts_scramble_style);		//加扰指示
							TSPacketInfo.ts_scramble_need_update = 0;
						}

						//if (pPacketInfo->packet_style_need_update && (pPacketInfo->ucClass != PID_CLASS_UNKNOWN))
						if (TSPacketInfo.packet_style_need_update)
						{
							sprintf_s(pszTemp, "[0x%08X]-%s\n", pid_style_code, TSPacketInfo.pszPacketStyle);
							m_listPID.SetItemText(nItemIndex, 9, pszTemp);			//类型描述
							TSPacketInfo.packet_style_need_update = 0;
						}
					}
				}
				nItemIndex ++;
			}
		}
	}
	else
	{
		m_listPID.DeleteAllItems();
	}
}

void CDlg_TSAnalyzer_Packets::Reset(void)
{
	CWnd*	pWnd;

	pWnd = GetDlgItem(IDC_STATIC_START);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_STATIC_END);
	pWnd->EnableWindow(FALSE);
	
	pWnd = GetDlgItem(IDC_STATIC_LENGTH);
	pWnd->EnableWindow(FALSE);
	pWnd->SetWindowText("抽选长度");

	pWnd = GetDlgItem(IDC_STATIC_TOTAL);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_EDIT_START);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_EDIT_END);
	pWnd->EnableWindow(FALSE);
	
	pWnd = GetDlgItem(IDC_EDIT_LENGTH);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_EDIT_TOTAL);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_SLIDER_FILE);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
	pWnd->EnableWindow(FALSE);
	pWnd->SetWindowText("抽选TS->TS");

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
	pWnd->EnableWindow(FALSE);
	pWnd->SetWindowText("抽选TS->PES");

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
	pWnd->EnableWindow(FALSE);
	pWnd->SetWindowText("抽选TS->ES");

	pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
	pWnd->EnableWindow(FALSE);
	pWnd->SetWindowText("停止");

	pWnd = GetDlgItem(IDC_BTN_LEFTMARK);
	pWnd->EnableWindow(FALSE);

	pWnd = GetDlgItem(IDC_BTN_RIGHTMARK);
	pWnd->EnableWindow(FALSE);

	m_listPID.DeleteAllItems();

	m_nStart = 0;
	m_nEnd = 0;
	m_nLength = 0;
	m_nTotal = 0;
	m_nLeftMark = 0;
	m_nRightMark = 0;
	m_bSetLeft = 1;

	m_sliderFile.SetPos(0);

	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_Packets::Set(int offline)
{
	CWnd*	pWnd;

	if (offline)
	{
//		pWnd = GetDlgItem(IDC_STATIC_START);
//		pWnd->EnableWindow(TRUE);

//		pWnd = GetDlgItem(IDC_STATIC_END);
//		pWnd->EnableWindow(TRUE);
		
		pWnd = GetDlgItem(IDC_STATIC_LENGTH);
//		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText("抽选长度");

//		pWnd = GetDlgItem(IDC_STATIC_TOTAL);
//		pWnd->EnableWindow(TRUE);

//		pWnd = GetDlgItem(IDC_EDIT_START);
//		pWnd->EnableWindow(TRUE);

//		pWnd = GetDlgItem(IDC_EDIT_END);
//		pWnd->EnableWindow(TRUE);
		
//		pWnd = GetDlgItem(IDC_EDIT_LENGTH);
//		pWnd->EnableWindow(TRUE);

//		pWnd = GetDlgItem(IDC_EDIT_TOTAL);
//		pWnd->EnableWindow(TRUE);

//		pWnd = GetDlgItem(IDC_SLIDER_FILE);
//		pWnd->EnableWindow(TRUE);

		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText("抽选TS->TS ");

//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
//		pWnd->EnableWindow(TRUE);
//		pWnd->SetWindowText("抽选TS->PES");

		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText("抽选TS->ES ");
	}
	else
	{
		pWnd = GetDlgItem(IDC_STATIC_START);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_STATIC_END);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_STATIC_LENGTH);
//		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText("录制长度");

		pWnd = GetDlgItem(IDC_STATIC_TOTAL);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_START);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_END);
		pWnd->EnableWindow(FALSE);
		
		pWnd = GetDlgItem(IDC_EDIT_LENGTH);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_EDIT_TOTAL);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_SLIDER_FILE);
		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
		pWnd->EnableWindow(TRUE);
		pWnd->SetWindowText("录制TS->TS ");

//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
//		pWnd->EnableWindow(TRUE);
//		pWnd->SetWindowText("录制TS->PES");

//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
//		pWnd->EnableWindow(TRUE);
//		pWnd->SetWindowText("录制TS->ES ");
	}
}

void CDlg_TSAnalyzer_Packets::ReportFileSize(__int64 filelength)			//unit: K 
{
	CWnd*	pWnd;
	CString str;
	int		page;
	int		line;

	if (m_sliderFile.GetSafeHwnd() != NULL)
	{
		m_nTotal = (int)filelength;

		str.Format("%d K", m_nTotal);
		pWnd = GetDlgItem(IDC_EDIT_TOTAL);
		pWnd->SetWindowText(str);

		m_nLength = m_nTotal;
		str.Format("%d K", m_nLength);
		pWnd = GetDlgItem(IDC_EDIT_LENGTH);
		pWnd->SetWindowText(str);

		m_nEnd = m_nTotal;
		str.Format("%d K", m_nEnd);
		pWnd = GetDlgItem(IDC_EDIT_END);
		pWnd->SetWindowText(str);

		m_sliderFile.SetRange(0, m_nTotal);

		page = m_nTotal / 100;
		m_sliderFile.SetPageSize(page);

		line = m_nTotal / 1000;
		m_sliderFile.SetLineSize(line);

		m_sliderFile.SetPos(0);
	}

	m_nLeftMark = m_nStart;
	m_nRightMark = m_nEnd;
}

void CDlg_TSAnalyzer_Packets::OnBtnDecimateTS2TS() 
{
	// TODO: Add your control notification handler code here
	int				nSelCount;
	WORD			usPID;
	int				nItemCount;
	int				item_index;
//	int				action = 0;
	char			pszText[MAX_TXT_CHARS];
	
	DWORD	state;

	CTSMagicView*		pWindow = CTSMagicView::GetView();
	thread_params_t*	pthread_params = &(pWindow->m_kThreadParams);
	CDB_TSPackets*		pDB_TSPackets = pWindow->GetTSPacketsDBase();
	//RECORD_TSPacket_t	TSPacketInfo;
	CWnd*				pWnd = NULL;
	S32					rtcode;

	UpdateData(TRUE);

	//用以判断其它地方的程序逻辑是否正确。
//	assert(pthread_params->main_thread_running == 1);
//	if (pthread_params->offline == 1)
//	{
//		assert(pthread_params->main_thread_stopped == 1);
//	}

	nItemCount = m_listPID.GetItemCount();
	nSelCount = 0;
	for (item_index = 0; item_index < nItemCount; item_index ++)
	{
		state = m_listPID.GetItemState(item_index, LVIS_SELECTED);
		if ((state & LVIS_SELECTED) == LVIS_SELECTED)
		{
			usPID = (WORD)m_listPID.GetItemData(item_index);

//			pPacketInfo = pDB_TSPackets->m_pRecords[usPID];
//			rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);
			rtcode = pDB_TSPackets->SetDecimateFlag(usPID, 1);
			if (rtcode == NO_ERROR)
			{
				//TSPacketInfo.bDecimate = 1;
				nSelCount ++;
			}
		}
	}

	if (nSelCount > 0)
	{
		if ((pWindow->m_kThreadParams.ts_trigger_thread_running == 1) ||
			(pWindow->m_kThreadParams.es_trigger_thread_running == 1) ||
			(pWindow->m_kThreadParams.section_trigger_thread_running == 1) ||
			(pWindow->m_kThreadParams.packet_decimate_thread_running == 1) ||
			(pWindow->m_kThreadParams.dsmcc_download_thread_running == 1))
		{
			if (pWindow->m_kThreadParams.ts_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制：未能启动，因为发现TS捕捉线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.es_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制：未能启动，因为发现ES捕捉线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.section_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制：未能启动，因为发现section捕捉线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.packet_decimate_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制：未能启动，因为发现TS录制线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.dsmcc_download_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制：未能启动，因为发现DSMCC下载线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
		}
		else
		{
			if (pthread_params->offline == 1)
			{
				if (pWindow->m_kThreadParams.main_thread_stopped == 1)
				{
					pthread_params->packet_decimate_thread_stopped = 0;			//若抽选线程退出时，此状态值将由抽选线程更改
					::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_ts2ts_decimate_thread, (LPVOID)&(pWindow->m_kThreadParams), 0, 0);
				}
			}

			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
			pWnd->EnableWindow(FALSE);

			pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
			pWnd->EnableWindow(TRUE);
		}
	}
	else
	{
		AfxMessageBox("未选择待录制的TS包！", MB_ICONSTOP);
	}
}

void CDlg_TSAnalyzer_Packets::OnBtnDecimateTS2PES()
{
	// TODO: 在此添加控件通知处理程序代码
/*
	int		nSel;
	WORD	usPID;
	int		rtcode;
	char	pszDebug[128];

	CTSMagicView*		pWindow = CTSMagicView::GetView();
	CTransportStream*	pts_stream = &(pWindow->m_transport_stream);

	UpdateData(TRUE);	

	nSel = m_listPID.GetSelectionMark();
	if (nSel >= 0)
	{
		usPID = (WORD)m_listPID.GetItemData(nSel);

		TS_packet_info_t*	pPacketInfo = TS_PACKET_STATISTIC_REPORT.m_pTS_packet_info[usPID];
		if (pPacketInfo != NULL)
		{
			CFileDialog fileDlg(FALSE, "*.*", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Media files(*.*)|*.*|", NULL);

			CString strFileName = _T("");

			if (fileDlg.DoModal() == IDOK)
			{
				strFileName = fileDlg.GetPathName();

				sprintf(pszDebug, "TS包抽选：存储文件名 %s", strFileName.GetBuffer(128));
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);

				m_dlgProgress.SetWindowText("流分解进度条");
				m_dlgProgress.ShowWindow(SW_SHOW);

				rtcode = pts_stream->PacketDecimate(usPID, m_nStart, m_nLength, strFileName.GetBuffer(128), DECIMATE_TS2PES);

				m_dlgProgress.ShowWindow(SW_HIDE);

				sprintf(pszDebug, "TS包抽选：文件大小 %d", rtcode);
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszDebug, (LPARAM)DEBUG_INFO);
			}
		}
	}
*/
}

void CDlg_TSAnalyzer_Packets::OnBtnDecimateTS2ES() 
{
	// TODO: Add your control notification handler code here

	int		nSel;
	WORD	usPID;
//	int		action = 0;
	CWnd*	pWnd = NULL;
	char	pszText[MAX_TXT_CHARS];

	CTSMagicView*		pWindow = CTSMagicView::GetView();
	thread_params_t*	pthread_params = &(pWindow->m_kThreadParams);
	CDB_TSPackets*		pDB_TSPackets = pWindow->GetTSPacketsDBase();
	//RECORD_TSPacket_t	TSPacketInfo;
	S32					rtcode;

	UpdateData(TRUE);	

	//用以判断其它地方的程序逻辑是否正确。
	assert(pthread_params->main_thread_running == 1);
	if (pthread_params->offline == 1)
	{
		assert(pthread_params->main_thread_stopped == 1);
	}

	nSel = m_listPID.GetSelectionMark();
	if (nSel >= 0)
	{
		if ((pWindow->m_kThreadParams.ts_trigger_thread_running == 1) ||
			(pWindow->m_kThreadParams.es_trigger_thread_running == 1) ||
			(pWindow->m_kThreadParams.section_trigger_thread_running == 1) ||
			(pWindow->m_kThreadParams.packet_decimate_thread_running == 1) ||
			(pWindow->m_kThreadParams.dsmcc_download_thread_running == 1))
		{
			if (pWindow->m_kThreadParams.ts_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制(TS->ES)：未能启动，因为发现TS捕捉线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.es_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制(TS->ES)：未能启动，因为发现ES捕捉线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.section_trigger_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制(TS->ES)：未能启动，因为发现section捕捉线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.packet_decimate_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制(TS->ES)：未能启动，因为发现TS录制线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
			if (pWindow->m_kThreadParams.dsmcc_download_thread_running == 1)
			{
				sprintf_s(pszText, sizeof(pszText), "TS包录制(TS->ES)：未能启动，因为发现DSMCC下载线程尚未结束！");
				::SendMessage(pWindow->GetSafeHwnd(), WM_TSMAGIC_APPEND_LOG, (WPARAM)pszText, (LPARAM)DEBUG_ERROR);
			}
		}
		else
		{
			usPID = (WORD)m_listPID.GetItemData(nSel);
//			pPacketInfo = pDB_TSPackets->m_pRecords[usPID];
			//rtcode = pDB_TSPackets->GetRecord(usPID, &TSPacketInfo);
			rtcode = pDB_TSPackets->SetDecimateFlag(usPID, 1);
			if (rtcode == NO_ERROR)
			{
				//pPacketInfo->bDecimate = 1;

				if (pthread_params->offline == 1)
				{
					if (pthread_params->main_thread_stopped == 1)
					{
						::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)TSMagic_ts2es_decimate_thread, (LPVOID)&(pWindow->m_kThreadParams), 0, 0);
					}
				}

				pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
				pWnd->EnableWindow(FALSE);

				pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
				pWnd->EnableWindow(FALSE);

				pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
				pWnd->EnableWindow(FALSE);

				pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
				pWnd->EnableWindow(TRUE);
			}
		}
	}
	else
	{
		AfxMessageBox("未选择待录制的TS包！", MB_ICONSTOP);
	}
}

void CDlg_TSAnalyzer_Packets::OnBnClickedBtnDecimateStop()
{
	// TODO: 在此添加控件通知处理程序代码
//	int		nSelCount;
	WORD	usPID;
	int		nItemCount;
	int		item_index;
//	int		action = 0;
//	DWORD	state;

	CTSMagicView*		pWindow = CTSMagicView::GetView();
	thread_params_t*	pthread_params = &(pWindow->m_kThreadParams);
	CDB_TSPackets*		pDB_TSPackets = pWindow->GetTSPacketsDBase();
	//RECORD_TSPacket_t*	pPacketInfo;
	CWnd*				pWnd = NULL;
	S32					rtcode;

	UpdateData(TRUE);

	//禁止停止按钮
	pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
	pWnd->EnableWindow(FALSE);

	nItemCount = m_listPID.GetItemCount();
//	nSelCount = 0;
	for (item_index = 0; item_index < nItemCount; item_index ++)
	{
		usPID = (WORD)m_listPID.GetItemData(item_index);

//		pPacketInfo = pDB_TSPackets->m_pRecords[usPID];
		rtcode = pDB_TSPackets->SetDecimateFlag(usPID, 0);
		//if (pPacketInfo != NULL)
		//{
		//	pPacketInfo->bDecimate = 0;
		//}
	}

	if (pthread_params->packet_decimate_thread_running == 1)
	{
		//若抽选线程仍在运行
		pthread_params->packet_decimate_thread_running = 0;			//强制抽选线程退出
		Sleep(100);
	}


//	if (pthread_params->offline == 0)
//	{
		//如果是实时分析，则设置一下控件状态
//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
//		pWnd->EnableWindow(TRUE);

//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
//		pWnd->EnableWindow(FALSE);
//	}

	//离线状态由消息响应函数

//	pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
//	pWnd->EnableWindow(TRUE);

//	if (pthread_params->offline == 1)
//	{
//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
//		pWnd->EnableWindow(TRUE);
//	}

//	pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
//	pWnd->EnableWindow(FALSE);

//	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_Packets::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	int pos;

	if ((CSliderCtrl*)pScrollBar == &m_sliderFile)
	{
		pos = m_sliderFile.GetPos();

		if (pos > m_nEnd)
		{
			m_nEnd = pos;
		}
		m_nStart = pos;
		m_nLength = m_nEnd - m_nStart;

		UpdateData(FALSE);
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDlg_TSAnalyzer_Packets::OnBtnLeftmark() 
{
	// TODO: Add your control notification handler code here
	m_nLeftMark = m_nStart;

	m_sliderFile.ClearTics();
	m_sliderFile.SetTic(m_nLeftMark);
	m_sliderFile.SetTic(m_nRightMark);
}

void CDlg_TSAnalyzer_Packets::OnBtnRightmark() 
{
	// TODO: Add your control notification handler code here
	m_nRightMark = m_nEnd;

	m_sliderFile.ClearTics();
	m_sliderFile.SetTic(m_nLeftMark);
	m_sliderFile.SetTic(m_nRightMark);
}

void CDlg_TSAnalyzer_Packets::OnChangeEditStart() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	if (m_nStart > m_nEnd)
	{
		m_nStart = m_nEnd;
	}

	m_sliderFile.SetPos(m_nStart);

	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_Packets::OnChangeEditLength() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	if ((m_nStart + m_nLength) > m_nTotal)
	{
		m_nLength = m_nTotal - m_nStart;
	}

	if ((m_nStart + m_nLength) > m_nEnd)
	{
		m_nEnd = m_nStart + m_nLength;
	}

	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_Packets::OnChangeEditEnd() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	
	if (m_nEnd < m_nStart)
	{
		m_nEnd = m_nStart;
	}

	UpdateData(FALSE);
}

void CDlg_TSAnalyzer_Packets::ActionMsg(WPARAM wParam)
{
	CTSMagicView*		pWindow = CTSMagicView::GetView();
	thread_params_t*	pthread_params = &(pWindow->m_kThreadParams);
	CWnd* pWnd = NULL;

	if (wParam == 1)		//线程进入
	{
//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
//		pWnd->EnableWindow(FALSE);

//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
//		pWnd->EnableWindow(FALSE);

//		pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
//		pWnd->EnableWindow(FALSE);

		pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
		pWnd->EnableWindow(TRUE);
	}
	else
	{
		if (pthread_params->offline == 1)
		{
			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
			pWnd->EnableWindow(TRUE);

//			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2PES);
//			pWnd->EnableWindow(TRUE);

			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2ES);
			pWnd->EnableWindow(TRUE);
		}
		else
		{
			pWnd = GetDlgItem(IDC_BTN_DECIMATE_TS2TS);
			pWnd->EnableWindow(TRUE);
		}

		pWnd = GetDlgItem(IDC_BTN_DECIMATE_STOP);
		pWnd->EnableWindow(FALSE);
	}
}




void CDlg_TSAnalyzer_Packets::OnRclickListPid(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	CMenu	menu, *pSubMenu;
	menu.LoadMenu(IDR_RIGHT_KEY_MENU);//装载自定义的右键菜单

	pSubMenu = menu.GetSubMenu(0);//获取第一个弹出菜单，所以第一个菜单必须有子菜单

	CPoint oPoint;//定义一个用于确定光标位置的位置
	GetCursorPos(&oPoint);//获取当前光标的位置，以便使得菜单可以跟随光标

	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, oPoint.x, oPoint.y, this); //在指定位置显示弹出菜单

	*pResult = 0;
}
