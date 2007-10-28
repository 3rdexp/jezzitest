
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
    int account_count() const;

private:
    UserInfo(const UserInfo&);
    UserInfo& operator=(const UserInfo&);

    // 变通之法
    // user1= psw1=
    // user2= psw2=
    mutable int ac_;    // account count
};

struct Action;
struct SiteInfo;

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
    HttpVerb method;        // HV_GET / HV_POST
    std::wstring content_type;   // always application/x-www-form-urlencoded
    SiteCharset charset;
    std::wstring vars;           // key={val}
    std::wstring referrer;
    std::wstring checked;

    ActionResponseType restype;
    int timeout;        // in seconds

    // 
    std::wstring result; // HTTP status code
    time_t time;
};

struct SiteInfo
{
    SiteInfo() : sid(0) {}
    virtual ~SiteInfo() {}
    int sid;
    std::wstring name, homepage;
};

//////////////////////////////////////////////////////////////////////////
//

class Site;
class Task;

class Site : public SiteInfo
{
public:
    Site() : task_(0) {}
//     std::vector<Action*> Find(ActionType type) const
//     {
//         std::vector<Action*> ret;
//         for (std::vector<Action>::const_iterator i = actions_.begin();
//             i != actions_.end(); ++i)
//         {
//             const Action * p = &*i;
//             if (i->type == type)
//                 ret.push_back(const_cast<Action*>(p));
//         }
//         return ret;
//     }
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
        // dict_.swap(dict);
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
    std::vector<Action*> actions_;
    Dictionary dict_;

    std::wstring username_;
    std::wstring passwd_;

    Task * task_;   // TODO: remove

    friend class EngineCrank; // TODO: remove
    friend class SiteTask;
};

struct Publish
{
    std::wstring title, keywords, content;
    int expire, frequency; // in seconds

    int pubid;
    std::vector<int> sites;

    Publish() : pubid(0) {}
};

struct Result
{
    int rid;
    int did; // 附加数据，如Publish pubid

    int sid;
    ActionType type;
    
    time_t time;
    std::wstring content; // ? remove
    bool success;

    Result() : rid(0) {}
};
