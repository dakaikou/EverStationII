#ifndef _MPEG_AUDIOTABLES_H_
#define _MPEG_AUDIOTABLES_H_

/*------------------------------------------------------------------------
 *
 * Copyright (c) 1997-1998 by Cornell University.
 * 
 * See the file "license.txt" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 *------------------------------------------------------------------------
 */
#define RESERVE				 -1
#define FREE_BITRATE		 0

extern const int MPA_sampling_rate_table[2][4];
extern const int MPA_bit_rate_table[2][3][16];

#endif
