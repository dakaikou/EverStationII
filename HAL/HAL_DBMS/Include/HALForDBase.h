#ifndef __API_TINYXML2_WRAPPER_H__
#define __API_TINYXML2_WRAPPER_H__

#include "../Compile.h"
#include "HAL\HAL_BitStream\Include\bitstream.h"
#include "thirdparty_libs/tinyxml2/include/tinyxml2.h"

class _CDL_EXPORT XMLDocForMpegSyntax : public tinyxml2::XMLDocument
{
	friend class XMLElement;

public:
	XMLDocForMpegSyntax(void);
	~XMLDocForMpegSyntax(void);
protected:
	unsigned char* m_origin_ptr;
	tinyxml2::XMLElement* m_pActive;

public:
	tinyxml2::XMLElement * NewTitleElement(tinyxml2::XMLElement* pxmlParent, const char* name);
	tinyxml2::XMLElement * NewSyntaxElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits=-1, const char* mnemonic=NULL, unsigned int value=-1, const BITS_t* pbits_map = NULL, const char* pszComment = NULL);
	tinyxml2::XMLElement * NewSyntaxElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits = -1, const char* mnemonic = NULL, unsigned int value = -1, unsigned char* start_buf = NULL, unsigned char* end_buf = NULL, const char* pszComment = NULL);

//	tinyxml2::XMLElement * x64NewElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits = -1, const char* mnemonic = NULL, int64_t value = -1, unsigned char* start_buf = NULL, unsigned char* end_buf = NULL, char* pszComment = NULL);
	tinyxml2::XMLElement * NewX64SyntaxElement(tinyxml2::XMLElement* pxmlParent, const char* name, int bits = -1, const char* mnemonic = NULL, uint64_t value = -1, const BITS_t* pbits_map = NULL, const char* pszComment = NULL);

	tinyxml2::XMLElement* QueryNodeByName(tinyxml2::XMLElement* pxmlParentNode, const char* pszNodeName);

	void SetOrigin(unsigned char* origin_ptr);

	void SetActiveNode(tinyxml2::XMLElement* pActive);
	tinyxml2::XMLElement* GetActiveNode(void);

	void UpdateBufMark(tinyxml2::XMLElement* pNode, unsigned char* start_ptr, unsigned char* end_ptr);

protected:

private:


public:
};


#endif	//__API_TINYXML2_WRAPPER_H__

