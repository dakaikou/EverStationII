#ifndef __API_TINYXML2_WRAPPER_H__
#define __API_TINYXML2_WRAPPER_H__

#include <stdint.h>

#include "../Compile.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"
#include "thirdparty_libs/tinyxml2/include/tinyxml2.h"

using namespace tinyxml2;

class _CDL_EXPORT XMLDocForMpegSyntax : public tinyxml2::XMLDocument
{
	friend class XMLElement;

public:
	XMLDocForMpegSyntax(void);
	~XMLDocForMpegSyntax(void);
protected:
	uint8_t* m_origin_ptr;

public:
	XMLElement * NewKeyValuePairElement(XMLElement* pxmlParent, const char* key_name);
	XMLElement * NewKeyValuePairElement(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits = -1, const char* mnemonic=NULL, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);
	XMLElement * NewKeyValuePairElement(XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);
	XMLElement * NewKeyValuePairElement(XMLElement* pxmlParent, const char* key_name, const char* string);

	XMLElement * NewKeyValuePairElementX64Mode(XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);

	XMLElement * NewKeyValuePairElementByteMode(XMLElement* pxmlParent, const char* field_name, unsigned int value = -1, int byte_length = -1, const char* pszComment = NULL, const BYTES_t* pbytes_map = NULL);
	XMLElement * NewKeyValuePairElementByteMode(XMLElement* pxmlParent, const char* field_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL, const BYTES_t* pbytes_map = NULL);

	XMLElement* QueryNodeByKeyName(XMLElement* pxmlParentNode, const char* pszNodeName);

	void SetOrigin(unsigned char* origin_ptr);
	void UpdateBufMark(XMLElement* pNode, uint8_t* start_ptr, uint8_t* end_ptr);

protected:

private:


public:
};


#endif	//__API_TINYXML2_WRAPPER_H__

