// studio.cpp : main source file for studio.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlsplit.h>

#include "engine/infoengine.h"
#include "engine/wndrunner.h"
#include "data/studiodata.h"
#include "data/basedata.h"
#include "data/mutabledata.h"

#include "resource.h"

#include "leftv.h"
#include "aboutdlg.h"
#include "MainFrm.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
    // login, config
    // 
    // SiteCrank
    // base / mutable data

    // 如此多的栈对象，找到更合适的方法
    // base / mutable 数据库必须分开

    SiteCrank crank;

    StudioData cons;
    bool f = cons.Open(L"data\\tools\\base.db", L"user.db");
    ASSERT(f);

    BaseData bd;
    f = bd.Init(cons.basecon_);
    ASSERT(f);

    cons.CloseBase();

    // 
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

    WindowRunner wr;
    HWND h = wr.Create(0);
    ATLASSERT(h);

    crank.Run(&wr);

    //
	CMainFrame wndMain;
    wndMain.InitData(&bd, 0);
	if(wndMain.CreateEx() == NULL)
	{
		ATLTRACE(_T("Main window creation failed!\n"));
		return 0;
	}

	wndMain.ShowWindow(nCmdShow);

	int nRet = theLoop.Run();

    // TODO:
    // crank.Stop();
    wr.DestroyWindow();

	_Module.RemoveMessageLoop();
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

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}

int main()
{
    _tWinMain((HINSTANCE)GetModuleHandle(0), 0, L"", 0);
    return 0;
}
