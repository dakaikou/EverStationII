#ifndef _TSMAGIC_TRIGGER_BASE_H_
#define _TSMAGIC_TRIGGER_BASE_H_

#include "HAL/HAL_Sys/Include/INTTYPES.H"

#define MAX_TRIGGER_COUNT			100

class CTrigger
{
public:
	CTrigger(void);
	~CTrigger(void);
protected:
	S32		m_bOpened;
	S32		m_bFull;;

//	S32		m_nBrowseIndex;
	S32		m_nGotCount;
	S32		m_nDemandCount;

	U8		m_ucMatchMasks[204];
	U8		m_ucMatchDatas[204];
	S32		m_nMatchLength;

	U8*		m_ucDataBuf[MAX_TRIGGER_COUNT];
	S32		m_nDataLength[MAX_TRIGGER_COUNT];
protected:
	//	void Clearparams(void);
public:

	U8*	GetMatchMasks(S32* plength);
	U8*	GetMatchDatas(S32* plength);
	U8*	GetCatchedDatas(S32 nIndex, S32* plength);

	S32  SaveTheWholePacket(U8* buf, S32 length);

	//以下三个函数为一组，必须成组使用
	S32  SaveAsNewStart(U8* buf, S32 length);		//返回记录的句柄
	S32  AppendToLast(S32 record_handle, U8* buf, S32 length);
	S32  SaveAndClose(S32 record_handle, U8* buf, S32 length);
	
	S32	 GetCatchedCount(void);

	S32 IsOpened(void);
	S32 IsFull(void);
	void Reset(void);

	void SetMatchParams(U8* match_mask, U8* match_data, S32 match_len, S32 demand_count);

	S32  IsMatched(U8* buf, S32 length);
};


#endif		//_TSMAGIC_TRIGGER_BASE_H_
