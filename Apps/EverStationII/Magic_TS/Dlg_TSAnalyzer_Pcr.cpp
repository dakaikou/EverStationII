// Dlg_Pcr.cpp : implementation file
//

#include <stdafx.h>
#include <assert.h>

#include "TSMagicView.h"
#include "Dlg_TSAnalyzer_Pcr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Pcr dialog
#include "MiddleWare\MiddleWare_TS_DBases\Include\MiddleWare_DB_Pcrs.h"

#include "..\Magic_TS\TSMagic_GuiApi.h"

#define ITEM_TEXT_ALARM_COLOR		RGB(255, 0, 0)
#define ITEM_TEXT_NORMAL_COLOR		RGB(0, 0, 0)

enum _PCR_ITEM_e
{
	PCR_ITEM_PCR_PID = 0,
	PCR_ITEM_H32_30_CODE_VALUE,
	PCR_ITEM_H29_15_CODE_VALUE,
	PCR_ITEM_NEGATIVE_DISCONTINUITY,
	PCR_ITEM_POSITIVE_DISCONTINUITY,
	PCR_ITEM_JITTER_MEAN_VALUE,
	PCR_ITEM_JITTER_VAR_VALUE,
	PCR_ITEM_JITTER_ERROR_COUNT,
	PCR_ITEM_INTERVAL_MEAN_VALUE,
	PCR_ITEM_INTERVAL_MAX_VALUE,
	PCR_ITEM_INTERVAL_ERROR_COUNT,
	PCR_ITEM_DATARATE_MEAN_VALUE,
	PCR_ITEM_DATARATE_VAR_VALUE,
} PCR_ITEM_e;

CDlg_TSAnalyzer_Pcr::CDlg_TSAnalyzer_Pcr(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TSAnalyzer_Pcr::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TSAnalyzer_Pcr)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	m_nPcrIndex = -1;
}


void CDlg_TSAnalyzer_Pcr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TSAnalyzer_Pcr)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Control(pDX, IDC_LIST_PCR_LOG, m_listPcrLog);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TSAnalyzer_Pcr, CDialog)
	//{{AFX_MSG_MAP(CDlg_TSAnalyzer_Pcr)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_Pcr message handlers

BOOL CDlg_TSAnalyzer_Pcr::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	int		width, height;
	CWnd*	pWnd;
	CRect   rectTemp;
	CRect	rectList;
	CRect   rectContainer;

	GetWindowRect(&rectContainer);
	ScreenToClient(&rectContainer);

	pWnd = GetDlgItem(IDC_LIST_PCR_LOG);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectList);
		ScreenToClient(&rectList);
		width = rectList.Width();

		m_listPcrLog.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		m_listPcrLog.InsertColumn(0, "PCR_PID", LVCFMT_CENTER, 70, -1);				//icon
		m_listPcrLog.InsertColumn(1, "科目", LVCFMT_RIGHT, 70, -1);
		m_listPcrLog.InsertColumn(2, "检测项", LVCFMT_RIGHT, 90, -1);
		width -= 240;
		m_listPcrLog.InsertColumn(3, "检测结果", LVCFMT_LEFT, width, -1);
	}

	width = (rectContainer.right - rectList.right - 10 - 10);
	height = rectList.Height();
	//	m_imageDebug.Create(IDB_LOG, 16, 1, RGB(0xab, 0xcd, 0xef));
//	m_listPcrLog.SetImageList(&m_imageDebug, LVSIL_SMALL);

	CRect rectPcrScatterDiagram(rectList.right + 10, rectList.top, rectList.right + 10, rectList.bottom);
	CRect rectPcrJitterHistgram(0, 0, 0, 0);
	CRect rectPcrIntervalHistgram(0, 0, 0, 0);
	//CRect rectPcrJitterWaveform(0, 0, 0, 0);
	//CRect rectPcrIntervalWaveform(0, 0, 0, 0);

	//rectPcrJitterWaveform.left = rectList.right + 10;
	//rectPcrJitterWaveform.top = rectList.top;
	//rectPcrJitterWaveform.right = rectPcrJitterWaveform.left + width;
	//rectPcrJitterWaveform.bottom = rectPcrJitterWaveform.top + height;

	//rectPcrIntervalWaveform.left = rectPcrJitterWaveform.right + 4;
	//rectPcrIntervalWaveform.top = rectList.top;
	//rectPcrIntervalWaveform.right = rectPcrIntervalWaveform.left + width;
	//rectPcrIntervalWaveform.bottom = rectPcrJitterWaveform.bottom;

#if SHOW_PCR_SCATTER_DIAGRAM
	//rectPcrScatterDiagram.left = rectList.right + 10;
	//rectPcrScatterDiagram.top = rectList.top;
#if SHOW_PCR_JITTER_HISTGRAM | SHOW_PCR_INTERVAL_HISTGRAM
	rectPcrScatterDiagram.right = rectPcrScatterDiagram.left + (int)(width * 0.7) - 2;
#else
	rectPcrScatterDiagram.right = rectContainer.right - 10;
#endif
	//rectPcrScatterDiagram.bottom = rectList.bottom;
#endif

#if SHOW_PCR_JITTER_HISTGRAM
	rectPcrJitterHistgram.left = rectPcrScatterDiagram.right + 5;
	rectPcrJitterHistgram.top = rectList.top;
	rectPcrJitterHistgram.right = rectContainer.right - 10;
#if SHOW_PCR_INTERVAL_HISTGRAM
	rectPcrJitterHistgram.bottom = rectList.top + (int)(height * 0.5 - 2);
#else
	rectPcrJitterHistgram.bottom = rectList.bottom;
#endif
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
	rectPcrIntervalHistgram.left = rectPcrScatterDiagram.right + 5;
#if SHOW_PCR_JITTER_HISTGRAM
	rectPcrIntervalHistgram.top = rectList.top + (int)(height * 0.5 + 2);
#else
	rectPcrIntervalHistgram.top = rectList.top;
#endif
	rectPcrIntervalHistgram.right = rectContainer.right - 10;
	rectPcrIntervalHistgram.bottom = rectList.bottom;
#endif

#if SHOW_PCR_JITTER_HISTGRAM
	if (!m_PcrJitterHistgramGraph.Create(NULL, "PCR_AC 直方图", WS_CHILD | WS_VISIBLE, rectPcrJitterHistgram, this, 0L))
	{
		TRACE0("未能创建TS流码率监控窗口\n");
		return FALSE; // 未能创建
	}
	//m_PcrJitterHistgramGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, -500, 500, "ns", -100000, 100000, 1000);				//均值点对称型
	m_PcrJitterHistgramGraph.Init_X_Axis(AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_KEYPOINT, -500, 500, "ns", -100000, 100000, 1000);				//均值点对称型
	m_PcrJitterHistgramGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 100, "%", 0, 100);				//
#endif

//#if SHOW_PCR_JITTER_WAVEFORM
//	if (!m_PcrJitterWaveformGraph.Create("PCR_AC 波形图", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, rectPcrJitterWaveform, this))
//	{
//		TRACE0("未能创建TS流码率监控窗口\n");
//		return FALSE; // 未能创建
//	}
//	m_PcrJitterWaveformGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_HIDE, 0, -1, "s", 0, -1);				//0到正无穷
//	m_PcrJitterWaveformGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY, RANGE_MARK_SHOWN, -500, 500, "ns", -10000, 10000);				//0点对称型
//#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	if (!m_PcrScatterDiagramGraph.Create(NULL, "PCR_AC 散点图", WS_CHILD | WS_VISIBLE, rectPcrScatterDiagram, this, 0L))
	{
		TRACE0("未能创建TS流码率监控窗口\n");
		return FALSE; // 未能创建
	}
	m_PcrScatterDiagramGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_SHOWN | RANGE_MARK_SHOWN_PARTIAL, 0, 40000, "us", 0, 160000, 20000);				//单向从小到大型
	m_PcrScatterDiagramGraph.Init_Y_Axis(AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY, MEASURE_PANEL_SHOWN | RANGE_MARK_SHOWN_PARTIAL, -500, 500, "ns", -100000, 100000, 1000);				//0点对称型
	//m_PcrScatterDiagramGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_SHOWN, 0, 40000, "us", 0, 150000, 10000);				//单向从小到大型
	//m_PcrScatterDiagramGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY, MEASURE_PANEL_SHOWN | RANGE_MARK_SHOWN_TOTAL, -500, 500, "ns", -100000, 100000, 1000);				//0点对称型
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
	if (!m_PcrIntervalHistgramGraph.Create(NULL, "PCR间隔 直方图", WS_CHILD | WS_VISIBLE, rectPcrIntervalHistgram, this, 0L))
	{
		TRACE0("未能创建PCR间隔监控窗口\n");
		return FALSE; // 未能创建
	}
	m_PcrIntervalHistgramGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_KEYPOINT, 0, 40000, "ms", 0, 160000, 20000);			//非对称型，DVB要求40ms，MPEG-2要求100ms
	m_PcrIntervalHistgramGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, MEASURE_PANEL_HIDE | RANGE_MARK_SHOWN_PARTIAL, 0, 100, "%", 0, 100);			//非对称型，0-100%
#endif

//#if SHOW_PCR_INTERVAL_WAVEFORM
//	if (!m_PcrIntervalWaveformGraph.Create("PCR间隔 波形图", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_BOTTOM | CBRS_FLOAT_MULTI, rectPcrIntervalWaveform, this))
//	{
//		TRACE0("未能创建TS流码率监控窗口\n");
//		return FALSE; // 未能创建
//	}
//	m_PcrIntervalWaveformGraph.Init_X_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_HIDE, 0, -1, "ms", 0, -1);			//0到正无穷
//	m_PcrIntervalWaveformGraph.Init_Y_Axis(AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX, RANGE_MARK_SHOWN, 0, 40, "ms", 0, 150, 10);			//非对称型
//#endif

	Reset();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TSAnalyzer_Pcr::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CWnd*	pWnd;
	CRect   rectTemp;
	CRect	rectList;
	CRect   rectContainer;
	CRect rectPcrJitter;
	CRect rectPcrInterval;

	GetWindowRect(&rectContainer);
	ScreenToClient(&rectContainer);

	pWnd = GetDlgItem(IDC_LIST_PCR_LOG);
	if (pWnd->GetSafeHwnd() != NULL)
	{
		pWnd->GetWindowRect(&rectList);
		ScreenToClient(&rectList);
		rectList.bottom = rectContainer.bottom - 10;
		pWnd->MoveWindow(&rectList);

		//CRect rectList;
		//m_listPcrLog.GetWindowRect(&rectList);
		int listwidth = rectList.Width();

		CHeaderCtrl* pHeaderCtrl = m_listPcrLog.GetHeaderCtrl();
		int columns = pHeaderCtrl->GetItemCount();

		for (int i = 0; i < columns - 1; i++)
		{
			listwidth -= m_listPcrLog.GetColumnWidth(i);
			listwidth -= 3;
		}
		m_listPcrLog.SetColumnWidth(columns - 1, listwidth);
	}

	int width = (rectContainer.right - rectList.right - 10 - 10);
	int height = rectList.Height();

	CRect rectPcrScatterDiagram(rectList.right + 10, rectList.top, rectList.right + 10, rectList.bottom);
	CRect rectPcrJitterHistgram(0, 0, 0, 0);
	CRect rectPcrIntervalHistgram(0, 0, 0, 0);
	//CRect rectPcrJitterWaveform(0, 0, 0, 0);
	//CRect rectPcrIntervalWaveform(0, 0, 0, 0);

	//rectPcrJitterWaveform.left = rectList.right + 10;
	//rectPcrJitterWaveform.top = rectList.top;
	//rectPcrJitterWaveform.right = rectPcrJitterWaveform.left + width;
	//rectPcrJitterWaveform.bottom = rectPcrJitterWaveform.top + height;

	//rectPcrIntervalWaveform.left = rectPcrJitterWaveform.right + 4;
	//rectPcrIntervalWaveform.top = rectList.top;
	//rectPcrIntervalWaveform.right = rectPcrIntervalWaveform.left + width;
	//rectPcrIntervalWaveform.bottom = rectPcrJitterWaveform.bottom;

#if SHOW_PCR_SCATTER_DIAGRAM
	//rectPcrScatterDiagram.left = rectList.right + 10;
	//rectPcrScatterDiagram.top = rectList.top;
#if SHOW_PCR_JITTER_HISTGRAM | SHOW_PCR_INTERVAL_HISTGRAM
	//rectPcrScatterDiagram.right = rectPcrScatterDiagram.left + (int)(width * 0.7) - 2;
	rectPcrScatterDiagram.right = rectPcrScatterDiagram.left + (int)(width * 0.618) - 2;
#else
	rectPcrScatterDiagram.right = rectContainer.right - 10;
#endif
	//rectPcrScatterDiagram.bottom = rectList.bottom;
#endif

#if SHOW_PCR_JITTER_HISTGRAM
	rectPcrJitterHistgram.left = rectPcrScatterDiagram.right + 5;
	rectPcrJitterHistgram.top = rectList.top;
	rectPcrJitterHistgram.right = rectContainer.right - 10;
#if SHOW_PCR_INTERVAL_HISTGRAM
	rectPcrJitterHistgram.bottom = rectList.top + (int)(height * 0.5 - 2);
#else
	rectPcrJitterHistgram.bottom = rectList.bottom;
#endif
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
	rectPcrIntervalHistgram.left = rectPcrScatterDiagram.right + 5;
#if SHOW_PCR_JITTER_HISTGRAM
	rectPcrIntervalHistgram.top = rectList.top + (int)(height * 0.5 + 2);
#else
	rectPcrIntervalHistgram.top = rectList.top;
#endif
	rectPcrIntervalHistgram.right = rectContainer.right - 10;
	rectPcrIntervalHistgram.bottom = rectList.bottom;
#endif

#if SHOW_PCR_JITTER_HISTGRAM
	if (m_PcrJitterHistgramGraph.GetSafeHwnd() != NULL)
	{
		m_PcrJitterHistgramGraph.MoveWindow(&rectPcrJitterHistgram);
	}
#endif

#if SHOW_PCR_INTERVAL_HISTGRAM
	if (m_PcrIntervalHistgramGraph.GetSafeHwnd() != NULL)
	{
		m_PcrIntervalHistgramGraph.MoveWindow(&rectPcrIntervalHistgram);
	}
#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	if (m_PcrScatterDiagramGraph.GetSafeHwnd() != NULL)
	{
		m_PcrScatterDiagramGraph.MoveWindow(&rectPcrScatterDiagram);
	}
#endif

#if SHOW_PCR_JITTER_WAVEFORM
	if (m_PcrJitterWaveformGraph.GetSafeHwnd() != NULL)
	{
		m_PcrJitterWaveformGraph.MoveWindow(&rectPcrJitterWaveform);
	}
#endif


#if SHOW_PCR_INTERVAL_WAVEFORM
	if (m_PcrIntervalWaveformGraph.GetSafeHwnd() != NULL)
	{
		m_PcrIntervalWaveformGraph.MoveWindow(&rectPcrIntervalWaveform);
	}
#endif
}


void CDlg_TSAnalyzer_Pcr::UpdatePCRDiagnosis(RECORD_PCR_t* pCurPcrInfo)
{
	//CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	char			pszText[MAX_TXT_CHARS];
	int				nItemCount;
	int				nOffset;

	if (pCurPcrInfo != NULL)
	{
		sprintf_s(pszText, MAX_TXT_CHARS, "0x%04X", pCurPcrInfo->PCR_PID);

		LVFINDINFO info;
		info.flags = LVFI_PARTIAL | LVFI_STRING;
		info.psz = pszText;

		nItemCount = m_listPcrLog.GetItemCount();
		nOffset = m_listPcrLog.FindItem(&info);
		if (nOffset != -1)
		{
			//找到匹配PCR_PID
			DWORD oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_H32_30_CODE_VALUE);
			if (pCurPcrInfo->PCR_code.base_32_30 != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%X", pCurPcrInfo->PCR_code.base_32_30);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_H32_30_CODE_VALUE, pCurPcrInfo->PCR_code.base_32_30);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_H29_15_CODE_VALUE);
			if (pCurPcrInfo->PCR_code.base_29_15 != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%04X", pCurPcrInfo->PCR_code.base_29_15);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H29_15_CODE_VALUE, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_H29_15_CODE_VALUE, pCurPcrInfo->PCR_code.base_29_15);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY);
			if (pCurPcrInfo->pcr_code_negative_discontinuity_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_code_negative_discontinuity_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_negative_discontinuity_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY);
			if (pCurPcrInfo->pcr_code_positive_discontinuity_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_code_positive_discontinuity_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_positive_discontinuity_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_JITTER_MEAN_VALUE);
			if (pCurPcrInfo->jitter_mean_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_mean_value);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, pCurPcrInfo->jitter_mean_value);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_JITTER_VAR_VALUE);
			if (pCurPcrInfo->jitter_rms_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_rms_value);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_VAR_VALUE, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_VAR_VALUE, pCurPcrInfo->jitter_rms_value);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_JITTER_ERROR_COUNT);
			if (pCurPcrInfo->pcr_ac_error_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_ac_error_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, pCurPcrInfo->pcr_ac_error_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE);
			if (pCurPcrInfo->interval_mean_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_mean_value);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, pCurPcrInfo->interval_mean_value);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE);
			if (pCurPcrInfo->interval_max_value != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_max_value);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, pCurPcrInfo->interval_max_value);
			}

			oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT);
			if (pCurPcrInfo->pcr_interval_error_count != oldValue)
			{
				sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_interval_error_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_ALARM_COLOR);
				m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, 3, pszText);
				m_listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, pCurPcrInfo->pcr_interval_error_count);
				m_listPcrLog.SetTextColor(ITEM_TEXT_NORMAL_COLOR);
			}

			//oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE);
			//if (pCurPcrInfo->encoder_bitrate_mean_value != oldValue)
			//{
			//	sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_mean_value);
			//	m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 3, pszText);
			//	m_listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, pCurPcrInfo->encoder_bitrate_mean_value);
			//}

			//oldValue = (uint32_t)m_listPcrLog.GetItemData(nOffset + PCR_ITEM_DATARATE_VAR_VALUE);
			//if (pCurPcrInfo->encoder_bitrate_rms_value != oldValue)
			//{
			//	sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_rms_value);
			//	m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, 3, pszText);
			//	m_listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, pCurPcrInfo->encoder_bitrate_rms_value);
			//}
		}
		else
		{
			//未找到匹配PCR_PID
			nOffset = nItemCount;

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_PCR_PID, pszText);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_PCR_PID, 1, "------");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_PCR_PID, 2, "--------");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_PCR_PID, 3, "---------------");
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_PCR_PID, pCurPcrInfo->PCR_PID);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_H32_30_CODE_VALUE, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 1, "PCR编码");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 2, "(32-30位)");
			sprintf_s(pszText, sizeof(pszText), "%X", pCurPcrInfo->PCR_code.base_32_30);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H32_30_CODE_VALUE, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_H32_30_CODE_VALUE, pCurPcrInfo->PCR_code.base_32_30);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_H29_15_CODE_VALUE, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H29_15_CODE_VALUE, 2, "(29-15位)");
			sprintf_s(pszText, sizeof(pszText), "%04X", pCurPcrInfo->PCR_code.base_29_15);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_H29_15_CODE_VALUE, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_H29_15_CODE_VALUE, pCurPcrInfo->PCR_code.base_29_15);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, 2, "负跳变");
			sprintf_s(pszText, MAX_TXT_CHARS, "%d 次", pCurPcrInfo->pcr_code_negative_discontinuity_count);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_NEGATIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_negative_discontinuity_count);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, 2, "正跳变");
			sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_code_positive_discontinuity_count);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_POSITIVE_DISCONTINUITY, pCurPcrInfo->pcr_code_positive_discontinuity_count);


			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 1, "PCR抖动");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 2, "均值");
			sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_mean_value);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_MEAN_VALUE, pCurPcrInfo->jitter_mean_value);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_JITTER_VAR_VALUE, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_VAR_VALUE, 2, "方差");
			sprintf_s(pszText, sizeof(pszText), "%d ns", pCurPcrInfo->jitter_rms_value);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_VAR_VALUE, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_VAR_VALUE, pCurPcrInfo->jitter_rms_value);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, 2, "异常");
			sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_ac_error_count);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_JITTER_ERROR_COUNT, pCurPcrInfo->pcr_ac_error_count);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 1, "PCR间隔");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 2, "均值");
			sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_mean_value);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MEAN_VALUE, pCurPcrInfo->interval_mean_value);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, 2, "最大值");
			sprintf_s(pszText, sizeof(pszText), "%d us", pCurPcrInfo->interval_max_value);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_MAX_VALUE, pCurPcrInfo->interval_max_value);

			m_listPcrLog.InsertItem(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, "");
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, 2, "异常");
			sprintf_s(pszText, sizeof(pszText), "%d 次", pCurPcrInfo->pcr_interval_error_count);
			m_listPcrLog.SetItemText(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, 3, pszText);
			m_listPcrLog.SetItemData(nOffset + PCR_ITEM_INTERVAL_ERROR_COUNT, pCurPcrInfo->pcr_interval_error_count);

			//m_listPcrLog.InsertItem(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, "");
			//m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 1, "码率");
			//m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 2, "均值");
			//sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_mean_value);
			//m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, 3, pszText);
			//m_listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_MEAN_VALUE, pCurPcrInfo->encoder_bitrate_mean_value);

			//m_listPcrLog.InsertItem(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, "");
			//m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, 2, "方差");
			//sprintf_s(pszText, sizeof(pszText), "%d bps", (int)pCurPcrInfo->encoder_bitrate_rms_value);
			//m_listPcrLog.SetItemText(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, 3, pszText);
			//m_listPcrLog.SetItemData(nOffset + PCR_ITEM_DATARATE_VAR_VALUE, pCurPcrInfo->encoder_bitrate_rms_value);
		}
	}
}


void CDlg_TSAnalyzer_Pcr::UpdatePCRObservation(int ID, int curInterval, int curJitter)
{
	CTSMagicView* pTSMagicView = CTSMagicView::GetView();

	//SAMPLE_ATTRIBUTE_t attrInterval, attrJitter;

#if SHOW_PCR_INTERVAL_HISTGRAM
	m_PcrIntervalHistgramGraph.AppendSample(ID, curInterval);
#endif

#if SHOW_PCR_INTERVAL_WAVEFORM
	m_PcrIntervalWaveformGraph.AppendSample(ID, curInterval, &attrInterval);
#endif

#if SHOW_PCR_JITTER_HISTGRAM
	m_PcrJitterHistgramGraph.AppendSample(ID, curJitter);
#endif

#if SHOW_PCR_JITTER_WAVEFORM
	m_PcrJitterWaveformGraph.AppendSample(ID, curJitter, &attrJitter);
#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	m_PcrScatterDiagramGraph.AppendSample(ID, curInterval, curJitter);
#endif
}


void CDlg_TSAnalyzer_Pcr::Reset(void)
{
#if SHOW_PCR_JITTER_HISTGRAM
	m_PcrJitterHistgramGraph.Reset();
#endif
#if SHOW_PCR_INTERVAL_HISTGRAM
	m_PcrIntervalHistgramGraph.Reset();
#endif

#if SHOW_PCR_SCATTER_DIAGRAM
	m_PcrScatterDiagramGraph.Reset();
#endif

#if SHOW_PCR_JITTER_WAVEFORM
	m_PcrJitterWaveformGraph.Reset();
#endif

#if SHOW_PCR_INTERVAL_WAVEFORM
	m_PcrIntervalWaveformGraph.Reset();
#endif

	m_listPcrLog.DeleteAllItems();

	UpdateData(FALSE);
}



void CDlg_TSAnalyzer_Pcr::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}
