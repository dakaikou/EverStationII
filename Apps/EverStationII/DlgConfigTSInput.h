#pragma once


// CDlgConfigTSInput 对话框

class CDlgConfigTSInput : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfigTSInput)

public:
	CDlgConfigTSInput(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgConfigTSInput();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_TSINPUT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedRadioTsinFileSimulate();
	afx_msg void OnRadioTsinDektec();
	afx_msg void OnRadioTsinSmartts();
	afx_msg void OnRadioTsinUdp();
	afx_msg void OnBnClickedBtnFileBrowse();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
protected:
	int m_nTSInputOption;
public:
};
