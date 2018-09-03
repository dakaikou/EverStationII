#ifndef _DVB_IPDC_SECTION_XML_H_
#define _DVB_IPDC_SECTION_XML_H_

#include <stdint.h>

#include "../../compile.h"

#include "../DVB_IPDC_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT	int	DVB_IPDC_MPE_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, datagram_section_t* pMPESection = NULL);
_CDL_EXPORT	int	DVB_IPDC_INT_DecodeSection_to_XML(uint8_t *buf, int length, XMLDocForMpegSyntax* pxmlDoc, IP_MAC_notification_section_t* pINTSection = NULL);

#endif
