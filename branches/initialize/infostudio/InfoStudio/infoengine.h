
#pragma once

#include <sstream>

#include "coreinfo.h"
#include "engine/asynctask.h"

/*
                        send request for auth image           
    [form not ready] ------------------------------>
start                                                block
    [form not ready] <------------------------------
                            server response
                        
                
                       wait for user input number
    [form not ready] ------------------------------>
start                                                block
    [form ready]     <------------------------------
                            user input it

                    
                             post form
                     ------------------------------>
start                                                block
                     <-----------------------------
                            server response

http request
deal response

need user input
http requst
deal response
*/

class SiteTask : public AsyncTask
{
public:

protected:
    virtual int ProcessStart()
    {
        curact_ = 0;
        ActionInfo * pa = actions_[curact_];
        
        bool f = StartAction(pa);
        return f ? STATE_BLOCKED : STATE_ERROR;
    }

    virtual int ProcessResponse() 
    {
        ActionInfo * pa = actions_[curact_];
        switch (pa->restype)
        {
        case ART_VERIFY_IMAGE:
            // 加入辨认验证码的队列 。。。。
            if (pa->timeout)
                set_timeout_seconds(pa->timeout);
            return STATE_BLOCKED;
        case ART_NONE:
            break;
        case ART_CHECK_RESULT:
            // do check
            break;
        default:
            break;
        }
        return STATE_STEP;
    }

    virtual int Process(int state)
    {
        if (STATE_STEP == state)
            return ProcessNextAction();
        else
            return AsyncTask::Process(state);
    }

protected:
    enum {
        STATE_STEP        = STATE_NEXT + 1,
    };

private:
    int ProcessNextAction()
    {
        curact_ ++;
        if (curact_ == actions_.size())
            return STATE_DONE;

        ActionInfo * pa = actions_[curact_];
        bool f = StartAction(pa);
        return f ? STATE_BLOCKED : STATE_ERROR;
    }

    bool StartAction(ActionInfo * pa)
    {
        bool f = false;
        if (pa->method == HV_POST)
        {
            std::stringstream ss;
            f = PrepareForm(&ss, pa);
            if (f)
                f = PreparePost(pa->url, pa->form_encoding, &ss, pa->referrer);
        }
        else if (pa->method == HV_GET)
            f = PrepareGet(pa->url, pa->referrer);

        if (f && pa->timeout)
            set_timeout_seconds(pa->timeout);
        return f;
    }

    bool PrepareForm(std::istream *, ActionInfo*)
    {
        return true;
    }   
    
private:
    std::vector<ActionInfo*> actions_;
    int curact_;
    SiteSpecial site_;
};

class Engine
{
public:
    bool Load()
    {
        // 
        return true;
    }
private:
    UserInfo user_;
    std::vector<SiteInfo> sites_;
};

