
// EverStationII.h: EverStationII 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号

// CEverStationIIApp:
// 有关此类的实现，请参阅 EverStationII.cpp
//

class CEverStationIIApp : public CWinAppEx
{
public:
	CEverStationIIApp();

	int	GetRunningState();
	int SetRunningState(int state);

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现
	CMultiDocTemplate* m_pDocTemplateForTS;
	CMultiDocTemplate* m_pDocTemplateForES;
	CMultiDocTemplate* m_pDocTemplateForPS;
	CMultiDocTemplate* m_pDocTemplateForYUV;

protected:
	int				m_bRuning;

protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
//	afx_msg void OnFileOpen();
	afx_msg void OnBtnTSMagic();
	afx_msg void OnBtnPSMagic();
	afx_msg void OnBtnESMagic();
	afx_msg void OnBtnYUVMagic();
	//afx_msg void OnFileSave();
	afx_msg void OnMenuSetupCoderDebug();
	afx_msg void OnMenuSetupWinpcapDebug();
	afx_msg void OnMenuSetupTsInput();
	afx_msg void OnUpdateMenuSetupCoderDebug(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuSetupWinpcapDebug(CCmdUI *pCmdUI);
	afx_msg void OnUpdateMenuSetupTsInput(CCmdUI *pCmdUI);
};

extern CEverStationIIApp theApp;
