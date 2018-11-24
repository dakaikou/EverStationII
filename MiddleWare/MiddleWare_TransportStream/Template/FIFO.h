#ifndef _BYTEFIFO_H_
#define _BYTEFIFO_H_

#include <stdio.h>
#include <stdint.h>
#include <Windows.h>

#include "FIFO_ErrorCode.h"
#define USE_FIFO_MUTEX			0

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

#if USE_FIFO_MUTEX
	HANDLE	m_hAccess;
#endif

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

#if USE_FIFO_MUTEX
	m_hAccess = ::CreateMutex(NULL, FALSE, NULL);
#endif

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

#if USE_FIFO_MUTEX
	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state = WAIT_OBJECT_0)
	{
#endif
		if (m_startptr != NULL)
		{
			delete m_startptr;
		}

		m_capacity = 0;
		m_rdptr = m_wrptr = m_endptr = m_startptr = NULL;
		m_available_payload = 0;
		m_available_space = 0;

#if USE_FIFO_MUTEX
		::CloseHandle(m_hAccess);
		m_hAccess = NULL;
	}
#endif
}

template <class DType, int N> void CFIFO<DType, N>::Reset(void)
{
#if USE_FIFO_MUTEX
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		m_rdptr = m_wrptr = m_startptr;
		m_available_payload = 0;
		m_available_space = m_capacity;

#if USE_FIFO_MUTEX
		::ReleaseMutex(m_hAccess);
	}
#endif
}

template <class DType, int N> int CFIFO<DType, N>::GetAvailableSpace(void)
{
	int		 rtcode = -1;

#if USE_FIFO_MUTEX
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		rtcode = m_available_space;

#if USE_FIFO_MUTEX
		::ReleaseMutex(m_hAccess);
	}
#endif

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::WriteData(DType* buf, int length)
{
	int			rtcode = FIFO_UNKNOWN_ERROR;
	uint32_t	wait_state = WAIT_FAILED;
	int			next_write_length;
	int			tail_length;

	if ((buf != NULL) && (length > 0))
	{
#if USE_FIFO_MUTEX
		wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
#endif
			if (m_available_space >= length)
			{
				tail_length = (int)(m_endptr - m_wrptr);
				if (tail_length < length)
				{
					//in this case, wrptr must great than rdptr 
					// |----------------|XXXXXXXXXXXXXXXXXX|------|
					//                  rdptr               wrptr  endptr
					//尾部空间不够，分两次写入
					memcpy(m_wrptr, buf, tail_length * sizeof(DType));
					m_wrptr = m_startptr;
					next_write_length = length - tail_length;

					memcpy(m_wrptr, buf + tail_length, next_write_length * sizeof(DType));
					m_wrptr += next_write_length;

					assert(m_wrptr <= m_rdptr);
				}
				else
				{
					//int this case, wrptr may less than rdptr, or great than then rdptr
					// |XXXXXXXXXX|------------------|XXXXXXXXXXXXXXXX|
					//            wrptr            rdptr           endptr
					// |----------|XXXXXXXXXXXXXXXXXX|----------------|
					//            rdptr               wrptr  endptr
					//

					//尾部空间足够，直接写入
					memcpy(m_wrptr, buf, length * sizeof(DType));
					m_wrptr += length;
					if (m_wrptr == m_endptr)
					{
						m_wrptr = m_startptr;
					}
				}

				m_available_payload += length;
				m_available_space -= length;

				rtcode = FIFO_NO_ERROR;
			}
			else
			{
				rtcode = FIFO_WRITE_OVERFLOW;
			}

#if USE_FIFO_MUTEX
			::ReleaseMutex(m_hAccess);
		}
		else
		{
			rtcode = wait_state;
		}
#endif
	}
	else
	{
		rtcode = FIFO_INVALID_PARAMETER_ERROR;
	}

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::GetAvailablePayload(void)
{
	int		 rtcode = -1;

#if USE_FIFO_MUTEX
	uint32_t wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		rtcode = m_available_payload;

#if USE_FIFO_MUTEX
		::ReleaseMutex(m_hAccess);
	}
#endif

	return rtcode;
}

template <class DType, int N> int CFIFO<DType, N>::PrefetchData(DType* buf, int length)
{
	int			rtcode = FIFO_UNKNOWN_ERROR;
	int			tail_length;
	int			next_read_length;
	uint32_t	wait_state = WAIT_FAILED;

	if ((buf != NULL) && (length > 0))
	{
#if USE_FIFO_MUTEX
		wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
		if (wait_state == WAIT_OBJECT_0)
		{
#endif
			if (m_available_payload >= length)
			{
				tail_length = (int)(m_endptr - m_rdptr);
				if (tail_length < length)
				{
					//int this case, rdptr must great than wrptr, and the tail payload is not enough
					// |XXXXXXXXXX|----------------|XXXXXXXXXX|CCCC
					//            wrptr            rdptr      endptr

					memcpy(buf, m_rdptr, tail_length * sizeof(DType));
					next_read_length = length - tail_length;
					memcpy(buf + tail_length, m_startptr, next_read_length * sizeof(DType));
				}
				else
				{
					//in this case, rdptr can great than wrptr, or less than wrptr 
					// |XXXXXXXXXX|----------------|CCCCCCCCCCXXXX|
					//            wrptr            rdptr           endptr
					//
					// |-------------|XXXXXXXXXXXXXXXXXX|----------------|
					//               rdptr               wrptr           endptr
					memcpy(buf, m_rdptr, length * sizeof(DType));
				}

				rtcode = FIFO_NO_ERROR;
			}
			else
			{
				rtcode = FIFO_READ_UNDERFLOW;
			}

#if USE_FIFO_MUTEX
			::ReleaseMutex(m_hAccess);
		}
		else
		{
			rtcode = wait_state;
		}
#endif
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

#if USE_FIFO_MUTEX
	uint32_t	wait_state = WAIT_FAILED;

	wait_state = ::WaitForSingleObject(m_hAccess, INFINITE);
	if (wait_state == WAIT_OBJECT_0)
	{
#endif
		int skip_legth = min(m_available_payload, length);

		m_rdptr += skip_legth;
		if (m_rdptr >= m_endptr)
		{
			m_rdptr = m_startptr + (m_rdptr - m_endptr);
		}

		m_available_payload -= skip_legth;
		m_available_space += skip_legth;

		rtcode = FIFO_NO_ERROR;

#if USE_FIFO_MUTEX
		::ReleaseMutex(m_hAccess);
	}
	else
	{
		rtcode = wait_state;
	}
#endif

	return rtcode;
}

#endif

