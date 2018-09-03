// AdaptersDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "KSniffer.h"
#include "AdaptersDlg.h"
#include "afxdialogex.h"
#include "KSnifferDlg.h"

// CAdaptersDlg 对话框

IMPLEMENT_DYNAMIC(CAdaptersDlg, CDialogEx)

CAdaptersDlg::CAdaptersDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAdaptersDlg::IDD, pParent)
{

}

CAdaptersDlg::CAdaptersDlg(CKSnifferDlg *pOwnerDlg, CWnd* pParent /*=NULL*/)
	: CDialogEx(CAdaptersDlg::IDD, pParent), m_pOwnerDlg(pOwnerDlg)
{

}

CAdaptersDlg::~CAdaptersDlg()
{
}

void CAdaptersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ADAPTERS, m_cAdapterList);
}


BEGIN_MESSAGE_MAP(CAdaptersDlg, CDialogEx)
	ON_LBN_DBLCLK(IDC_LIST_ADAPTERS, &CAdaptersDlg::OnLbnDblclkListAdapters)
END_MESSAGE_MAP()


// CAdaptersDlg 消息处理程序


BOOL CAdaptersDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pcap_if_t *d = NULL;
	pcap_if_t *alldevs = m_pOwnerDlg->GetAllDevs();
	USES_CONVERSION;

	for (d = alldevs; d != NULL; d = d->next)
	{
		WCHAR *pszName = A2W(d->name);
		WCHAR *pszDesc = A2W(d->description);
		WCHAR szInfo[ADAPTER_ADAPTER_NAME_LEN] = {0};

		wcscpy(szInfo, pszName);
		if (pszDesc != NULL)
		{
			wcscat(szInfo, _T(" "));
			wcscat(szInfo, pszDesc);
		}

		m_cAdapterList.InsertString(m_cAdapterList.GetCount(), szInfo);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

// 双击选择
void CAdaptersDlg::OnLbnDblclkListAdapters()
{
	CString strText;
	int nIndex = m_cAdapterList.GetCurSel();
	m_cAdapterList.GetText(nIndex, strText);

	m_pOwnerDlg->SetCurAdapter(nIndex);
	m_pOwnerDlg->SetCurAdapter(strText);

	CAdaptersDlg::OnOK();
}

