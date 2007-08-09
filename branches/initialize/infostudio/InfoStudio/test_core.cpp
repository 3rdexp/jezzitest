
#include "coreinfo.h"
#include "infoengine.h"
#include "engine/task.h"

#include <atlbase.h>
#include <atlwin.h>
#include <atlapp.h>

#include <atlctrls.h>

CAppModule _Module;

class CrankRunner : public TaskRunner
{
public:
    CrankRunner(CWindow & wnd) : wnd_(wnd) {}
    virtual void WakeTasks()
    {
        wnd_.PostMessage(WM_USER);
    }

    virtual int64 CurrentTime()
    {
        return Time();
    }

    void OnMessage()
    {
        // TODO: 
        RunTasks();
    }

private:
    CWindow & wnd_;
};

class CWndRunner : public CWindowImpl<CWndRunner, CEdit, CFrameWinTraits>
{
public:
    CWndRunner() : runner_(*this) {}
    BEGIN_MSG_MAP(CWndRunner)
        MESSAGE_HANDLER(WM_USER, foo)
        MESSAGE_HANDLER(WM_USER + 1, bar)
    END_MSG_MAP()

    LRESULT foo(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        runner_.OnMessage();
        return 0;
    }

    LRESULT bar(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        runner_.WakeTasks();
        return 0;
    }

    SiteCrank crank;
    CrankRunner runner_;
};

void test_core()
{
    _Module.Init(NULL, GetModuleHandle(0));

    CMessageLoop theLoop;
    _Module.AddMessageLoop(&theLoop);

    CWndRunner wnd;
    wnd.Create(NULL);
    
    SiteTask task(&wnd.runner_);
    wnd.crank.Load(task);
    wnd.runner_.StartTask(&task);

    wnd.PostMessage(WM_USER + 1);

    int nRet = theLoop.Run();
    _Module.RemoveMessageLoop();

    __asm int 3;
}

