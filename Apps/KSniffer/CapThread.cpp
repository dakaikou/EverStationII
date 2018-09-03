#include "stdafx.h"
#include "CapThread.h"
#include "GlobalVar.h"
#include <iostream>
#include <queue>
using namespace std;

CCapThread::CCapThread(pcap_if_t *pDev, CaptureFilter sCapFilter)
	: m_pDevice(pDev), m_sCapFilter(sCapFilter), m_pDevHandle(NULL)
{
	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;
}

BOOL CCapThread::InitInstance()
{
    // TODO: ������г�ʼ������
	// ���ɹ����ַ���
	char szFilter[512] = {0};
	if (m_sCapFilter.dwProtocol & CAP_FILTER_TCP) strcat(szFilter,  " or (ip and tcp)");
	if (m_sCapFilter.dwProtocol & CAP_FILTER_UDP) strcat(szFilter,  " or (ip and udp)");
	if (m_sCapFilter.dwProtocol & CAP_FILTER_ICMP) strcat(szFilter, " or (ip and icmp)");
	if (m_sCapFilter.dwProtocol & CAP_FILTER_IGMP) strcat(szFilter, " or (ip and igmp)");
	if (m_sCapFilter.dwProtocol & CAP_FILTER_ARP) strcat(szFilter,  " or (arp)");
	wsprintfA(m_szFilter, "(%s)", szFilter+4);
	//strcpy(m_szFilter, szFilter + 4);

	// IP��ַ����
	char buffer[128];
	in_addr in = {0};
	// ���ͷ�IP��ַ
	if (m_sCapFilter.bSrcIp)
	{
		in.S_un.S_addr = htonl(m_sCapFilter.dwSrcIp);
		wsprintfA(buffer, " and (src host %s)", inet_ntoa(in));
		strcat(m_szFilter, buffer);
	}
	// ���շ�IP��ַ
	if (m_sCapFilter.bDstIp)
	{
		in.S_un.S_addr = htonl(m_sCapFilter.dwDstIp);
		wsprintfA(buffer, " and (dst host %s)", inet_ntoa(in));
		strcat(m_szFilter, buffer);
	}

	// �˿ڹ���: TCP / UDP
	if (m_sCapFilter.dwProtocol & CAP_FILTER_TCP ||
		m_sCapFilter.dwProtocol & CAP_FILTER_UDP) 
	{
		if (m_sCapFilter.bSrcPort && m_sCapFilter.bDstPort)
		{
			wsprintfA(buffer, " and ((src port %lu) and (dst port %lu))",
				m_sCapFilter.dwSrcPort, m_sCapFilter.dwDstPort);
			strcat(m_szFilter, buffer);
		}
		else
		{
			if (m_sCapFilter.bSrcPort)
			{
				wsprintfA(buffer, " and (src port %lu)",
					m_sCapFilter.dwSrcPort);
				strcat(m_szFilter, buffer);
			}
			else if (m_sCapFilter.bDstPort)
			{
				wsprintfA(buffer, " and (dst port %lu)",
					m_sCapFilter.dwDstPort);
				strcat(m_szFilter, buffer);
			}
		}
	}

	// ������
	char errbuf[PCAP_ERRBUF_SIZE];
	m_pDevHandle = pcap_open(
						m_pDevice->name,			// Ҫ�򿪵��豸
						65536,						// ��֤ץȡ�����ݰ���ȫ������
						PCAP_OPENFLAG_PROMISCUOUS,	// ����ģʽ
						1000,						// ���ں˶�ȡ���ݰ�Ƶ��
						NULL,						// Զ��
						errbuf);					// ���󻺳���
	if (m_pDevHandle == NULL)
	{
		USES_CONVERSION;
		WCHAR *pszErrBuf = A2W(errbuf);
		MessageBox(NULL, pszErrBuf, _T("�����������򿪳���"), MB_ICONERROR);
		return FALSE;
	}

	// ��ȡ������һ����ַ������
	if (m_pDevice->addresses != NULL)
	{
		m_ulMask = ((struct sockaddr_in *)(m_pDevice->addresses->netmask))->sin_addr.S_un.S_addr;
	}
	else
	{
		m_ulMask = 0x00FFFFFF;
	}

	// ���������
	struct bpf_program fcode;
	if (pcap_compile(m_pDevHandle, &fcode, m_szFilter, 1, m_ulMask) < 0)
	{
		MessageBox(NULL, _T("���˹����������޷���ɱ��룡"), 
			_T("���˹���������"), MB_ICONERROR);
		return FALSE;
	}

	// ���ù�����
	if (pcap_setfilter(m_pDevHandle, &fcode) < 0)
	{
		MessageBox(NULL, _T("���˹������ó����޷�������ã�"), 
			_T("���˹������ó���"), MB_ICONERROR);
		return FALSE;
	}

    return CThreadEx::InitInstance();
}

int CCapThread::ExitInstance()
{
    // TODO: ������з���ʼ������

    return CThreadEx::ExitInstance();
}

CCapThread::~CCapThread()
{
	if(m_bSafeExit)
		return;
	
	m_bSafeExit = TRUE;
	while(m_bThreadRolling)
    {
		Sleep(10);
    }

	KillThread();
}

int CCapThread::Run()
{
	int res = 0;
	struct pcap_pkthdr *header;
	const u_char *pkt_data;

	while(!m_bSafeExit /* �����ж����� */ && (res = pcap_next_ex(m_pDevHandle, &header, &pkt_data)) >= 0)
	{
		m_bThreadRolling = TRUE;
		
		if (res == 0) continue;
		try
		{
            // TODO: ��Ҫ�߼�����
			// ���ݰ����
			StorePacket(header, pkt_data);
		}
		catch(...)
		{
			m_bThreadRolling = FALSE;   
			break;
		}
	}
    
	// �رմ򿪵�����������
	pcap_close(m_pDevHandle);
	m_bThreadRolling = FALSE;
    
	return 0;
}
