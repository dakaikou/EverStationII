// dllmain.cpp : 定义 DLL 应用程序的入口点。
//#include "stdafx.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "tinyxml2/lib/tinyxml2D(x64).lib") 
#else
#pragma comment(lib, "tinyxml2/lib/tinyxml2(x64).lib") 
#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "tinyxml2/lib/tinyxml2D(x32).lib") 
#else
#pragma comment(lib, "tinyxml2/lib/tinyxml2(x32).lib") 
#endif

#endif

//BOOL APIENTRY DllMain( HMODULE hModule,
//                       DWORD  ul_reason_for_call,
//                       LPVOID lpReserved
//                     )
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}

