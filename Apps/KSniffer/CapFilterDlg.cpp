// CapFilterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KSniffer.h"
#include "CapFilterDlg.h"
#include "afxdialogex.h"
#include "KSnifferDlg.h"

// CCapFilterDlg 对话框

IMPLEMENT_DYNAMIC(CCapFilterDlg, CDialogEx)

CCapFilterDlg::CCapFilterDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCapFilterDlg::IDD, pParent)
{

}

CCapFilterDlg::CCapFilterDlg(CKSnifferDlg *pOwnerDlg, 
			CaptureFilter sCapFilter, CWnd* pParent /*=NULL*/)
	: CDialogEx(CCapFilterDlg::IDD, pParent), m_pOwnerDlg(pOwnerDlg), 
			m_sCapFilter(sCapFilter)
{

}

CCapFilterDlg::~CCapFilterDlg()
{
}

void CCapFilterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_TCP, m_cTcp);
	DDX_Control(pDX, IDC_CHECK_UDP, m_cUdp);
	DDX_Control(pDX, IDC_CHECK_ARP, m_cArp);
	DDX_Control(pDX, IDC_CHECK_ICMP, m_cIcmp);
	DDX_Control(pDX, IDC_CHECK_IGMP, m_cIgmp);
	DDX_Control(pDX, IDC_CHECK_SENDERIP, m_cChkSenderIp);
	DDX_Control(pDX, IDC_CHECK_TARGETIP, m_cChkTargetIp);
	DDX_Control(pDX, IDC_IPADDRESS_SENDER, m_cIpSender);
	DDX_Control(pDX, IDC_IPADDRESS_TARGET, m_cIpTarget);
	DDX_Control(pDX, IDC_CHECK2, m_cChkSrcPort);
	DDX_Control(pDX, IDC_CHECK4, m_cChkDstPort);
	DDX_Control(pDX, IDC_EDIT3, m_cEditSrcPort);
	DDX_Control(pDX, IDC_EDIT5, m_cEditDstPort);
}


BEGIN_MESSAGE_MAP(CCapFilterDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CCapFilterDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_TCP, &CCapFilterDlg::OnBnClickedCheckTcp)
	ON_BN_CLICKED(IDC_CHECK_UDP, &CCapFilterDlg::OnBnClickedCheckUdp)
END_MESSAGE_MAP()


// CCapFilterDlg 消息处理程序


BOOL CCapFilterDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_sCapFilter.dwProtocol & CAP_FILTER_TCP) m_cTcp.SetCheck(BST_CHECKED);
	if (m_sCapFilter.dwProtocol & CAP_FILTER_UDP) m_cUdp.SetCheck(BST_CHECKED);
	if (m_sCapFilter.dwProtocol & CAP_FILTER_ARP) m_cArp.SetCheck(BST_CHECKED);
	if (m_sCapFilter.dwProtocol & CAP_FILTER_ICMP) m_cIcmp.SetCheck(BST_CHECKED);
	if (m_sCapFilter.dwProtocol & CAP_FILTER_IGMP) m_cIgmp.SetCheck(BST_CHECKED);

	if (!(m_sCapFilter.dwProtocol & CAP_FILTER_TCP) &&
		!(m_sCapFilter.dwProtocol & CAP_FILTER_UDP))
	{
		m_cChkSrcPort.EnableWindow(FALSE);
		m_cChkDstPort.EnableWindow(FALSE);
		m_cEditSrcPort.SetReadOnly(TRUE);
		m_cEditDstPort.SetReadOnly(TRUE);
	}
	else
	{
		if (m_sCapFilter.bSrcPort)
		{
			m_cChkSrcPort.SetCheck(BST_CHECKED);
			SetDlgItemInt(m_cEditSrcPort.GetDlgCtrlID(), 
				m_sCapFilter.dwSrcPort, 0);
		}
		if (m_sCapFilter.bDstPort)
		{
			m_cChkDstPort.SetCheck(BST_CHECKED);
			SetDlgItemInt(m_cEditDstPort.GetDlgCtrlID(), 
				m_sCapFilter.dwDstPort, 0);
		}
	}

	if (m_sCapFilter.bSrcIp)
	{
		m_cChkSenderIp.SetCheck(BST_CHECKED);
		m_cIpSender.SetAddress(m_sCapFilter.dwSrcIp);
	}

	if (m_sCapFilter.bDstIp)
	{
		m_cChkTargetIp.SetCheck(BST_CHECKED);
		m_cIpTarget.SetAddress(m_sCapFilter.dwDstIp);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 保存抓包过滤选项
void CCapFilterDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwCapFilter = 0;
	if (m_cTcp.GetCheck()) dwCapFilter |= CAP_FILTER_TCP;
	if (m_cUdp.GetCheck()) dwCapFilter |= CAP_FILTER_UDP;
	if (m_cArp.GetCheck()) dwCapFilter |= CAP_FILTER_ARP;
	if (m_cIcmp.GetCheck()) dwCapFilter |= CAP_FILTER_ICMP;
	if (m_cIgmp.GetCheck()) dwCapFilter |= CAP_FILTER_IGMP;

	if (!(dwCapFilter))
	{
		MessageBox(_T("至少需要选择一种数据包格式！"),
			_T("提示信息"), MB_ICONWARNING);
		return ;
	}

	char buff[16];
	WCHAR wBuff[16];
	DWORD dwNum;
	if (m_cTcp.GetCheck() || m_cUdp.GetCheck())
	{
		if (m_cChkSrcPort.GetCheck())
		{
			m_cEditSrcPort.GetWindowText(wBuff, 16);
			if (wcslen(wBuff) == 0)
			{
				MessageBox(_T("端口数值范围必须在区间[0, 65535]上！"),
					_T("提示信息"), MB_ICONWARNING);
				return ;
			}
			else
			{
				WideCharToMultiByte(CP_ACP, 0, wBuff, -1, buff, 16, NULL, NULL);
				dwNum = atoi(buff);
				if (dwNum >= 0 && dwNum <= 65535)
				{
					m_sCapFilter.bSrcPort = 1;
					m_sCapFilter.dwSrcPort = dwNum;
				}
				else
				{
					MessageBox(_T("端口数值范围必须在区间[0, 65535]上！"),
						_T("提示信息"), MB_ICONWARNING);
					return ;
				}
			}
		}
		else
		{
			m_sCapFilter.bSrcPort = 0;
		}

		if (m_cChkDstPort.GetCheck())
		{
			m_cEditDstPort.GetWindowText(wBuff, 16);
			if (wcslen(wBuff) == 0)
			{
				MessageBox(_T("端口数值范围必须在区间[0, 65535]上！"),
					_T("提示信息"), MB_ICONWARNING);
				return ;
			}
			else
			{
				WideCharToMultiByte(CP_ACP, 0, wBuff, -1, buff, 16, NULL, NULL);
				dwNum = atoi(buff);
				if (dwNum >= 0 && dwNum <= 65535)
				{
					m_sCapFilter.bDstPort = 1;
					m_sCapFilter.dwDstPort = dwNum;
				}
				else
				{
					MessageBox(_T("端口数值范围必须在区间[0, 65535]上！"),
						_T("提示信息"), MB_ICONWARNING);
					return ;
				}
			}
		}
		else
		{
			m_sCapFilter.bDstPort = 0;
		}
	}

	// 发送方IP
	if (m_cChkSenderIp.GetCheck())
	{
		m_sCapFilter.bSrcIp = 1;
		m_cIpSender.GetAddress(m_sCapFilter.dwSrcIp);
	}
	else
	{
		m_sCapFilter.bSrcIp = 0;
	}

	// 发送方IP
	if (m_cChkTargetIp.GetCheck())
	{
		m_sCapFilter.bDstIp = 1;
		m_cIpTarget.GetAddress(m_sCapFilter.dwDstIp);
	}
	else
	{
		m_sCapFilter.bDstIp = 0;
	}

	m_sCapFilter.dwProtocol = dwCapFilter;
	m_pOwnerDlg->SetCapFilter(m_sCapFilter);

	CDialogEx::OnOK();
}


void CCapFilterDlg::OnBnClickedCheckTcp()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwState = m_cTcp.GetCheck();
	if (dwState != BST_CHECKED)
		dwState = m_cUdp.GetCheck();
	m_cChkSrcPort.EnableWindow(dwState);
	m_cChkDstPort.EnableWindow(dwState);
	m_cEditSrcPort.SetReadOnly(!dwState);
	m_cEditDstPort.SetReadOnly(!dwState);
}


void CCapFilterDlg::OnBnClickedCheckUdp()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwState = m_cUdp.GetCheck();
	if (dwState != BST_CHECKED)
		dwState = m_cTcp.GetCheck();
	m_cChkSrcPort.EnableWindow(dwState);
	m_cChkDstPort.EnableWindow(dwState);
	m_cEditSrcPort.SetReadOnly(!dwState);
	m_cEditDstPort.SetReadOnly(!dwState);
}
