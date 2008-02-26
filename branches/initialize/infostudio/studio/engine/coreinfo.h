
#pragma once

#include <vector>
#include <string>

#include "base/httpcomm.h"
#include "coreconst.h"
#include "querymap.h"

// 如何表示多组 user/passwd 呢？是个难题
// TODO: maybe multimap
struct UserInfo : public std::multimap<std::wstring, std::wstring>
{
    typedef std::multimap<std::wstring, std::wstring> base_t;
public:
    void insert(const std::wstring & key, const std::wstring & value)
    {
        base_t::insert(value_type(key, value));
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

    std::wstring & operator[](const std::wstring & key)
    {
        static std::wstring w;
        return w;
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

class Site
{
public:
    Site(int sid = 0) : sid(sid) {}

    void AddAction(Action * beg, Action * end)
    {
        // std::copy(beg, end, std::back_inserter(actions_));
        while (beg != end)
            actions_.push_back(*(beg++));
    }
    void ClearAction()
    {
        actions_.clear();
    }

    void SetDict(const Dictionary & dict)
    {
        dict_ = dict;
    }
    
    std::vector<Action> & actions() { return actions_; }
    const Dictionary & dict() const { return dict_; }


    int sid;
    std::wstring name, homepage;

    std::vector<Action> actions_;
    Dictionary dict_;

    std::wstring username_;
    std::wstring passwd_;
};

//////////////////////////////////////////////////////////////////////////
// 发布的内容
struct PublishContent
{
    PublishContent(int cid = 0) : cid(cid) {}

    int cid;
    std::wstring title, keywords, content;
    int expire, frequency; // in seconds
    
    std::vector<int> sites;
};

// 一次发布中，对某个站点产生的结果
struct PublishResult
{
    PublishResult(int rid = 0, Site * site = 0) : rid(rid), site(site) {}

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
    Publish(PublishContent * c = 0) : content(c), pubid(0)
        , start(0), end(0)
    {}

    int pubid;
    time_t start,end;
    std::wstring name;

    PublishContent * content;
    std::vector<PublishResult> resutl;
};


