#include "stdafx.h"
#include "ThreadEx.h"
#include <process.h>
#include "commondef.h"

unsigned WINAPI _ThreadEntry(LPVOID pParam)
{
	CThreadEx * pThread = (CThreadEx *)pParam;

	unsigned nResult = 0;

	if (pThread->InitInstance()) 
	{
		nResult = pThread->Run();
	}

	nResult = pThread->ExitInstance();
	pThread->m_hThread = NULL;

	_endthreadex(nResult);

	return nResult;
}

CThreadEx::CThreadEx()
{
	InitialRes();
	CreateThread(CREATE_SUSPENDED,0,NULL);
}

CThreadEx::~CThreadEx()
{
	KillThread();
}

BOOL CThreadEx::InitInstance()
{
	return (m_hEvtExit != NULL);
}

BOOL CThreadEx::CreateThread(unsigned dwCreateFlags, UINT nStackSize, 
						   LPSECURITY_ATTRIBUTES lpSecurityAttrs)
{
	LPVOID _MyThread = (LPVOID)this;
	
	m_hThread = (HANDLE)_beginthreadex(lpSecurityAttrs,
									   nStackSize,
									   _ThreadEntry,
									   _MyThread,
									   dwCreateFlags,
									   &m_nThreadID);

	m_iSuspendCount = (dwCreateFlags | CREATE_SUSPENDED)?1:0;
	return (m_hThread)?TRUE:FALSE;
}

DWORD CThreadEx::ResumeThread()
{
	DWORD prevCount = -1;
	if(m_hThread != NULL) 
	{
		prevCount = m_iSuspendCount;
		if(m_iSuspendCount > 0)
		{
			if((prevCount = ::ResumeThread(m_hThread)) != 0xFFFFFFFF)
			{
				InterlockedDecrement((LPLONG)&m_iSuspendCount);
			} 
		}
	}
	return prevCount;
}

BOOL CThreadEx::SetThreadPriority(int nPriority)
{
	CheckPointer(m_hThread,FALSE); 

	return ::SetThreadPriority(m_hThread, nPriority); 
}

int CThreadEx::GetThreadPriority()
{
	CheckPointer(m_hThread,FALSE);
	
	return ::GetThreadPriority(m_hThread); 
}

BOOL CThreadEx::KillThread()
{
	InterlockedExchange((LPLONG)&m_bExit,TRUE);   
	if(m_hThread != NULL)
	{
		SetEvent(m_hEvtExit);
		while(m_iSuspendCount > 0) 
		{
			if(ResumeThread() == 0xFFFFFFFF)
				break;
		}
		WaitForSingleObject(m_hThread, 3000);
		m_hThread = NULL;
	}

	if(m_hEvtExit != NULL) 
	{
		CloseHandle(m_hEvtExit);
		m_hEvtExit = NULL;
	}

	return TRUE;
}

void CThreadEx::InitialRes()
{
	m_iSuspendCount = 0;
	m_hThread       = NULL;
	m_nThreadID     = 0;
	m_bExit		    = FALSE;
	m_hEvtExit		= CreateEvent(NULL,FALSE,FALSE,NULL);
}

DWORD CThreadEx::SuspendThread()
{
	CheckPointer(m_hThread,0);
	
	DWORD prevCount = m_iSuspendCount;
	if((prevCount = ::SuspendThread(m_hThread)) != 0xFFFFFFFF)
		InterlockedIncrement((LPLONG)&m_iSuspendCount);

	return prevCount;
}

int  CThreadEx::Run()
{
	return 0;
}

int  CThreadEx::ExitInstance()
{
	return 0;
}
