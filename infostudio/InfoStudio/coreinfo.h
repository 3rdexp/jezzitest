#pragma once

#include <map>
#include <vector>
#include <list>
#include <string>

#include "coreconst.h"

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
  checked   // 检查是否成功等标志

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
    virtual ~ActionInfo() {}
    int aid;
    SiteInfo* site;
    ActionInfo * front; // 前置任务
    ActionType type;
    string url;
    HttpVerb method;      // HV_GET / HV_POST
    string form_encoding;
    SiteCharset charset;
    wstring vars;
    string referrer;
    wstring checked;
};

struct SiteInfo
{
    virtual ~SiteInfo() {}
    int sid;
    string homepage;
    vector<wstring> industries;
};

struct FieldValue
{
    wstring name;
    wstring domain;
    vector<wstring> option;
};



//////////////////////////////////////////////////////////////////////////
//
class SiteSpecial ; public SiteInfo
{
public:
    vector<ActionInfo*> find(ActionType type);

private:
    list<ActionInfo> actions_;

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


