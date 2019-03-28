#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _TOOL_GRAPHICS_EXPORT
#       define TOOL_GRAPHICS_LIB __declspec(dllexport)
#   elif defined(_TOOL_GRAPHICS_IMPORT)
#       define TOOL_GRAPHICS_LIB __declspec(dllimport)
#   else
#       define TOOL_GRAPHICS_LIB
#   endif
#elif __GNUC__ >= 4
#   define TOOL_GRAPHICS_LIB __attribute__((visibility("default")))
#else
#   define TOOL_GRAPHICS_LIB
#endif

#include <stdint.h>

#define YUV2RGB_FLOATING_POINT_CALCULATION			0
//#define YUV2RGB_FOR_PC								1

TOOL_GRAPHICS_LIB int GRAPHICS_yuv2rgb(int colorSpace, uint8_t YD, uint8_t UD, uint8_t VD, uint8_t* pRD, uint8_t* pGD, uint8_t* pBD, int bFullScale = 1);
TOOL_GRAPHICS_LIB int GRAPHICS_PICTURE_Enlarge(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int coeff);
TOOL_GRAPHICS_LIB int GRAPHICS_PICTURE_Reduce(uint8_t* src, int src_w, int src_h, uint8_t* dst, int dst_w, int dst_h, int coeff);

#endif

