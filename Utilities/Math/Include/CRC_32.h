#ifndef __CRC32_H__
#define __CRC32_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _TOOL_MATH_EXPORT
#       define TOOL_MATH_LIB __declspec(dllexport)
#   elif defined(_TOOL_MATH_IMPORT)
#       define TOOL_MATH_LIB __declspec(dllimport)
#   else
#       define TOOL_MATH_LIB
#   endif
#elif __GNUC__ >= 4
#   define TOOL_DIRECTORY_LIB __attribute__((visibility("default")))
#else
#   define TOOL_DIRECTORY_LIB
#endif

#include <stdint.h>

TOOL_MATH_LIB int Decode_CRC_32(unsigned char* buf, int length);
TOOL_MATH_LIB int Encode_CRC_32(unsigned char* buf, int length);

#endif

