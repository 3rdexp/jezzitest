
#ifndef __ASYNCHTTP_H__
#define __ASYNCHTTP_H__

#include <windows.h>
#include <wininet.h>
#include <map>

#include "criticalsection.h"

#ifndef ASSERT
#define ASSERT 
#endif

class AsyncInet;

class AsyncHttp 
{
public:
    AsyncHttp();
    virtual ~AsyncHttp();

    bool PrepareGet(const std::wstring & url, const std::wstring & referrer = L"");
    bool PreparePost(const std::string & url, const std::string & content_type
        , std::istream* request_doc);
    bool SendRequest();

protected:
    friend class AsyncInet;
    friend void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext
        , DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    // 
    void HandleCreated(HINTERNET hInet);
    BOOL NameResolved(LPCTSTR pszIpAddress);
    BOOL Redirect(LPCTSTR szUrl, DWORD dwUrlLength);
    BOOL ResponseReceived(DWORD dwBytes);
    BOOL RequestComplete(DWORD dwResult, DWORD dwError);

    // 1 对于简单任务，直接返回一个新的Connection
    // 2 对于复杂任务，可以一个站点公用一个Connection
    virtual HINTERNET GetConnection(LPCTSTR szHost);

protected:
    HINTERNET _hConn;
    HINTERNET _hRequest;
};


class AsyncInet
{
public:
    AsyncInet();
    ~AsyncInet();

    bool Open(const std::wstring & agent, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG, 
        LPCWSTR lpszProxyName = NULL, LPCWSTR lpszProxyBypass = NULL,
        DWORD dwFlag = INTERNET_FLAG_ASYNC);

    void Close();

    HINTERNET get()
    {
        ASSERT(hInet_);
        return hInet_; 
    }

    static AsyncInet & GetInstance();

    static bool Init();
    static void Release(); // Release 以后就不应该继续使用

    typedef std::map<std::string, HINTERNET> ConnectionMap;

private:
    HINTERNET hInet_;

    ConnectionMap allConn_;
    CriticalSection csConn_;
};



#endif // __ASYNCHTTP_H__
