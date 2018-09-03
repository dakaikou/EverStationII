
// KSnifferDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include "AdaptersDlg.h"
#include "afxwin.h"
#include "CapFilterDlg.h"
#include "CapThread.h"
#include "ShowThread.h"
#include "resource.h"
#include "hexedit.h"

// CKSnifferDlg 对话框
class CKSnifferDlg : public CDialogEx
{
// 构造
public:
	CKSnifferDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_KSNIFFER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// 初始化控件
	BOOL InitControls(void);
private:
	CListCtrl m_cPacketsList;
	CStatic m_cPacketNumBar;
	pcap_if_t *m_pAllDevs;
	int	m_nCurAdapter;
	CString m_strCurAdapter;
	//DWORD m_dwCapFilter;
	CaptureFilter m_sCapFilter;
	BOOL m_bCapWorking;
	CStatic m_cStatusCurAdapter;
	DWORD m_dwTcp;
	DWORD m_dwUdp;
	DWORD m_dwArp;
	DWORD m_dwIgmp;
	DWORD m_dwIcmp;
private:
	// 线程
	CCapThread *m_tCapThread;
	CShowThread *m_tShowThread;

public:
	CListCtrl* GetPacketsListWnd(void);
	CStatic* GetPacketNumBarWnd(void);
	void UpdatePackNum(DWORD dwTcp, DWORD dwUdp, 
		DWORD dwArp, DWORD dwIgmp, DWORD dwIcmp);
	BOOL InitWinpcap(void);
	void UnInitWinpcap(void);
	afx_msg void OnClose();
	pcap_if_t* GetAllDevs(void);
	void SetCurAdapter(int nIndex);
	void SetCurAdapter(CString strInfo);
	afx_msg void OnSelectAdapters();
	void SetCapFilter(CaptureFilter sCapFilter);

public:
	afx_msg void OnSetCapFilter();
	afx_msg void OnOptStartcap();
	afx_msg void OnOptStopcap();
	CHexEdit m_cHexView;
	afx_msg void OnNMClickListPackets(NMHDR *pNMHDR, LRESULT *pResult);
	CTreeCtrl m_cTreeView;
	void UpdateTreeViewData(DWORD dwIndex);
	afx_msg void OnTvnSelchangedTreePackets(NMHDR *pNMHDR, LRESULT *pResult);
	void DeleteTreeData();
	afx_msg void OnTvnDeleteitemTreePackets(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnFileClose();
	afx_msg void OnHelpAbout();
};
