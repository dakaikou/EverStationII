#ifndef _BYTEFIFO_H_
#define _BYTEFIFO_H_

#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#include "FIFO_ErrorCode.h"

template <class DType, int N> class CFIFO
{
public:
	CFIFO(void);
	~CFIFO(void);
protected:
	int			m_capacity;				//FIFO的总容量
	DType*		m_startptr;				//FIFO起始指针
	DType*		m_endptr;					//FIFO终止指针					

	DType*		m_rdptr;					//FIFO读指针
	DType*		m_wrptr;					//FIFO写指针

	int		m_available_payload;		//可读的载荷数量
	int		m_available_space;		//可写的空间容量

	HANDLE	m_hAccess;

public:
	void Reset(void);

	int WriteData(DType* buf, int length);
	int PrefetchData(DType* buf, int length);
	int SkipData(int length);

	int GetAvailablePayload(void);
	int GetAvailableSpace(void);
};

template <class DType, int N> CFIFO<DType, N>::CFIFO(void)
{
	m_capacity = 0;
	m_startptr = NULL;
	m_endptr = NULL;
	m_wrptr = NULL;
	m_rdptr = NULL;
	m_available_payload = 0;
	m_available_space = 0;

	m_hAccess = ::CreateMutex(NULL, FALSE, NULL);
	m_startptr = new DType[N];
	if (m_startptr != NULL)
	{
		m_capacity = N;
		m_rdptr = m_wrptr = m_startptr;
		m_endptr = m_startptr + N;

		m_available_space = m_capacity;
		m_available_payload = 0;

		memset(m_startptr, 0, m_capacity * sizeof(DType));
	}
}

template <class DType, int N> CFIFO<DType, N>::~CFIFO(void)
{
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);

	if (m_startptr != NULL)
	{
		delete m_startptr;
	}

	m_capacity = 0;
	m_rdptr = m_wrptr = m_endptr = m_startptr = NULL;
	m_available_payload = 0;
	m_available_space = 0;

	::CloseHandle(m_hAccess);
	m_hAccess = NULL;
}

template <class DType, int N> void CFIFO<DType, N>::Reset(void)
{
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
		m_rdptr = m_wrptr = m_startptr;
		m_available_payload = 0;
		m_available_space = m_capacity;

		::ReleaseMutex(m_hAccess);
	}
}

template <class DType, int N> int CFIFO<DType, N>::GetAvailablePayload(void)
{
	int		 rtcode = FIFO_UNKNOWN_ERROR;
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
		rtcode = m_available_payload;

		::ReleaseMutex(m_hAccess);
	}

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::GetAvailableSpace(void)
{
	int		 rtcode = FIFO_UNKNOWN_ERROR;
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
		rtcode = m_available_space;

		::ReleaseMutex(m_hAccess);
	}

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::WriteData(DType* buf, int length)
{
	int			rtcode = FIFO_UNKNOWN_ERROR;
	uint32_t	wait_state = WAIT_FAILED;
	int			left_length;
	int			tail_length;

	if ((buf != NULL) && (length > 0))
	{
		wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
			if (m_available_space >= length)
			{
				tail_length = (int)(m_endptr - m_wrptr);
				if (tail_length >= length)
				{
					//尾部空间足够，直接写入
					memcpy(m_wrptr, buf, length * sizeof(DType));
					m_wrptr += length;
					if (m_wrptr == m_endptr)
					{
						m_wrptr = m_startptr;
					}
				}
				else
				{
					//尾部空间不够，分两次写入
					memcpy(m_wrptr, buf, tail_length * sizeof(DType));
					m_wrptr = m_startptr;
					left_length = length - tail_length;

					memcpy(m_wrptr, buf + tail_length, left_length * sizeof(DType));
					m_wrptr += left_length;

					assert(m_wrptr <= m_rdptr);
				}

				m_available_payload += length;
				m_available_space -= length;

				rtcode = FIFO_NO_ERROR;
			}
			else
			{
				rtcode = FIFO_WRITE_OVERFLOW;
			}

			::ReleaseMutex(m_hAccess);
		}
		else
		{
			rtcode = wait_state;
		}
	}
	else
	{
		rtcode = FIFO_INVALID_PARAMETER_ERROR;
	}

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::PrefetchData(DType* buf, int length)
{
	int			rtcode = FIFO_UNKNOWN_ERROR;
	int			tail_length;
	int			left_length;
	uint32_t	wait_state = WAIT_FAILED;

	if ((buf != NULL) && (length > 0))
	{
		wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
			tail_length = (int)(m_endptr - m_rdptr);
			if (tail_length >= length)
			{
				memcpy(buf, m_rdptr, length * sizeof(DType));
			}
			else
			{
				memcpy(buf, m_rdptr, tail_length * sizeof(DType));
				left_length = length - tail_length;
				memcpy(buf + tail_length, m_startptr, left_length * sizeof(DType));
			}

			::ReleaseMutex(m_hAccess);

			rtcode = FIFO_NO_ERROR;
		}
		else
		{
			rtcode = wait_state;
		}
	}
	else
	{
		rtcode = FIFO_INVALID_PARAMETER_ERROR;
	}

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::SkipData(int length)
{
	int			rtcode = FIFO_UNKNOWN_ERROR;
	uint32_t	wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
		m_rdptr += length;
		if (m_rdptr >= m_endptr)
		{
			m_rdptr = m_startptr + (m_rdptr - m_endptr);
		}

		m_available_payload -= length;
		m_available_space += length;

		::ReleaseMutex(m_hAccess);

		rtcode = FIFO_NO_ERROR;
	}
	else
	{
		rtcode = wait_state;
	}

	return rtcode;
}

#endif

