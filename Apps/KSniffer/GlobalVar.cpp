#include "stdafx.h"
#include <Windows.h>
#include "GlobalVar.h"
#include <vector>
#include <iterator>
using namespace std;

std::vector<PacketNode *> g_vPacketChain;

WCHAR g_szIcmpType[42][40] = {
	_T("Echo (ping) reply"),
	_T("Reserved"),
	_T("Reserved"),
	_T("Destination unreachable"),
	_T("Source quench (flow control)"),
	_T("Redirect"),
	_T("Alternate host address"),
	_T("Type not defined"),						// 7 未定义
	_T("Echo (ping) request"),
	_T("Router advertisement"),
	_T("Router solicitation"),
	_T("Time-to-live exceeded"),
	_T("Parameter problem"),
	_T("Timestamp request"),
	_T("Timestamp reply"),
	_T("Information request"),
	_T("Information reply"),
	_T("Address mask request"),
	_T("Address mask reply"),					// 18
	_T("Reserved (for security)"),				// 19
	_T("Type not defined"),						// 20 未定义
	_T("Type not defined"),						// 21 未定义
	_T("Type not defined"),						// 22 未定义
	_T("Type not defined"),						// 23 未定义
	_T("Type not defined"),						// 24 未定义
	_T("Type not defined"),						// 25 未定义
	_T("Type not defined"),						// 26 未定义
	_T("Type not defined"),						// 27 未定义
	_T("Type not defined"),						// 28 未定义
	_T("Type not defined"),						// 29 未定义
	_T("Traceroute"),
	_T("Datagram Conversion Error"),
	_T("Mobile Host Redirect"),
	_T("IPv6 Where-Are-You"),
	_T("IPv6 I-Am-Here"),
	_T("Mobile Registration Request"),
	_T("Mobile Registration Reply"),
	_T("Domain Name Request"),
	_T("Domain Name Reply"),
	_T("SKIP"),
	_T("Photuris"),
	_T("Experimental mobility protocols")		// 41
};

WCHAR g_szIgmpType[35][40] = {
	_T("Type not defined"),						// 0
	_T("Create Group Request"),
	_T("Create Group Reply"),
	_T("Join Group Request"),
	_T("Join Group Reply"),
	_T("Leave Group Request"),
	_T("Leave Group Reply"),
	_T("Confirm Group Request"),
	_T("Confirm Group Reply"),					
	_T("Type not defined"),						// 9
	_T("Type not defined"),						// 10
	_T("Membership Query"),
	_T("Membership Report"),
	_T("DVMRP Protocol"),
	_T("PIM Routing Message"),
	_T("Type not defined"),						// 15
	_T("Membership Report"),
	_T("Leave Group"),
	_T("Type not defined"),						// 18
	_T("Type not defined"),						// 19
	_T("Type not defined"),						// 20
	_T("Type not defined"),						// 21
	_T("Type not defined"),						// 22
	_T("Type not defined"),						// 23
	_T("Type not defined"),						// 24
	_T("Type not defined"),						// 25
	_T("Type not defined"),						// 26
	_T("Type not defined"),						// 27
	_T("Type not defined"),						// 28
	_T("Type not defined"),						// 29
	_T("Traceroute Response"),
	_T("Traceroute Query or Request"),
	_T("Type not defined"),						// 32
	_T("Type not defined"),						// 33
	_T("Membership Report")
};