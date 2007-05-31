
#if(defined(_DEBUG) && defined(_CONSOLE))
#define CONSOLE_TEST
#endif

#ifdef CONSOLE_TEST
#include <iostream>
#endif

#include "HttpSession.h"


namespace tting {
///////////////////////////////////////////////////////////////////////////////
// HttpTask
///////////////////////////////////////////////////////////////////////////////
HttpTask::HttpTask(const std::string & agent) 
        : client_(agent, 0)
        , complete_(false), err_(0)
{
}

HttpTask::~HttpTask()
{
    ASSERT(done());
    // if (!done())
}

#ifdef CONSOLE_TEST
static void DumpResponse(talk_base::HttpResponseData& r, talk_base::HttpRequestData & q)
{
    using std::cout;
    using std::endl;

    cout << "code:" << r.scode << "\nheader:\n";

    talk_base::HttpData::const_iterator i = r.begin();
    for (; i!=r.end(); ++i)
    {
        cout << (*i).first << " : " << (*i).second << endl;
    }

    talk_base::MemoryStream *stream = static_cast<talk_base::MemoryStream*>(r.document.get());
    stream->Rewind();
    size_t length;
    stream->GetSize(&length);
    cout << "content length:" << length << "\ndata:\n";

    if (r.scode != 200)
    {
        if (length)
            cout.write(stream->GetBuffer(), length);
        else
            cout << q.path;
    }
}
#endif

void HttpTask::OnTaskComplete(int error)
{
    if (error == 0)
    {
#ifdef CONSOLE_TEST
        DumpResponse(client().response(), client().request());
#endif
    }
}

void HttpTask::OnHttpHeaderAvailable(const talk_base::HttpClient*,bool,size_t)
{
    // do nothing
}
void HttpTask::OnHttpComplete(talk_base::HttpClient* c,int err)
{
    ASSERT(c == &client_);
    complete_ = true;
    err_ = err;

    SignalTaskClose(this);
    
    OnTaskComplete(err);
}
void HttpTask::OnHttpClosed(talk_base::HttpClient* c, int err)
{
    ASSERT(c == &client_);
    err_ = err;

    SignalTaskClose(this);
    OnTaskComplete(err);
}

void HttpTask::ConnectSignal()
{
    client_.SignalHeaderAvailable.connect(this, &HttpTask::OnHttpHeaderAvailable);
    client_.SignalHttpClientComplete.connect(this, &HttpTask::OnHttpComplete);
    client_.SignalHttpClientClosed.connect(this, &HttpTask::OnHttpClosed);
}

///////////////////////////////////////////////////////////////////////////////
// HttpDownloadFile
///////////////////////////////////////////////////////////////////////////////
bool HttpDownloadFile::Download(const std::string & url, const std::string & path)
{
    talk_base::FileStream * stream = new talk_base::FileStream();
    if (stream->Open(path, "wb"))
    {
        client().prepare_get(url);
        client().response().document.reset(stream);
        return true;
    }
    delete stream;
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// HttpSession
///////////////////////////////////////////////////////////////////////////////

enum {
    MSG_HS_START,
    MSG_HS_CANCELTASK,
    MSG_HS_QUIT, 
    MSG_HS_DELETE,
};

HttpSession::HttpSession(bool manage_task)
    : pool_(new talk_base::NewSocketPool(&ss_))
    , workthread_(0)
    , manage_(manage_task) // false
{}

HttpSession::~HttpSession()
{
    if (!tasks_.empty())
        workthread_->Send(this, MSG_HS_QUIT);

    delete pool_;
    if (workthread_)
        EndWorkThread();
}

bool HttpSession::StartTask(HttpTask* task)
{
    ASSERT(task);
    task->ConnectSignal();

    if (!workthread_)
        CreateWorkThread();

    ASSERT(workthread_);

    task->client().SignalHttpClientComplete.connect(this, &HttpSession::OnHttpClientComplete);
    task->SignalTaskClose.connect(this, &HttpSession::OnTaskFinish);
    task->client().set_pool(pool_);
    // 保证tasks_的操作都在一个线程中
    // tasks_.push_back(task);

    workthread_->Post(this, MSG_HS_START, talk_base::WrapMessageData(task));
    return true;
}

bool HttpSession::StopTask(HttpTask* task)
{
    ASSERT(task && workthread_);
    
    workthread_->Send(this, MSG_HS_CANCELTASK, talk_base::WrapMessageData(task));
    return true;
}

bool HttpSession::ReleaseTask(HttpTask* task)
{
    ASSERT(!manage_);
    ASSERT(task && workthread_);
    // in tasks_ or not in
    if (task->done())
    {
        // ASSERT(std::find(tasks_.begin(), tasks_.end(), task) == tasks_.end());
        workthread_->Post(this, MSG_HS_DELETE, talk_base::WrapMessageData(task));
    }
    else
    {
        ASSERT(std::find(tasks_.begin(), tasks_.end(), task) != tasks_.end());
        workthread_->Send(this, MSG_HS_CANCELTASK, talk_base::WrapMessageData(task));
        workthread_->Post(this, MSG_HS_DELETE, talk_base::WrapMessageData(task));
    }
    return true;
}

#if 0
virtual talk_base::StreamInterface* 
RequestConnectedStream(const talk_base::SocketAddress& remote, int* err)
{
    return 0;
}
virtual void ReturnConnectedStream(talk_base::StreamInterface* stream)
{
}
#endif

void HttpSession::OnMessage(talk_base::Message *pmsg)
{
    ASSERT(workthread_ == talk_base::Thread::Current());
    switch(pmsg->message_id)
    {
    case MSG_HS_START:
        {
            HttpTask* task = talk_base::UseMessageData<HttpTask*>(pmsg->pdata);
            if (task)
            {
                tasks_.push_back(task);
                task->client().start();
            }
        }
        break;
    case MSG_HS_QUIT:
        for (tasks_type::iterator i = tasks_.begin(); i != tasks_.end(); )
        {
            (*i)->client().reset();
            i = tasks_.begin();
        }
        tasks_.clear();
        break;
    case MSG_HS_CANCELTASK:
        {
            HttpTask* task = talk_base::UseMessageData<HttpTask*>(pmsg->pdata);
            if (task)
            {
                task->client().reset();
            }
        }
        break;
    case MSG_HS_DELETE:
        {
            HttpTask* task = talk_base::UseMessageData<HttpTask*>(pmsg->pdata);
            if (task)
            {
                task->client().reset();
                delete task;
            }
        }
        break;
    }
}

void HttpSession::OnHttpClientComplete(talk_base::HttpClient * http, int err)
{
    ASSERT(workthread_ == talk_base::Thread::Current());
    ss_.WakeUp();
}

void HttpSession::OnTaskFinish(HttpTask* task)
{
    ASSERT(workthread_ == talk_base::Thread::Current());
    tasks_type::iterator i = std::find(tasks_.begin(), tasks_.end(), task);
    if (i != tasks_.end())
        tasks_.erase(i);

    if (manage_)
        workthread_->Post(this, MSG_HS_DELETE, talk_base::WrapMessageData(task));
}

#ifndef NDEBUG
bool HttpSession::Wait(int cms)
{
    return ss_.Wait(cms, true);
}
#endif

} // namespace tting


