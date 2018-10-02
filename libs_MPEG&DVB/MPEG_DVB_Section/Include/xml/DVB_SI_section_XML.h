#ifndef _DVB_SI_SECTION_XML_H_
#define _DVB_SI_SECTION_XML_H_

#include "../../compile.h"

#include "../DVB_SI_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT	int	DVB_SI_NIT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, network_information_section_t* pNITSection = NULL);
_CDL_EXPORT int	DVB_SI_BAT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, bouquet_association_section_t* pBATSection = NULL);
_CDL_EXPORT	int	DVB_SI_SDT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, service_description_section_t* pSDTSection = NULL);
_CDL_EXPORT	int	DVB_SI_EIT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, event_information_section_t* pEITSection = NULL);
_CDL_EXPORT	int	DVB_SI_RST_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, running_status_section_t* pRSTSection = NULL);

_CDL_EXPORT	int	DVB_SI_TDT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, time_date_section_t* pTDTSection = NULL);
_CDL_EXPORT	int	DVB_SI_TOT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, time_offset_section_t* pTOTSection = NULL);
_CDL_EXPORT	int	DVB_SI_CMT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, CA_message_section_t* pCMTSection = NULL);

#endif
