#pragma once

#include <afxwin.h>
// CInstrumentView_Kernel 视图
#define ON_PAINTING_USE_MUTEX			1

#define SCREEN_BKGROUNDCOLOR			RGB(0, 0, 0)
#define SCREEN_BKWAVEFORMCOLOR			RGB(0, 0, 0)
#define SCREEN_BKMEASUREPANELCOLOR		RGB(0, 0, 0)

#define SCREEN_TEXTCOLOR				RGB(0, 255, 0)
#define SCREEN_TITLECOLOR				RGB(225, 255, 0)
#define SCREEN_UNITCOLOR				RGB(100, 220, 128)
#define SCREEN_GRIDCOLOR				RGB(128, 128, 128)
#define SCREEN_AXISCOLOR				RGB(220, 220, 220)
#define SCREEN_MAXLIMITCOLOR			RGB(200, 0, 0)
#define SCREEN_MINLIMITCOLOR			RGB(160, 0, 0)
#define X_SEPARATOR						4
#define Y_SEPARATOR						4

#define RECT_MEASURE_WIDTH				160
#define RECT_XMARK_HEIGHT				40
#define RECT_XMARK_WIDTH				80
#define RECT_YMARK_HEIGHT				30
#define RECT_YMARK_WIDTH				80
#define RECT_TITLE_HEIGHT				40

#define FONT_TITLE_HEIGHT				20
#define FONT_MARK_HEIGHT				16
#define FONT_MEASURE_HEIGHT				16

#define VERTICAL_GRID_DIVISION						20
#define HORIZONTAL_GRID_DIVISION					20
#define LOGARITHMIC_RANK							(VERTICAL_GRID_DIVISION/2)

#define UNCREDITABLE_MAX_VALUE						-123456789
#define UNCREDITABLE_MIN_VALUE						123456789
#define AXIS_STYLE_UNKNOWN							0
#define AXIS_STYLE_CARTESIAN_MEAN_SYMMETRY			1
#define AXIS_STYLE_CARTESIAN_FROM_MIN_TO_MAX		2
#define AXIS_STYLE_LOGARITHMIC_MEAN_SYMMETRY		3
#define AXIS_STYLE_LOGARITHMIC_FROM_MIN_TO_MAX		4

#define RANGE_MARK_SHOWN_KEYPOINT					0x00000004
#define RANGE_MARK_SHOWN_PARTIAL					0x00000002
#define RANGE_MARK_SHOWN_TOTAL						0x00000001
#define RANGE_MARK_HIDE								0x00000000

#define MEASURE_PANEL_SHOWN							0x00010000
#define MEASURE_PANEL_HIDE							0x00000000

class CInstrumentView_Kernel : public CView
{
	DECLARE_DYNCREATE(CInstrumentView_Kernel)

protected:
	CInstrumentView_Kernel();           // 动态创建所使用的受保护的构造函数
	virtual ~CInstrumentView_Kernel();

protected:
	int		  m_bNeedRedrawAllBmp;
	int		  m_bNeedRedrawWaveformBmp;
	int		  m_bNeedRedrawMeasurePanelBmp;

	double  m_dYLogArithmicCoeff;
	double  m_dXLogArithmicCoeff;

	char	m_pszXUnits[16];
	char	m_pszYUnits[16];

	int		m_nXPanelShownOption;
	int		m_nXMarkShownOption;
	int		m_nXAxisStyle;

	int		m_nYPanelShownOption;
	int		m_nYMarkShownOption;
	int		m_nYAxisStyle;

	CString		m_strTitle;

	int		m_nMeasuredYMeanValue;
	int		m_nMeasuredYRmsValue;
	int		m_nMeasuredYMinValue;
	int		m_nMeasuredYMaxValue;

	int		m_nMeasuredXMeanValue;
	int		m_nMeasuredXRmsValue;
	int		m_nMeasuredXMinValue;
	int		m_nMeasuredXMaxValue;

	int		m_nYNegtiveMark;
	int		m_nYZeroMark;
	int		m_nYPositiveMark;
	int		m_nYAlarmMinLimit;
	int		m_nYAlarmMaxLimit;
	int		m_nYFloor;
	int		m_nYCeil;
	int		m_nYStep;

	int		m_nXNegtiveMark;
	int		m_nXZeroMark;
	int		m_nXPositiveMark;
	int		m_nXAlarmMinLimit;
	int		m_nXAlarmMaxLimit;
	int		m_nXFloor;
	int		m_nXCeil;
	int		m_nXStep;

	UINT_PTR	m_uiTimerID;

	CBrush* m_pBkgroundBrush;
	CBrush* m_pWaveformBrush;
	CBrush* m_pMeasurePanelBrush;

	CBitmap* m_pBkgroundBmp;
	CBitmap* m_pWaveformBmp;
	CBitmap* m_pMeasurePanelBmp;

	CDC* m_pMemDC;

	CRect	m_rectClient;
	CRect	m_rectWaveform;
	CRect	m_rectMeasurePanel;

#if ON_PAINTING_USE_MUTEX
	HANDLE	m_hPaintingAccess;
#endif

	virtual void DisplayTheWholeSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);
	virtual void DisplayTheNewSamplesInMemory(CDC* pMemDC, CBitmap* pGraphBmp);

	virtual void DisplayXAlarmLineInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine);
	virtual void DisplayYAlarmLineInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectAlarmLine);

	virtual void DisplayBkGridInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectWaveform);
	virtual void DisplayLegendInMemory(CDC* pMemDC, CBitmap* pBkBmp, CRect rectWaveform);

	void DisplayMeasurePanelInMemory(CDC* pMemDC, CBitmap* pBkBmp);

	void AdjustLayout(void);

	int XMAP_Value2Pos(int value, CRect rectPicture);
	int YMAP_Value2Pos(int value, CRect rectPicture);

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	void SetTitle(char* pszTitle);
	void Init_X_Axis(int nXAxisStyle, int nXShownOption, int nXMinAlarm, int nXMaxAlarm, char* pszXUnits, int nXFloor, int nXCeil, int nStep = 100);
	void Init_Y_Axis(int nYAxisStyle, int nYShownOption, int nYMinAlarm, int nYMaxAlarm, char* pszYUnits, int nYFloor, int nYCeil, int nStep = 100);

	void Set(void);
	void Reset(void);
	void Clear(void);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


