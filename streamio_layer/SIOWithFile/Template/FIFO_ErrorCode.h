#ifndef _FIFO_ERROR_H_
#define _FIFO_ERROR_H_

#ifdef FIFO_NO_ERROR
#undef FIFO_NO_ERROR
#endif

#define FIFO_NO_ERROR									0x00000000

//ϵͳ�������
#define FIFO_INVALID_PARAMETER_ERROR	0x00000001
#define FIFO_NO_ENOUGH_MEMORY					0x00000003
#define FIFO_WRITE_OVERFLOW						0x00000004

#define FIFO_UNKNOWN_ERROR						0xFFFFFFFF

#endif