#ifndef _DVB_SI_SECTION_XML_H_
#define _DVB_SI_SECTION_XML_H_

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

#include "syntax_translate_layer/MPEG2_DVB_Section/Include/DVB_SI_section.h"
#include "thirdparty_abstract_layer/TALForXML/Include/TALForXML.h"

XML_MPEG_DVB_LIB	int	DVB_SI_NIT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, network_information_section_t* pnit_section);
XML_MPEG_DVB_LIB int	DVB_SI_BAT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, bouquet_association_section_t* pbat_section);
XML_MPEG_DVB_LIB	int	DVB_SI_SDT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, service_description_section_t* psdt_section);
XML_MPEG_DVB_LIB	int	DVB_SI_EIT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, event_information_section_t* peit_section);
XML_MPEG_DVB_LIB	int	DVB_SI_RST_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, running_status_section_t* prst_ection);

XML_MPEG_DVB_LIB	int	DVB_SI_TDT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, time_date_section_t* ptdt_section);
XML_MPEG_DVB_LIB	int	DVB_SI_TOT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, time_offset_section_t* ptot_section);
XML_MPEG_DVB_LIB	int	DVB_SI_CMT_PresentSection_to_XML(TALForXMLDoc* pxmlDoc, CA_message_section_t* pcmt_ection);

XML_MPEG_DVB_LIB	int	DVB_SI_NIT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, network_information_section_t* pNITSection = NULL);
XML_MPEG_DVB_LIB int	DVB_SI_BAT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, bouquet_association_section_t* pBATSection = NULL);
XML_MPEG_DVB_LIB	int	DVB_SI_SDT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, service_description_section_t* pSDTSection = NULL);
XML_MPEG_DVB_LIB	int	DVB_SI_EIT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, event_information_section_t* pEITSection = NULL);
XML_MPEG_DVB_LIB	int	DVB_SI_RST_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, running_status_section_t* pRSTSection = NULL);

XML_MPEG_DVB_LIB	int	DVB_SI_TDT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, time_date_section_t* pTDTSection = NULL);
XML_MPEG_DVB_LIB	int	DVB_SI_TOT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, time_offset_section_t* pTOTSection = NULL);
XML_MPEG_DVB_LIB	int	DVB_SI_CMT_DecodeSection_to_XML(uint8_t *buf, int length, TALForXMLDoc* pxmlDoc, CA_message_section_t* pCMTSection = NULL);

#endif
