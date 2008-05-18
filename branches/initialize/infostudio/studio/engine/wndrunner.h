
#pragma once

#include "base/task.h"
#include "base/logging.h"
#include "wndpump.h"

#define WM_WAKETASK         WM_USER


class WindowRunner : public TaskRunner
    , public WindowPipe
{
public:
    WindowRunner(WindowPump & pump) : pump_(pump) {
        pump_.Add(this);
    }
    ~WindowRunner() {
        pump_.Remove(this);
    }

    virtual void WakeTasks()
    {
        // LOG(LS_INFO) << "WakeTasks";
        pump_.PostMessage(WM_WAKETASK);
    }

    virtual int64 CurrentTime()
    {
        return Time();
    }

    virtual BOOL HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        BOOL bHandled = TRUE;
        LRESULT lResult;
        MESSAGE_HANDLER(WM_WAKETASK, OnWakeTask)
        return FALSE;
    }

    LRESULT OnWakeTask(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RunTasks();
        return 0;
    }
private:
    WindowPump & pump_;
};
