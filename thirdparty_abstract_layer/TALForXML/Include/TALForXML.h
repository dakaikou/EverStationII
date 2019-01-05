#ifndef __HAL_TINYXML2_H__
#define __HAL_TINYXML2_H__

#if defined(_WIN32) || defined(_WIN64)
#   ifdef _TAL_XML_EXPORT
#       define TAL_XML_LIB __declspec(dllexport)
#   elif defined(_TAL_XML_IMPORT)
#       define TAL_XML_LIB __declspec(dllimport)
#   else
#       define TAL_XML_LIB
#   endif
#elif __GNUC__ >= 4
#   define TAL_XML_LIB __attribute__((visibility("default")))
#else
#   define TAL_XML_LIB
#endif

#include <stdint.h>

#include "thirdparty_libs/tinyxml2/include/tinyxml2.h"

#define MAX_TREE_LEVEL			10

typedef struct
{
	int offset;
	int i_left;

	int max_length;

} BITS_TRACER_t;

using namespace tinyxml2;

class TAL_XML_LIB TALForXMLDoc : public tinyxml2::XMLDocument
{
	friend class XMLElement;

public:
	TALForXMLDoc(void);
	~TALForXMLDoc(void);

protected:

	BITS_TRACER_t m_bs_tracer;				//added by chendelin 2018.9.29

public:

	XMLElement * QueryNodeByKeyName(XMLElement* pxmlParentNode, const char* pszNodeName);

	//bitstream position control function
	void SetSyncOffset(int offset);
	//int GetSyncOffset(void);
	//void Align(void);

	//void SetAnchor(XMLElement* pxmlBranchNode);
	//void ClearAnchor(XMLElement* pxmlBranchNode);

	//New branch node
	XMLElement* NewRootElement(const char* key_name, const char* pszComment = NULL, int field_length = -1);
	XMLElement* NewBranchElement(XMLElement* pxmlParent, const char* key_name, const char* pszComment = NULL, int field_length = -1, int sync_offset = -1);

	//New leaf node
	XMLElement * NewElementForString(XMLElement* pxmlParent, const char* key_name, const char* pszComment = NULL);
	XMLElement * NewElementForBits(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL);
	XMLElement * NewElementForByteBuf(XMLElement* pxmlParent, const char* key_name, const uint8_t* byte_buf, int byte_length, const char* pszComment = NULL);
	XMLElement * NewElementForX64Bits(XMLElement* pxmlParent, const char* key_name, const uint64_t key_value, int bits = -1, const char* mnemonic = NULL, const char* pszComment = NULL);
	//XMLElement * NewElementForCharBuf(XMLElement* pxmlParent, const char* key_name, const uint8_t* char_buf, int char_length, const uint8_t* trimmed_char = NULL);

	XMLElement * NewElementForByteMode(XMLElement* pxmlParent, const char* key_name, uint32_t key_value, int byte_length = -1, const char* pszComment = NULL);
};


#endif	//__HAL_TINYXML2_H__

