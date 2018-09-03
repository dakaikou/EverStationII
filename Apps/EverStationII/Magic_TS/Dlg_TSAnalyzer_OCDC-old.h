#if !defined(AFX_DLG_WATCH_OCDC_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
#define AFX_DLG_WATCH_OCDC_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Dlg_Watch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlg_TSAnalyzer_OCDC dialog
#include "..\Magic_TS\TSMagic_GuiApi.h"
//#include "..\Magic_TS\Dlg_DtsPts.h"
#include "..\Common\Dlg_ShowVideo.h"
#include "..\Common\GuiCommon_TreeFunction.h"

#if !MFC_CONTROL
#include "..\MFCExt\TreeList\TreeListCtrl.h"
#endif

class CDlg_TSAnalyzer_OCDC : public CDialog
{
// Construction
public:
	CDlg_TSAnalyzer_OCDC(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlg_TSAnalyzer_OCDC)
	enum { IDD = IDD_TS_ANALYZER_OCDC };
	CListCtrl		m_listOCDCServices;
	#if MFC_CONTROL
	CTreeCtrl		m_treeOCDCSyntax;
	#else
	CTreeListCtrl	m_treeOCDCSyntax;
	#endif
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlg_TSAnalyzer_OCDC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:

	void Reset(void);
	void Set(int offline);
	void InitTree(void);
	void InitList(void);

protected:

	CImageList		m_ImageList;
//	CDlg_ShowVideo	m_dlgTV;

	TREEITEM		m_hRootItem;

	FILE*			fp_debug;

	U16				m_usPID_in_OCDCArray[MAX_SERVICES_PER_STREAM];
	U16				m_usDataBroadcastID_in_OCDCArray[MAX_SERVICES_PER_STREAM];
	U16				m_usCarouselTypeID_in_OCDCArray[MAX_SERVICES_PER_STREAM];
	U32				m_uiTransactionID_in_OCDCArray[MAX_SERVICES_PER_STREAM];
	U16				m_usServiceID_in_OCDCArray[MAX_SERVICES_PER_STREAM];
	U8				m_ucComponentTag_in_OCDCArray[MAX_SERVICES_PER_STREAM];

	// Generated message map functions
	//{{AFX_MSG(CDlg_TSAnalyzer_OCDC)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkListEsPid(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg LRESULT OnUpdatePMT(WPARAM wParam, LPARAM lParam);
public:

	void UpdateDSMCC(CDSMCC* pDSMCC);
	void UpdateSDT(CSDT* pSDT);

	void DC_GatherInfo(int PID, int table_id_extension, int carousel_type_id);
	void OC_GatherInfo(int PID, int table_id_extension);
	void OC_BuildDownloadTable(int PID, unsigned int downloadId, unsigned short moduleId, unsigned int objectKey_data, char* pszDir);
	void OC_BuildDirectory(int PID, HTREEITEM hParent, unsigned short moduleId, unsigned int objectKey_data);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnNMRclickListOcdcServices(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDownloadAllModule();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLG_WATCH_OCDC_H__9AC3530E_7E02_4030_AF61_A0EA3A88C277__INCLUDED_)
