#if !defined(AFX_TIME_BASE_INSTRUMENT_PANEL_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
#define AFX_TIME_BASE_INSTRUMENT_PANEL_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//

#include "InstrumentCtrl_Wnd.h"

#define SCREEN_WAVECOLOR0		RGB(250, 250, 0)
#define SCREEN_WAVECOLOR1		RGB(50, 250, 50)
#define SCREEN_WAVECOLOR2		RGB(150, 250, 100)
#define SCREEN_WAVECOLOR3		RGB(100, 250, 150)
#define SCREEN_WAVECOLOR4		RGB(250, 200, 0)
#define SCREEN_WAVECOLOR5		RGB(200, 200, 50)
#define SCREEN_WAVECOLOR6		RGB(150, 200, 100)
#define SCREEN_WAVECOLOR7		RGB(100, 200, 150)
#define SCREEN_WAVECOLOR8		RGB(250, 150, 0)
#define SCREEN_WAVECOLOR9		RGB(200, 150, 50)
#define SCREEN_WAVECOLOR10		RGB(150, 150, 100)
#define SCREEN_WAVECOLOR11		RGB(100, 150, 150)

#ifndef SAMPLE_ATTRIBUTE
#define SAMPLE_ATTRIBUTE
typedef struct
{
	int min, mean, max, rms;
} SAMPLE_ATTRIBUTE_t;
#endif		//SAMPLE_ATTRIBUTE

#ifndef SAMPLE_CHANNEL
#define SAMPLE_CHANNEL
typedef struct
{
	int* pnXArray;
	int* pnYArray;

	int nSampleCount;
	int nWrIndex;
	int nRdIndex;
	int bFull;
	int bEmpty;
	int ID;
	DWORD color;

#if INSTRUMENT_PANEL_USE_MUTEX
	HANDLE	hSampleAccess;
#endif

} SAMPLE_CHANNEL_t;
#endif		//SAMPLE_CHANNEL

#define MAX_CHANNEL_COUNT		12
#define DEFAULT_CHANNEL_DEPTH	256

/////////////////////////////////////////////////////////////////////////////
// CInstrumentPanel_Base window
class CInstrumentPanel_Base : public CInstrumentCtrl_Wnd
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
	int				  m_nChannleCount;
	int				  m_nChannleDepth;

	//int				  m_bNeedRedrawAllBmp;
	//int				  m_bNeedRedrawWaveformBmp;
	//int				  m_bNeedRedrawMeasurePanelBmp;

	//double	m_dMeasureDeltX;
	//double	m_dMeasureDeltY;

	//double  m_dYLogArithmicCoeff;
	//double  m_dXLogArithmicCoeff;

	//char	m_pszXUnits[16];
	//char	m_pszYUnits[16];
	
	//int		m_nXPanelShownOption;
	//int		m_nXMarkShownOption;
	//int		m_nXAxisStyle;

	//int		m_nYPanelShownOption;
	//int		m_nYMarkShownOption;
	//int		m_nYAxisStyle;

	//CString m_strTitle;

	//int		m_nMeasuredYMeanValue;		
	//int		m_nMeasuredYRmsValue;		
	//int		m_nMeasuredYMinValue;			
	//int		m_nMeasuredYMaxValue;			

	//int		m_nMeasuredXMeanValue;		
	//int		m_nMeasuredXRmsValue;		
	//int		m_nMeasuredXMinValue;			
	//int		m_nMeasuredXMaxValue;			

	//int		m_nYNegtiveMark;
	//int		m_nYZeroMark;
	//int		m_nYPositiveMark;			
	//int		m_nYAlarmMinLimit;
	//int		m_nYAlarmMaxLimit;
	//int		m_nYFloor;
	//int		m_nYCeil;
	//int		m_nYStep;

	//int		m_nXNegtiveMark;
	//int		m_nXZeroMark;
	//int		m_nXPositiveMark;
	//int		m_nXAlarmMinLimit;
	//int		m_nXAlarmMaxLimit;
	//int		m_nXFloor;
	//int		m_nXCeil;
	//int		m_nXStep;

	//UINT_PTR	m_uiTimerID;

	//CBrush* m_pBkgroundBrush;
	//CBrush* m_pWaveformBrush;
	//CBrush* m_pMeasurePanelBrush;

	//CBitmap* m_pBkgroundBmp;
	//CBitmap* m_pWaveformBmp;
	//CBitmap* m_pMeasurePanelBmp;

	COLORREF  m_Palette[MAX_CHANNEL_COUNT];
	
	//CDC*	m_pMemDC;

	//CRect	m_rectClient;
	//CRect	m_rectWaveform;
	//CRect	m_rectMeasurePanel;

//#if ON_PAINTING_USE_MUTEX
//	HANDLE	m_hPaintingAccess;
//#endif

	//virtual void DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);
	//virtual void DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);

	//virtual void DisplayXAlarmLineInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine);
	//virtual void DisplayYAlarmLineInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine);

	//virtual void DisplayBkGridInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectWaveform);
	//virtual void DisplayLegendInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectWaveform);

	//void DisplayMeasurePanelInMemory(CDC* pMemDC, CBitmap* pBkBmp);

	//void AdjustLayout(CRect rectContainer);

	//int XMAP_Value2Pos(int value, CRect rectPicture);
	//int YMAP_Value2Pos(int value, CRect rectPicture);

public:
	void AppendXSample(int chID, int x);
	void AppendYSample(int chID, int y);
	void AppendXYSample(int chID, int x, int y);

	//void Init_X_Axis(int nXAxisStyle, int nXShownOption, int nXMinAlarm, int nXMaxAlarm, char* pszXUnits, int nXFloor, int nXCeil, int nStep=100);
	//void Init_Y_Axis(int nYAxisStyle, int nYShownOption, int nYMinAlarm, int nYMaxAlarm, char* pszYUnits, int nYFloor, int nYCeil, int nStep=100);

	void Reset(void);
	//void Clear(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CInstrumentPanel_Base)
	//afx_msg void OnPaint();
	//afx_msg void OnDestroy();
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	//afx_msg void OnTimer(UINT_PTR nIDEvent);
	//afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIME_BASE_INSTRUMENT_PANEL_H__2CB59A93_04A1_11D5_B4BD_0050BA6DB00C__INCLUDED_)
