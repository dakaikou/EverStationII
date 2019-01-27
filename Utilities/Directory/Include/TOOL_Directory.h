#ifndef _XSTREAM_TOOLS_DIRECTORY_H_
#define _XSTREAM_TOOLS_DIRECTORY_H_

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _TOOL_DIRECTORY_EXPORT
#       define TOOL_DIRECTORY_LIB __declspec(dllexport)
#   elif defined(_TOOL_DIRECTORY_IMPORT)
#       define TOOL_DIRECTORY_LIB __declspec(dllimport)
#   else
#       define TOOL_DIRECTORY_LIB
#   endif
#elif __GNUC__ >= 4
#   define TOOL_DIRECTORY_LIB __attribute__((visibility("default")))
#else
#   define TOOL_DIRECTORY_LIB
#endif

#include <stdint.h>

//TOOL_DIRECTORY_LIB int DIR_GetModulePathLength(char* pszFile);
TOOL_DIRECTORY_LIB int DIR_BuildDirectory(char* pszFilePath);

#endif
