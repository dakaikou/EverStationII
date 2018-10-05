#ifndef _MPEG2_PSISI_SECTION_XML_H_
#define _MPEG2_PSISI_SECTION_XML_H_

#include "../compile.h"

#include "../nativeInclude/Mpeg2_PSI_section.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2.h"

_CDL_EXPORT int MPEG2_PSI_PAT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, program_association_section_t* ppat_section);
_CDL_EXPORT	int MPEG2_PSI_PMT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, TS_program_map_section_t* ppmt_section);
_CDL_EXPORT	int	MPEG2_PSI_CAT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, CA_section_t* pcat_section);
_CDL_EXPORT int MPEG2_PSI_TSDT_PresentSection_to_XML(HALForXMLDoc* pxmlDoc, TS_description_section_t* ptsdt_ection);

_CDL_EXPORT int MPEG2_PSI_PAT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, program_association_section_t* pPATSection = NULL);
_CDL_EXPORT	int MPEG2_PSI_PMT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, TS_program_map_section_t* pPMTSection = NULL);
_CDL_EXPORT	int	MPEG2_PSI_CAT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, CA_section_t* pCATSection = NULL);
_CDL_EXPORT int MPEG2_PSI_TSDT_DecodeSection_to_XML(uint8_t *buf, int length, HALForXMLDoc* pxmlDoc, TS_description_section_t* pTSDTSection = NULL);

#endif
