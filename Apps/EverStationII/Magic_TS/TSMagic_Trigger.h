#ifndef _TSMAGIC_TRIGGER_BASE_H_
#define _TSMAGIC_TRIGGER_BASE_H_

#define MAX_TRIGGER_COUNT			100

class CTrigger
{
public:
	CTrigger(void);
	~CTrigger(void);
protected:
	int		m_bOpened;
	int		m_bFull;;

//	int		m_nBrowseIndex;
	int		m_nGotCount;
	int		m_nDemandCount;

	uint8_t		m_ucMatchMasks[204];
	uint8_t		m_ucMatchDatas[204];
	int		m_nMatchLength;

	uint8_t*		m_ucDataBuf[MAX_TRIGGER_COUNT];
	int		m_nDataLength[MAX_TRIGGER_COUNT];
protected:
	//	void Clearparams(void);
public:

	uint8_t*	GetMatchMasks(int* plength);
	uint8_t*	GetMatchDatas(int* plength);
	uint8_t*	GetCatchedDatas(int nIndex, int* plength);

	int  SaveTheWholePacket(uint8_t* buf, int length);

	//以下三个函数为一组，必须成组使用
	int  SaveAsNewStart(uint8_t* buf, int length);		//返回记录的句柄
	int  AppendToLast(int record_handle, uint8_t* buf, int length);
	int  SaveAndClose(int record_handle, uint8_t* buf, int length);
	
	int	 GetCatchedCount(void);

	int IsOpened(void);
	int IsFull(void);
	void Reset(void);

	void SetMatchParams(uint8_t* match_mask, uint8_t* match_data, int match_len, int demand_count);

	int  IsMatched(uint8_t* buf, int length);
};


#endif		//_TSMAGIC_TRIGGER_BASE_H_
