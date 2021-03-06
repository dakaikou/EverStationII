// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _DEBUG

#ifdef _WIN64
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/Mpeg2_DVB_SectionD(x64).lib")
#pragma comment(lib, "utilities/Directory/lib/DirectoryD(x64).lib")
#else
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/Mpeg2_DVB_SectionD(x32).lib")
#pragma comment(lib, "utilities/Directory/lib/DirectoryD(x32).lib")
#endif

#else

#ifdef _WIN64
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/Mpeg2_DVB_Section(x64).lib")
#pragma comment(lib, "utilities/Directory/lib/Directory(x64).lib")
#else
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/Mpeg2_DVB_Section(x32).lib")
#pragma comment(lib, "utilities/Directory/lib/Directory(x32).lib")
#endif

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

