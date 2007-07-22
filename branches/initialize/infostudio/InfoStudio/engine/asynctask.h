
#ifndef __ASYNCTASK_H__
#define __ASYNCTASK_H__

#include <windows.h>
#include <wininet.h>

#include <set>

#include "sigslot.h"
#include "task.h"

class AsyncInet;

class AsyncTask : public Task
    , public sigslot::has_slots<sigslot::multi_threaded_local>
{
public:
    AsyncTask(Task * parent);
    virtual ~AsyncTask();

    virtual int ProcessStart();
    virtual int ProcessResponse();

    //
    bool PrepareGet(LPCTSTR szUrl);
    bool PreparePost(LPCTSTR szUrl, LPCTSTR szContentType, std::istream* request_doc);
    bool SendRequest();
    
    static AsyncInet & GetAsyncInet();

protected:
    friend class AsyncInet;
    // 
    void HandleCreated(HINTERNET hInet);
    BOOL NameResolved(LPCTSTR pszIpAddress);
    BOOL Redirect(LPCTSTR szUrl, DWORD dwUrlLength);
    BOOL ResponseReceived(DWORD dwBytes);
    BOOL RequestComplete(DWORD dwResult, DWORD dwError);

    // 1 对于简单任务，直接返回一个新的Connection
    // 2 对于复杂任务，可以一个站点公用一个Connection
    virtual HINTERNET GetConnection(LPCTSTR szHost);

    //
    struct HostConnection {
        std::string host;
        HINTERNET hconn;
    };

    typedef std::vector<HostConnection> ConnectColl;
    ConnectColl _conns;

protected:
    HINTERNET _hConn;
};

class AsyncRootTask : public AsyncTask
{
public:
    AsyncRootTask();
    virtual HINTERNET GetConnection(LPCTSTR szHost);
};

void InitWinInet();
void ReleaseWinInet();
HINTERNET GetWinInet();

#endif // __ASYNCTASK_H__
