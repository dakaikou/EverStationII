#ifndef _GLOBALVAR_H
#define _GLOBALVAR_H

#include "stdafx.h"

struct PacketNode
{
	pcap_pkthdr		*pHeader;
	BYTE			*pData;
};

#define ICMP_TYPE_VALUE_MAX	41
#define IGMP_TYPE_VALUE_MAX 34

#endif