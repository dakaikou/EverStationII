// Dlg_EpgContainer.cpp: 实现文件
//

#include "stdafx.h"
#include "Dlg_EpgContainer.h"
#include "afxdialogex.h"


// CDlg_EpgContainer 对话框

IMPLEMENT_DYNAMIC(CDlg_EpgContainer, CDialog)

CDlg_EpgContainer::CDlg_EpgContainer(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_CDlg_EpgContainer, pParent)
{

}

CDlg_EpgContainer::~CDlg_EpgContainer()
{
}

void CDlg_EpgContainer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_EpgContainer, CDialog)
END_MESSAGE_MAP()


// CDlg_EpgContainer 消息处理程序
