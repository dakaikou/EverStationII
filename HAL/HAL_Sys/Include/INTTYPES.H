#ifndef _INTTYPES_H
#define _INTTYPES_H

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
#ifdef ARCH_X86
typedef signed long long int64_t;
#endif

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#ifdef ARCH_X86
typedef unsigned long long uint64_t;
#endif

typedef char    S8;
typedef short   S16;
typedef int     S32;
typedef __int64 S64;

typedef unsigned char    U8;
typedef unsigned short   U16;
typedef unsigned int     U32;
typedef unsigned __int64 U64;

#ifdef TRUE
#undef TRUE
#endif
#define TRUE			 1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE			 0

#ifdef NULL
#undef NULL
#endif
#define NULL			0x00000000

#define UNCREDITABLE_MAX_VALUE				-123456789
#define UNCREDITABLE_MIN_VALUE				123456789

#endif
