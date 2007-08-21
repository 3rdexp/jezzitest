
#pragma once

#include <sstream>
#include <iterator>

#include "coreinfo.h"
#include "../engine/asynctask.h"

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
    SiteTask(Site & site, const UserInfo & userinfo, TaskRunner * parent);
    void AddAction(const std::vector<Action*> & acts)
    {
        std::copy(acts.begin(), acts.end(), std::back_inserter(actions_));
    }
    void AddAction(Action* act)
    {
        actions_.push_back(act);
    }

    void EnterVerifyCode(const std::wstring & code);
protected:
    virtual int ProcessStart();
    virtual int ProcessResponse();
    virtual int Process(int state);
    

    virtual void RequestDone();

protected:
    enum {
        STATE_INPUT_VERIFYCODE = STATE_NEXT + 1,
        STATE_ACTION_DONE        = STATE_NEXT + 2,
    };

private:
    int ProcessNextAction();
    bool StartAction(Action * pa);
    
    bool PrepareForm(std::ostream & out, const std::wstring & vars
        , SiteCharset charset) const;
public:// private:
    std::vector<Action*> actions_;
    int curact_;
    Site & site_;
    const UserInfo & userinfo_;
    std::wstring verifycode_; // 输入的。。。
};

//////////////////////////////////////////////////////////////////////////
//
class SiteCrank
{
public:
    // input: vector<sid> ?, ActionType
    bool Init()
    {
        {
            userinfo_.insert(L"sex", L"male");
            userinfo_.insert(L"pasw", L"strongpsw");
            userinfo_.insert(L"mail", L"a@b.com");
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
            act.method = HV_GET;
            act.restype = ART_VERIFY_IMAGE;
            site_.Add(act);
        }

        {
            Action act;
            act.aid = 1;
            act.url = L"https://passport.baidu.com/?reg";
            act.method = HV_POST;
            act.vars = L"tpl=&tpl_ok=&skip_ok=&aid=0&need_pay=&need_coin=0&pay_method=0&u=&next_target=&return_method=&next_type=&more_param=&username={loginname}&loginpass={pasw}&verifypass={pasw}&sex={sex}&email=&verifycode={verifycode}&space_flag=on&submit= 同意";
            act.type = AT_REGISTER;
            act.restype = ART_NONE;
            site_.Add(act);
        }

        return true;
    }

    void Run(TaskRunner * runner)
    {
        // TODO:
        // for_each site in sites_
        // new task
        // ....
# if 0
        for (std::vector<Site>::iterator i = sites_.begin();
            i != sites_.end(); ++i)
        {
            Site & site = *i;

            SiteTask * task = new SiteTask(site, userinfo_, runner);

            task->AddAction(site_.Find(AT_REGISTER));

            runner->StartTask(task);
        }
#endif
        SiteTask * task = new SiteTask(site_, userinfo_, runner);

        task->AddAction(site_.Find(AT_REGISTER));

        runner->StartTask(task);
    }
private:
    UserInfo userinfo_;
    // TODO:
    std::vector<Site> sites_;
    Site site_;
};

