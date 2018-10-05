#ifndef _MIDDLEWARE_PSISI_TABLE_COMMON_H_
#define _MIDDLEWARE_PSISI_TABLE_COMMON_H_

#include "../compile.h"
#include <stdint.h>

#include "libs_MPEG&DVB\MPEG_DVB_Section\nativeInclude\MPEG_DVB_Common.h"

/*------------------------------------------------------------
		PSISI Table Base definition 
-------------------------------------------------------------*/
class _CDL_EXPORT CPVT
{
public:
	CPVT(uint16_t Key, uint16_t PID, uint8_t table_id, uint16_t table_id_extension);
	~CPVT(void);
protected:

	uint16_t				m_usKey;				//随机数，唯一性标识
	int						m_bNormalSectionSyntax;	//是不是规则section语法

	/*------------------unique ID part---------------------------*/
	uint16_t				m_usPID;
	uint8_t					m_ucTableID;
	uint16_t				m_usTableIDExtension;

	/*------------------syntax part---------------------------*/
	uint8_t					m_ucVersionNumber;
	int						m_nSectionCount;
	uint8_t*				m_aucSectionFlag;

	int						m_bCollectOver;

	/*------------------syntax part---------------------------*/

public:
	void Init(void);
	void Reset(void);
	int	 AddSection(uint16_t usPID, private_section_t* pprivate_section);

	uint16_t GetKey(void);
	uint16_t GetTableIDExtension(void);
	uint8_t  GetTableID(void);
	uint16_t GetPID(void);
	uint8_t GetVersionNumber(void);

	int IsCollectOver(void);
	int IsNormalSectionSyntax(void);
};

#endif
