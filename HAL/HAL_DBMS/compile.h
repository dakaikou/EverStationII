#ifndef _HAL_DBMS_WRAPPER_H_
#define _HAL_DBMS_WRAPPER_H_

#ifndef _CDL_EXPORT
#ifdef _USRDLL
#define _CDL_EXPORT	__declspec(dllexport) // Always export CJLibrary files...
#else
#define _CDL_EXPORT	// not used with static builds.
#endif
#endif

#endif

