
#pragma once

#include "engine/task.h"

#define WM_WAKETASK         WM_USER


class WindowRunner : public TaskRunner
    , public CWindowImpl<WindowRunner, CEdit, CFrameWinTraits>
{
public:
    virtual void WakeTasks()
    {
        PostMessage(WM_WAKETASK);
    }

    virtual int64 CurrentTime()
    {
        return Time();
    }

    BEGIN_MSG_MAP(WindowRunner)
        MESSAGE_HANDLER(WM_WAKETASK, OnWakeTask)
    END_MSG_MAP()

    LRESULT OnWakeTask(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RunTasks();
        return 0;
    }
};
