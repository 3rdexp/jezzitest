

#include "asynchttp.h"
#include "urlcomp.h"
#include "criticalsection.h"

#ifndef ASSERT
    #define ASSERT(x)
#endif

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

static void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext
    , DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    AsyncHttp * p = (AsyncHttp*)dwContext;
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

//////////////////////////////////////////////////////////////////////////
static AsyncInet * gai = 0;

AsyncInet & AsyncInet::GetInstance()
{
    if (gai != 0)
    {
        // TODO: Lock
        gai = new AsyncInet();
    }
    return *gai;
}

const std::wstring DefaultAgent(L"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;"
                                L" .NET CLR 1.1.4322)");

bool AsyncInet::Init()
{
    AsyncInet & ai = AsyncInet::GetInstance();
    return ai.Open(DefaultAgent);
}

void AsyncInet::Release()
{
    // TODO: Lock
    delete gai;
    gai = 0;
}

AsyncInet::AsyncInet() : hInet_(0) 
{}

AsyncInet::~AsyncInet()
{
    Close();
}

bool AsyncInet::Open(const std::wstring & agent, DWORD dwAccessType, LPCWSTR lpszProxyName
        , LPCWSTR lpszProxyBypass, DWORD dwFlag)
{
    ASSERT(!hInet_);
    hInet_ = InternetOpenW(agent.c_str(), dwAccessType, lpszProxyName, lpszProxyBypass, dwFlag);
    ASSERT(hInet_);
    if(hInet_ && dwFlag & INTERNET_FLAG_ASYNC)
    {
        InternetSetStatusCallback(hInet_, InternetStatusCallback);
    }
    return hInet_ != NULL;
}

void AsyncInet::Close()
{
    ASSERT(hInet_);
    BOOL f = InternetCloseHandle(hInet_);
    ASSERT(f);
    hInet_ = 0;
}

//////////////////////////////////////////////////////////////////////////
//
AsyncHttp::AsyncHttp()
    : _hConn(0)
{
}

AsyncHttp::~AsyncHttp()
{
}

bool AsyncHttp::PrepareGet(const std::wstring & url, const std::wstring & referrer)
{
    UrlComponet uc;
    if (!uc.Crack(url.c_str()))
        return false;

    ASSERT(!_hConn);

    AsyncInet & ai = AsyncInet::GetInstance();

    _hConn = InternetConnectW(ai.get(), uc.lpszHostName, uc.nPort, 
        uc.lpszUserName, uc.lpszPassword, uc.nScheme, // INTERNET_SERVICE_HTTP )
        INTERNET_FLAG_PASSIVE, (DWORD_PTR)this);
    ASSERT(_hConn);

    LPCWSTR szAcceptType[] = { L"*.*", 0 };

    _hRequest = HttpOpenRequestW(_hConn, L"GET", uc.lpszUrlPath, 
        NULL, // Version
        referrer.empty() ? NULL : referrer.c_str(), // Referrer
        szAcceptType,
        INTERNET_FLAG_NO_UI,
        (DWORD_PTR)this);
    ASSERT(_hRequest);

    return true;
}

bool AsyncHttp::PreparePost(const std::string & url, const std::string & content_type
                            , std::istream* request_doc)
{
    return true;
}

bool AsyncHttp::SendRequest()
{
    HttpSendRequest(_hRequest, NULL, 0, NULL, 0);
    return true;
}

