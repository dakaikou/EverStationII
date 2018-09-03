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
	DWORD dwProtocol;		// 协议过滤规则
	BOOL  bSrcIp;			// 发送方IP过滤开关
	DWORD dwSrcIp;			// 过滤IP
	BOOL  bDstIp;			// 接收方IP过滤开关
	DWORD dwDstIp;			// 过滤IP

	BOOL  bSrcPort;			// 发送方端口过滤开关
	DWORD dwSrcPort;		// 端口过滤值
	BOOL  bDstPort;			// 接收方端口过滤开关
	DWORD dwDstPort;		// 端口过滤值
};

#endif