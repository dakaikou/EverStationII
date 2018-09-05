#ifndef __API_TINYXML2_WRAPPER_H__
#define __API_TINYXML2_WRAPPER_H__

#include <stdint.h>

#include "../Compile.h"
#include "HAL\HAL_BitStream\Include\HALForBitStream.h"
#include "HAL\HAL_ByteStream\Include\HALForByteStream.h"
#include "thirdparty_libs/tinyxml2/include/tinyxml2.h"

class _CDL_EXPORT XMLDocForMpegSyntax : public tinyxml2::XMLDocument
{
	friend class XMLElement;

public:
	XMLDocForMpegSyntax(void);
	~XMLDocForMpegSyntax(void);
protected:
	unsigned char* m_origin_ptr;
	//tinyxml2::XMLElement* m_pActive;

public:
	//tinyxml2::XMLElement * NewKeyValueElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, unsigned int key_value, int bits = -1, const char* key_commnet = NULL);
	//tinyxml2::XMLElement * NewTitleElement(tinyxml2::XMLElement* pxmlParent, const char* title_name);

	tinyxml2::XMLElement * NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name);
	tinyxml2::XMLElement * NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits = -1, const char* mnemonic=NULL, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);
	tinyxml2::XMLElement * NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);
	tinyxml2::XMLElement * NewKeyValuePairElement(tinyxml2::XMLElement* pxmlParent, const char* key_name, const char* string);

	tinyxml2::XMLElement * NewKeyValuePairElementX64Mode(tinyxml2::XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL, const BITS_t* pbits_map = NULL);

	tinyxml2::XMLElement * NewKeyValuePairElementByteMode(tinyxml2::XMLElement* pxmlParent, const char* field_name, unsigned int value = -1, int byte_length = -1, const char* pszComment = NULL, const BYTES_t* pbytes_map = NULL);
	tinyxml2::XMLElement * NewKeyValuePairElementByteMode(tinyxml2::XMLElement* pxmlParent, const char* field_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL, const BYTES_t* pbytes_map = NULL);


	tinyxml2::XMLElement* QueryNodeByKeyName(tinyxml2::XMLElement* pxmlParentNode, const char* pszNodeName);

	void SetOrigin(unsigned char* origin_ptr);

	//void SetActiveNode(tinyxml2::XMLElement* pActive);
	//tinyxml2::XMLElement* GetActiveNode(void);

	void UpdateBufMark(tinyxml2::XMLElement* pNode, uint8_t* start_ptr, uint8_t* end_ptr);

protected:

private:


public:
};


#endif	//__API_TINYXML2_WRAPPER_H__

