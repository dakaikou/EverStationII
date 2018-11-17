#ifndef _DVB_MHP_SECTION_XML_H_
#define _DVB_MHP_SECTION_XML_H_

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

#include "translate_layer/MPEG2_DVB_Section/Include/DVB_MHP_section.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB	int	DVB_MHP_AIT_DecodeSection_to_XML(uint8_t *section_buf, int section_length, TALForXMLDoc* pxmlDoc, application_information_section_t* pait_section = NULL);
XML_MPEG_DVB_LIB	int DVB_MHP_AIT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, application_information_section_t* pait_section);

#endif
