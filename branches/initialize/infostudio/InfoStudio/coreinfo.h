#pragma once

#include <map>
#include <vector>
#include <list>
#include <string>

#include "coreconst.h"

using std::string;
using std::wstring;

/*
���� UserInfo

����Ҫ���ʵ���վ SiteInfo
  LoadAction(ActionType) �����ݿ���� actions
  

�������ݺ�ִ�� Get/Post
*/

/**********************************************************************
PART-1  ¼������
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
  checked   // ����Ƿ�ɹ��ȱ�־

FieldValue
  name
  domain
  value

DomainWord
  name
  domain
  option

**********************************************************************
PART-2 �û�����ǰ�������
UserInfo
  name
  value

**********************************************************************
PART-2 ����Task�ڼ����Ϣά���õ�
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
    ActionInfo * front; // ǰ������
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


