#pragma once

#include <set>
#include <iterator>

#include "base/task.h"
#include "base/synchttp.h"
#include "data/mutabledata.h"
#include "coreinfo.h"


#if 1

class EnginePump: public TaskRunner {
public:
     virtual void WakeTasks();
     virtual int64 CurrentTime();
};

class NormalRequestTask : public Task {
public:  
    NormalRequestTask(Task * parent, Site & site) : 
      Task(parent)
      , done_(false) 
      , site_(site)
      {}
    virtual int ProcessStart();

    // void OnResponse(int status_code, const char * buf, int len);
    void OnResponse();

private:
    Action action;
    Site & site_;
    bool done_;
    SyncHttp http_;
};

class VerifyTask : public Task {
public:
    VerifyTask(Task * parent) 
        : Task(parent) 
        , done_(false)
    {}
    virtual int ProcessStart() {
        // Show UI(this);
        return STATE_BLOCKED;
    }

    void EnterVerifyCode(const std::string & code) {
        done_ = true;
        Wake();
    }
private:
    bool done_;
};


class EngineCrank {
public:
};





#else







// 类工厂
Action * CreateAction(const Action & a);

class SiteTask;
class EngineCrank;

//
typedef sigslot::signal2<SiteTask*, int, sigslot::multi_threaded_local> SignalStateChange;
typedef sigslot::signal2<SiteTask*, const std::wstring &, sigslot::multi_threaded_local> SignalVerifyCode;
typedef sigslot::signal3<SiteTask*, const Action *, bool> SignalActionResult;

class SiteTask : public AsyncTask
{
public:
    SiteTask(Site & site, const UserInfo & userinfo, TaskRunner * parent
        , SignalStateChange &, SignalVerifyCode &, SignalActionResult &);
    void AddAction(const std::vector<Action*> & acts)
    {
        std::copy(acts.begin(), acts.end(), std::back_inserter(actions_));
    }
    void AddAction(Action* act)
    {
        actions_.push_back(act);
    }

    Site & GetSite() { return site_; }

    void EnterVerifyCode(const std::wstring & code);
protected:
    virtual int ProcessStart();
    virtual int ProcessResponse();
    virtual int Process(int state);

    virtual void OnResponse();

    virtual std::string GetStateName(int state) const;

protected:
    enum {
        STATE_INPUT_VERIFYCODE = STATE_NEXT + 1,
        STATE_ACTION_DONE        = STATE_NEXT + 2,
    };

private:
    bool ProcessAction(Action * pa);
    bool PrepareForm(std::ostream & out, const std::wstring & vars
        , SiteCharset charset) const;

private:
    SignalStateChange & sigStateChange_;
    SignalVerifyCode & sigVerifyCode_;
    SignalActionResult & sigActionResult_;
    
    // 
    Site & site_;
    const UserInfo & userinfo_;
    std::vector<Action*> actions_; // 本 SiteTask 运行时需要完成的 Actions

    // 运行时临时信息
    int curact_;
    std::wstring verifycode_;
    std::wstring tmpfile_;

    int account_index_;
    std::wstring username_, passwd_;

    friend class EngineCrank;
};


//////////////////////////////////////////////////////////////////////////
//
class EngineCrank : public sigslot::has_slots<sigslot::multi_threaded_local>
{
public:
    EngineCrank(MutableData & md, TaskRunner * runner) : md_(md)
        , runner_(runner)
    {}
    
    bool Init()
    {
/*
        {
            userinfo_.insert(L"sex", L"male");
            userinfo_.insert(L"pasw", L"strongpsw");
            userinfo_.insert(L"mail", L"boostguy@gmail.com");
        }


        // baidu.com
        {
            Site site;            
            Dictionary dict;
            {
                VariableMap vm;
                vm.insert(VariableMap::value_type(L"male", L"1"));
                vm.insert(VariableMap::value_type(L"female", L"2"));
                dict.Insert(L"sex", vm);
            }
            site.SetDict(dict);

            site.homepage = L"http://baidu.com";
            // site.industries = L"all";
            {
                Action act;
                act.aid = 0;
                act.type = AT_REGISTER;
                act.url = L"https://passport.baidu.com/?verifypic";
                act.method = HV_GET;
                act.restype = ART_VERIFY_IMAGE;
                site.Add(act);
            }

            {
                Action act;
                act.aid = 1;
                act.url = L"https://passport.baidu.com/?reg";
                act.method = HV_POST;
                act.vars = L"tpl=&tpl_ok=&skip_ok=&aid=0&need_pay=&need_coin=0&pay_method=0&u=&next_target=&return_method=&next_type=&more_param=&username={loginname}&loginpass={pasw}&verifypass={pasw}&sex={sex}&email=&verifycode={verifycode}&space_flag=on&submit= 同意";
                act.type = AT_REGISTER;
                act.restype = ART_NONE;
                site.Add(act);
            }

            sites_.push_back(site);
        }

        // sohu.com
        // http://db.b2b.sohu.com/qy/logon/Logon_free.html
        // username={username}&password={passwd}&repassword={passwd}&contact={contract}&email={mail}&PhoneCountry=86&PhoneArea={phone-pre}&PhoneNumber={phone}&faxCountry=86&faxArea={fax-pre}&faxNumber={fax}&mobile={mobile}&zipcode={post}&companyname={name}&classname=表面处理/电镀&classkey=p031002&=选择行业类目&provinceCN=p101002&capitalCN=p101002019&cityCN=p101002019001&select=&CountryListHot=CN&CountryList=CN&province1=&province2=&address={address}
        {
            Site site;
            Dictionary dict;
            {
                {
                    VariableMap vm;
                    vm.insert(VariableMap::value_type(L"male", L"1"));
                    vm.insert(VariableMap::value_type(L"female", L"2"));
                    dict.Insert(L"sex", vm);
                }

                {   
#define make_province(code, province) vm.insert(VariableMap::value_type(province, code));
                    VariableMap vm;
                    make_province(L"p101001", L"安徽")
                    make_province(L"p101002", L"北京")

                    make_province(L"p101003", L"海南")
                    make_province(L"p101004", L"福建")
                    make_province(L"p101005", L"甘肃")
                    make_province(L"p101006", L"广东")
                    make_province(L"p101007", L"广西")
                    make_province(L"p101008", L"贵州")

                    make_province(L"p101010", L"河北")
                    make_province(L"p101011", L"黑龙江")
                    make_province(L"p101012", L"河南")
                    make_province(L"p101013", L"香港")
                    make_province(L"p101014", L"湖北")
                    make_province(L"p101015", L"湖南")

                    make_province(L"p101016", L"江苏")
                    make_province(L"p101017", L"江西")
                    make_province(L"p101018", L"吉林")
                    make_province(L"p101019", L"辽宁")
                    make_province(L"p101020", L"澳门")
                    make_province(L"p101021", L"内蒙古")

                    make_province(L"p101022", L"宁夏")
                    make_province(L"p101024", L"青海")
                    make_province(L"p101025", L"陕西")
                    make_province(L"p101026", L"山东")
                    make_province(L"p101027", L"上海")
                    make_province(L"p101028", L"山西")

                    make_province(L"p101029", L"四川")
                    make_province(L"p101030", L"台湾")
                    make_province(L"p101031", L"天津")
                    make_province(L"p101033", L"新疆")
                    make_province(L"p101034", L"西藏")
                    make_province(L"p101035", L"云南")

                    make_province(L"p101036", L"浙江")
                    make_province(L"p101037", L"重庆")
#undef make_province

                    dict.Insert(L"province", vm);    
                }
            }
            site.SetDict(dict);

            {
                site.homepage = L"b2b.sohu.com";
                {
                    Action act;
                    act.aid = 0;
                    act.type = AT_REGISTER;
                    act.url = L"http://db.b2b.sohu.com/qy/logon/Logon_free.html";
                    act.method = HV_POST;
                    act.restype = ART_CHECK_RESULT;
                    act.checked = L"成功";
                    site.Add(act);
                }
            }

            sites_.push_back(site);
            return true;
        
*/
        return true;
    }

    void Add(Site & site);

    // 
    void OnActionResult(SiteTask*, const Action *, bool);

    // 
    void OnSiteRegister(Site & site, bool success);
    void OnSiteLogin(Site & site, bool success);
    void OnPublish();

    SignalStateChange SigStateChange;
    SignalVerifyCode SigVerifyCode;
    SignalActionResult SigActionResult;

private:
    MutableData & md_;
    std::set<Site*> sites_;
    TaskRunner * runner_;
};

#endif
