
#ifndef __ASYNCTASK_H__
#define __ASYNCTASK_H__

#pragma once

#include "asynchttp.h"
#include "task.h"
#include "criticalsection.h"

class AsyncTask : public Task, public AsyncHttp
{
public:
    AsyncTask(Task * parent);
    ~AsyncTask();

protected:
    // Task
    virtual int ProcessStart();
    virtual int ProcessResponse() { return STATE_DONE; }

    virtual void RecLock() { csglobal_.Enter(); }
    virtual void Unlock() { csglobal_.Leave(); }

    // AsyncHttp
    virtual void RequestDone();

private:
    static CriticalSection csglobal_;
};

#endif // __ASYNCTASK_H__
