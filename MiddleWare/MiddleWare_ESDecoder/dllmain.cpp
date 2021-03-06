// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "thirdparty_abstract_layer/TALForDirectX/lib/TALForDirectXD(x64).lib") 
#pragma comment(lib, "syntax_translate_layer/ES_MPV/lib/ES_MPVD(x64).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg2_TSPacket/lib/Mpeg2_TSPacketD(x64).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg_PESPacket/lib/Mpeg_PESPacketD(x64).lib") 
#pragma comment(lib, "Utilities/Graphics/lib/GraphicsD(x64).lib") 
#else
#pragma comment(lib, "thirdparty_abstract_layer/TALForDirectX/lib/TALForDirectXD(x32).lib") 
#pragma comment(lib, "syntax_translate_layer/ES_MPV/lib/ES_MPVD(x32).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg2_TSPacket/lib/Mpeg2_TSPacketD(x32).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg_PESPacket/lib/Mpeg_PESPacketD(x32).lib") 
#pragma comment(lib, "Utilities/Graphics/lib/GraphicsD(x32).lib") 
#endif
#else
#ifdef _WIN64
#pragma comment(lib, "thirdparty_abstract_layer/TALForDirectX/lib/TALForDirectX(x64).lib") 
#pragma comment(lib, "syntax_translate_layer/ES_MPV/lib/ES_MPV(x64).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg2_TSPacket/lib/Mpeg2_TSPacket(x64).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg_PESPacket/lib/Mpeg_PESPacket(x64).lib") 
#pragma comment(lib, "Utilities/Graphics/lib/Graphics(x64).lib") 
#else
#pragma comment(lib, "thirdparty_abstract_layer/TALForDirectX/lib/TALForDirectX(x32).lib") 
#pragma comment(lib, "syntax_translate_layer/ES_MPV/lib/ES_MPV(x32).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg2_TSPacket/lib/Mpeg2_TSPacket(x32).lib") 
#pragma comment(lib, "syntax_translate_layer/Mpeg_PESPacket/lib/Mpeg_PESPacket(x32).lib") 
#pragma comment(lib, "Utilities/Graphics/lib/Graphics(x32).lib") 
#endif
#endif

//#pragma comment(lib, "thirdparty_libs/directx/libs/ddraw.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/dsound.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/dxguid.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/quartz.lib") 
//#pragma comment(lib, "thirdparty_libs/directx/libs/strmiids.lib") 

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

