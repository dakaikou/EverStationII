// DlgConfigTSInput.cpp: 实现文件
//

#include "stdafx.h"
#include "EverStationII.h"
#include "DlgConfigTSInput.h"
#include "afxdialogex.h"

#include <WS2tcpip.h>
#include <assert.h>

#include "MiddleWare/MiddleWare_Utilities/Include/MiddleWare_Utilities_MediaFile.h"
#include "Utilities\Directory\Include\TOOL_Directory.h"

// CDlgConfigTSInput 对话框

IMPLEMENT_DYNAMIC(CDlgConfigTSInput, CDialogEx)

CDlgConfigTSInput::CDlgConfigTSInput(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIG_TSINPUT, pParent)
{

}

CDlgConfigTSInput::~CDlgConfigTSInput()
{
}

void CDlgConfigTSInput::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_TSIN_FILE_SIMULATE, m_nTSInputOption);
}


BEGIN_MESSAGE_MAP(CDlgConfigTSInput, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILE_BROWSE, &CDlgConfigTSInput::OnBnClickedBtnFileBrowse)
	ON_BN_CLICKED(IDOK, &CDlgConfigTSInput::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_TSIN_FILE_SIMULATE, &CDlgConfigTSInput::OnClickedRadioTsinFileSimulate)
	ON_COMMAND(IDC_RADIO_TSIN_DEKTEC, &CDlgConfigTSInput::OnRadioTsinDektec)
	ON_COMMAND(IDC_RADIO_TSIN_SMARTTS, &CDlgConfigTSInput::OnRadioTsinSmartts)
	ON_COMMAND(IDC_RADIO_TSIN_UDP, &CDlgConfigTSInput::OnRadioTsinUdp)
END_MESSAGE_MAP()


// CDlgConfigTSInput 消息处理程序


void CDlgConfigTSInput::OnBnClickedBtnFileBrowse()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog fileDlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "Media files(*.*)|*.*|", NULL);

	CString strFileName = _T("");

	if (fileDlg.DoModal() == IDOK)
	{
		strFileName = fileDlg.GetPathName();

		if (FILE_CheckStreamType(STREAM_TS, strFileName.GetBuffer(256)))
		{
			CWnd* pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
			pWnd->SetWindowTextA(strFileName);
		}
		else
		{
			AfxMessageBox("<---文件类型不正确,请确认是否为TS文件!--->", MB_OK | MB_ICONSTOP);
		}
	}
}


BOOL CDlgConfigTSInput::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char	pszExeFile[MAX_PATH];
	char	pszIniFile[MAX_PATH];
	char	pszTSFile[MAX_PATH];
	int		len;
	char	pszLastSelected[64];
	char	pszUDPIP[64];
	char	pszUDPPort[8];


	CWnd* pWnd;

	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	char* ptemp = strrchr(pszExeFile, '\\');
	len = (int)(ptemp - pszExeFile);

	//len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile);

	GetPrivateProfileStringA("TS流输入", "最近选择", "Dektec", pszLastSelected, sizeof(pszLastSelected), pszIniFile);
	GetPrivateProfileStringA("TS流输入", "文件模拟", "e:\\java.ts", pszTSFile, sizeof(pszTSFile), pszIniFile);
	GetPrivateProfileStringA("TS流输入", "UDP IP", "224.0.2.111", pszUDPIP, sizeof(pszUDPIP), pszIniFile);
	GetPrivateProfileStringA("TS流输入", "UDP 端口", "1234", pszUDPPort, sizeof(pszUDPPort), pszIniFile);

	if (strcmp(pszLastSelected, "文件模拟") == 0)
	{
		m_nTSInputOption = 0;
	}
	else if (strcmp(pszLastSelected, "Dektec") == 0)
	{
		m_nTSInputOption = 1;
	}
	else if (strcmp(pszLastSelected, "SmartTS") == 0)
	{
		m_nTSInputOption = 2;
	}
	else if (strcmp(pszLastSelected, "UDP") == 0)
	{
		m_nTSInputOption = 3;
	}
	else
	{
		m_nTSInputOption = 0;
	}

	pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
	pWnd->EnableWindow(m_nTSInputOption == 0);
	pWnd->SetWindowTextA(pszTSFile);

	pWnd = GetDlgItem(IDC_BTN_FILE_BROWSE);
	pWnd->EnableWindow(m_nTSInputOption == 0);

	pWnd = GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->EnableWindow(m_nTSInputOption == 3);

	IN_ADDR ia;
	inet_pton(AF_INET, pszUDPIP, &ia);
	DWORD dwIP = htonl(ia.S_un.S_addr);
	CIPAddressCtrl* pIPCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	pIPCtrl->SetAddress(dwIP);

	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->SetWindowTextA(pszUDPPort);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CDlgConfigTSInput::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	char	pszExeFile[MAX_PATH];
	char	pszIniFile[MAX_PATH];
	char	pszTSFile[MAX_PATH];
	int		len;
	char	pszUDPIP[64];
	char	pszUDPPort[8];
	char	pszLastSelected[64];

	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	char* ptemp = strrchr(pszExeFile, '\\');
	len = (int)(ptemp - pszExeFile);

	//len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile);

	UpdateData(TRUE);

	CWnd* pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
	pWnd->GetWindowTextA(pszTSFile, MAX_PATH);
	WritePrivateProfileStringA("TS流输入", "文件模拟", pszTSFile, pszIniFile);

	//GetPrivateProfileStringA("TS流输入", "最近选择", "DEKTEC", pszLastSelected, MAX_PATH, pszIniFile);

	CIPAddressCtrl* pIPCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	DWORD dwIP;
	pIPCtrl->GetAddress(dwIP);

	IN_ADDR ia;
	CString strIP;
	ia.S_un.S_addr = htonl(dwIP);
	strIP = inet_ntop(AF_INET, &ia, pszUDPIP, sizeof(pszUDPIP));
	WritePrivateProfileStringA("TS流输入", "UDP IP", strIP, pszIniFile);

	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->GetWindowTextA(pszUDPPort, 8);
	WritePrivateProfileStringA("TS流输入", "UDP 端口", pszUDPPort, pszIniFile);

	if (m_nTSInputOption == 0)
	{
		strcpy_s(pszLastSelected, sizeof(pszLastSelected), "文件模拟");
	}
	else if (m_nTSInputOption == 1)
	{
		strcpy_s(pszLastSelected, sizeof(pszLastSelected), "Dektec");
	}
	else if (m_nTSInputOption == 2)
	{
		strcpy_s(pszLastSelected, sizeof(pszLastSelected), "SmartTS");
		m_nTSInputOption = 2;
	}
	else if (m_nTSInputOption == 3)
	{
		strcpy_s(pszLastSelected, sizeof(pszLastSelected), "UDP");
		m_nTSInputOption = 3;
	}
	else
	{
		strcpy_s(pszLastSelected, sizeof(pszLastSelected), "文件模拟");
	}
	WritePrivateProfileStringA("TS流输入", "最近选择", pszLastSelected, pszIniFile);

	CDialogEx::OnOK();
}

void CDlgConfigTSInput::OnClickedRadioTsinFileSimulate()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd* pWnd;

	UpdateData();

	pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
	pWnd->EnableWindow(m_nTSInputOption == 0);
	pWnd = GetDlgItem(IDC_BTN_FILE_BROWSE);
	pWnd->EnableWindow(m_nTSInputOption == 0);

	pWnd = GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
}


void CDlgConfigTSInput::OnRadioTsinDektec()
{
	// TODO: 在此添加命令处理程序代码
	CWnd* pWnd;
	UpdateData();

	pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
	pWnd->EnableWindow(m_nTSInputOption == 0);
	pWnd = GetDlgItem(IDC_BTN_FILE_BROWSE);
	pWnd->EnableWindow(m_nTSInputOption == 0);

	pWnd = GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
}


void CDlgConfigTSInput::OnRadioTsinSmartts()
{
	// TODO: 在此添加命令处理程序代码
	CWnd* pWnd;
	UpdateData();

	pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
	pWnd->EnableWindow(m_nTSInputOption == 0);
	pWnd = GetDlgItem(IDC_BTN_FILE_BROWSE);
	pWnd->EnableWindow(m_nTSInputOption == 0);

	pWnd = GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
}


void CDlgConfigTSInput::OnRadioTsinUdp()
{
	// TODO: 在此添加命令处理程序代码
	CWnd* pWnd;
	UpdateData();

	pWnd = GetDlgItem(IDC_EDIT_TS_FILE);
	pWnd->EnableWindow(m_nTSInputOption == 0);
	pWnd = GetDlgItem(IDC_BTN_FILE_BROWSE);
	pWnd->EnableWindow(m_nTSInputOption == 0);

	pWnd = GetDlgItem(IDC_IPADDRESS_TS_INPUT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
	pWnd = GetDlgItem(IDC_EDIT_TS_UDP_PORT);
	pWnd->EnableWindow(m_nTSInputOption == 3);
}
