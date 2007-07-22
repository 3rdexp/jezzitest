

#include "asynctask.h"
#include "urlhlp.h"
#include "criticalsection.h"

#ifndef ASSERT
    #define ASSERT(x)
#endif

// TODO: unicode string
const TCHAR * DefaultAgent = _T("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322)");

class AutoInternetHandle 
{
public:
    explicit AutoInternetHandle(HINTERNET handle) : handle_(handle) {}
    ~AutoInternetHandle() 
    {
        if (handle_) {
            InternetCloseHandle(handle_);
        }
    }

    HINTERNET get() { return handle_; }

private:
    HINTERNET handle_;
};

class AsyncInet
{
public:
    AsyncInet() : _hInet(0) {}
    ~AsyncInet()
    {
        BOOL f = Close();
        ASSERT(f);
    }
    HINTERNET GetHandle()
    {
        ASSERT(_hInet);
        return _hInet; 
    }
    BOOL Open(LPCTSTR lpszAgent = NULL, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG, 
        LPCTSTR lpszProxyName = NULL, LPCTSTR lpszProxyBypass = NULL,
        DWORD dwFlag = INTERNET_FLAG_ASYNC)
    {
        _hInet = InternetOpenW(lpszAgent, dwAccessType, lpszProxyName, lpszProxyBypass, dwFlag);
        ASSERT(_hInet);
        if(_hInet && dwFlag & INTERNET_FLAG_ASYNC)
        {
            InternetSetStatusCallback(_hInet, InternetStatusCallback);
        }
        return _hInet != NULL;
    }
    BOOL Close()
    {
        return _hInet ? InternetCloseHandle(_hInet) : TRUE;
    }
private:
    static void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext, DWORD dwInternetStatus,
        LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
    {
        AsyncTask * p = (AsyncTask*)dwContext;
#if defined(VERBOSE_LEVEL) && (VERBOSE_LEVEL > 2)
        std::cout << (void*)p->Handle() << " [" << GetCurrentThreadId() << "] :" 
            << dwInternetStatus << " " << GetStatusText(dwInternetStatus) << " State:" << p->_state << "\n";
#endif
        if(!p) return;

        if(INTERNET_STATUS_HANDLE_CREATED == dwInternetStatus )
        {
            p->HandleCreated( (HINTERNET)((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult );
        }
        else if( INTERNET_STATUS_REDIRECT == dwInternetStatus )
        {
            p->Redirect((LPCTSTR)lpvStatusInformation, dwStatusInformationLength);
        }
        else if( INTERNET_STATUS_RESPONSE_RECEIVED == dwInternetStatus )
        {
            p->ResponseReceived(*(DWORD *)lpvStatusInformation);
        }
        else if( INTERNET_STATUS_REQUEST_COMPLETE == dwInternetStatus )
        {
            p->RequestComplete(
                ((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult, 
                ((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwError
                );
        }
        else if(INTERNET_STATUS_NAME_RESOLVED == dwInternetStatus)
        {
            p->NameResolved((LPCTSTR)lpvStatusInformation);
        }
        //		else if(INTERNET_STATUS_HANDLE_CLOSING == dwInternetStatus)
        //		{
        //			p->HandleClosing();
        //		}
    }
public:
    HINTERNET _hInet;
};

AsyncInet & AsyncTask::GetAsyncInet()
{
    static AsyncInet ai;
    return ai;
}

//////////////////////////////////////////////////////////////////////////


void InitWinInet()
{
    AsyncInet & ai = AsyncTask::GetAsyncInet();
    ai.Open(DefaultAgent);
}

void ReleaseWinInet()
{
    AsyncTask::GetAsyncInet();
}

HINTERNET GetWinInet()
{
    AsyncInet & ai = AsyncTask::GetAsyncInet();
    return ai._hInet;
}

//////////////////////////////////////////////////////////////////////////
//
AsyncTask::AsyncTask()
    : _hConn(0)
{
}

AsyncTask::~AsyncTask()
{
}

bool AsyncTask::PrepareGet(LPCTSTR szUrl)
{
    UrlComponet url;
    if(!url.Crack(szUrl))
        return false;

    ASSERT(!_hConn);

    AsyncInet & ai = AsyncTask::GetAsyncInet();

    _hConn = InternetConnect(ai.GetHandle(), url.lpszHostName, url.nPort, 
        url.lpszUserName, url.lpszPassword, url.nScheme, // INTERNET_SERVICE_HTTP )
        INTERNET_FLAG_PASSIVE, (DWORD_PTR)this);
    ASSERT(_hConn);

    LPCTSTR szAcceptType[] = { _T("*.*"), 0 };

    _hRequest = HttpOpenRequest(hConn, _T("GET"), url.lpszUrlPath, 
        NULL, // Version
        NULL, // Referrer
        szAcceptType,
        INTERNET_FLAG_NO_UI,
        (DWORD_PTR)this);
    ASSERT(hReq);

    return true;
}

bool AsyncTask::PreparePost(LPCTSTR szUrl, LPCTSTR szContentType
                            , std::istream* request_doc)
{
    return true;
}

bool AsyncTask::SendRequest()
{
    HttpSendRequest(hReq, lpszHeaders, 0, NULL, 0);
    return true;
}

