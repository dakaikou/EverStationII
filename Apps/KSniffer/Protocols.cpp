#include "stdafx.h"
#include "Protocols.h"
#include "GlobalVar.h"
#include <vector>
#include <iterator>
using namespace std;

extern std::vector<PacketNode *> g_vPacketChain;

void StorePacket(pcap_pkthdr *header, const u_char* pkt_data)
{
	PacketNode *pNode = (PacketNode *)malloc(sizeof(PacketNode));
	if (pNode == NULL)
	{
		return ;
	}

	pNode->pHeader = (pcap_pkthdr*)malloc(sizeof(pcap_pkthdr));
	if (pNode->pHeader == NULL)
	{
		free(pNode);
		return ;
	}

	pNode->pData = (BYTE *)malloc(header->caplen);
	if (pNode->pData == NULL)
	{
		free(pNode->pHeader);
		free(pNode);
	}

	memcpy(pNode->pHeader, header, sizeof(pcap_pkthdr));
	memcpy(pNode->pData, pkt_data, header->caplen);

	g_vPacketChain.push_back(pNode);
}

void DeletePacket()
{
	PacketNode *pNode = NULL;
	vector<PacketNode*>::iterator iter;
	for (iter = g_vPacketChain.begin();
		iter != g_vPacketChain.end();
		)
	{
		pNode = *iter;
		free(pNode->pHeader);
		free(pNode->pData);
		iter = g_vPacketChain.erase(iter);		// 注意这一行的写法
		free(pNode);
	}
}