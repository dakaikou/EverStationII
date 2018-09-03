#if !defined(AFX_TIME_HISTOGRAM_INSTRUMENT_PANEL_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
#define AFX_TIME_HISTOGRAM_INSTRUMENT_PANEL_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#define INSTRUMENT_PANEL_USE_DIRECTX	0

#define X_SEPARATOR						5
#define Y_SEPARATOR						10

#define RECT_MEASURE_WIDTH				120

#define RECT_MARK_HEIGHT				20
#define RECT_MARK_WIDTH					60

#define RECT_TITLE_HEIGHT				30

#define FONT_TITLE_HEIGHT				RECT_TITLE_HEIGHT - 12
#define FONT_MARK_HEIGHT				RECT_MARK_HEIGHT - 6
#define FONT_MEASURE_HEIGHT				12



#define GRID_DIVISION_VERTICAL			6
#define GRID_DIVISION_HORIZONTAL		20

//#include "XStream_FIFO\Include\FIFO.h"

typedef struct
{
	int min, mean, max, rms;
} SAMPLE_ATTRIBUTE_t;

typedef struct
{
//	CFIFO<int, 1024> fifo;
	int* pnXSampleArray;
	int* pnYSampleArray;
	int nSampleCount;
	int nSampleIndex;
	int ID;
	DWORD color;
	HANDLE	hSampleAccess;

} SAMPLE_CHANNEL_t;

#define MAX_CHANNEL_COUNT		12
#define DEFAULT_CHANNEL_DEPTH	256

#define AXIS_STYLE_UNKNOWN				0
#define AXIS_STYLE_MEAN_SYMMETRY		1
#define AXIS_STYLE_FROM_MIN_TO_MAX		2

#define RANGE_MARK_SHOWN	1
#define RANGE_MARK_HIDE		0

//#define SCAN_STYLE_FIXED_POS		1
//#define SCAN_STYLE_IN_SEQUENCE		0

/////////////////////////////////////////////////////////////////////////////
// CGraph_Oscilloscope window
class CInstrumentPanel_Base : public CStatic
{
// Construction
public:
	CInstrumentPanel_Base();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInstrumentPanel_Base)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInstrumentPanel_Base();

// Implementation
protected:

	SAMPLE_CHANNEL_t* m_pChannel[MAX_CHANNEL_COUNT];
	int				   m_nChannleCount;
	int					m_nChannleDepth;

	int					m_bNeedUpdate;

	double	m_dGridDelty;
	double	m_dGridDeltx;

	double	m_dMeasureDeltX;
	double	m_dMeasureDeltY;

	char	m_pszXUnits[16];
	char	m_pszYUnits[16];
	
	int		m_nXMarkShownOption;
	int		m_nXAxisStyle;

	int		m_nYMarkShownOption;
	int		m_nYAxisStyle;

	CString m_strTitle;

	int		m_nMeasuredYMeanValue;		
	int		m_nMeasuredYRmsValue;		
	int		m_nMeasuredYMinValue;			
	int		m_nMeasuredYMaxValue;			

	int		m_nMeasuredXMeanValue;		
	int		m_nMeasuredXRmsValue;		
	int		m_nMeasuredXMinValue;			
	int		m_nMeasuredXMaxValue;			

	int		m_nYNegtiveMark;			
	int		m_nYPositiveMark;			
	int		m_nYAlarmMinLimit;
	int		m_nYAlarmMaxLimit;
	int		m_nYFloor;
	int		m_nYCeil;
	int		m_nYStep;

	int		m_nXNegtiveMark;
	int		m_nXPositiveMark;
	int		m_nXAlarmMinLimit;
	int		m_nXAlarmMaxLimit;
	int		m_nXFloor;
	int		m_nXCeil;
	int		m_nXStep;

	//int		m_nObserveMinValue;
	//int		m_nObserveMaxValue;


	UINT_PTR	m_uiTimerID;

	//CPen* m_pWavePen;
	CPen* m_pGridPen;
	CPen* m_pDotPen;
	CPen* m_pDashPen;
	CPen* m_pAxisPen;

	CPen* m_pMaxLimitPen;
	CPen* m_pMinLimitPen;
	CFont* m_pMeasureFont;
	CFont* m_pTitleFont;
	CFont* m_pMarkFont;

	CBrush* m_pBkBrush;
	
	CBitmap* m_pBkgroundBmp;
	CBitmap* m_pWaveformBmp;
	//CBitmap* m_pAlarmLineBmp;
	//CBitmap* m_pValueBmp;
	//CBitmap* m_pLeftMarkBmp;
	//CBitmap* m_pMidMarkBmp;
	//CBitmap* m_pRightMarkBmp;
	
	CDC* m_pMemDC;

	CRect	m_rectClient;
	CRect   m_rectTitle;
	CRect	m_rectWaveform;
	CRect	m_rectMeasuredValue;
	CRect   m_rectXLeftMark;
	CRect   m_rectXMidMark;
	CRect   m_rectXRightMark;
	CRect   m_rectYTopMark;
	CRect   m_rectYMidMark;
	CRect   m_rectYBottomMark;

	virtual void DisplayMeasureGraph(CDC* pMemDC, CBitmap* pGraphBmp);

	void DisplayMeasuredValue(CDC* pMemDC, CBitmap* pBkBmp, CRect rectMeasure);
	void DisplayMeasureScale(CDC* pMemDC, CBitmap* pBkBmp, CRect rectMark, int nMark);
	void DisplayXAlarmLine(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine);
	void DisplayYAlarmLine(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine);
	void DisplayBkGrid(CDC* pMemDC, CBitmap* pBkBmp, CRect rectWaveform);

	void AdjustLayout(CRect rectContainer);

	void CombineDraw(void);
public:
	void AppendXSample(int ID, int x);
	void AppendYSample(int ID, int y);
	void AppendXYSample(int ID, int x, int y);

	void Init_X_Axis(int nXAxisStyle, int nXShownOption, int nXMinAlarm, int nXMaxAlarm, char* pszXUnits, int nXFloor, int nXCeil, int nStep=100);
	void Init_Y_Axis(int nYAxisStyle, int nYShownOption, int nYMinAlarm, int nYMaxAlarm, char* pszYUnits, int nYFloor, int nYCeil, int nStep=100);

	void Reset(void);

	//void AppendSampleForMeanSymmetryGraph(int ID, int curValue, int minValue, int meanValue, int maxValue, int rmsValue);
	//void AppendSampleForMinMaxGraph(int ID, int curValue, int minValue, int meanValue, int maxValue, int rmsValue);
	//void AppendSampleForScatterDiagram(int ID, int curValue, int minValue, int meanValue, int maxValue, int rmsValue);

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraph_Oscilloscope)
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIME_HISTOGRAM_INSTRUMENT_PANEL_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
