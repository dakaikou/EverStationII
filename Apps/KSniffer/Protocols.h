#ifndef _PROTOCOLS_H
#define _PROTOCOLS_H

#include "pcap.h"

#pragma pack(1)
// IP Address Struct
struct IpAddr
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
};

// MAC Address Struct
struct MacAddr
{
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
	unsigned char e;
	unsigned char f;
};

// EthernetHeader Struct
struct EthernetHeader
{ 
    MacAddr sDstMac;				// 目标MAC地址
    MacAddr sSrcMac;				// 源MAC地址
    unsigned short nEthType;		// 数据帧类型
};

// ArpHeader Struct
struct ArpHeader
{
    unsigned short nHardType;		// 硬件类型
    unsigned short nProtoType;		// 协议类型
    unsigned char nMacLen;			// 硬件地址长度
    unsigned char nProtoLen;		// 协议地址长度
    unsigned short nOpCode;			// 操作类型
    MacAddr sSrcMac;				// 源MAC地址
    IpAddr sSrcIp;					// 源IP地址
    MacAddr sDstMac;				// 目标MAC地址
    IpAddr sDstIp;					// 目标IP地址
};

// IPHeader Struct
struct IpHeader
{
	// 注意小端格式声明次序
	// 结构体先声明的占低位、后声明的占高位
	// 网络传输为大端格式
	// nHeaderLen占高位，相对于大端格式就应该放到低位，所以先声明
	unsigned char nHeaderLen : 4;	// 4位首部长度
	unsigned char nVer : 4;			// 4位版本
	unsigned char nTos;				// 服务类型
	unsigned short nTotalLen;		// 总长度
	unsigned short nIdent;			// 标识
	unsigned short nFragOff;		// 分段偏移
	unsigned char nTtl;				// 存活时间
	unsigned char nProtocol;		// 协议
	unsigned short nCrc;			// 首部校验和
	IpAddr sSrcIp;					// 源地址
	IpAddr sDstIp;					// 目的地址
	unsigned int nOpPad;			// 选项与填充
};

// UdpHeader Struct
struct UdpHeader
{
	unsigned short nSrcPort;		// 原端口
	unsigned short nDstPort;		// 目的端口
	unsigned short nLen;			// 数据包长度
	unsigned short nCrc;			// 校验和
};

// TcpHeader Struct
struct TcpHeader
{
	unsigned short nSrcPort;		// 原端口号
	unsigned short nDstPort;		// 目的端口号
	unsigned long nSeqNum;			// 序列号
	unsigned long nAckNum;			// 确认序列号
	unsigned char rReserved1 : 4;	// 保留
	unsigned char nHeaderLen : 4;	// 头部长度, 实际需要乘以4
	unsigned char bFin : 1;			// FIN
	unsigned char bSyn : 1;			// SYN
	unsigned char bRst : 1;			// RST
	unsigned char bPsh : 1;			// PSH
	unsigned char bAck : 1;			// ACK
	unsigned char bUgr : 1;			// UGR
	unsigned char rReserved2 : 2;	// 保留
	unsigned short nWinSize;		// 窗口大小
	unsigned short nCheckSum;		// 校验和
	unsigned short nUrgPtr;			// 16位紧急指针
};

// ICMP Header Struct
struct IcmpHeader
{
	unsigned char nType;			// 消息类型
	unsigned char nCode;			// 消息代码
	unsigned short nCheckSum;		// 校验和
	// ...							// 简单解析，只定义上面的字段
};

// IGMP Header Struct
struct IgmpHeader
{
	unsigned char nType;			// 消息类型
	unsigned char nCode;			// 消息代码
	unsigned short nCheckSum;		// 校验和
	// ...							// 简单解析，只定义上面的字段
};

#pragma pack()

void StorePacket(pcap_pkthdr *header, const u_char* pkt_data);
void DeletePacket();

#endif