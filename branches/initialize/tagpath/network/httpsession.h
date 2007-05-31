
#ifndef __HTTP_SESSION_H__
#define __HTTP_SESSION_H__

#include <string>
#include <list>

#include "talk/base/common.h"
#include "talk/base/thread.h"
#include "talk/base/httpclient.h"
#include "talk/base/sigslot.h"
#include "talk/base/physicalsocketserver.h"
#include "talk/base/socketpool.h"
// #include "talk/base/criticalsection.h"

namespace tting {

class HttpSession;

// sizeof(HttpTask) > 32K
class HttpTask : public sigslot::has_slots<>
{
public:
    HttpTask(const std::string & agent);
    virtual ~HttpTask();

    talk_base::HttpClient & client() { return client_; }
    const talk_base::HttpClient & client() const { return client_; }

    bool done() const { return complete_; }
    int error() const { return err_; }

    sigslot::signal1<HttpTask*> SignalTaskClose;

    virtual void OnTaskComplete(int error);

    // talk_base::HttpClient Signals
    void OnHttpHeaderAvailable(const talk_base::HttpClient*,bool,size_t);
    void OnHttpComplete(talk_base::HttpClient*,int);
    void OnHttpClosed(talk_base::HttpClient*,int);
protected:
    void ConnectSignal();
protected:
    friend class HttpSession;
    talk_base::HttpClient client_;
    bool complete_;
    int err_;
private:
    DISALLOW_EVIL_CONSTRUCTORS(HttpTask);
};

//////////////////////////////////////////////////////////////////////////
// 保存下载文件的下载器
class HttpDownloadFile : public HttpTask
{
public:
    HttpDownloadFile(const std::string & agent) : HttpTask(agent)
    {}

    bool Download(const std::string & url, const std::string & path);
};

class HttpSession : public talk_base::MessageHandler
    , public sigslot::has_slots<>
{
public:
    HttpSession(bool manage_task = false);
    virtual ~HttpSession();

    bool StartTask(HttpTask* task);
    bool StopTask(HttpTask* task);
    bool ReleaseTask(HttpTask* task);

    // TODO: after ReturnConnectedStream we should delete stream
    // StreamPool Interface
//     virtual talk_base::StreamInterface* 
//         RequestConnectedStream(const talk_base::SocketAddress& remote, int* err);
//     virtual void ReturnConnectedStream(talk_base::StreamInterface* stream);

    // MessageHandler Interface
    virtual void OnMessage(talk_base::Message *pmsg);

    // HttpClient Signals
    void OnHttpClientComplete(talk_base::HttpClient * http, int err);

    // HttpTask Signals
    void OnTaskFinish(HttpTask* );

#ifndef NDEBUG
    bool Wait(int cms);
#endif

private:
    void CreateWorkThread()
    {
        workthread_ = new talk_base::Thread(&ss_);
        workthread_->Start();
    }
    void EndWorkThread()
    {
        ASSERT(workthread_);
        // workthread_->Post(this, MSG_QUIT_THREAD);
        // workthread_->Join();
        delete workthread_;
        workthread_ = 0;
    }

    talk_base::Thread * workthread_;
    talk_base::PhysicalSocketServer ss_;
    talk_base::NewSocketPool* pool_;
    
    typedef std::list<HttpTask*> tasks_type;
    tasks_type tasks_;
    bool manage_;
    // prevent list operations occur Race Condition
    // talk_base::CriticalSection crit_;
private:
    DISALLOW_EVIL_CONSTRUCTORS(HttpSession);
};


} // namespace tting

#endif // __HTTP_SESSION_H__
