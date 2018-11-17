
#include "../Include/SIOWithUDP.h"

#include "translate_layer/INET_IPv4/Include/INET_IPv4.h"
#include "toolbox_libs/TOOL_Directory/include/TOOL_Directory.h"

//#include <sys/types.h>
//#include <sys/stat.h>
//#include <windows.h>
#include <assert.h>
#include <string.h>

#include <Winsock2.h>

#include "thirdparty_libs/Winpcap/Include/pcap.h"

static	pcap_if_t	 *alldevs;
static 	pcap_t		 *adhandle;
static  int			 recv_byte_count;
static  int			 recv_bitrate;
static  int			 udp_receiving = 0;
static  struct timeval old_time_tick;

int packet_handler(uint8_t *param, const struct pcap_pkthdr *header, const uint8_t *pkt_data, uint8_t* recv_buf, int count);


int udp_transmit_init(void)
{
	int rtcode = NETWORK_NO_ERROR;

	return rtcode;
}

int udp_start_transmit(int bitrate)
{
	int rtcode = NETWORK_NO_ERROR;
		
	return rtcode;
}

int udp_transmit(unsigned char *buffer, int count)
{
	int rtcode = NETWORK_NO_ERROR;
		
	return rtcode;
}

int udp_stop_transmit(void)
{
	int rtcode = NETWORK_NO_ERROR;
		
	return rtcode;
}

int udp_transmit_fini(void)
{
	return 0;
}

int udp_receive_init(char* pszDstIP, int dst_port)
{
	int rtcode = NETWORK_NO_ERROR;

	pcap_if_t	 *d;
	int			 device_count;
	char		 errbuf[PCAP_ERRBUF_SIZE];
	uint32_t	 netmask;
	//char		 packet_filter[] = "ip and udp";
	char		 packet_filter[128];
	char		 pszExeFile[256];
	char		 pszExePath[256];
	char		 pszIniFile[256];
	char		 pszIFSelected[256];
	//char		 pszDebugFile[256];
	int			 len;

	struct bpf_program fcode;

	udp_receiving = 0;

	assert(pszDstIP != NULL);

//	time_t local_tv_sec;
//	struct tm *ltime;
//	char timestr[16];
	FILE*		fp_debug = NULL;

	GetModuleFileNameA(NULL, pszExeFile, sizeof(pszExeFile)); 
	len = DIR_GetModulePathLength(pszExeFile);
	assert(len > 0);

	memcpy(pszExePath, pszExeFile, len);
	pszExePath[len] = '\0';
	sprintf_s(pszIniFile, sizeof(pszIniFile), "%s\\config.ini", pszExePath);

	GetPrivateProfileStringA((LPCSTR)"winpcap", (LPCSTR)"网卡名称", NULL, pszIFSelected, sizeof(pszIFSelected), (LPCSTR)pszIniFile);

	//errno_t err = fopen_s(&fp_debug, pszDebugFile, "wt");

	//获取可用的网卡设备
	if (pcap_findalldevs(&alldevs, errbuf) == -1)
	{
		if (fp_debug != NULL)
		{
			fprintf(fp_debug, "Error in pcap_findalldevs: %s\n\r", errbuf);
		}

		rtcode = NETWORK_NO_DEVICES;
	}
	else
	{
		// Print the list 
		device_count = 0;
		int nSel = -1;
		for (d = alldevs; d; d = d->next)
		{
			if (strcmp(d->name, pszIFSelected) == 0)
			{
				nSel = device_count;
				break;
			}

			device_count++;
		}

		if (nSel == -1)
		{
			nSel = device_count - 1;
		}

		assert(nSel >= 0);

		int i;
		for (d = alldevs, i = 0; i < nSel; d = d->next, i++);

		// Open the adapter 
		if ((adhandle= pcap_open_live(d->name,			// name of the device
										65536,			// portion of the packet to capture. 
														// 65536 grants that the whole packet will be captured on all the MACs.
										1,				// promiscuous mode (nonzero means promiscuous)
										1000,				// read timeout
										errbuf			// error buffer
										)) == NULL)
		{
			if (fp_debug != NULL)
			{
				fprintf(fp_debug, "Unable to open the adapter. %s is not supported by WinPcap\n\r", d->name);
			}
	
			rtcode = NETWORK_OPEN_ERROR;
		}
		else
		{
			// Check the link layer. We support only Ethernet for simplicity. 
			int link_protocol = pcap_datalink(adhandle);
			if (link_protocol != DLT_EN10MB)
			{
				if (fp_debug != NULL)
				{
					fprintf(fp_debug, "This program works only on Ethernet networks.\n\r");
				}

				rtcode = NETWORK_LINK_LAYER_TYPE_ERROR;
			}
			else
			{
				if (d->addresses != NULL)
				{
					netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
				}
				else
				{
					// If the interface is without addresses we suppose to be in a C class network 
					netmask = 0xffffff; 
				}

				if (strlen(pszDstIP) > 0)
				{
					sprintf_s(packet_filter, "ip dst host %s and udp", pszDstIP);
				}
				else
				{
					//使用默认的本机地址
					sockaddr_in* nifaddr = (struct sockaddr_in *)(d->addresses->addr);

					sprintf_s(packet_filter, "ip dst host %d.%d.%d.%d and udp", nifaddr->sin_addr.S_un.S_un_b.s_b1,
																				nifaddr->sin_addr.S_un.S_un_b.s_b2,
																				nifaddr->sin_addr.S_un.S_un_b.s_b3,
																				nifaddr->sin_addr.S_un.S_un_b.s_b4);
				}

				if (dst_port > 0)
				{
					sprintf_s(packet_filter, sizeof(packet_filter), "%s port %d", packet_filter, dst_port);
				}

				//compile the filter
				if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) <0 )
				{
					if (fp_debug != NULL)
					{
						fprintf(fp_debug, "Unable to compile the packet filter[%s]. Check the syntax.\n\r", packet_filter);
					}
					rtcode = NETWORK_COMPILE_FILTER_ERROR;
				}
				else
				{
					//set the filter
					if (pcap_setfilter(adhandle, &fcode) < 0)
					{
						if (fp_debug != NULL)
						{
							fprintf(fp_debug, "Error setting the filter.\n\r");
						}
						rtcode = NETWORK_SET_FILTER_ERROR;
					}
					else
					{
						if (fp_debug != NULL)
						{
							fprintf(fp_debug, "listening on %s...\n\r", d->description);
						}
					}
				}
			}
		}
	}

	if (fp_debug != NULL)
	{
		fclose(fp_debug);
	}

	return rtcode;
}

int udp_start_receive(void)
{
	int rtcode = NETWORK_NO_ERROR;

	recv_byte_count = 0;
	recv_bitrate = 0;

	udp_receiving = 1;

	//old_tick_count = ::GetTickCount();

	old_time_tick.tv_sec = -1;
	old_time_tick.tv_usec = -1;

	return rtcode;

}

int udp_receive(unsigned char *buffer, int count)
{
	int		 rtcode = NETWORK_NO_ERROR;
	int			 res = 0;
	struct pcap_pkthdr *header;
	const uint8_t *pkt_data;
	double		 tick_delt;
	double		 data_rate;

	res = pcap_next_ex(adhandle, &header, &pkt_data);
	if (res > 0)
	{
		rtcode = packet_handler(NULL, header, pkt_data, buffer, count);
		recv_byte_count += rtcode;

		if (old_time_tick.tv_sec == -1)
		{
			old_time_tick.tv_sec = header->ts.tv_sec;
			old_time_tick.tv_usec = header->ts.tv_usec;
		}
		else
		{
			int delt_second = header->ts.tv_sec - old_time_tick.tv_sec;
			if (delt_second > 0)		//实际上是大于等于1s
			{
				int delt_usecond = header->ts.tv_usec - old_time_tick.tv_usec;
				if (delt_usecond < 0)
				{
					delt_second -= 1;
					delt_usecond += 1000000;
				}

				tick_delt = delt_second * 1000000 + delt_usecond;
				data_rate = (double)recv_byte_count / tick_delt;
				
				recv_bitrate = (int)(8000000.0 * data_rate);

				recv_byte_count = 0;
				old_time_tick.tv_sec = header->ts.tv_sec;
				old_time_tick.tv_usec = header->ts.tv_usec;
			}
		}
	}

	return rtcode;
}

int udp_stop_receive(void)
{
	int rtcode = NETWORK_NO_ERROR;

	recv_byte_count = 0;
	recv_bitrate = 0;

	udp_receiving = 0;
		
	return rtcode;
}

int udp_receive_fini(void)
{
	int rtcode = NETWORK_NO_ERROR;

	udp_receiving = 0;

	pcap_freealldevs(alldevs);

	return rtcode;
}

int udp_get_bitrate(void)
{
	int rtcode = recv_bitrate;		//临时增加

	return rtcode;
}


// Callback function invoked by libpcap for every incoming packet 
int packet_handler(uint8_t *param, const struct pcap_pkthdr *header, const uint8_t *pkt_data, uint8_t* recv_buf, int count)
{
	int				rtcode = 0;

//	char			pszDebugInfo[256];
	//struct tm		ltime;
	//char			timestr[16];

	unsigned char*	IP_buf;
	int				IP_length;
	INET_IPv4_t		IPv4;

	unsigned char*	UDP_buf;
	int				UDP_length;
	INET_UDP_t		UDP;

//	unsigned char*	sp_buf;
//	int				sp_length;

	in_addr			src_addr;
	in_addr			dst_addr;

	uint16_t			sport,dport;
	//time_t			local_tv_sec;

	//根校验相关的参数

	/* convert the timestamp to readable format */
	//local_tv_sec = header->ts.tv_sec;
	//localtime_s(&ltime, &local_tv_sec);
	//strftime( timestr, sizeof timestr, "%H:%M:%S", &ltime);

	/* print timestamp and length of the packet */
//	sprintf_s(pszDebugInfo, "%s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);

	/* retireve the position of the ip header */
	IP_buf = (unsigned char*)(pkt_data + 14);
	IP_length = header->len - 14;
	INET_IPv4_Parse(IP_buf, IP_length, &IPv4);

	if (IPv4.Protocol == IPPROTO_UDP)
	{
		src_addr.S_un.S_addr = IPv4.SourceAddress;
		dst_addr.S_un.S_addr = IPv4.DestinationAddress;

		UDP_buf = IPv4.data_buf;
		UDP_length = IPv4.data_length;
		INET_UDP_Parse(UDP_buf, UDP_length, &UDP);

		sport = UDP.SourcePort;
		dport = UDP.DestinationPort;

		if (recv_buf != NULL)
		{
			if (UDP.data_length > count)
			{
				memcpy(recv_buf, UDP.data_buf, count);
				rtcode = count;
			}
			else
			{
				memcpy(recv_buf, UDP.data_buf, UDP.data_length);
				rtcode = UDP.data_length;
			}
		}
	}

	return rtcode;
}
