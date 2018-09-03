#ifndef _BITSTREAM_H
#define _BITSTREAM_H

#include <stdint.h>
#include <assert.h>
#include <string.h>

//ported from x264 reference code, see bs.h file		chendelin	2006-12-27 8:45
typedef struct
{
	uint8_t* p_start;
	uint8_t* p_old;
	uint8_t* p_cur;
	uint8_t* p_end;
	int		 i_left;				//the left bits of current byte	(*p)

} BITS_t;

static _inline int BITS_map(BITS_t* pbs, uint8_t* buf, int length)
{
	pbs->p_start = buf;
	pbs->p_cur = buf;
	pbs->p_old = buf;							//
	pbs->p_end = pbs->p_start + length;
	pbs->i_left = 8;

	return 0;
}

static _inline void BITS_align(BITS_t *pbs)
{
    if (pbs->i_left != 8)
    {
        pbs->i_left = 8;
        pbs->p_cur++;
    }
}

static _inline int BITS_bealigned(BITS_t *pbs)
{
	return (pbs->i_left == 8);
}

static _inline int BITS_pos(BITS_t *pbs)
{
    return (((int)(pbs->p_cur - pbs->p_start) << 3) + 8 - pbs->i_left);
}

static _inline int BITS_eof(BITS_t *pbs)
{
    return ((pbs->p_cur >= pbs->p_end) ? 1 : 0);
}

static _inline unsigned int BITS_byteCopy(void* dst, BITS_t *pbs, int bytes)
{
    unsigned int  i_result = 0;
    
    assert(pbs->i_left == 8);
    
    pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.5.23
    
    if ((pbs->p_cur + bytes) <= pbs->p_end)
    {
        memcpy(dst, pbs->p_cur, bytes);
        pbs->p_cur += bytes;
    }

    return (i_result);
}

static _inline unsigned int BITS_byteSkip(BITS_t *pbs, int bytes)
{
	unsigned int  i_result = 0;

	assert(pbs->i_left == 8);

	pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.5.23

	if ((pbs->p_cur + bytes) <= pbs->p_end)
	{
		pbs->p_cur += bytes;
	}

	return (i_result);
}

static uint32_t i_mask[33] =
{
	0x00,
	0x01,      0x03,      0x07,      0x0f,
	0x1f,      0x3f,      0x7f,      0xff,
	0x1ff,     0x3ff,     0x7ff,     0xfff,
	0x1fff,    0x3fff,    0x7fff,    0xffff,
	0x1ffff,   0x3ffff,   0x7ffff,   0xfffff,
	0x1fffff,  0x3fffff,  0x7fffff,  0xffffff,
	0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
	0x1fffffff,0x3fffffff,0x7fffffff,0xffffffff
};

static _inline uint32_t BITS_get(BITS_t *pbs, int bits)
{
    int		      i_shr;
    unsigned int  i_result = 0;

    pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.5.23
    while (bits > 0)
    {
        if (pbs->p_cur >= pbs->p_end)
        {
            break;
        }

        if ((i_shr = pbs->i_left - bits) >= 0)
        {
            /* more in the buffer than requested */
            i_result |= (*pbs->p_cur >> i_shr) & i_mask[bits];
            pbs->i_left -= bits;
            if (pbs->i_left == 0)
            {
                pbs->p_cur++;
                pbs->i_left = 8;
            }

            return (i_result);
        }
        else
        {
            /* less in the buffer than requested */
           i_result |= (*pbs->p_cur & i_mask[pbs->i_left]) << -i_shr;
           bits  -= pbs->i_left;
           pbs->p_cur++;
           pbs->i_left = 8;
        }
    }

    return (i_result);
}

static _inline uint64_t BITS_x64_get(BITS_t *pbs, int bits)
{
    int      i_shr;
    uint64_t   i_result = 0;

	pbs->p_old = pbs->p_cur;				//保存一下旧指针    added by chendelin  2018.5.23
    while (bits > 0)
    {
        if (pbs->p_cur >= pbs->p_end)
        {
            break;
        }

        if ((i_shr = pbs->i_left - bits) >= 0)
        {
            /* more in the buffer than requested */
            i_result |= (*pbs->p_cur >> i_shr) & i_mask[bits];
            pbs->i_left -= bits;
            if (pbs->i_left == 0)
            {
                pbs->p_cur++;
                pbs->i_left = 8;
            }

            return (i_result);
        }
        else
        {
            /* less in the buffer than requested */
           i_result |= (uint64_t)(*pbs->p_cur & i_mask[pbs->i_left]) << -i_shr;
           bits  -= pbs->i_left;
           pbs->p_cur++;
           pbs->i_left = 8;
        }
    }

    return (i_result);
}

static _inline uint32_t BITS_get1(BITS_t *pbs)
{
    if (pbs->p_cur < pbs->p_end)
    {
		uint32_t i_result;

        pbs->i_left --;
        i_result = (*pbs->p_cur >> pbs->i_left) & 0x01;
        
		if (pbs->i_left == 0)
        {
            pbs->p_cur++;
            pbs->i_left = 8;
        }

        return i_result;
    }

    return 0;
}

static _inline uint32_t BITS_show(BITS_t *pbs, int bits)		//this function has a bug.		chendelin	
{
	uint8_t* p = pbs->p_cur;

	pbs->p_old = pbs->p_cur;
    if ((p < pbs->p_end) && (bits > 0))
    {
		uint32_t i_cache = ((pbs->p_cur[0] << 24) + (pbs->p_cur[1] << 16) + (pbs->p_cur[2] << 8) + pbs->p_cur[3]) << (8 - pbs->i_left);

        return (i_cache >> (32 - bits));
    }

    return 0;
}

//static _inline U64 BITS_x64_show(BITS_t *pbs, S32 bits)		//this function has a bug.		chendelin	
//{
//	U8* p = pbs->p_cur;
//
//	pbs->p_old = pbs->p_cur;
//    if ((p < pbs->p_end) && (bits > 0))
//    {
//        U64 i_cache = ((pbs->p_cur[7] << 56) + (pbs->p_cur[6] << 48) + (pbs->p_cur[5] << 40) + (pbs->p_cur[4] << 32) + (pbs->p_cur[3] << 24) + (pbs->p_cur[2] << 16) + (pbs->p_cur[1] << 8) + pbs->p_cur[0]) << (8 - pbs->i_left);
//
//        return (i_cache >> (64 - bits));
//    }
//
//    return 0;
//}

static _inline uint32_t BITS_set(BITS_t *pbs, int bits, uint32_t value)
{
	uint8_t	a;
    while (bits > 0)
    {
        if (pbs->p_cur >= pbs->p_end)
        {
            break;
        }

        bits --;

        if ((value >> bits) & 0x01)
        {
            a = 1 << (pbs->i_left - 1);
			*pbs->p_cur |= a;
//			*pbs->p |= 1 << (pbs->i_left - 1);
        }
        else
        {
            a = 1 << (pbs->i_left - 1);
			a = ~a;
			*pbs->p_cur &= a; 
//           *pbs->p &= ~(1 << (pbs->i_left - 1));
        }

        pbs->i_left --;

        if (pbs->i_left == 0)
        {
            pbs->p_cur++;
            pbs->i_left = 8;
        }
    }

	return 0;
}

static inline void BITS_set1(BITS_t *pbs, int value)
{
    if (pbs->p_cur < pbs->p_end )
    {
        pbs->i_left--;

        if (value & 0x01)
        {
            *pbs->p_cur |= 1 << pbs->i_left;
        }
        else
        {
            *pbs->p_cur &= ~(1 << pbs->i_left);
        }

        if (pbs->i_left == 0)
        {
            pbs->p_cur++;
            pbs->i_left = 8;
        }
    }
}

/* TODO optimize */
static _inline void BITS_skip(BITS_t *pbs, int bits)
{
	pbs->p_old = pbs->p_cur;
    pbs->i_left -= bits;

    while (pbs->i_left <= 0)
    {
        pbs->p_cur++;
        pbs->i_left += 8;
    }
}

static _inline uint32_t BITS_ue(BITS_t *pbs)
{
    int i = 0;

    while ((BITS_get1(pbs) == 0) && (pbs->p_cur < pbs->p_end) && (i < 32))
    {
        i++;
    }

    return ((1 << i) - 1 + BITS_get(pbs, i));
}

static _inline int BITS_se(BITS_t *pbs)
{
    int val = BITS_ue(pbs);

    return (val & 0x01 ? (val + 1 ) / 2 : - (val / 2));
}

static _inline int BITS_ae(BITS_t* pbs)
{
	return 0;
}

static _inline int BITS_te(BITS_t *pbs, int x)
{
    if (x == 1)
    {
        return 1 - BITS_get1(pbs);
    }
    else if (x > 1)
    {
        return BITS_ue(pbs);
    }

    return 0;
}

typedef struct tagBitMaskShift
{
	uint8_t		mask;
	int		shift;
} MASK_SHIFT_t, *pMASK_SHIFT_t;

#endif
