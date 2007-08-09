
#pragma once

#include <sstream>
#include <iterator>

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
    SiteTask(Site & site, TaskRunner * parent) 
        : site_(site), AsyncTask(parent) 
    {}
    void AddAction(const std::vector<Action*> & acts)
    {
        std::copy(acts.begin(), acts.end(), std::back_inserter(actions_));
    }
    void AddAction(Action* act)
    {
        actions_.push_back(act);
    }
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
    
public:// private:
    std::vector<ActionInfo*> actions_;
    int curact_;
    Site & site_;
};

class SiteCrank
{
public:
    // input: vector<sid> ?, ActionType
    bool Init()
    {
        {
            user_.insert(L"sex", L"male");
            user_.insert(L"pasw", L"strongpsw");
            user_.insert(L"mail", L"a@b.com");
        }

        Dictionary bd;
        {
            VariableMap vm;
            vm.insert(VariableMap::value_type(L"male", L"1"));
            vm.insert(VariableMap::value_type(L"female", L"2"));
            bd.Insert(L"sex", vm);
        }
        site_.SetDict(bd);

        site_.homepage = L"http://baidu.com";
        // site_.industries = L"all";
        {
            Action act;
            act.aid = 0;
            act.type = AT_REGISTER;
            act.url = L"https://passport.baidu.com/?verifypic";
            act.restype = ART_VERIFY_IMAGE;
            site_.Add(act);
        }

        {
            Action act;
            act.aid = 1;
            act.url = L"https://passport.baidu.com/?reg";
            act.vars = L"tpl=&tpl_ok=&skip_ok=&aid=0&need_pay=&need_coin=0&pay_method=0&u=&next_target=&return_method=&next_type=&more_param=&username={loginname}&loginpass={pasw}&verifypass={pasw}&sex={sex}&email=&verifycode={verifycode}&space_flag=on&submit= 同意";
            act.type = AT_REGISTER;
            act.restype = ART_NONE;
            site_.Add(act);
        }

        return true;
    }

    void Run(TaskRunner * runner)
    {
        SiteTask * task = new SiteTask(site_, runner);

        task->AddAction(site_.Find(AT_REGISTER));

        runner->RunTasks();
    }
private:
    UserInfo user_;
    // TODO:
    // std::vector<Site> sites_;
    Site site_;
};

