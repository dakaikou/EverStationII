#ifndef _COMMON_DEF_H_
#define _COMMON_DEF_H_

#define ADAPTER_ADAPTER_NAME_LEN 256

#define CAP_FILTER_TCP  0x00000001
#define CAP_FILTER_UDP  0x00000002
#define CAP_FILTER_ARP  0x00000004
#define CAP_FILTER_ICMP 0x00000008
#define CAP_FILTER_IGMP 0x00000010

#define SAFE_DELETE(p) {if(p != NULL) { delete (p); (p) = NULL; } }
#define CheckPointer(pointer,hr) {if(pointer == NULL) return hr;};

struct TreeNodeData
{
	DWORD dwStartPos;
	DWORD dwEndPos;
};

struct CaptureFilter
{
	DWORD dwProtocol;		// Э����˹���
	BOOL  bSrcIp;			// ���ͷ�IP���˿���
	DWORD dwSrcIp;			// ����IP
	BOOL  bDstIp;			// ���շ�IP���˿���
	DWORD dwDstIp;			// ����IP

	BOOL  bSrcPort;			// ���ͷ��˿ڹ��˿���
	DWORD dwSrcPort;		// �˿ڹ���ֵ
	BOOL  bDstPort;			// ���շ��˿ڹ��˿���
	DWORD dwDstPort;		// �˿ڹ���ֵ
};

#endif