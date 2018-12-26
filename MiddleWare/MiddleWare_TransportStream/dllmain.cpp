// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "streamio_layer/SIOWithDektec/lib/SIOWithDektecD(x64).lib") 
#pragma comment(lib, "streamio_layer/SIOWithUDP/lib/SIOWithUDPD(x64).lib") 
#pragma comment(lib, "streamio_layer/SIOWithFile/lib/SIOWithFileD(x64).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Directory/lib/TOOL_DirectoryD(x64).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Math/lib/TOOL_MathD(x64).lib") 
#else
#pragma comment(lib, "streamio_layer/SIOWithDektec/lib/SIOWithDektec(x64).lib") 
#pragma comment(lib, "streamio_layer/SIOWithUDP/lib/SIOWithUDP(x64).lib") 
#pragma comment(lib, "streamio_layer/SIOWithFile/lib/SIOWithFile(x64).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Directory/lib/TOOL_Directory(x64).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Math/lib/TOOL_Math(x64).lib") 
#endif

#else
#ifdef _DEBUG
#pragma comment(lib, "streamio_layer/SIOWithDektec/lib/SIOWithDektecD(x32).lib") 
#pragma comment(lib, "streamio_layer/SIOWithUDP/lib/SIOWithUDPD(x32).lib") 
#pragma comment(lib, "streamio_layer/SIOWithFile/lib/SIOWithFileD(x32).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Directory/lib/TOOL_DirectoryD(x32).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Math/lib/TOOL_MathD(x32).lib") 
#else
#pragma comment(lib, "streamio_layer/SIOWithDektec/lib/SIOWithDektec(x32).lib") 
#pragma comment(lib, "streamio_layer/SIOWithUDP/lib/SIOWithUDP(x32).lib") 
#pragma comment(lib, "streamio_layer/SIOWithFile/lib/SIOWithFile(x32).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Directory/lib/TOOL_Directory(x32).lib") 
#pragma comment(lib, "toolbox_libs/TOOL_Math/lib/TOOL_Math(x32).lib") 
#endif

#pragma comment(lib, "thirdparty_HW/SmartTS/smartts_api.lib") 

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

