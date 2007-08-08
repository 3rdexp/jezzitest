
#pragma once

#include <vector>
#include <list>
#include <string>

#include "coreconst.h"
#include "querymap.h"
#include "engine/httpcomm.h"

using std::string;
using std::wstring;

/*
加载 UserInfo

加载要访问的网站 SiteInfo
  LoadAction(ActionType) 从数据库填充 actions

处理数据后，执行 Get/Post
*/

/**********************************************************************
PART-1  录入数据
SiteInfo
  sid        key
  homepage  : string
  industry : string;


ActionInfo
  aid
  sid
  front_aid
  type
  url
  method
  form_encoding
  charset
  vars
  referrer
  checked       // 检查是否成功等标志
  verify_url    // 验证码地址
  verify_code

FieldValue
  name
  domain
  value

DomainWord
  name
  domain
  option

**********************************************************************
PART-2 用户运行前填充数据
UserInfo
  name
  value

**********************************************************************
PART-2 运行Task期间的信息维护用的
SiteSpecial : public SiteInfo
  ssid
  sid
  login_name
  passwd

ActionSpecial : public ActionInfo
  asid
  aid
  result
  time

**********************************************************************/
struct UserInfo : public VariableMap
{
public:
};


struct ActionInfo;
struct SiteInfo;

struct ActionInfo
{
    ActionInfo() 
        : aid(0), site(0), front(0)
        , type(AT_UTILITY)
        , method(HV_POST)
        , charset(SC_ANSI)
    {
    }
//    ActionInfo(int aid, SiteInfo* site)
//        : aid(aid), site(site)
//    {
//    }
    virtual ~ActionInfo() {}
    int aid;
    SiteInfo* site;
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
    string homepage;
    std::vector<wstring> industries;
};


//////////////////////////////////////////////////////////////////////////
//
class SiteSpecial : public SiteInfo
{
public:
    std::vector<ActionInfo*> find(ActionType type);

private:
    std::list<ActionInfo> actions_;

    wstring username_;
    wstring passwd_;
};


class ActionSpecial : public ActionInfo
{
public:
    wstring result; // TODO: enum
    time_t time;
};




//struct Industry
//{
//    Industry() 
//        : parent(0)
//        , id(-1) 
//    {}
//    Industry * parent;
//    list<Industry> children;
//
//    int id;
//    wstring cname;
//    wstring ename;
//
//    Industry * find();
//};

//////////////////////////////////////////////////////////////////////////
//


