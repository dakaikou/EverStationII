// DocumentForTS.cpp: 实现文件
//

#include "stdafx.h"
#include "../EverStationII.h"
#include "TSMagicDoc.h"

#include "Utilities\Directory\Include\TOOL_Directory.h"

// CTSMagicDoc

IMPLEMENT_DYNCREATE(CTSMagicDoc, CDocument)

CTSMagicDoc::CTSMagicDoc()
{
	m_strDebug = "天王盖地虎";
}

BOOL CTSMagicDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CTSMagicDoc::~CTSMagicDoc()
{
}


BEGIN_MESSAGE_MAP(CTSMagicDoc, CDocument)
END_MESSAGE_MAP()


// CTSMagicDoc 诊断

#ifdef _DEBUG
void CTSMagicDoc::AssertValid() const
{
	CDocument::AssertValid();
}

#ifndef _WIN32_WCE
void CTSMagicDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif
#endif //_DEBUG

#ifndef _WIN32_WCE
// CTSMagicDoc 序列化

void CTSMagicDoc::Serialize(CArchive& ar)
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


// CTSMagicDoc 命令

#include "thirdparty_libs/SQLite3/Include/sqlite3.h"  

int user_callback(void* param, int nColCount, char** pColValue, char** pColName)
{
	char pszMsg[256];
	//CString s;
	for (int i = 0; i < nColCount; i++)
	{
		sprintf_s(pszMsg, sizeof(pszMsg), "<col=%d>%s:%s\n", i, pColName[i], pColValue[i]);
		//	s += pColName[i];
		//	s += ":";
		//	s += pColValue[i];
		//	s += "\t";
		TRACE("user_callback: %s", pszMsg);
	}
	//s += "\n";
	//TRACE("user_callback: %s", s.GetBuffer());
	//cout << s << endl;
	return 0;
}

int sqlite3_test()
{
	char	pszDbFile[MAX_PATH];

	char	pszExeFile[MAX_PATH];
	char	exeDrive[3];
	char	pszDbaseDir[MAX_PATH];
	GetModuleFileName(NULL, pszExeFile, MAX_PATH);
	exeDrive[0] = pszExeFile[0];
	exeDrive[1] = pszExeFile[1];
	exeDrive[2] = '\0';

	sprintf_s(pszDbaseDir, sizeof(pszDbaseDir), "%s\\~EverStationII\\dbase", exeDrive);
	DIR_BuildDirectory(pszDbaseDir);

	sprintf_s(pszDbFile, sizeof(pszDbFile), "%s\\EverStationII.db", pszDbaseDir);

	char strSql[256];

	sqlite3* pdb;
	int nResult = sqlite3_open(pszDbFile, &pdb);
	if (nResult != SQLITE_OK)
	{
		//TRACE("打开数据库失败：%s\n", sqlite3_errmsg(pdb));
		//cout << "打开数据库失败：" << sqlite3_errmsg(db) << endl;

		return 0;
	}
	else
	{
		//TRACE("数据库打开成功!\n");
		//cout << "数据库打开成功" << endl;
	}

	char* errmsg;
	nResult = sqlite3_exec(pdb, "create table APP(id integer primary key autoincrement,name varchar(100))", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("创建表失败(errmsg)：%s\n", errmsg);
		//TRACE("创建表失败(sqlite3_errmsg)：%s\n", sqlite3_errmsg(pdb));
		sqlite3_free(errmsg);
	}

	//CString strSql;
	//for (int i = 0; i<100; i++)
	//{
	//	strSql += "insert into fuck values(null,'caonima');";
	//}
	//TRACE(strSql);

	//cout << strSql << endl;
	//nResult = sqlite3_exec(pdb, strSql.GetBuffer(), NULL, NULL, &errmsg);
	nResult = sqlite3_exec(pdb, "insert into APP values(null,'packets');", NULL, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		sqlite3_free(errmsg);
		//TRACE("插入数据失败(errmsg)：%s", errmsg);
		//TRACE("插入数据失败(sqlite3_errmsg)：%s", sqlite3_errmsg(pdb));
	}

	strcpy_s(strSql, sizeof(strSql), "select * from APP");
	nResult = sqlite3_exec(pdb, strSql, user_callback, NULL, &errmsg);
	if (nResult != SQLITE_OK)
	{
		//TRACE("读数据表(errmsg)：%s\n", errmsg);
		//cout << errmsg << endl;
		sqlite3_free(errmsg);
		return 0;
	}

	sqlite3_close(pdb);
	return 0;
}

