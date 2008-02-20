
#pragma once

#include <vector>
#include <string>

#include "../base/httpcomm.h"
#include "coreconst.h"
#include "querymap.h"

struct UserInfo : public VariableMap
{
public:
    void insert(const std::wstring & key, const std::wstring & value)
    {
        VariableMap::insert(value_type(key, value));
    }

    UserInfo() : ac_(0) {}

    bool ready() const;
    int account_count() const { return name_.size(); }
    void account_index(int index);
    void add_account(const std::wstring & name, const std::wstring & passwd)
    {
        name_.push_back(name);
        passwd_.push_back(passwd);
    }

private:
    UserInfo(const UserInfo&);
    UserInfo& operator=(const UserInfo&);

    int ac_;    // account count
    std::vector<std::wstring> name_, passwd_;
};

struct Action
{
    Action() : aid(0), sid(0), pre_aid(0)
        , type(AT_UTILITY)
        , method(HV_POST), charset(SC_ANSI)
        , timeout(0), time(0)
    {}
    virtual ~Action() {}
    int aid;
    int sid;
    int pre_aid; // 前置任务
    ActionType type;

    std::wstring url;
    HttpVerb method;            // HV_GET / HV_POST
    std::wstring content_type;  // always application/x-www-form-urlencoded
    SiteCharset charset;
    std::wstring vars;          // key={val}
    std::wstring referrer;
    std::wstring checked;

    ActionResponseType restype;
    int timeout;                // in seconds

    std::wstring result;        // HTTP status code
    time_t time;
};

class Task;

class Site
{
public:
    Site() : sid(0)
        , task_(0)
    {}

    void Add(Action* act)
    {
        actions_.push_back(act);
    }
    void Add(const std::vector<Action*> & acts)
    {
        std::copy(acts.begin(), acts.end(), std::back_inserter(actions_));
    }
    void SetDict(Dictionary & dict)
    {
        dict_ = dict;
    }
    void SetTask(Task* task)
    {
        ASSERT(!task_ && task);
        task_ = task;
    }
    std::vector<Action*> & actions() { return actions_; }

    bool action_empty() const { return actions_.empty(); }
    const Dictionary & dict() const { return dict_; }

private:
    int sid;
    std::wstring name, homepage;

    std::vector<Action*> actions_;
    Dictionary dict_;

    std::wstring username_;
    std::wstring passwd_;

    Task * task_;

    friend class EngineCrank; // TODO: remove
    friend class SiteTask;
};

//////////////////////////////////////////////////////////////////////////
// 发布的内容
struct PublishContent
{
    PublishContent() : cid(0) {}

    int cid;
    std::wstring title, keywords, content;
    int expire, frequency; // in seconds
    
    std::vector<int> sites;
};

// 一次发布中，对某个站点产生的结果
struct PublishResult
{
    PublishResult() : rid(0), site(0) {}

    int rid;
    Site * site;
    ActionType type;

    time_t time;
    std::wstring content; // remove ?
    bool success;
};

// 执行一次发布
struct Publish
{
    Publish(PublishContent * c) : content(c), pubid(0)
        , start(0), end(0)
    {}

    int pubid;
    time_t start,end;
    std::wstring name;

    PublishContent * content;
    std::vector<PublishResult> resutl;
};


struct ActionPunch
{
    virtual ~ActionPunch() {}
    
    Site * site_;
    Action* action_;
    UserInfo * user_;
    
    virtual void Start(AsyncHttp *)
    {
        // QueryMap(action->vars)
        // QueryMap::Expand(userinfo, site->dict, action->charset)
        // PrepareGet/Post(action->url, form);
    }

    virtual bool OnResponse(AsyncHttp *)
    {
        // ---- Verify image ----
        // write to file
        // sigVerifCode ???
        // return false

        // ---- Register ----
        // save register info

        // ---- post ----
        // Action + PublishContent => form
        // new PublishResult
        return false;
    }
};

struct ActionUtility : public ActionPunch
{
};

struct ActionRegister : public ActionPunch
{
    ActionRegister(Site * s) : ActionPunch(s) 
    {}
};

// 
// Site + Action + content(userinfo, PublishContent)
// + Task
// + Runner in Application Context
