#ifndef _AVS_VIDEO_SEQUENCE_XML_H_
#define _AVS_VIDEO_SEQUENCE_XML_H_

#include <stdint.h>

#include "../../compile.h"
#include "HAL\HAL_XML\Include\HALForTinyXML2Doc.h"

_CDL_EXPORT int avs_decode_unaligned_nal_to_xml(uint8_t *nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc);
_CDL_EXPORT int avs_decode_unknown_nal_to_xml(uint8_t *nal_buf, int nal_length, XMLDocForMpegSyntax* pxmlDoc);


#endif

