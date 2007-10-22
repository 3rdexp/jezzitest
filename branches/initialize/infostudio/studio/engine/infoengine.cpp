
#include "stdafx.h"

#include <sstream>
#include <fstream>
#include <iomanip>
#include <iterator>

#include "base/logging.h"

#include "resource.h"
#include "verifyimgdlg.h"
#include "infoengine.h"


//////////////////////////////////////////////////////////////////////////
// temp code
HWND GetVerifyWindow()
{
    ASSERT(0);
    // extern VerifyImgDlg * gvidlg;
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

    bool f = StartAction(pa);
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
    bool f = false;
    Action * pa = actions_[curact_];
    switch (pa->restype)
    {
    case ART_VERIFY_IMAGE:
        ASSERT(false); // 在OnResponse中处理了
        return STATE_BLOCKED;
    case ART_NONE:
        f = true;
        break;
    case ART_CHECK_RESULT:
        {
            const std::vector<char> & buf = getContent();
            std::string ret(buf.begin(), buf.end());

            // TODO: charset
            std::string::size_type pos = ret.find(w2string(pa->result));
            LOG(LS_VERBOSE) << "check:" << pos;
            if (pos != std::string::npos)
                f = false;

            // 还是应该在外部保存到数据库
            if (f && pa->type == AT_REGISTER)
            {
            }
        }
        break;
    default:
        break;
    }
    sigActionResult_(this, pa, f);
    return STATE_ACTION_DONE;
}

int SiteTask::Process(int state)
{
    int n;
    if (STATE_ACTION_DONE == state)
        n = ProcessNextAction();
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
    else
    {
        SetState(STATE_RESPONSE);
        sigStateChange_(this, STATE_RESPONSE);
        Wake();
    }
}

std::string SiteTask::GetStateName(int state) const
{
    switch (state) {
    case STATE_INPUT_VERIFYCODE: return "INPUT VERIFYCODE";
    case STATE_ACTION_DONE: return "ACTION OVER";
    }
    return AsyncTask::GetStateName(state);
}

int SiteTask::ProcessNextAction()
{
    curact_ ++;
    if (curact_ == actions_.size())
        return STATE_DONE;

    Action * pa = actions_[curact_];
    bool f = StartAction(pa);
    return f ? STATE_BLOCKED : STATE_ERROR;
}

bool SiteTask::StartAction(Action * pa)
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
    return f;
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
    
    std::vector<Action> & a= site.actions();
    ASSERT(!a.empty());
    for (std::vector<Action>::iterator i=a.begin(); i != a.end(); ++i)
    {
        task->AddAction(&*i);
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
#error tomorow here
    }
}