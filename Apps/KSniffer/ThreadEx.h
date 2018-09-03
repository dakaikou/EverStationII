#ifndef _CTHREADEX_H_H
#define _CTHREADEX_H_H

#include <Windows.h>

class CThreadEx
{
public:
	HANDLE m_hThread;
	unsigned int m_nThreadID;
public:
	CThreadEx();
	DWORD ResumeThread();
	DWORD SuspendThread();
	int   GetThreadPriority();
	BOOL  SetThreadPriority(int nPriority);
	BOOL  CreateThread(unsigned int dwCreateFlags,
					   UINT  nStackSize,
					   LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL);
	
	BOOL  ThreadRunning() {return (m_iSuspendCount == 0);};

	virtual BOOL KillThread();
	virtual BOOL InitInstance();
	virtual ~CThreadEx();

	virtual int  Run()			= 0;
	virtual int  ExitInstance() = 0;

public:
	int			m_iSuspendCount;	//Suspend Count;
	BOOL		m_bExit;			//�߳��˳���־λ
	HANDLE		m_hEvtExit;			//�߳��˳��¼�

protected:
	void	InitialRes();
};

#endif