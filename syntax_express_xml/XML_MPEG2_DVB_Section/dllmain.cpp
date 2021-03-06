// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _DEBUG

#ifdef _WIN64
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_SectionD(x64).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2D(x64).lib")
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXMLD(x64).lib")
//#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x64).lib")
#else
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_SectionD(x32).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2D(x32).lib")
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXMLD(x32).lib")
//#pragma comment(lib, "libs_Utilities/lib/XStream_UtilitiesD(x32).lib")
#endif

#else

#ifdef _WIN64
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_Section(x64).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2(x64).lib")
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXML(x64).lib")
//#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x64).lib")
#else
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_Section(x32).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2(x32).lib")
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXML(x32).lib")
//#pragma comment(lib, "libs_Utilities/lib/XStream_Utilities(x32).lib")
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

