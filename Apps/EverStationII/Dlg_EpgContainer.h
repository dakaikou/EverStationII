#pragma once


// CDlg_EpgContainer 对话框

class CDlg_EpgContainer : public CDialog
{
	DECLARE_DYNAMIC(CDlg_EpgContainer)

public:
	CDlg_EpgContainer(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlg_EpgContainer();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CDlg_EpgContainer };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
