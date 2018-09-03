#pragma once

#include "afxwin.h"
#include "resource.h"
// CAdaptersDlg 对话框

class CKSnifferDlg;

class CAdaptersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdaptersDlg)

public:
	CAdaptersDlg(CWnd* pParent = NULL);   // 标准构造函数
	CAdaptersDlg(CKSnifferDlg *pOwnerDlg, CWnd* pParent = NULL);
	virtual ~CAdaptersDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADAPTERS };

private:
	CKSnifferDlg *m_pOwnerDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CListBox m_cAdapterList;
public:
	afx_msg void OnLbnDblclkListAdapters();
};
