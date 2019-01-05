#ifndef _MPEG_DSMCC_SECTION_XML_H_
#define _MPEG_DSMCC_SECTION_XML_H_

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

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/Mpeg2_DSMCC_section.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB	int MPEG2_DSMCC_DecodeSection_to_XML(uint8_t *section_buf, int section_size, TALForXMLDoc* pxmlDoc, dsmcc_section_t* pDSMCCSection = NULL);
XML_MPEG_DVB_LIB	int MPEG2_DSMCC_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, dsmcc_section_t* pdsmcc_section);


#endif
