#ifndef _MPEG2_PSISI_SECTION_XML_H_
#define _MPEG2_PSISI_SECTION_XML_H_

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

#include "translate_layer/MPEG2_DVB_Section/Include/Mpeg2_PSI_section.h"
#include "thirdparty_AL/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB int MPEG2_PSI_PAT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, program_association_section_t* ppat_section);
XML_MPEG_DVB_LIB	int MPEG2_PSI_PMT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, TS_program_map_section_t* ppmt_section);
XML_MPEG_DVB_LIB	int	MPEG2_PSI_CAT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, CA_section_t* pcat_section);
XML_MPEG_DVB_LIB int MPEG2_PSI_TSDT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, TS_description_section_t* ptsdt_ection);

XML_MPEG_DVB_LIB int MPEG2_PSI_PAT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, program_association_section_t* pPATSection = NULL);
XML_MPEG_DVB_LIB	int MPEG2_PSI_PMT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, TS_program_map_section_t* pPMTSection = NULL);
XML_MPEG_DVB_LIB	int	MPEG2_PSI_CAT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, CA_section_t* pCATSection = NULL);
XML_MPEG_DVB_LIB int MPEG2_PSI_TSDT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, TS_description_section_t* pTSDTSection = NULL);

#endif
