
#include "asynctask.h"

CriticalSection AsyncTask::csglobal_;

AsyncTask::AsyncTask(Task * parent) : Task(parent)
{
}

AsyncTask::~AsyncTask()
{
}

int AsyncTask::ProcessStart()
{
    PrepareGet("http://www.google.com/");
    SendRequest();
    return STATE_BLOCKED;
}

// virtual int AsyncTask::ProcessResponse() { return STATE_DONE; }


void AsyncTask::RequestDone()
{
    // 
    Wake();
}