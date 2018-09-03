// Dlg_DtsPts.cpp : implementation file
//

#include "stdafx.h"
#include "Dlg_DtsPts.h"
#include "..\common\define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlg_TS_DtsPts dialog


CDlg_TS_DtsPts::CDlg_TS_DtsPts(CWnd* pParent /*=NULL*/)
	: CDialog(CDlg_TS_DtsPts::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlg_TS_DtsPts)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlg_TS_DtsPts::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlg_TS_DtsPts)
	DDX_Control(pDX, IDC_STATIC_VID_JITTER_GRAPH, m_graphVidJitter);
	DDX_Control(pDX, IDC_STATIC_VID_INTERVAL_GRAPH, m_graphVidInterval);
	DDX_Control(pDX, IDC_STATIC_AUD_JITTER_GRAPH, m_graphAudJitter);
	DDX_Control(pDX, IDC_STATIC_AUD_INTERVAL_GRAPH, m_graphAudInterval);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlg_TS_DtsPts, CDialog)
	//{{AFX_MSG_MAP(CDlg_TS_DtsPts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlg_TS_DtsPts message handlers

BOOL CDlg_TS_DtsPts::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CenterWindow();

	m_graphVidJitter.Init(1000, "ms");
	m_graphVidInterval.Init(2000, "ms");
	m_graphAudJitter.Init(1000, "ms");
	m_graphAudInterval.Init(2000, "ms");

	m_nMode = DTS;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlg_TS_DtsPts::Reset(void)
{
	m_graphVidJitter.Reset();
	m_graphVidInterval.Reset();
	m_graphAudJitter.Reset();
	m_graphAudInterval.Reset();
}

void CDlg_TS_DtsPts::Set(int mode, DWORD dwVidCode, DWORD dwAudCode)
{
	CWnd* pWnd = NULL;
	char		   pszText[MAX_TXT_CHARS];
	unsigned short usPcrPID;
	unsigned short usVidPID;
	unsigned short usAudPID;

	usPcrPID = (unsigned short)((dwVidCode & 0x03ffe000) >> 13);
	usVidPID = (unsigned short)(dwVidCode & 0x00001fff);
	usAudPID = (unsigned short)(dwAudCode & 0x00001fff);

	if (mode == DTS)
	{
		SetWindowText("DTS vs PCR分析");

		pWnd = GetDlgItem(IDC_STATIC_TITLE);
		pWnd->SetWindowText("DTS vs. PCR列表");

		sprintf_s(pszText, sizeof(pszText), "视频 DTS - PCR: ES_PID = 0x%04x, PCR_PID = 0x%04x", usVidPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_VID_JITTER);
		pWnd->SetWindowText(pszText);

		sprintf_s(pszText, sizeof(pszText), "视频 DTS间隔: ES_PID = 0x%04x, PCR_PID = 0x%04x", usVidPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_VID_INTERVAL);
		pWnd->SetWindowText(pszText);

		sprintf_s(pszText, sizeof(pszText), "音频 DTS - PCR: ES_PID = 0x%04x, PCR_PID = 0x%04x", usAudPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_AUD_JITTER);
		pWnd->SetWindowText(pszText);

		sprintf_s(pszText, sizeof(pszText), "音频 DTS间隔: ES_PID = 0x%04x, PCR_PID = 0x%04x", usAudPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_AUD_INTERVAL);
		pWnd->SetWindowText(pszText);
	}
	else
	{
		SetWindowText("PTS vs PCR分析");

		pWnd = GetDlgItem(IDC_STATIC_TITLE);
		pWnd->SetWindowText("PTS vs. PCR列表");

		sprintf_s(pszText, sizeof(pszText), "视频 PTS - PCR: ES_PID = 0x%04x, PCR_PID = 0x%04x", usVidPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_VID_JITTER);
		pWnd->SetWindowText(pszText);

		sprintf_s(pszText, sizeof(pszText), "视频 PTS间隔: ES_PID = 0x%04x, PCR_PID = 0x%04x", usVidPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_VID_INTERVAL);
		pWnd->SetWindowText(pszText);

		sprintf_s(pszText, sizeof(pszText), "音频 PTS - PCR: ES_PID = 0x%04x, PCR_PID = 0x%04x", usAudPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_AUD_JITTER);
		pWnd->SetWindowText(pszText);

		sprintf_s(pszText, sizeof(pszText), "音频 PTS间隔: ES_PID = 0x%04x, PCR_PID = 0x%04x", usAudPID, usPcrPID); 
		pWnd = GetDlgItem(IDC_STATIC_AUD_INTERVAL);
		pWnd->SetWindowText(pszText);
	}

	m_nMode = mode;

	m_graphAudJitter.Reset();
	m_graphAudInterval.Reset();
	m_graphVidJitter.Reset();
	m_graphVidInterval.Reset();
}

int CDlg_TS_DtsPts::GetMode(void)
{
	return m_nMode;
}

