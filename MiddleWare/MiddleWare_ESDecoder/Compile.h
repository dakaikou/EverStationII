#ifndef _ESDECODER_COMPILE_H_
#define _ESDECODER_COMPILE_H_

#ifndef _CDLLIB_INLINE
#define _CDLLIB_INLINE inline
#endif

#ifndef _CDL_EXT_CLASS
#ifdef _USRDLL
#define _CDL_EXT_CLASS __declspec(dllexport) // Always export CJLibrary files...
#else
#define _CDL_EXT_CLASS	// not used with static builds.
#endif
#endif

#ifndef _CDL_EXPORT
#ifdef _USRDLL
#define _CDL_EXPORT	__declspec(dllexport) // Always export CJLibrary files...
#else
#define _CDL_EXPORT	// not used with static builds.
#endif
#endif


#endif

