
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
    SendRequest();
    return STATE_BLOCKED;
}

void AsyncTask::RequestDone()
{
    SetState(STATE_RESPONSE);
    Wake();
}