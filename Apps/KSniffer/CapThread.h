#ifndef _CAP_THREAD_H
#define _CAP_THREAD_H

#include "ThreadEx.h"
#include <vector>
#include <string>
#include "stdafx.h"
using namespace std;

class CCapThread : public CThreadEx
{
public:
	CCapThread(pcap_if_t *pDev, CaptureFilter sCapFilter);
	virtual ~CCapThread();
	
protected:
	int					Run();
	int					ExitInstance();
	BOOL				InitInstance();
	
private:
	BOOL				m_bSafeExit;			// �߳��˳���־
	BOOL				m_bThreadRolling;		// �߳�ѭ�������ļ����ݱ�־

private:
	pcap_if_t* m_pDevice;						// Ҫ����ץ�����豸
	CaptureFilter m_sCapFilter;					// ���˹���
	char m_szFilter[512];						// ���˹����ַ���
	pcap_t *m_pDevHandle;						// �������
	unsigned long m_ulMask;						// ��ַ���� 
};

#endif