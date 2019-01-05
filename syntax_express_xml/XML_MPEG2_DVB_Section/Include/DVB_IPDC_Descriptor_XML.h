#ifndef __DVB_IPDC_DESCRIPTOR_XML_H__
#define __DVB_IPDC_DESCRIPTOR_XML_H__

#include <stdint.h>

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _XML_MPEG_DVB_EXPORT
#       define XML_MPEG_DVB_LIB __declspec(dllexport)
#   elif defined(_XML_MPEG_DVB_IMPORT)
#       define XML_MPEG_DVB_LIB __declspec(dllimport)
#   else
#       define XML_MPEG_DVB_LIB
#   endif
#elif __GNUC__ >= 4
#   define XML_MPEG_DVB_LIB __attribute__((visibility("default")))
#else
#   define XML_MPEG_DVB_LIB
#endif

#include <stdint.h>

#include "translate_layer/MPEG_DVB_Section/Include/DVB_IPDC_Descriptor.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"


#endif