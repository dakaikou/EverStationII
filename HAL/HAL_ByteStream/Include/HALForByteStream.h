#ifndef _BYTESTREAM_H
#define _BYTESTREAM_H

#include <stdint.h>
#include <assert.h>
#include <string.h>

#define MIN(x, y)						(x < y? x : y)

//ported from x264 reference code, see bs.h file		chendelin	2017-12-27 8:45
typedef struct
{
	uint8_t* p_start;
	uint8_t* p_old;				//to be obselete   chendelin 2018.10.5
	uint8_t* p_cur;
	uint8_t* p_end;

} BYTES_t;

static _inline int BYTES_map(BYTES_t* pbs, uint8_t* buf, int length)
{
	pbs->p_start = buf;
	pbs->p_cur = buf;
	pbs->p_old = buf;							//
	pbs->p_end = pbs->p_start + length;

	return 0;
}

static _inline int BYTES_copy(void* dstBuf, int dstSize, BYTES_t *pbs, int copyLength)
{
    int  i_result = 0;
    
    pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.7.28
    
    if ((pbs->p_cur + copyLength) <= pbs->p_end)
    {
		if (dstBuf != NULL)
		{
			int op_len = MIN(dstSize, copyLength);
			memcpy(dstBuf, pbs->p_cur, op_len);
		}

        pbs->p_cur += copyLength;
    }
	else
	{
		i_result = 0xFFFFFFFF;
	}

    return (i_result);
}

static _inline int BYTES_skip(BYTES_t *pbs, int skipLength)
{
	int  i_result = 0;

	pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.7.28

	if ((pbs->p_cur + skipLength) <= pbs->p_end)
	{
		pbs->p_cur += skipLength;
	}
	else
	{
		i_result = 0xFFFFFFFF;
	}

	return (i_result);
}

static _inline uint32_t BYTES_get(BYTES_t *pbs, int getCount)
{
	assert(getCount <= 4);
    unsigned int  i_result = 0;

    pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.7.25
    while (getCount > 0)
    {
        if (pbs->p_cur >= pbs->p_end)
        {
            break;
        }

        i_result |= *pbs->p_cur;
        pbs->p_cur ++;
		getCount--;
        
        if (getCount > 0)
        {
            i_result <<= 8;
        }
    }

    return (i_result);
}

static _inline uint32_t BYTES_show(BYTES_t *pbs, int showCount)
{
	assert(showCount <= 4);
    unsigned int  i_result = 0;
    uint8_t*		  rd_ptr = pbs->p_cur;

	if ((rd_ptr + showCount) <= pbs->p_end)
	{
	    pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.7.25
	    while (showCount > 0)
	    {
	        i_result |= *rd_ptr;
	        rd_ptr ++;
			showCount--;
	        
	        if (showCount > 0)
	        {
	            i_result <<= 8;
	        }
	    }
	}
		
    return (i_result);
}

static _inline uint64_t BYTES_x64_get(BYTES_t *pbs, int getCount)
{
	assert(getCount <= 8);
    uint64_t   i_result = 0;

	pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.7.25
    while (getCount > 0)
    {
        if (pbs->p_cur >= pbs->p_end)
        {
            break;
        }

        i_result |= *pbs->p_cur;
        pbs->p_cur ++;
		getCount--;

        if (getCount > 0)
        {
            i_result <<= 8;
        }
    }

    return (i_result);
}


#endif
