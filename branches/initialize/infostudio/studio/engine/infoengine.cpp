
#include "stdafx.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iterator>

#include <boost/bind.hpp>

#include "base/logging.h"

#include "resource.h"
#include "verifyimgdlg.h"
#include "infoengine.h"


#if 1
//////////////////////////////////////////////////////////////////////////
// 

int GeneralTask::ProcessStart() {
    if (sent_)
        return STATE_RESPONSE;

    http_.SetCallback(boost::bind(&GeneralTask::OnHttpResponse, this, _1, _2, _3));
    BuildRequest(http_);
    
    return STATE_BLOCKED;
}

int GeneralTask::ProcessResponse() {
    Assert(sent_);
    return STATE_DONE;
}

void GeneralTask::OnHttpResponse(int status_code, const char * buf, int len) {
    sent_ = true;
    GotResponse(status_code, buf, len); // here?
    Wake();
}

//////////////////////////////////////////////////////////////////////////
//

int SiteTask::ProcessResponse() {
    if (curact_ == site_.actions().size() - 1)
        return STATE_DONE;

    return STATE_START;
}

void SiteTask::BuildRequest(SyncHttp & http) {
    const Action & action_ = site_.actions()[++curact_];
    switch (action_.method) {
    case HV_GET :
        {
            std::wstring url(action_.url);
            if (!action_.vars.empty()) {
                std::ostringstream ss;
                bool f = PrepareForm(ss, action_);
                Assert(f);
                url += L"?";
                // url += ss.str();
            }
            // TODO: url escape
            bool f = http.PrepareGet(url, action_.referrer);
            Assert(f);
        }
        break;
    case HV_POST :
        {
            Assert(!action_.vars.empty());
            std::stringstream ss;
            bool f = PrepareForm(ss, action_);
            Assert(f);
            f = http.PreparePost(action_.url, action_.content_type
                , &ss
                , action_.referrer);
            Assert(f);
        }
        break;
    default:
        Assert(false);
        break;
    }
}

bool SiteTask::PrepareForm(std::ostream & out, const Action & action) const {
    // userinfo_, action_.vars, charset
    // site_.passwd username

    VariableMap vm = userinfo_;
    // Site special data
    if (!site_.username().empty()) {
        // vm.insert("", site_.username());
        // vm.insert("", site_.passwd());
    }

    QueryMap qm(action.vars);
    out << qm.Expand(vm, site_.dict(), action.charset);
    return true;
}

#else

struct UtilityAction : public Action
{
    // TODO: 这样写对吗？ 或者有更好的办法
    UtilityAction(const Action & a)
    {
        static_cast<Action&>(*this) = a;
    }
};

struct RegisterAction : public Action
{
    RegisterAction(const Action & a)
    {
        static_cast<Action&>(*this) = a;
    }
};

struct PublishAction : public Action
{
    PublishAction(const Action & a)
    {
        static_cast<Action&>(*this) = a;
    }
};

Action * CreateAction(const Action & a)
{
    if (a.type == AT_REGISTER)
        return new RegisterAction(a);
    else if (a.type == AT_UTILITY)
        return new UtilityAction(a);
    else if (a.type == AT_PUBLISH)
        return new PublishAction(a);

    ASSERT(false);
    return 0;
}

//////////////////////////////////////////////////////////////////////////
//
SiteTask::SiteTask(Site & site, const UserInfo & userinfo, TaskRunner * parent
    , SignalStateChange & sc, SignalVerifyCode & vc, SignalActionResult & ar)
    : site_(site), userinfo_(userinfo)
    , AsyncTask(parent)
    , curact_(0)
    , sigStateChange_(sc), sigVerifyCode_(vc)
    , sigActionResult_(ar)
    , account_index_(0)
{}

void SiteTask::EnterVerifyCode(const std::wstring & code)
{
    ASSERT(STATE_INPUT_VERIFYCODE == GetState());
    verifycode_ = code;
    SetState(STATE_ACTION_DONE);
    Wake();

    ATLASSERT(!tmpfile_.empty());
    BOOL f = DeleteFile(tmpfile_.c_str());
    ASSERT(f);
    tmpfile_.clear();
}

int SiteTask::ProcessStart()
{
    curact_ = 0;
    Action * pa = actions_[curact_];

    bool f = ProcessAction(pa);
    if (f && pa->restype == ART_VERIFY_IMAGE)
    {
        SetState(STATE_INPUT_VERIFYCODE);
        sigStateChange_(this, STATE_INPUT_VERIFYCODE);
    }
    return f ? STATE_BLOCKED : STATE_ERROR;
}

int SiteTask::ProcessResponse()
{
    LOG_F(INFO) << "curact_:" << curact_;
    Action * pa = actions_[curact_];
    switch (pa->restype)
    {
    case ART_VERIFY_IMAGE:
        ASSERT(false); // 在OnResponse中处理了
        return STATE_BLOCKED;
    case ART_NONE:
        sigActionResult_(this, pa, true);
        break;
    case ART_CHECK_RESULT:
        {
            const std::vector<char> & buf = getContent();
            std::string ret(buf.begin(), buf.end());

            bool success = false;
            std::string::size_type pos = std::string::npos;
            if (pa->charset == SC_ANSI)
                pos = ret.find(w2string(pa->checked));
            else if (pa->charset == SC_UTF8)
                pos = ret.find(string2utf8(pa->checked));
#if 1
            std::ofstream flog("log", std::ios::binary);
            flog << ret;
#endif
            LOG(LS_VERBOSE) << "check:" << pos;
            if (pos != std::string::npos)
                success = true;

            if (pa->type == AT_REGISTER)
            {
                if (success)
                {
                    // TODO: read username via account_index_
                    site_.username_ = username_;
                    site_.passwd_ = passwd_;

                    sigActionResult_(this, pa, true);
                }
                else
                {
                    curact_ --;
                    account_index_ ++;
                    if (userinfo_.account_count() == account_index_)
                        success = false;

                    sigActionResult_(this, pa, success);

                    return success ? STATE_ACTION_DONE : STATE_ERROR;
                }
            }
        }
        break;
    default:
        break;
    }
    
    return STATE_ACTION_DONE;
}

int SiteTask::Process(int state)
{
    int n = 0;
    if (STATE_ACTION_DONE == state)
    {
        curact_ ++;
        if (curact_ == actions_.size())
            return STATE_DONE;

        Action * pa = actions_[curact_];
        bool f = ProcessAction(pa);
        n = (f ? STATE_BLOCKED : STATE_ERROR);
    }
    else
        n = AsyncTask::Process(state);

    return n;
}

void SiteTask::OnResponse()
{
    LOG_F(INFO) << GetStateName(GetState());
    
    if (GetState() == STATE_INPUT_VERIFYCODE)
    {
        Action * pa = actions_[curact_];

        // 加入辨认验证码的队列 。。。。
        // state_ 依然是 Blocked

        // write buf_ to file
        TCHAR temppath[MAX_PATH], filename[MAX_PATH];
        GetTempPath(MAX_PATH - 1, temppath);
        GetTempFileName(temppath, _T("IST_"), 0, filename);

        tmpfile_ = filename;

        std::ofstream outf_((const char*)CT2A(filename), std::ios::binary);

        std::copy(buf_.begin(), buf_.end(), std::ostream_iterator<char>(outf_));
        outf_.close();

#if 0
        CWindow wnd = GetVerifyWindow();
        wnd.SendMessage(WM_ADDIMAGE, (WPARAM)filename, (LPARAM)this);
#else
        sigVerifyCode_(this, tmpfile_);
#endif

        if (pa->timeout)
            set_timeout_seconds(pa->timeout);
    }
    
    SetState(STATE_RESPONSE);
    sigStateChange_(this, STATE_RESPONSE);
    Wake();
}

std::string SiteTask::GetStateName(int state) const
{
    switch (state) {
    case STATE_INPUT_VERIFYCODE: return "INPUT VERIFYCODE";
    case STATE_ACTION_DONE: return "ACTION OVER";
    }
    return AsyncTask::GetStateName(state);
}

bool SiteTask::ProcessAction(Action * pa)
{
    std::stringstream ss;
    bool f = PrepareForm(ss, pa->vars, pa->charset);
    if (f)
    {
        if (pa->method == HV_POST)
            f = PreparePost(pa->url, pa->content_type, &ss, pa->referrer);
        else if (pa->method == HV_GET)
            f = PrepareGet(pa->url, pa->referrer);
    }

    if (f)
    {
        SendRequest();
        if (pa->timeout)
            set_timeout_seconds(pa->timeout);
    }
    return f; // 
}

bool SiteTask::PrepareForm(std::ostream & out, const std::wstring & vars, SiteCharset charset) const
{
    std::string s;
    
    if (!verifycode_.empty())
    {
        VariableMap vm = userinfo_;
        vm.insert(VariableMap::value_type(L"vc", verifycode_));
        QueryMap qm(vars);
        s = qm.Expand(vm, site_.dict(), charset);

        // TODO: remove verifycode_ ?
        // verifycode_.clear();
    }
    else
    {
        QueryMap qm(vars);
        s = qm.Expand(userinfo_, site_.dict(), charset);
    }
     
    out << s;
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
void EngineCrank::Add(Site & site)
{
    ASSERT(runner_);
    
    SiteTask * task = new SiteTask(site,  md_.GetUserInfo(),  runner_
        , SigStateChange, SigVerifyCode, SigActionResult);
    
    std::vector<Action*> & a= site.actions();
    ASSERT(!a.empty());
    for (std::vector<Action*>::iterator i=a.begin(); i != a.end(); ++i)
    {
        task->AddAction(*i);
    }

    LOG(LS_VERBOSE) << "crank start site:" << site.sid << " task:" << std::hex << static_cast<void*>(task);

    site.SetTask(task);
    runner_->StartTask(task);
}

void EngineCrank::OnActionResult(SiteTask * task, const Action * pa, bool success)
{
    LOG(LS_VERBOSE) << "task:" << std::hex << static_cast<void*>(task)
        << " action:" << pa->type << " result:" << success;

    if (success)
    {
        // Save to ...
        if (pa->type == AT_REGISTER)
        {
            OnSiteRegister(task->site_, success);
        }
    }
}

void EngineCrank::OnSiteRegister(Site & site, bool success)
{
    UserInfo & u = md_.GetUserInfo();
//    task->site_.username_ = u[L"user"];
//    task->site_.passwd_ = u[L"psw"];
}
#endif