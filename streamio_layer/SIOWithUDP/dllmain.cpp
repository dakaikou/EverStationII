// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "syntax_translate_layer/INET_IPv4/lib/INET_IPv4D(x64).lib") 
//#pragma comment(lib, "Utilities/Directory/lib/DirectoryD(x64).lib") 
#else
#pragma comment(lib, "syntax_translate_layer/INET_IPv4/lib/INET_IPv4(x64).lib") 
//#pragma comment(lib, "Utilities/Directory/lib/Directory(x64).lib") 
#endif

#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x64/wpcap(x64).lib")

#else

#ifdef _DEBUG
#pragma comment(lib, "syntax_translate_layer/INET_IPv4/lib/INET_IPv4D(x32).lib") 
//#pragma comment(lib, "Utilities/Directory/lib/DirectoryD(x32).lib") 
#else
#pragma comment(lib, "syntax_translate_layer/INET_IPv4/lib/INET_IPv4(x32).lib") 
//#pragma comment(lib, "Utilities/Directory/lib/Directory(x32).lib") 
#endif

#pragma comment(lib, "thirdparty_libs/Winpcap/lib/x32/wpcap(x32).lib")

#endif // _DEBUG


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

