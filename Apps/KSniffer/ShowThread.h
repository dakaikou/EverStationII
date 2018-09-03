#ifndef _SHOW_THREAD_H
#define _SHOW_THREAD_H

#include "ThreadEx.h"
#include <vector>
#include <string>
#include "stdafx.h"
using namespace std;

class CKSnifferDlg;

class CShowThread : public CThreadEx
{
public:
	CShowThread(DWORD dwTcp, DWORD dwUdp, DWORD dwArp,
		DWORD dwIgmp, DWORD dwIcmp, CKSnifferDlg* pDlg);
	virtual ~CShowThread();
	
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
	
private:
	BOOL				m_bSafeExit;					// �߳��˳���־
	BOOL				m_bThreadRolling;				// �߳�ѭ�������ļ����ݱ�־

private:
	CKSnifferDlg *m_pDlg;
	DWORD m_dwTcp;
	DWORD m_dwUdp;
	DWORD m_dwArp;
	DWORD m_dwIgmp;
	DWORD m_dwIcmp;

	CListCtrl *m_pListCtrl;
	CStatic *m_pCountBar;
};	

#endif