#ifndef _KERNEL_MPEG_VIDEO_TABLE_H_
#define _KERNEL_MPEG_VIDEO_TABLE_H_

/* Table B-14, DCT coefficients table zero,
 * codes 0100 ... 1xxx (used for first (DC) coefficient)
 */
static DCT_COEFFICIENT_TABLE_t DCTtabfirst[12] =
{
	{0,2,4},												//0100
	{2,1,4},												//0101
	{1,1,3}, {1,1,3},										//011x
	{0,1,1}, {0,1,1}, {0,1,1}, {0,1,1},						//1xxx
	{0,1,1}, {0,1,1}, {0,1,1}, {0,1,1}					
};

/* Table B-14, DCT coefficients table zero,
 * codes 0100 ... 1xxx (used for all other coefficients)
 */
static DCT_COEFFICIENT_TABLE_t DCTtabnext[12] =
{
	{0,2,4},												//0100
	{2,1,4},												//0101
	{1,1,3},  {1,1,3},										//011x
	{64,0,2}, {64,0,2}, {64,0,2}, {64,0,2},					//10xx
	{0,1,2},  {0,1,2},  {0,1,2},  {0,1,2}					//11xx
};

/* Table B-14, DCT coefficients table zero,
 * codes 000001xx ... 00111xxx
 */
static DCT_COEFFICIENT_TABLE_t DCTtab0[60] =
{
	{65,0,6}, {65,0,6}, {65,0,6}, {65,0,6}, //0000 01xx				/* Escape */
	{2,2,7}, {2,2,7},						//0000 100x
	{9,1,7}, {9,1,7},						//0000 101x
	{0,4,7}, {0,4,7},						//0000 110x
	{8,1,7}, {8,1,7},						//0000 111x
	{7,1,6}, {7,1,6}, {7,1,6}, {7,1,6},		//0001 00xx
	{6,1,6}, {6,1,6}, {6,1,6}, {6,1,6},		//0001 01xx
	{1,2,6}, {1,2,6}, {1,2,6}, {1,2,6},		//0001 10xx
	{5,1,6}, {5,1,6}, {5,1,6}, {5,1,6},		//0001 11xx
	{13,1,8},								//0010 0000
	{0,6,8},								//0010 0001
	{12,1,8},								//0010 0010
	{11,1,8},								//0010 0011
	{3,2,8},								//0010 0100
	{1,3,8},								//0010 0101
	{0,5,8},								//0010 0110
	{10,1,8},								//0010 0111
	{0,3,5}, {0,3,5}, {0,3,5}, {0,3,5},		//0010 1xxx
	{0,3,5}, {0,3,5}, {0,3,5}, {0,3,5},
	{4,1,5}, {4,1,5}, {4,1,5}, {4,1,5},		//0011 0xxx
	{4,1,5}, {4,1,5}, {4,1,5}, {4,1,5},
	{3,1,5}, {3,1,5}, {3,1,5}, {3,1,5},		//0011 1xxx
	{3,1,5}, {3,1,5}, {3,1,5}, {3,1,5}
};

/* Table B-15, DCT coefficients table one,
 * codes 000001xx ... 11111111
*/
static DCT_COEFFICIENT_TABLE_t DCTtab0a[252] =
{
	{65,0,6}, {65,0,6}, {65,0,6}, {65,0,6}, //0000 01xx Escape 
	{7,1,7}, {7,1,7},						//0000 100x	
	{8,1,7}, {8,1,7},						//0000 101x
	{6,1,7}, {6,1,7},						//0000 110x
	{2,2,7}, {2,2,7},						//0000 111x
	{0,7,6}, {0,7,6}, {0,7,6}, {0,7,6},		//0001 00xx
	{0,6,6}, {0,6,6}, {0,6,6}, {0,6,6},		//0001 01xx
	{4,1,6}, {4,1,6}, {4,1,6}, {4,1,6},		//0001 10xx
	{5,1,6}, {5,1,6}, {5,1,6}, {5,1,6},		//0001 11xx
	{1,5,8},								//0010 0000
	{11,1,8},								//0010 0001
	{0,11,8},								//0010 0010
	{0,10,8},								//0010 0011
	{13,1,8},								//0010 0100
	{12,1,8},								//0010 0101
	{3,2,8},								//0010 0110
	{1,4,8},								//0010 0111
	{2,1,5}, {2,1,5}, {2,1,5}, {2,1,5},		//0010 1xxx	
	{2,1,5}, {2,1,5}, {2,1,5}, {2,1,5},
	{1,2,5}, {1,2,5}, {1,2,5}, {1,2,5},		//0011 0xxx
	{1,2,5}, {1,2,5}, {1,2,5}, {1,2,5},
	{3,1,5}, {3,1,5}, {3,1,5}, {3,1,5},		//0011 1xxx
	{3,1,5}, {3,1,5}, {3,1,5}, {3,1,5},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},		//010x xxxx
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{1,1,3}, {1,1,3}, {1,1,3}, {1,1,3},
	{64,0,4}, {64,0,4}, {64,0,4}, {64,0,4}, //0110 xxxx		EOB 
	{64,0,4}, {64,0,4}, {64,0,4}, {64,0,4},
	{64,0,4}, {64,0,4}, {64,0,4}, {64,0,4},
	{64,0,4}, {64,0,4}, {64,0,4}, {64,0,4},
	{0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},		//0111 xxxx
	{0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
	{0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
	{0,3,4}, {0,3,4}, {0,3,4}, {0,3,4},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},		//10xx xxxx
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,1,2}, {0,1,2}, {0,1,2}, {0,1,2},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},		//110x xxxx
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,2,3}, {0,2,3}, {0,2,3}, {0,2,3},
	{0,4,5}, {0,4,5}, {0,4,5}, {0,4,5},		//1110 0xxx
	{0,4,5}, {0,4,5}, {0,4,5}, {0,4,5},
	{0,5,5}, {0,5,5}, {0,5,5}, {0,5,5},		//1110 1xxx
	{0,5,5}, {0,5,5}, {0,5,5}, {0,5,5},
	{9,1,7}, {9,1,7},						//1111 000x
	{1,3,7}, {1,3,7},						//1111 001x
	{10,1,7}, {10,1,7},						//1111 010x
	{0,8,7}, {0,8,7},						//1111 011x
	{0,9,7}, {0,9,7},						//1111 100x
	{0,12,8},								//1111 1010
	{0,13,8},								//1111 1011
	{2,3,8},								//1111 1100
	{4,2,8},								//1111 1101
	{0,14,8},								//1111 1110
	{0,15,8}								//1111 1111
};

/* Table B-14, DCT coefficients table zero,
 * codes 0000001000 ... 0000001111
 */
static DCT_COEFFICIENT_TABLE_t DCTtab1[8] =
{
	{16,1,10},									//0000 0010 00
	{5,2,10},									//0000 0010 01
	{0,7,10},									//0000 0010 10
	{2,3,10},									//0000 0010 11
	{1,4,10},									//0000 0011 00
	{15,1,10},									//0000 0011 01
	{14,1,10},									//0000 0011 10
	{4,2,10}									//0000 0011 11
};

/* Table B-15, DCT coefficients table one,
 * codes 000000100x ... 000000111x
 */
static DCT_COEFFICIENT_TABLE_t DCTtab1a[8] =
{
	{5,2,9}, {5,2,9},						//00 0000 100x
	{14,1,9}, {14,1,9},						//00 0000 101x	
	{2,4,10},								//00 0000 1100
	{16,1,10},								//00 0000 1101
	{15,1,9}, {15,1,9}						//00 0000 111x
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 000000010000 ... 000000011111
 */
static DCT_COEFFICIENT_TABLE_t DCTtab2[16] =
{
	{0,11,12},									//0000 0001 0000
	{8,2,12},									//0000 0001 0001
	{4,3,12},									//0000 0001 0010
	{0,10,12},									//0000 0001 0011
	{2,4,12},									//0000 0001 0100
	{7,2,12},									//0000 0001 0101
	{21,1,12},									//0000 0001 0110
	{20,1,12},									//0000 0001 0111
	{0,9,12},									//0000 0001 1000
	{19,1,12},									//0000 0001 1001
	{18,1,12},									//0000 0001 1010
	{1,5,12},									//0000 0001 1011
	{3,3,12},									//0000 0001 1100
	{0,8,12},									//0000 0001 1101
	{6,2,12},									//0000 0001 1110
	{17,1,12}									//0000 0001 1111
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 0000000010000 ... 0000000011111
 */
static DCT_COEFFICIENT_TABLE_t DCTtab3[16] =
{
	{10,2,13},									//0000 0000 1000 0	
	{9,2,13},									//0000 0000 1000 1
	{5,3,13},									//0000 0000 1001 0
	{3,4,13},									//0000 0000 1001 1
	{2,5,13},									//0000 0000 1010 0
	{1,7,13},									//0000 0000 1010 1
	{1,6,13},									//0000 0000 1011 0
	{0,15,13},									//0000 0000 1011 1
	{0,14,13},									//0000 0000 1100 0
	{0,13,13},									//0000 0000 1100 1
	{0,12,13},									//0000 0000 1101 0
	{26,1,13},									//0000 0000 1101 1
	{25,1,13},									//0000 0000 1110 0
	{24,1,13},									//0000 0000 1110 1
	{23,1,13},									//0000 0000 1111 0
	{22,1,13}									//0000 0000 1111 1
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 00000000010000 ... 00000000011111
 */
static DCT_COEFFICIENT_TABLE_t DCTtab4[16] =
{
	{0,31,14},									//0000 0000 0100 00
	{0,30,14},									//0000 0000 0100 01
	{0,29,14},									//0000 0000 0100 10
	{0,28,14},									//0000 0000 0100 11
	{0,27,14},									//0000 0000 0101 00
	{0,26,14}, 									//0000 0000 0101 01
	{0,25,14}, 									//0000 0000 0101 10
	{0,24,14},									//0000 0000 0101 11
	{0,23,14}, 									//0000 0000 0110 00
	{0,22,14}, 									//0000 0000 0110 01
	{0,21,14}, 									//0000 0000 0110 10
	{0,20,14},									//0000 0000 0110 11
	{0,19,14}, 									//0000 0000 0111 00
	{0,18,14}, 									//0000 0000 0111 01
	{0,17,14}, 									//0000 0000 0111 10
	{0,16,14}									//0000 0000 0111 11
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 000000000010000 ... 000000000011111
 */
static DCT_COEFFICIENT_TABLE_t DCTtab5[16] =
{
	{0,40,15},									//0000 0000 0010 000
	{0,39,15},									//0000 0000 0010 001
	{0,38,15},									//0000 0000 0010 010
	{0,37,15},									//0000 0000 0010 011
	{0,36,15},									//0000 0000 0010 100
	{0,35,15},									//0000 0000 0010 101
	{0,34,15},									//0000 0000 0010 110
	{0,33,15},									//0000 0000 0010 111			
	{0,32,15},									//0000 0000 0011 000
	{1,14,15},									//0000 0000 0011 001									
	{1,13,15}, 									//0000 0000 0011 010
	{1,12,15},									//0000 0000 0011 011
	{1,11,15}, 									//0000 0000 0011 100
	{1,10,15}, 									//0000 0000 0011 101
	{1,9,15}, 									//0000 0000 0011 110
	{1,8,15}									//0000 0000 0011 111
};

/* Table B-14/15, DCT coefficients table zero / one,
 * codes 0000000000010000 ... 0000000000011111
 */
static DCT_COEFFICIENT_TABLE_t DCTtab6[16] =
{
	{1,18,16}, 									//0000 0000 0001 0000
	{1,17,16},  								//0000 0000 0001 0001
	{1,16,16}, 									//0000 0000 0001 0010 
	{1,15,16}, 									//0000 0000 0001 0011
	{6,3,16},  									//0000 0000 0001 0100
	{16,2,16}, 									//0000 0000 0001 0101
	{15,2,16}, 									//0000 0000 0001 0110 
	{14,2,16}, 									//0000 0000 0001 0111
	{13,2,16}, 									//0000 0000 0001 1000 
	{12,2,16}, 									//0000 0000 0001 1001 
	{11,2,16}, 									//0000 0000 0001 1010 
	{31,1,16}, 									//0000 0000 0001 1011
	{30,1,16}, 									//0000 0000 0001 1100 
	{29,1,16}, 									//0000 0000 0001 1101 
	{28,1,16}, 									//0000 0000 0001 1110 
	{27,1,16} 									//0000 0000 0001 1111
};

static VLC_TABLE_t macroblock_address_increment_tab1[] =
{
	{ 1, 8}, { 1, 8}, { 1, 8}, { 1, 8}, { 1, 8}, { 1, 8},//0000 0000 ~ 0000 0101
	{15, 8},											//0000 0110
	{14, 8},											//0000 0111
	{13, 8},											//0000 1000
	{12, 8},											//0000 1001
	{11, 8},											//0000 1010
	{10, 8},											//0000 1011
	{ 9, 7}, { 9, 7},									//0000 1100 ~ 0000 1101		
	{ 8, 7}, { 8, 7},									//0000 1110 ~ 0000 1111
	{ 7, 5}, { 7, 5}, { 7, 5}, { 7, 5},					//0001 0000 ~ 0001 0011
	{ 7, 5}, { 7, 5}, { 7, 5}, { 7, 5},					//0001 0100 ~ 0001 0111
	{ 6, 5}, { 6, 5}, { 6, 5}, { 6, 5},					//0001 1000 ~ 0001 1011
	{ 6, 5}, { 6, 5}, { 6, 5}, { 6, 5},					//0001 1100 ~ 0001 1111
	{ 5, 4}, { 5, 4}, { 5, 4}, { 5, 4},					//0010 0000 ~ 0010 0011
	{ 5, 4}, { 5, 4}, { 5, 4}, { 5, 4},					//0010 0100 ~ 0010 0111
	{ 5, 4}, { 5, 4}, { 5, 4}, { 5, 4},					//0010 1000 ~ 0010 1011
	{ 5, 4}, { 5, 4}, { 5, 4}, { 5, 4},					//0010 1100 ~ 0010 1111
	{ 4, 4}, { 4, 4}, { 4, 4}, { 4, 4},					//0011 0000 ~ 0011 0011
	{ 4, 4}, { 4, 4}, { 4, 4}, { 4, 4},					//0011 0100 ~ 0011 0111
	{ 4, 4}, { 4, 4}, { 4, 4}, { 4, 4},					//0011 1000 ~ 0011 1011
	{ 4, 4}, { 4, 4}, { 4, 4}, { 4, 4},					//0011 1100 ~ 0011 1111
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0100 0000 ~ 0100 0011
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0100 0100 ~ 0100 0111
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0100 1000 ~ 0100 1011
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0100 1100 ~ 0100 1111
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0101 0000 ~ 0101 0011
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0101 0100 ~ 0101 0111
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0101 1000 ~ 0101 1011
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},					//0101 1100 ~ 0101 1111
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0110 0000 ~ 0110 0011
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0110 0100 ~ 0110 0111
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0110 1000 ~ 0110 1011
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0110 1100 ~ 0110 1111
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0111 0000 ~ 0111 0011
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0111 0100 ~ 0111 0111
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0111 1000 ~ 0111 1011
	{ 2, 3}, { 2, 3}, { 2, 3}, { 2, 3},					//0111 1100 ~ 0111 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1000 0000 ~ 1000 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1000 0100 ~ 1000 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1000 1000 ~ 1000 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1000 1100 ~ 1000 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1001 0000 ~ 1001 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1001 0100 ~ 1001 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1001 1000 ~ 1001 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1001 1100 ~ 1001 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1010 0000 ~ 1010 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1010 0100 ~ 1010 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1010 1000 ~ 1010 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1010 1100 ~ 1010 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1011 0000 ~ 1011 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1011 0100 ~ 1011 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1011 1000 ~ 1011 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1011 1100 ~ 1011 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1100 0000 ~ 1100 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1100 0100 ~ 1100 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1100 1000 ~ 1100 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1100 1100 ~ 1100 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1101 0000 ~ 1101 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1101 0100 ~ 1101 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1101 1000 ~ 1101 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1101 1100 ~ 1101 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1110 0000 ~ 1110 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1110 0100 ~ 1110 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1110 1000 ~ 1110 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1110 1100 ~ 1110 1111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1111 0000 ~ 1111 0011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1111 0100 ~ 1111 0111
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1111 1000 ~ 1111 1011
	{ 1, 1}, { 1, 1}, { 1, 1}, { 1, 1},					//1111 1100 ~ 1111 1111
};

static VLC_TABLE_t macroblock_address_increment_tab2[] =
{
	{33, 11}, {33, 11}, {33, 11}, {33, 11},				//0000 0000 000 ~ 0000 0000 011 
	{33, 11}, {33, 11}, {33, 11}, {33, 11},				//0000 0000 100 ~ 0000 0000 111 
	{33, 11}, {33, 11}, {33, 11}, {33, 11},				//0000 0001 000 ~ 0000 0001 011 
	{33, 11}, {33, 11}, {33, 11}, {33, 11},				//0000 0001 100 ~ 0000 0001 111 
	{33, 11}, {33, 11}, {33, 11}, {33, 11},				//0000 0010 000 ~ 0000 0010 011 
	{33, 11}, {33, 11}, {33, 11}, {33, 11},				//0000 0010 100 ~ 0000 0010 111 
	{33, 11},											//0000 0011 000
	{32, 11},											//0000 0011 001
	{31, 11},											//0000 0011 010
	{30, 11},											//0000 0011 011
	{29, 11},											//0000 0011 100
	{28, 11},											//0000 0011 101
	{27, 11},											//0000 0011 110
	{26, 11},											//0000 0011 111
	{25, 11},											//0000 0100 000
	{24, 11},											//0000 0100 001
	{23, 11},											//0000 0100 010
	{22, 11},											//0000 0100 011
	{21, 10}, {21, 10},									//0000 0100 100 ~ 0000 0100 101
	{20, 10}, {20, 10},									//0000 0100 110 ~ 0000 0100 111
	{19, 10}, {19, 10},									//0000 0101 000 ~ 0000 0101 001
	{18, 10}, {18, 10},									//0000 0101 010 ~ 0000 0101 011
	{17, 10}, {17, 10},									//0000 0101 100 ~ 0000 0101 101
	{16, 10}, {16, 10},									//0000 0101 110 ~ 0000 0101 111
};

static VLC_TABLE_t macroblock_type_tab1[] =				//I frame
{
	{0xff, 2},											//00
	{0x11, 2},											//01
	{0x01, 1}, {0x01, 1},								//10 ~ 11
};

static VLC_TABLE_t macroblock_type_tab2[] =				//P frame
{
	{0xff, 6},											//0000 00
	{0x11, 6},											//0000 01
	{0x12, 5}, {0x12, 5},								//0000 10 ~ 0000 11
	{0x1a, 5}, {0x1a, 5},								//0001 00 ~ 0001 01
	{0x01, 5}, {0x01, 5},								//0001 10 ~ 0001 11
	{0x08, 3}, {0x08, 3}, {0x08, 3}, {0x08, 3},			//0010 00 ~ 0010 11
	{0x08, 3}, {0x08, 3}, {0x08, 3}, {0x08, 3},			//0011 00 ~ 0011 11
	{0x02, 2}, {0x02, 2}, {0x02, 2}, {0x02, 2},			//0100 00 ~ 0100 11
	{0x02, 2}, {0x02, 2}, {0x02, 2}, {0x02, 2},			//0101 00 ~ 0101 11
	{0x02, 2}, {0x02, 2}, {0x02, 2}, {0x02, 2},			//0110 00 ~ 0110 11
	{0x02, 2}, {0x02, 2}, {0x02, 2}, {0x02, 2},			//0111 00 ~ 0111 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1000 00 ~ 1000 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1001 00 ~ 1001 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1010 00 ~ 1010 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1011 00 ~ 1011 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1100 00 ~ 1100 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1101 00 ~ 1101 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1110 00 ~ 1110 11
	{0x0a, 1}, {0x0a, 1}, {0x0a, 1}, {0x0a, 1},			//1111 00 ~ 1111 11
};

static VLC_TABLE_t macroblock_type_tab3[] =				//B frame
{
	{0xff, 6},											//0000 00
	{0x11, 6},											//0000 01
	{0x16, 6},											//0000 10
	{0x1a, 6},											//0000 11
	{0x1e, 5}, {0x1e, 5},								//0001 00 ~ 0001 01
	{0x01, 5}, {0x01, 5},								//0001 10 ~ 0001 11
	{0x08, 4}, {0x08, 4}, {0x08, 4}, {0x08, 4},			//0010 00 ~ 0010 11
	{0x0a, 4}, {0x0a, 4}, {0x0a, 4}, {0x0a, 4},			//0011 00 ~ 0011 11
	{0x04, 3}, {0x04, 3}, {0x04, 3}, {0x04, 3},			//0100 00 ~ 0100 11
	{0x04, 3}, {0x04, 3}, {0x04, 3}, {0x04, 3},			//0101 00 ~ 0101 11
	{0x06, 3}, {0x06, 3}, {0x06, 3}, {0x06, 3},			//0110 00 ~ 0110 11
	{0x06, 3}, {0x06, 3}, {0x06, 3}, {0x06, 3},			//0111 00 ~ 0111 11
	{0x0c, 2}, {0x0c, 2}, {0x0c, 2}, {0x0c, 2},			//1000 00 ~ 1000 11
	{0x0c, 2}, {0x0c, 2}, {0x0c, 2}, {0x0c, 2},			//1001 00 ~ 1001 11
	{0x0c, 2}, {0x0c, 2}, {0x0c, 2}, {0x0c, 2},			//1010 00 ~ 1010 11
	{0x0c, 2}, {0x0c, 2}, {0x0c, 2}, {0x0c, 2},			//1011 00 ~ 1011 11
	{0x0e, 2}, {0x0e, 2}, {0x0e, 2}, {0x0e, 2},			//1100 00 ~ 1100 11
	{0x0e, 2}, {0x0e, 2}, {0x0e, 2}, {0x0e, 2},			//1101 00 ~ 1101 11
	{0x0e, 2}, {0x0e, 2}, {0x0e, 2}, {0x0e, 2},			//1110 00 ~ 1110 11
	{0x0e, 2}, {0x0e, 2}, {0x0e, 2}, {0x0e, 2},			//1111 00 ~ 1111 11
};

static VLC_TABLE_t	coded_block_pattern_tab1[] =
{
	{63, 6},								//0011 00
	{ 3, 6},								//0011 01
	{36, 6},								//0011 10
	{24, 6},								//0011 11
	{62, 5}, {62, 5},						//0100 0x
	{ 2, 5}, { 2, 5},						//0100 1x
	{61, 5}, {61, 5},						//0101 0x
	{ 1, 5}, { 1, 5},						//0101 1x
	{56, 5}, {56, 5},						//0110 0x
	{52, 5}, {52, 5},						//0110 1x
	{44, 5}, {44, 5},						//0111 0x
	{28, 5}, {28, 5},						//0111 1x
	{40, 5}, {40, 5},						//1000 0x
	{20, 5}, {20, 5},						//1000 1x
	{48, 5}, {48, 5},						//1001 0x
	{12, 5}, {12, 5},						//1001 1x
	{32, 4}, {32, 4}, {32, 4}, {32, 4},		//1010 xx
	{16, 4}, {16, 4}, {16, 4}, {16, 4},		//1011 xx
	{ 8, 4}, { 8, 4}, { 8, 4}, { 8, 4},		//1100 xx
	{ 4, 4}, { 4, 4}, { 4, 4}, { 4, 4},		//1101 xx
	{60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3}, {60, 3},		//111x xx
};

static VLC_TABLE_t	coded_block_pattern_tab2[] =
{
	{34, 7},										//0010 000
	{18, 7},										//0010 001
	{10, 7},										//0010 010
	{ 6, 7},										//0010 011
	{33, 7},										//0010 100
	{17, 7},										//0010 101
	{ 9, 7},										//0010 110
	{ 5, 7},										//0010 111
};

static VLC_TABLE_t	coded_block_pattern_tab3[] =
{
	{58, 8},										//0000 0100
	{54, 8},										//0000 0101
	{46, 8},										//0000 0110
	{30, 8},										//0000 0111
	{57, 8},										//0000 1000
	{53, 8},										//0000 1001
	{45, 8},										//0000 1010
	{29, 8},										//0000 1011
	{38, 8},										//0000 1100
	{26, 8},										//0000 1101
	{37, 8},										//0000 1110
	{25, 8},										//0000 1111
	{43, 8},										//0001 0000
	{23, 8},										//0001 0001
	{51, 8},										//0001 0010
	{15, 8},										//0001 0011
	{42, 8},										//0001 0100
	{22, 8},										//0001 0101
	{50, 8},										//0001 0110
	{14, 8},										//0001 0111
	{41, 8},										//0001 1000
	{21, 8},										//0001 1001
	{49, 8},										//0001 1010
	{13, 8},										//0001 1011
	{35, 8},										//0001 1100
	{19, 8},										//0001 1101
	{11, 8},										//0001 1110
	{ 7, 8},										//0001 1111
};

static VLC_TABLE_t	coded_block_pattern_tab4[] =
{
	{-1, 0},										//0000 0000 0
	{ 0, 9},										//0000 0000 1		not used in 4:2:0 mode
	{39, 9},										//0000 0001 0
	{27, 9},										//0000 0001 1
	{59, 9},										//0000 0010 0
	{55, 9},										//0000 0010 1
	{47, 9},										//0000 0011 0
	{31, 9},										//0000 0011 1
};

static VLC_TABLE_t	dct_dc_size_luminance_tab1[] =
{
	{ 1, 2}, { 1, 2}, { 1, 2}, { 1, 2},				//0000 0 ~ 0001 1
	{ 1, 2}, { 1, 2}, { 1, 2}, { 1, 2},				//0010 0 ~ 0011 1
	{ 2, 2}, { 2, 2}, { 2, 2}, { 2, 2},				//0100 0 ~ 0101 1
	{ 2, 2}, { 2, 2}, { 2, 2}, { 2, 2},				//0110 0 ~ 0111 1
	{ 0, 3}, { 0, 3}, { 0, 3}, { 0, 3},				//1000 0 ~ 1001 1
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},				//1010 0 ~ 1011 1
	{ 4, 3}, { 4, 3}, { 4, 3}, { 4, 3},				//1100 0 ~ 1101 1
	{ 5, 4}, { 5, 4},								//1110 0 ~ 1110 1
	{ 6, 5},										//1111 0
};

static VLC_TABLE_t	dct_dc_size_luminance_tab2[] =
{
	{ 7, 6}, { 7, 6}, { 7, 6}, { 7, 6},				//1111 1000 0 ~ 1111 1001 1
	{ 7, 6}, { 7, 6}, { 7, 6}, { 7, 6},				//1111 1010 0 ~ 1111 1011 1
	{ 8, 7}, { 8, 7}, { 8, 7}, { 8, 7},				//1111 1100 0 ~ 1111 1101 1
	{ 9, 8}, { 9, 8},								//1111 1110 0 ~ 1111 1110 1
	{10, 9},										//1111 1111 0 
	{11, 9},										//1111 1111 1 
};

static VLC_TABLE_t	dct_dc_size_chrominance_tab1[] =
{
	{ 0, 2}, { 0, 2}, { 0, 2}, { 0, 2},				//0000 0 ~ 0001 1
	{ 0, 2}, { 0, 2}, { 0, 2}, { 0, 2},				//0010 0 ~ 0011 1
	{ 1, 2}, { 1, 2}, { 1, 2}, { 1, 2},				//0100 0 ~ 0101 1
	{ 1, 2}, { 1, 2}, { 1, 2}, { 1, 2},				//0110 0 ~ 0111 1
	{ 2, 2}, { 2, 2}, { 2, 2}, { 2, 2},				//1000 0 ~ 1001 1
	{ 2, 2}, { 2, 2}, { 2, 2}, { 2, 2},				//1010 0 ~ 1011 1
	{ 3, 3}, { 3, 3}, { 3, 3}, { 3, 3},				//1100 0 ~ 1101 1
	{ 4, 4}, { 4, 4},								//1110 0 ~ 1110 1
	{ 5, 5},										//1111 0
};

static VLC_TABLE_t	dct_dc_size_chrominance_tab2[] =
{
	{ 6, 6}, { 6, 6}, { 6, 6}, { 6, 6},				//1111 1000 00 ~ 1111 1000 11
	{ 6, 6}, { 6, 6}, { 6, 6}, { 6, 6},				//1111 1001 00 ~ 1111 1001 11
	{ 6, 6}, { 6, 6}, { 6, 6}, { 6, 6},				//1111 1010 00 ~ 1111 1010 11
	{ 6, 6}, { 6, 6}, { 6, 6}, { 6, 6},				//1111 1011 00 ~ 1111 1011 11
	{ 7, 7}, { 7, 7}, { 7, 7}, { 7, 7},				//1111 1100 00 ~ 1111 1100 11
	{ 7, 7}, { 7, 7}, { 7, 7}, { 7, 7},				//1111 1101 00 ~ 1111 1101 11
	{ 8, 8}, { 8, 8}, { 8, 8}, { 8, 8},				//1111 1110 00 ~ 1111 1110 11
	{ 9, 9}, { 9, 9},								//1111 1111 00 ~ 1111 1111 01 
	{10,10},										//1111 1111 10 
	{11,10},										//1111 1111 11 
};

static uint8_t scan[2][8][8] = {
							{
								{ 0,  1,  5,  6, 14, 15, 27, 28},
								{ 2,  4,  7, 13, 16, 26, 29, 42},
								{ 3,  8, 12, 17, 25, 30, 41, 43},
								{ 9, 11, 18, 24, 31, 40, 44, 53},
								{10, 19, 23, 32, 39, 45, 52, 54},
								{20, 22, 33, 38, 46, 51, 55, 60},
								{21, 34, 37, 47, 50, 56, 57, 61},
								{35, 36, 48, 49, 57, 58, 62, 63}
							},
							{
								{ 0,  4,  6, 20, 22, 36, 38, 52},
								{ 1,  5,  7, 21, 23, 37, 39, 53},
								{ 2,  8, 19, 24, 34, 40, 50, 54},
								{ 3,  9, 18, 25, 35, 41, 51, 55},
								{10, 17, 26, 30, 42, 46, 56, 60},
								{11, 16, 27, 31, 43, 47, 57, 61},
								{12, 15, 28, 32, 44, 48, 58, 62},
								{13, 14, 29, 33, 45, 49, 59, 63}
							}
						};

static S16 DW[4][8][8] = {
							{
								{ 8, 16, 19, 22, 26, 27, 29, 34},
								{16, 16, 22, 24, 27, 29, 34, 37},
								{19, 22, 26, 27, 29, 34, 34, 38},
								{22, 22, 26, 27, 29, 34, 37, 40},
								{22, 26, 27, 29, 32, 35, 40, 48},
								{26, 27, 29, 32, 35, 40, 48, 58},
								{26, 27, 29, 34, 38, 46, 56, 69},
								{27, 29, 35, 38, 46, 56, 69, 83}
							},
							{
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16}
							},
							{
								{ 8, 16, 19, 22, 26, 27, 29, 34},
								{16, 16, 22, 24, 27, 29, 34, 37},
								{19, 22, 26, 27, 29, 34, 34, 38},
								{22, 22, 26, 27, 29, 34, 37, 40},
								{22, 26, 27, 29, 32, 35, 40, 48},
								{26, 27, 29, 32, 35, 40, 48, 58},
								{26, 27, 29, 34, 38, 46, 56, 69},
								{27, 29, 35, 38, 46, 56, 69, 83}
							},
							{
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16},
								{16, 16, 16, 16, 16, 16, 16, 16}
							}
						};

static int LUT_dc_dct_pred[4] = {128, 256, 512, 1024};
static int LUT_intra_dc_multi[4] = {8, 4, 2, 1};
static int LUT_cc[12] = {0, 0, 0, 0, 1, 2, 1, 2, 1, 2, 1, 2};

static int LUT_quantiser_scale[2][32] =
{
	{0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62},
	{0, 1, 2, 3, 4,  5,  6,  7,  8, 10, 12, 14, 16, 18, 20, 22, 24, 28, 32, 36, 40, 44, 48, 52, 56, 64, 72, 80, 88, 96, 104, 112}
};

#endif
