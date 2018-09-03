#include "stdafx.h"
#include "Protocols.h"
#include "GlobalVar.h"
#include "KSnifferDlg.h"
#include "ShowThread.h"
#include <vector>
#include <string>
#include <iterator>
using namespace std;

extern std::vector<PacketNode *> g_vPacketChain;
extern WCHAR g_szIcmpType[42][40];
extern WCHAR g_szIgmpType[34][40];

CShowThread::CShowThread(DWORD dwTcp, DWORD dwUdp, DWORD dwArp,
		DWORD dwIgmp, DWORD dwIcmp, CKSnifferDlg *pDlg)
{
	m_bSafeExit			= FALSE;
	m_bThreadRolling	= FALSE;

	m_pDlg = pDlg;
	m_dwTcp = dwTcp;
	m_dwUdp = dwUdp;
	m_dwArp = dwArp;
	m_dwIgmp = dwIgmp;
	m_dwIcmp = dwIcmp;

	m_pListCtrl = m_pDlg->GetPacketsListWnd();
	m_pCountBar = m_pDlg->GetPacketNumBarWnd();
}

BOOL CShowThread::InitInstance()
{
    // TODO: ������г�ʼ������
    
    return CThreadEx::InitInstance();
}

int CShowThread::ExitInstance()
{
    // TODO: ������з���ʼ������
	m_pDlg->UpdatePackNum(m_dwTcp, m_dwUdp, m_dwArp, m_dwIgmp, m_dwIcmp);
    
    return CThreadEx::ExitInstance();
}

CShowThread::~CShowThread()
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

int CShowThread::Run()
{
	while(!m_bSafeExit /* �����ж����� */)
	{
		m_bThreadRolling = TRUE;
		
		try
		{
            // TODO: ��Ҫ�߼�����
			DWORD dwCount = m_pListCtrl->GetItemCount();
			DWORD dwVectorSize = g_vPacketChain.size();
			if (dwVectorSize == dwCount)
			{
				Sleep(100);
				continue;
			}
			DWORD idx = dwCount;
			for (; idx < dwVectorSize; ++idx)
			{
				// ��һ����¼���������Ϊѡ��
				// ����Ϊ�Զ�ѡ���һ��
				// �����οؼ���ʮ�����ƿؼ������ݿ���ʾ
				if (idx == 1)
				{
					CRect rt;
					POINT pt = {0};
					m_pListCtrl->GetItemRect(0, &rt, LVIR_BOUNDS);
					pt.x = rt.left + rt.Width() / 2;
					pt.y = rt.top + rt.Height() / 2;
					PostMessage(m_pListCtrl->GetSafeHwnd(), 
						WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pt.x, pt.y));
					PostMessage(m_pListCtrl->GetSafeHwnd(), 
						WM_LBUTTONUP, NULL, MAKELPARAM(pt.x, pt.y));
				}

				PacketNode *pNode = g_vPacketChain[idx];
				// �б���ͼ���е����
				DWORD dwListIndex = idx + 1;
				static WCHAR szIndex[32];
				ZeroMemory(szIndex, sizeof(szIndex));
				wsprintf(szIndex, _T("%d"), dwListIndex);	// �Ҷ���
				// ����ʱ���
				char t_hms[16];
				char t_ms[7];
				static WCHAR szTimeStamp[64];
				ZeroMemory(szTimeStamp, sizeof(szTimeStamp));
				char strTimeStamp[64];
				time_t local_tv_sec = pNode->pHeader->ts.tv_sec;
				struct tm *ltime = localtime(&local_tv_sec);
				strftime(t_hms, sizeof(t_hms), "%H:%M:%S", ltime);
				sprintf(t_ms, ".%06d", pNode->pHeader->ts.tv_usec);
				wsprintfA(strTimeStamp, "%s%s", t_hms, t_ms);
				MultiByteToWideChar(CP_ACP, 0, strTimeStamp, -1, szTimeStamp,
					sizeof(szTimeStamp)/sizeof(szTimeStamp[0]));
				// Դ������Ŀ��������Э�顢���ȡ���ϸ��Ϣ
				static WCHAR szSrcIp[32];
				static WCHAR szDstIp[32];
				static WCHAR szProtocol[32];
				static WCHAR szLen[32];
				static WCHAR szDetail[256];
				static WCHAR szSrcMac[32];
				// �ж�Э������
				EthernetHeader *pEthdr = (EthernetHeader *)(pNode->pData); 
				// ==========================================================================
				// ARP���ݰ�����
				// ==========================================================================
				if (ntohs(pEthdr->nEthType) == 0x0806)	// ARP
				{
					ZeroMemory(szSrcIp, sizeof(szSrcIp));
					ZeroMemory(szDstIp, sizeof(szDstIp));
					ZeroMemory(szLen, sizeof(szLen));
					ZeroMemory(szDetail, sizeof(szDetail));
					ZeroMemory(szProtocol, sizeof(szProtocol));
					
					ArpHeader *pArpHdr = (ArpHeader *)(pNode->pData + sizeof(EthernetHeader));
					wsprintf(szSrcIp, _T("%d.%d.%d.%d"), pArpHdr->sSrcIp.a, pArpHdr->sSrcIp.b,
						pArpHdr->sSrcIp.c, pArpHdr->sSrcIp.d);
					wsprintf(szDstIp, _T("%d.%d.%d.%d"), pArpHdr->sDstIp.a, pArpHdr->sDstIp.b,
						pArpHdr->sDstIp.c, pArpHdr->sDstIp.d);
					wcscpy(szProtocol, _T("ARP"));

					wsprintf(szLen, _T("%d"), pNode->pHeader->caplen);

					unsigned short uType = ntohs(pArpHdr->nOpCode);
					if (uType == 1)				// Request
					{
						wsprintf(szDetail, _T("Who has %s?    Tell %s"), szDstIp, szSrcIp);
					}
					else if (uType == 2)		// Response
					{
						wsprintf(szSrcMac, _T("%02X-%02X-%02X-%02X-%02X-%02X"),
							pArpHdr->sSrcMac.a, pArpHdr->sSrcMac.b, pArpHdr->sSrcMac.c,
							pArpHdr->sSrcMac.d, pArpHdr->sSrcMac.e, pArpHdr->sSrcMac.f);
						wsprintf(szDetail, _T("%s is at %s"), szSrcIp, szSrcMac);
					}
					m_dwArp++;
				}
				// ==========================================================================
				// IP���ݰ�����
				// ==========================================================================
				else if (ntohs(pEthdr->nEthType) == 0x0800)	// IP
				{
					IpHeader *pIpHdr = (IpHeader *)(pNode->pData + sizeof(EthernetHeader));
					DWORD dwIpHdrLen = pIpHdr->nHeaderLen * 4;	// һ��Ҫ����4

					wsprintf(szSrcIp, _T("%d.%d.%d.%d"), pIpHdr->sSrcIp.a,
						pIpHdr->sSrcIp.b, pIpHdr->sSrcIp.c, pIpHdr->sSrcIp.d);
					wsprintf(szDstIp, _T("%d.%d.%d.%d"), pIpHdr->sDstIp.a,
						pIpHdr->sDstIp.b, pIpHdr->sDstIp.c, pIpHdr->sDstIp.d);
					wsprintf(szLen, _T("%d"), pNode->pHeader->caplen);
					// ======================================================================
					// ICMP���ݰ�����
					// ======================================================================
					if (pIpHdr->nProtocol == 1)
					{
						wsprintf(szProtocol, _T("ICMP"));

						// ICMPͷ��
						IcmpHeader *pIcmpHdr = (IcmpHeader *)((BYTE *)pIpHdr + dwIpHdrLen);
						if (pIcmpHdr->nType >= 0 && pIcmpHdr->nType <= ICMP_TYPE_VALUE_MAX)
						{
							wsprintf(szDetail, _T("Type: %s  Code: 0x%02X Checksum: 0x%04X"),
								g_szIcmpType[pIcmpHdr->nType],
								pIcmpHdr->nCode, ntohs(pIcmpHdr->nCheckSum));
						}
						else
						{
							wsprintf(szDetail, _T("Type: Type not defined  Code: 0x%02X Checksum: 0x%04X"),
								pIcmpHdr->nCode, ntohs(pIcmpHdr->nCheckSum));
						}
						
						m_dwIcmp++;
					}
					// ======================================================================
					// IGMP���ݰ�����
					// ======================================================================
					else if (pIpHdr->nProtocol == 2)
					{
						wsprintf(szProtocol, _T("IGMP"));

						// ICMPͷ��
						IgmpHeader *pIgmpHdr = (IgmpHeader *)((BYTE *)pIpHdr + dwIpHdrLen);
						if (pIgmpHdr->nType >= 0 && pIgmpHdr->nType <= IGMP_TYPE_VALUE_MAX)
						{
							wsprintf(szDetail, _T("Type: %s  Code: 0x%02X Checksum: 0x%04X"),
								g_szIgmpType[pIgmpHdr->nType],
								pIgmpHdr->nCode, ntohs(pIgmpHdr->nCheckSum));
						}
						else
						{
							wsprintf(szDetail, _T("Type: Type not defined  Code: 0x%02X Checksum: 0x%04X"),
								pIgmpHdr->nCode, ntohs(pIgmpHdr->nCheckSum));
						}
						
						m_dwIgmp++;
					}
					// ======================================================================
					// TCP���ݰ�����
					// ======================================================================
					else if (pIpHdr->nProtocol == 6)
					{
						wcscpy(szProtocol, _T("TCP"));

						// TCPͷ��
						TcpHeader *pTcpHdr = (TcpHeader *)((BYTE*)pIpHdr + dwIpHdrLen);	// ע��pIpHdrת��ΪPBYTE
						WCHAR szFlags[32] = {0};
						if (pTcpHdr->bFin) wcscat(szFlags, _T("FIN,"));
						if (pTcpHdr->bSyn) wcscat(szFlags, _T("SYN,"));
						if (pTcpHdr->bRst) wcscat(szFlags, _T("RST,"));
						if (pTcpHdr->bPsh) wcscat(szFlags, _T("PSH,"));
						if (pTcpHdr->bAck) wcscat(szFlags, _T("ACK,"));
						if (pTcpHdr->bUgr) wcscat(szFlags, _T("UGR,"));
						if (wcslen(szFlags) != 0)
							szFlags[wcslen(szFlags)-1] = _T('\0');
						wsprintf(szDetail, 
							_T("%u -> %u [%s] Seq=%lu Ack=%lu Win=%u Len=%d"),
							ntohs(pTcpHdr->nSrcPort),
							ntohs(pTcpHdr->nDstPort),
							szFlags,
							ntohl(pTcpHdr->nSeqNum),
							ntohl(pTcpHdr->nAckNum),
							ntohs(pTcpHdr->nWinSize),
							pNode->pHeader->caplen - sizeof(EthernetHeader) - dwIpHdrLen);

						m_dwTcp++;
					}
					// ======================================================================
					// UDP���ݰ�����
					// ======================================================================
					else if (pIpHdr->nProtocol == 17)
					{
						wcscpy(szProtocol, _T("UDP"));

						// UDPͷ��
						UdpHeader *pUdpHdr = (UdpHeader *)((BYTE*)pIpHdr + dwIpHdrLen);
						wsprintf(szDetail, _T("Source port: %u    Destination port: %u"),
							ntohs(pUdpHdr->nSrcPort), ntohs(pUdpHdr->nDstPort));

						m_dwUdp++;
					}
				}
				m_pListCtrl->InsertItem (idx, szIndex);
				m_pListCtrl->SetItemText(idx, 1, szTimeStamp);
				m_pListCtrl->SetItemText(idx, 2, szSrcIp);
				m_pListCtrl->SetItemText(idx, 3, szDstIp);
				m_pListCtrl->SetItemText(idx, 4, szProtocol);
				m_pListCtrl->SetItemText(idx, 5, szLen);
				
				m_pListCtrl->SetItemText(idx, 6, szDetail);

				m_pDlg->UpdatePackNum(m_dwTcp, m_dwUdp, m_dwArp, m_dwIgmp, m_dwIcmp);
			}
		}
		catch(...)
		{
			m_bThreadRolling = FALSE;   
			break;
		}

		Sleep(50);
	}
    
	m_bThreadRolling = FALSE;
    
	return 0;
}
