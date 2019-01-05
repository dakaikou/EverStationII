#include <assert.h>
#include <memory.h>

#include "../Include/INET_IPv4.h"
#include "../../common/BitStream.h"

int	INET_IPv4_Encode(uint8_t* buf, int* plength, INET_IPv4_t* pIPv4)
{
	int			rtcode = 0;
	BITS_t		bs;

	if ((pIPv4 != NULL) && (buf != NULL) && (plength != NULL))
	{
		assert(*plength >= pIPv4->TotalLength);
		
		BITS_map(&bs, buf, *plength);

		BITS_set(&bs, 4, pIPv4->Version);
		BITS_set(&bs, 4, pIPv4->IHL);
		BITS_set(&bs, 8, pIPv4->TypeofService);
		BITS_set(&bs, 16, pIPv4->TotalLength);

		BITS_set(&bs, 16, pIPv4->Identification);
		BITS_set(&bs, 3, pIPv4->Flags);
		BITS_set(&bs, 13, pIPv4->FragmentOffset);

		BITS_set(&bs, 8, pIPv4->TimetoLive);
		BITS_set(&bs, 8, pIPv4->Protocol);
		BITS_set(&bs, 16, pIPv4->HeaderChecksum);

		BITS_set(&bs, 32, pIPv4->SourceAddress);
		BITS_set(&bs, 32, pIPv4->DestinationAddress);

		bs.p_cur = bs.p_start + (pIPv4->IHL << 2);

		if (bs.p_cur != pIPv4->data_buf)
		{
			memcpy(bs.p_cur, pIPv4->data_buf, pIPv4->data_length);
		}
		bs.p_cur += pIPv4->data_length;

		*plength = (int)(bs.p_cur - bs.p_start);

		assert(*plength == pIPv4->TotalLength);
	}
	else
	{
		rtcode = 1;
	}

	return rtcode;
}

int	INET_IPv4_Parse(uint8_t* buf, int length, INET_IPv4_t* pIPv4)
{
	int			rtcode = 0;
	int			header_length = 0;
	BITS_t		bs;

	if ((pIPv4 != NULL) && (buf != NULL))
	{
		assert(length >= IPv4_HEAD_LENGTH_MIN);
		BITS_map(&bs, buf, length);

		pIPv4->Version = BITS_get(&bs, 4);

		if (pIPv4->Version == 4)					//IPv4
		{
			pIPv4->IHL = BITS_get(&bs, 4);
			pIPv4->TypeofService = BITS_get(&bs, 8);
			pIPv4->TotalLength = BITS_get(&bs, 16);

			assert(pIPv4->TotalLength <= length);	

			pIPv4->Identification = BITS_get(&bs, 16);
			pIPv4->Flags = BITS_get(&bs, 3);
			pIPv4->FragmentOffset = BITS_get(&bs, 13);
			pIPv4->TimetoLive = BITS_get(&bs, 8);
			pIPv4->Protocol = BITS_get(&bs, 8);
			pIPv4->HeaderChecksum = BITS_get(&bs, 16);
			pIPv4->SourceAddress = BITS_get(&bs, 32);
			pIPv4->DestinationAddress = BITS_get(&bs, 32);

			header_length = pIPv4->IHL * 4;	// 4byte 的倍数
			pIPv4->data_length = pIPv4->TotalLength - header_length;
			pIPv4->data_buf = buf + header_length; 
		}
		else
		{
			rtcode = 2;
		}
	}
	else
	{
		rtcode = 1;					//指针为空
	}

	return rtcode;
}

int	INET_UDP_Encode(uint8_t* buf, int* plength, INET_UDP_t* pUDP)
{
	int			rtcode = 0;
	BITS_t		bs;

	if ((pUDP != NULL) && (buf != NULL) && (*plength != NULL))
	{
		assert(pUDP->Length == (pUDP->data_length + 8));

		BITS_map(&bs, buf, *plength);

		BITS_set(&bs, 16, pUDP->SourcePort);
		BITS_set(&bs, 16, pUDP->DestinationPort);
		BITS_set(&bs, 16, pUDP->Length);
		BITS_set(&bs, 16, pUDP->Checksum);

		if (bs.p_cur != pUDP->data_buf)
		{
			memcpy(bs.p_cur, pUDP->data_buf, pUDP->data_length);
		}
		bs.p_cur += pUDP->data_length;

		*plength = (int)(bs.p_cur - bs.p_start);
		assert(*plength == pUDP->Length);
	}
	else
	{
		rtcode = 1;
	}
	
	return rtcode;
}

int	INET_UDP_Parse(uint8_t* buf, int length, INET_UDP_t* pUDP)
{
	int			rtcode = 0;
	BITS_t		bs;

	if ((pUDP != NULL) && (buf != NULL))
	{
		assert(length >= UDP_HEAD_LENGTH);
		BITS_map(&bs, buf, length);
		
		pUDP->SourcePort = BITS_get(&bs, 16);
		pUDP->DestinationPort = BITS_get(&bs, 16);

		pUDP->Length = BITS_get(&bs, 16);
		pUDP->Checksum = BITS_get(&bs, 16);
		
		assert(pUDP->Length <= length);

		pUDP->data_buf = bs.p_cur;
		pUDP->data_length = pUDP->Length - 8;
		bs.p_cur += pUDP->data_length;
		
		assert(pUDP->Length == (bs.p_cur - bs.p_start));
	}
	else
	{
		rtcode = 1;
	}
	
	return rtcode;
}

int	INET_RTP_Parse(uint8_t* buf, int length, INET_RTP_t* pRTP)
{
	int			rtcode = 0;
	BITS_t		bs;
	int			i;

	if ((pRTP != NULL) && (buf != NULL))
	{
		BITS_map(&bs, buf, length);

		pRTP->version = BITS_get(&bs, 2);
		pRTP->padding = BITS_get(&bs, 1);
		pRTP->extension = BITS_get(&bs, 1);
		pRTP->CSRC_count = BITS_get(&bs, 4);

		pRTP->marker = BITS_get(&bs, 1);
		pRTP->payload_type = BITS_get(&bs, 7);

		pRTP->sequence_number = BITS_get(&bs, 16);
		pRTP->timestamp = BITS_get(&bs, 32);
		pRTP->SSRC = BITS_get(&bs, 32);

		for (i = 0; i < pRTP->CSRC_count; i++)
		{
			pRTP->CSRC[i] = BITS_get(&bs, 32);
		}

		pRTP->data_buf = bs.p_cur;
		pRTP->data_length = (int)(bs.p_end - bs.p_cur);

		bs.p_cur = bs.p_end;
	}
	else
	{
		rtcode = 1;
	}

	return rtcode;
}

int	INET_RTP_Encode(uint8_t* buf, int* plength, INET_RTP_t* pRTP)
{
	int			rtcode = 0;
	BITS_t		bs;
	int			i;

	if ((pRTP != NULL) && (buf != NULL))
	{
		BITS_map(&bs, buf, *plength);

		BITS_set(&bs, 2, pRTP->version);
		BITS_set(&bs, 1, pRTP->padding);
		BITS_set(&bs, 1, pRTP->extension);
		BITS_set(&bs, 4, pRTP->CSRC_count);

		BITS_set(&bs, 1, pRTP->marker);
		BITS_set(&bs, 7, pRTP->payload_type);

		BITS_set(&bs, 16, pRTP->sequence_number);
		BITS_set(&bs, 32, pRTP->timestamp);
		BITS_set(&bs, 32, pRTP->SSRC);

		for (i = 0; i < pRTP->CSRC_count; i++)
		{
			BITS_set(&bs, 32, pRTP->CSRC[i]);
		}

		if (pRTP->data_buf != NULL)
		{
			memcpy(bs.p_cur, pRTP->data_buf, pRTP->data_length);
			bs.p_cur += pRTP->data_length;
		}

		*plength = (int)(bs.p_cur - bs.p_start);
	}
	else
	{
		rtcode = 1;
	}

	return rtcode;
}

//int	CDL_INET_protocol_table(uint8_t protocol, S8* pszText, int size)
//{
//	int		rtcode = 0;
//
//	if (pszText != NULL)
//	{
//		if (protocol == 1)
//		{
//			strcpy_s(pszText, size, "ICMP");
//		}
//		else if (protocol == 4)
//		{
//			strcpy_s(pszText, size, "IP");
//		}
//		else if (protocol == 6)
//		{
//			strcpy_s(pszText, size, "TCP");
//		}
//		else if (protocol == 17)
//		{
//			strcpy_s(pszText, size, "UDP");
//		}
//		else
//		{
//			strcpy_s(pszText, size, "未知");
//		}
//	}
//	else
//	{
//		rtcode = 1;
//	}
//
//	return rtcode;
//}
