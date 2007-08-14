
#include "infoengine.h"

//////////////////////////////////////////////////////////////////////////
//
SiteTask::SiteTask(Site & site, const UserInfo & userinfo, TaskRunner * parent) 
    : site_(site), uiserinfo_(userinfo)
    , AsyncTask(parent)
    , curact_(0)
{}

void SiteTask::EnterVerifyCode(const std::wstring & code)
{
    ASSERT(STATE_INPUT_VERIFYCODE == GetState());
    verifycode_ = code;
    SetState(STATE_ACTION_DONE);
    Wake();
}

int SiteTask::ProcessStart()
{
    curact_ = 0;
    Action * pa = actions_[curact_];

    bool f = StartAction(pa);
    if (f && pa->restype == ART_VERIFY_IMAGE)
    {
        SetState(STATE_INPUT_VERIFYCODE);
    }
    return f ? STATE_BLOCKED : STATE_ERROR;
}

int SiteTask::ProcessResponse() 
{
    Action * pa = actions_[curact_];
    switch (pa->restype)
    {
    case ART_VERIFY_IMAGE:
        ASSERT(false);
        return STATE_BLOCKED;
    case ART_NONE:
        break;
    case ART_CHECK_RESULT:
        // do check
        break;
    default:
        break;
    }
    return STATE_ACTION_DONE;
}

int SiteTask::Process(int state)
{
    if (STATE_ACTION_DONE == state)
        return ProcessNextAction();
    else
        return AsyncTask::Process(state);
}

void SiteTask::RequestDone()
{
    if (GetState() == STATE_INPUT_VERIFYCODE)
    {
        Action * pa = actions_[curact_];

        // 加入辨认验证码的队列 。。。。
        // state_ 依然是 Blocked
        if (pa->timeout)
            set_timeout_seconds(pa->timeout);
    }
    else
    {
        SetState(STATE_RESPONSE);
        Wake();
    }
}


int SiteTask::ProcessNextAction()
{
    curact_ ++;
    if (curact_ == actions_.size())
        return STATE_DONE;

    Action * pa = actions_[curact_];
    bool f = StartAction(pa);
    return f ? STATE_BLOCKED : STATE_ERROR;
}

bool SiteTask::StartAction(Action * pa)
{
    std::stringstream ss;
    bool f = PrepareForm(ss, pa->vars);
    if (f)
    {
        if (pa->method == HV_POST)
            f = PreparePost(pa->url, pa->form_encoding, &ss, pa->referrer);
        else if (pa->method == HV_GET)
            f = PrepareGet(pa->url, pa->referrer);
    }

    if (f)
    {
        SendRequest();
        if (pa->timeout)
            set_timeout_seconds(pa->timeout);
    }
    return f;
}

// a={b} Site, UserInfo
bool SiteTask::PrepareForm(std::ostream & out, const std::wstring & vars)
{
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
