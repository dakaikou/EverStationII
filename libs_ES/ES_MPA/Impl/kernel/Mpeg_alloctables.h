#ifndef _MPEG_ALLOCTABLES_H_
#define _MPEG_ALLOCTABLES_H_

/*------------------------------------------------------------------------
 *
 * Copyright (c) 1997-1998 by Cornell University.
 * 
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *------------------------------------------------------------------------
 */
typedef struct AllocTableEntry
{
    int	 steps;
    int	 bits;
    int	 group;
    int	 quant;

} AllocTableEntry;

typedef AllocTableEntry AllocTable[32][16];
typedef AllocTable*		AllocTablePtr;

extern const AllocTableEntry alloc_0[27][16];
extern const AllocTableEntry alloc_1[30][16];
extern const AllocTableEntry alloc_2[8][16];
extern const AllocTableEntry alloc_3[12][16];


#endif
