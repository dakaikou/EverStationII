#ifndef _DVB_SI_UTILITIES_INNER_H_
#define _DVB_SI_UTILITIES_INNER_H_

#include <stdint.h>

//defined in EN 300 468

//非DLL导出函数，仅限模块内部使用   chendelin 2018.8.26
uint8_t* DVB_SI_StringPrefixTrim(uint8_t* pstr_src);

#endif
