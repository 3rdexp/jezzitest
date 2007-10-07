
#pragma once

#include <vector>
#include <vector>
#include <string>

#include "coreconst.h"
#include "querymap.h"
#include "../base/httpcomm.h"

using std::string;
using std::wstring;

/*
加载 UserInfo

加载要访问的网站 SiteInfo
  LoadAction(ActionType) 从数据库填充 actions

处理数据后，执行 Get/Post
*/


struct UserInfo : public VariableMap
{
public:
    void insert(const std::wstring & key, const std::wstring & value)
    {
    }
};


struct ActionInfo;
struct SiteInfo;

struct ActionInfo
{
    ActionInfo()
        : aid(0), sid(0), front(0)
        , type(AT_UTILITY)
        , method(HV_POST)
        , charset(SC_ANSI)
        , timeout(0)
    {}
    virtual ~ActionInfo() {}
    int aid;
    int sid;
    ActionInfo * front; // 前置任务
    ActionType type;

    wstring url;
    HttpVerb method;      // HV_GET / HV_POST
    wstring form_encoding;
    SiteCharset charset;
    wstring vars;
    wstring referrer;
    wstring checked;

    ActionResponseType restype;
    int timeout;        // seconds
};

struct SiteInfo
{
    SiteInfo() : sid(0) {}
    virtual ~SiteInfo() {}
    int sid;
    wstring name, homepage;
};

//////////////////////////////////////////////////////////////////////////
//

class Site;
class Action;

class Task;

class Action : public ActionInfo
{
public:
    Action() : time(0) {}
    wstring result; // TODO: HTTP code, HTML string
    time_t time;
};

class Site : public SiteInfo
{
public:
    Site() : task_(0) {}
    std::vector<Action*> Find(ActionType type) const
    {
        std::vector<Action*> ret;
        for (std::vector<Action>::const_iterator i = actions_.begin();
            i != actions_.end(); ++i)
        {
            const Action * p = &*i;
            if (i->type == type)
                ret.push_back(const_cast<Action*>(p));
        }
        return ret;
    }
    void Add(const Action & act)
    {
        actions_.push_back(act);
    }
    void Add(std::vector<ActionInfo> & acts)
    {
        for(std::vector<ActionInfo>::const_iterator i=acts.begin();
            i!=acts.end(); ++i)
        {
            const ActionInfo & ai = *i;
            Action a;
            static_cast<ActionInfo &>(a) = ai;
            actions_.push_back(a);
        }
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
    std::vector<Action> & actions() { return actions_; }

    bool action_empty() const { return actions_.empty(); }
    const Dictionary & dict() const { return dict_; }

private:
    std::vector<Action> actions_;
    Dictionary dict_;

    wstring username_;
    wstring passwd_;
    Task * task_;
};
