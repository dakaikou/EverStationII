#ifndef _DVB_SI_SECTION_XML_H_
#define _DVB_SI_SECTION_XML_H_

#include "../../compile.h"

#include "../DVB_SI_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT	int	DVB_SI_NIT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, network_information_section_t* pnit_section);
_CDL_EXPORT int	DVB_SI_BAT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, bouquet_association_section_t* pbat_section);
_CDL_EXPORT	int	DVB_SI_SDT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, service_description_section_t* psdt_section);
_CDL_EXPORT	int	DVB_SI_EIT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, event_information_section_t* peit_section);
_CDL_EXPORT	int	DVB_SI_RST_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, running_status_section_t* prst_ection);

_CDL_EXPORT	int	DVB_SI_TDT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, time_date_section_t* ptdt_section);
_CDL_EXPORT	int	DVB_SI_TOT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, time_offset_section_t* ptot_section);
_CDL_EXPORT	int	DVB_SI_CMT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, CA_message_section_t* pcmt_ection);

_CDL_EXPORT	int	DVB_SI_NIT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, network_information_section_t* pNITSection = NULL);
_CDL_EXPORT int	DVB_SI_BAT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, bouquet_association_section_t* pBATSection = NULL);
_CDL_EXPORT	int	DVB_SI_SDT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, service_description_section_t* pSDTSection = NULL);
_CDL_EXPORT	int	DVB_SI_EIT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, event_information_section_t* pEITSection = NULL);
_CDL_EXPORT	int	DVB_SI_RST_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, running_status_section_t* pRSTSection = NULL);

_CDL_EXPORT	int	DVB_SI_TDT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, time_date_section_t* pTDTSection = NULL);
_CDL_EXPORT	int	DVB_SI_TOT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, time_offset_section_t* pTOTSection = NULL);
_CDL_EXPORT	int	DVB_SI_CMT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, CA_message_section_t* pCMTSection = NULL);

#endif
