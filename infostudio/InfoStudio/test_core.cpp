
#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>

#include <atlctrls.h>

#define ASSERT ATLASSERT

#include "coreinfo.h"
#include "infoengine.h"
#include "wndrunner.h"


extern CAppModule _Module;

void test_core()
{
    _Module.Init(NULL, GetModuleHandle(0));

    AsyncInet::Init();

    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    WindowRunner wr;
    HWND h = wr.Create(NULL);
    ATLASSERT(h);

    SiteCrank crank;
    crank.Init();

    crank.Run(&wr);

    int nRet = theLoop.Run();
    _Module.RemoveMessageLoop();

    AsyncInet::Release();
}

