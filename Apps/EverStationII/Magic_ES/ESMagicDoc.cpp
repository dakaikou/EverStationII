// DocumentForES.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "ESMagicDoc.h"


// CTSMagicDoc

IMPLEMENT_DYNCREATE(CESMagicDoc, CDocument)

CESMagicDoc::CESMagicDoc()
{
	m_strDebug = "视音频流分析";
}

BOOL CESMagicDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CESMagicDoc::~CESMagicDoc()
{
}


BEGIN_MESSAGE_MAP(CESMagicDoc, CDocument)
END_MESSAGE_MAP()


// CTSMagicDoc 诊断

#ifdef _DEBUG
void CESMagicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CESMagicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CTSMagicDoc 序列化

void CESMagicDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO:  在此添加存储代码
	}
	else
	{
		// TODO:  在此添加加载代码
	}
}
#endif


// CESMagicDoc 命令
