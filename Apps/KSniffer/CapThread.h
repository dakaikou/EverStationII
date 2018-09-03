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
	BOOL				m_bSafeExit;			// 线程退出标志
	BOOL				m_bThreadRolling;		// 线程循环处理文件数据标志

private:
	pcap_if_t* m_pDevice;						// 要进行抓包的设备
	CaptureFilter m_sCapFilter;					// 过滤规则
	char m_szFilter[512];						// 过滤规则字符串
	pcap_t *m_pDevHandle;						// 网卡句柄
	unsigned long m_ulMask;						// 地址掩码 
};

#endif