#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"

class CKSnifferDlg;
// CCapFilterDlg 对话框

class CCapFilterDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCapFilterDlg)

public:
	CCapFilterDlg(CWnd* pParent = NULL);   // 标准构造函数
	CCapFilterDlg(CKSnifferDlg *m_pOwnerDlg, CaptureFilter sCapFilter, CWnd* pParent = NULL);
	virtual ~CCapFilterDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_CAPFILTER };

private:
	CaptureFilter m_sCapFilter;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CButton m_cTcp;
	CButton m_cUdp;
	CButton m_cArp;
	CButton m_cIcmp;
	CButton m_cIgmp;

	CKSnifferDlg *m_pOwnerDlg;
public:
	afx_msg void OnBnClickedOk();
	CButton m_cChkSenderIp;
	CButton m_cChkTargetIp;
	CIPAddressCtrl m_cIpSender;
	CIPAddressCtrl m_cIpTarget;
	CButton m_cChkSrcPort;
	CButton m_cChkDstPort;
	CEdit m_cEditSrcPort;
	CEdit m_cEditDstPort;
	afx_msg void OnBnClickedCheckTcp();
	afx_msg void OnBnClickedCheckUdp();
};
