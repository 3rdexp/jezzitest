
#pragma once

#include <set>
#include <iterator>

#include "../base/asynctask.h"
#include "coreinfo.h"
#include "data/mutabledata.h"

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
    int ProcessNextAction();
    bool StartAction(Action * pa);

    bool PrepareForm(std::ostream & out, const std::wstring & vars
        , SiteCharset charset) const;
public:// private:
    std::vector<Action*> actions_;
    int curact_;
    Site & site_;
    const UserInfo & userinfo_;
    std::wstring verifycode_;
    std::wstring tmpfile_;
    SignalStateChange & sigStateChange_;
    SignalVerifyCode & sigVerifyCode_;
    SignalActionResult & sigActionResult_;
};

#if 0
struct TaskNotify
{
    virtual void StateChange(SiteTask * task, int state) = 0;
    virtual void VerifyCode(SiteTask* task, const std::wstring & imgfn) = 0;
};
#endif

//////////////////////////////////////////////////////////////////////////
//
class EngineCrank : public sigslot::has_slots<sigslot::multi_threaded_local>
{
public:
    EngineCrank(MutableData & md, TaskRunner * runner) : md_(md)
        , runner_(runner)
    {}
    // input: vector<sid> ?, ActionType
    bool Init()
    {
#if 0
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
                act.vars = L"tpl=&tpl_ok=&skip_ok=&aid=0&need_pay=&need_coin=0&pay_method=0&u=&next_target=&return_method=&next_type=&more_param=&username={loginname}&loginpass={pasw}&verifypass={pasw}&sex={sex}&email=&verifycode={verifycode}&space_flag=on&submit= ͬ��";
                act.type = AT_REGISTER;
                act.restype = ART_NONE;
                site.Add(act);
            }

            sites_.push_back(site);
        }

        // sohu.com
        // http://db.b2b.sohu.com/qy/logon/Logon_free.html
        // username={username}&password={passwd}&repassword={passwd}&contact={contract}&email={mail}&PhoneCountry=86&PhoneArea={phone-pre}&PhoneNumber={phone}&faxCountry=86&faxArea={fax-pre}&faxNumber={fax}&mobile={mobile}&zipcode={post}&companyname={name}&classname=���洦��/���&classkey=p031002&=ѡ����ҵ��Ŀ&provinceCN=p101002&capitalCN=p101002019&cityCN=p101002019001&select=&CountryListHot=CN&CountryList=CN&province1=&province2=&address={address}
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
                    make_province(L"p101001", L"����")
                    make_province(L"p101002", L"����")

                    make_province(L"p101003", L"����")
                    make_province(L"p101004", L"����")
                    make_province(L"p101005", L"����")
                    make_province(L"p101006", L"�㶫")
                    make_province(L"p101007", L"����")
                    make_province(L"p101008", L"����")

                    make_province(L"p101010", L"�ӱ�")
                    make_province(L"p101011", L"������")
                    make_province(L"p101012", L"����")
                    make_province(L"p101013", L"���")
                    make_province(L"p101014", L"����")
                    make_province(L"p101015", L"����")

                    make_province(L"p101016", L"����")
                    make_province(L"p101017", L"����")
                    make_province(L"p101018", L"����")
                    make_province(L"p101019", L"����")
                    make_province(L"p101020", L"����")
                    make_province(L"p101021", L"���ɹ�")

                    make_province(L"p101022", L"����")
                    make_province(L"p101024", L"�ຣ")
                    make_province(L"p101025", L"����")
                    make_province(L"p101026", L"ɽ��")
                    make_province(L"p101027", L"�Ϻ�")
                    make_province(L"p101028", L"ɽ��")

                    make_province(L"p101029", L"�Ĵ�")
                    make_province(L"p101030", L"̨��")
                    make_province(L"p101031", L"���")
                    make_province(L"p101033", L"�½�")
                    make_province(L"p101034", L"����")
                    make_province(L"p101035", L"����")

                    make_province(L"p101036", L"�㽭")
                    make_province(L"p101037", L"����")
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
                    act.checked = L"�ɹ�";
                    site.Add(act);
                }
            }

            sites_.push_back(site);
            return true;
        
#endif
        return true;
    }

    void Add(Site & site);

    void OnActionResult(SiteTask*, const Action *, bool);

    // TODO: lock
#if 0
    void SetTaskNotify(TaskNotify * p) { notifier_ = p; }
#else
    SignalStateChange SigStateChange;
    SignalVerifyCode SigVerifyCode;
    SignalActionResult SigActionResult;
#endif
private:
    MutableData & md_;
    std::set<Site*> sites_;
    TaskRunner * runner_;
};
