// DocumentForPS.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "PSMagicDoc.h"


// CPSMagicDoc

IMPLEMENT_DYNCREATE(CPSMagicDoc, CDocument)

CPSMagicDoc::CPSMagicDoc()
{
	m_strDebug = "节目流分析";
}

BOOL CPSMagicDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CPSMagicDoc::~CPSMagicDoc()
{
}


BEGIN_MESSAGE_MAP(CPSMagicDoc, CDocument)
END_MESSAGE_MAP()


// CPSMagicDoc 诊断

#ifdef _DEBUG
void CPSMagicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CPSMagicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CPSMagicDoc 序列化

void CPSMagicDoc::Serialize(CArchive& ar)
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


// CPSMagicDoc 命令
