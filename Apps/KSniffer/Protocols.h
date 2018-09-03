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
    MacAddr sDstMac;				// Ŀ��MAC��ַ
    MacAddr sSrcMac;				// ԴMAC��ַ
    unsigned short nEthType;		// ����֡����
};

// ArpHeader Struct
struct ArpHeader
{
    unsigned short nHardType;		// Ӳ������
    unsigned short nProtoType;		// Э������
    unsigned char nMacLen;			// Ӳ����ַ����
    unsigned char nProtoLen;		// Э���ַ����
    unsigned short nOpCode;			// ��������
    MacAddr sSrcMac;				// ԴMAC��ַ
    IpAddr sSrcIp;					// ԴIP��ַ
    MacAddr sDstMac;				// Ŀ��MAC��ַ
    IpAddr sDstIp;					// Ŀ��IP��ַ
};

// IPHeader Struct
struct IpHeader
{
	// ע��С�˸�ʽ��������
	// �ṹ����������ռ��λ����������ռ��λ
	// ���紫��Ϊ��˸�ʽ
	// nHeaderLenռ��λ������ڴ�˸�ʽ��Ӧ�÷ŵ���λ������������
	unsigned char nHeaderLen : 4;	// 4λ�ײ�����
	unsigned char nVer : 4;			// 4λ�汾
	unsigned char nTos;				// ��������
	unsigned short nTotalLen;		// �ܳ���
	unsigned short nIdent;			// ��ʶ
	unsigned short nFragOff;		// �ֶ�ƫ��
	unsigned char nTtl;				// ���ʱ��
	unsigned char nProtocol;		// Э��
	unsigned short nCrc;			// �ײ�У���
	IpAddr sSrcIp;					// Դ��ַ
	IpAddr sDstIp;					// Ŀ�ĵ�ַ
	unsigned int nOpPad;			// ѡ�������
};

// UdpHeader Struct
struct UdpHeader
{
	unsigned short nSrcPort;		// ԭ�˿�
	unsigned short nDstPort;		// Ŀ�Ķ˿�
	unsigned short nLen;			// ���ݰ�����
	unsigned short nCrc;			// У���
};

// TcpHeader Struct
struct TcpHeader
{
	unsigned short nSrcPort;		// ԭ�˿ں�
	unsigned short nDstPort;		// Ŀ�Ķ˿ں�
	unsigned long nSeqNum;			// ���к�
	unsigned long nAckNum;			// ȷ�����к�
	unsigned char rReserved1 : 4;	// ����
	unsigned char nHeaderLen : 4;	// ͷ������, ʵ����Ҫ����4
	unsigned char bFin : 1;			// FIN
	unsigned char bSyn : 1;			// SYN
	unsigned char bRst : 1;			// RST
	unsigned char bPsh : 1;			// PSH
	unsigned char bAck : 1;			// ACK
	unsigned char bUgr : 1;			// UGR
	unsigned char rReserved2 : 2;	// ����
	unsigned short nWinSize;		// ���ڴ�С
	unsigned short nCheckSum;		// У���
	unsigned short nUrgPtr;			// 16λ����ָ��
};

// ICMP Header Struct
struct IcmpHeader
{
	unsigned char nType;			// ��Ϣ����
	unsigned char nCode;			// ��Ϣ����
	unsigned short nCheckSum;		// У���
	// ...							// �򵥽�����ֻ����������ֶ�
};

// IGMP Header Struct
struct IgmpHeader
{
	unsigned char nType;			// ��Ϣ����
	unsigned char nCode;			// ��Ϣ����
	unsigned short nCheckSum;		// У���
	// ...							// �򵥽�����ֻ����������ֶ�
};

#pragma pack()

void StorePacket(pcap_pkthdr *header, const u_char* pkt_data);
void DeletePacket();

#endif