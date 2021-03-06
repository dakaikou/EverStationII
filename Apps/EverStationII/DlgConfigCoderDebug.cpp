// DlgConfigSysDebug.cpp: 实现文件
//

#include "stdafx.h"
#include "EverStationII.h"
#include "DlgConfigCoderDebug.h"
#include "afxdialogex.h"

#include <assert.h>

#include "Utilities\Directory\Include\TOOL_Directory.h"

// CDlgConfigCoderDebug 对话框

IMPLEMENT_DYNAMIC(CDlgConfigCoderDebug, CDialogEx)

CDlgConfigCoderDebug::CDlgConfigCoderDebug(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CONFIG_SYSDEBUG, pParent)
{

}

CDlgConfigCoderDebug::~CDlgConfigCoderDebug()
{
}

void CDlgConfigCoderDebug::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_ENABLE_PCR_DEBUG, m_bEnablePcrDebug);
	DDX_Check(pDX, IDC_CHECK_ENABLE_TSRATE_DEBUG, m_bEnableTSRateDebug);
}


BEGIN_MESSAGE_MAP(CDlgConfigCoderDebug, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgConfigCoderDebug::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgConfigCoderDebug 消息处理程序


void CDlgConfigCoderDebug::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);

	char	pszExeFile[MAX_PATH];
	char	pszIniFile[MAX_PATH];
	int		len;
	char	pszTemp[64];

	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	char* ptemp = strrchr(pszExeFile, '\\');
	len = (int)(ptemp - pszExeFile);

	//len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile);

	_itoa_s(m_bEnablePcrDebug, pszTemp, sizeof(pszTemp), 2);
	WritePrivateProfileString("编程者诊断", "PCR", pszTemp, pszIniFile);
	_itoa_s(m_bEnableTSRateDebug, pszTemp, sizeof(pszTemp), 2);
	WritePrivateProfileString("编程者诊断", "TS码率", pszTemp, pszIniFile);

	CDialogEx::OnOK();
}


BOOL CDlgConfigCoderDebug::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	char	pszExeFile[MAX_PATH];
	char	pszIniFile[MAX_PATH];
	int		len;

	GetModuleFileNameA(NULL, pszExeFile, MAX_PATH);
	char* ptemp = strrchr(pszExeFile, '\\');
	len = (int)(ptemp - pszExeFile);

	//len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszIniFile, pszExeFile, len);
	pszIniFile[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszIniFile);

	m_bEnablePcrDebug = GetPrivateProfileIntA("编程者诊断", "PCR", 0, pszIniFile);
	m_bEnableTSRateDebug = GetPrivateProfileIntA("编程者诊断", "TS码率", 0, pszIniFile);

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
