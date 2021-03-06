// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _WIN64

#ifdef _DEBUG
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXMLD(x64).lib")

#pragma comment(lib, "syntax_translate_layer/MPEG2_TSPacket/lib/MPEG2_TSPacketD(x64).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG_PESPacket/lib/MPEG_PESPacketD(x64).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_SectionD(x64).lib")
#pragma comment(lib, "syntax_express_xml/XML_MPEG2_DVB_Section/lib/XML_MPEG2_DVB_SectionD(x64).lib")
#pragma comment(lib, "Utilities/Directory/lib/DirectoryD(x64).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTableD(x64).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/sqlite3D(x64).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2D(x64).lib")

#else
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXML(x64).lib")

#pragma comment(lib, "syntax_translate_layer/MPEG2_TSPacket/lib/MPEG2_TSPacket(x64).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG_PESPacket/lib/MPEG_PESPacket(x64).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_Section(x64).lib")
#pragma comment(lib, "syntax_express_xml/XML_MPEG2_DVB_Section/lib/XML_MPEG2_DVB_Section(x64).lib")
#pragma comment(lib, "Utilities/Directory/lib/Directory(x64).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTable(x64).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/sqlite3(x64).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2(x64).lib")

#endif

#else

#ifdef _DEBUG
#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXMLD(x32).lib")

#pragma comment(lib, "syntax_translate_layer/MPEG2_TSPacket/lib/MPEG2_TSPacketD(x32).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG_PESPacket/lib/MPEG_PESPacketD(x32).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_SectionD(x32).lib")
#pragma comment(lib, "syntax_express_xml/XML_MPEG2_DVB_Section/lib/XML_MPEG2_DVB_SectionD(x32).lib")
#pragma comment(lib, "Utilities/Directory/lib/DirectoryD(x32).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTableD(x32).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/sqlite3D(x32).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2D(x32).lib")

#else

#pragma comment(lib, "thirdparty_abstract_layer/TALForXML/lib/TALForXML(x32).lib")

#pragma comment(lib, "syntax_translate_layer/MPEG2_TSPacket/lib/MPEG2_TSPacket(x32).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG_PESPacket/lib/MPEG_PESPacket(x32).lib")
#pragma comment(lib, "syntax_translate_layer/MPEG2_DVB_Section/lib/MPEG2_DVB_Section(x32).lib")
#pragma comment(lib, "syntax_express_xml/XML_MPEG2_DVB_Section/lib/XML_MPEG2_DVB_Section(x32).lib")
#pragma comment(lib, "Utilities/Directory/lib/Directory(x32).lib")

#pragma comment(lib, "MiddleWare/MiddleWare_PsiSiTable/lib/MiddleWare_PsiSiTable(x32).lib")
#pragma comment(lib, "thirdparty_libs/sqlite3/lib/sqlite3(x32).lib")
#pragma comment(lib, "thirdparty_libs/tinyxml2/lib/tinyxml2(x32).lib")
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

