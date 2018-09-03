#pragma once

#include "afxwin.h"
#include "resource.h"
// CAdaptersDlg �Ի���

class CKSnifferDlg;

class CAdaptersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdaptersDlg)

public:
	CAdaptersDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CAdaptersDlg(CKSnifferDlg *pOwnerDlg, CWnd* pParent = NULL);
	virtual ~CAdaptersDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG_ADAPTERS };

private:
	CKSnifferDlg *m_pOwnerDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	CListBox m_cAdapterList;
public:
	afx_msg void OnLbnDblclkListAdapters();
};
