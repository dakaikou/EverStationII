// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _WIN64
#pragma comment(lib, "thirdparty_libs/directx/libs/x64/ddraw.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x64/dsound.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x64/dxguid.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x64/quartz.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x64/strmiids.lib") 
#else
#pragma comment(lib, "thirdparty_libs/directx/libs/x32/ddraw.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x32/dsound.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x32/dxguid.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x32/quartz.lib") 
#pragma comment(lib, "thirdparty_libs/directx/libs/x32/strmiids.lib") 
#endif

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

