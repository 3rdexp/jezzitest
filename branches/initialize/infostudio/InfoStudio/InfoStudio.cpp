// InfoStudio.cpp : main source file for InfoStudio.exe
//
/*
公司信息
黄页信息
供求信息
论坛群发
产品发布
计划任务
*/

#include "stdafx.h"
// #include <vld.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>

#include "resource.h"

#include "aboutdlg.h"
#include "maindlg.h"
#include "./util.h"

CAppModule _Module;

CAdoConnection* _pDb = NULL;
CInfoManage*	_pInfoManage = NULL;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	_pDb = new CAdoConnection();
	BOOL bRet = _pDb->ConnectAccess( GetExePath() + "db.mdb" );
	if (!bRet)
	{
		MessageBox(NULL, "读取数据库失败", MSGTITLE, MB_OK);
		if ( _pDb )
			delete _pDb;
		_pDb = NULL;
		return 0;
	}

	_pInfoManage = new CInfoManage();
	_pInfoManage->Init();

	CMainDlg dlgMain;

	if(dlgMain.Create(NULL) == NULL)
	{
		ATLTRACE(_T("Main dialog creation failed!\n"));
		return 0;
	}

	dlgMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

	_Module.RemoveMessageLoop();
	
	if ( _pInfoManage )
		delete _pInfoManage;

	if ( _pDb )
		delete _pDb;

	return nRet;
}

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = ::CoInitialize(NULL);
// If you are running on NT 4.0 or higher you can use the following call instead to 
// make the EXE free threaded. This means that calls come in on a random RPC thread.
//	HRESULT hRes = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	ATLASSERT(SUCCEEDED(hRes));

	// this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
	::DefWindowProc(NULL, 0, 0, 0L);

	AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);	// add flags to support other controls

	hRes = _Module.Init(NULL, hInstance, &LIBID_ATLLib);
	//hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
