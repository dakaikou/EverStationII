// DocumentForYUV.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "YUVMagicDoc.h"


// CYUVMagicDoc

IMPLEMENT_DYNCREATE(CYUVMagicDoc, CDocument)

CYUVMagicDoc::CYUVMagicDoc()
{
	m_strDebug = "宝塔镇河妖";
}

BOOL CYUVMagicDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CYUVMagicDoc::~CYUVMagicDoc()
{
}


BEGIN_MESSAGE_MAP(CYUVMagicDoc, CDocument)
END_MESSAGE_MAP()


// CYUVMagicDoc 诊断

#ifdef _DEBUG
void CYUVMagicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CYUVMagicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CYUVMagicDoc 序列化

void CYUVMagicDoc::Serialize(CArchive& ar)
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


// CYUVMagicDoc 命令
