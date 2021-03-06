#pragma once

// CDlgConfigWinpcap 对话框
#include "thirdparty_libs\Winpcap\Include\pcap.h"
#include "thirdparty_libs\Winpcap\Include\winpcap_user.h"

class CDlgConfigWinpcap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgConfigWinpcap)

public:
	CDlgConfigWinpcap(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgConfigWinpcap();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_WINPCAP };
#endif

public:
	int		m_nDiagnosing;
protected:
	CListCtrl			m_listDevices;
	CListCtrl			m_listLogs;
	pcap_if_t *m_palldevs;
	int       m_nDeviceCount;

public:
	void AppendLog(char* pszTime, char* pszMessage);
	void AppendDevice(char* pszDevice, char* pszDescription);
	int  GetSelectedDevice(void);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickedBtnPcapDiagnose();
	virtual BOOL DestroyWindow();
	afx_msg void OnBnClickedOk();
};
