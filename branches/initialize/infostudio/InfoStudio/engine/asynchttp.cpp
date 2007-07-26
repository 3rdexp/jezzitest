
#include <windows.h>

#include "urlcomp.h"
#include "logging.h"
#include "asynchttp.h"

#ifndef ASSERT
    #define ASSERT(x)
#endif

#ifdef _DEBUG
    #include <iostream>
    #define LOGTHREAD " [" << GetCurrentThreadId() << "]"
#endif

#pragma comment(lib, "wininet.lib")

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

    LOG(LS_VERBOSE) << "AsyncHttp:" << (void*)p 
        << " status:" << dwInternetStatus
        << " ";
    ASSERT(p);
    if(!p) 
        return;

    if(INTERNET_STATUS_HANDLE_CREATED == dwInternetStatus )
    {
        p->HandleCreated( (HINTERNET)((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult );
    }
    else if( INTERNET_STATUS_REDIRECT == dwInternetStatus )
    {
        p->Redirect((LPCWSTR)lpvStatusInformation, dwStatusInformationLength);
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
        p->NameResolved((LPCWSTR)lpvStatusInformation);
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
    if (gai == 0)
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
        InternetSetStatusCallbackW(hInet_, InternetStatusCallback);
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
    : hConn_(0), hRequest_(0)
    , content_length_(0)
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

    ASSERT(!hConn_);

    AsyncInet & ai = AsyncInet::GetInstance();

    hConn_ = InternetConnectW(ai.get(), uc.lpszHostName, uc.nPort, 
        uc.lpszUserName, uc.lpszPassword, uc.nScheme, // INTERNET_SERVICE_HTTP )
        INTERNET_FLAG_PASSIVE, (DWORD_PTR)this);
    ASSERT(hConn_);
    if (!hConn_)
        return false;

    LPCWSTR szAcceptType[] = { L"*.*", 0 };

    hRequest_ = HttpOpenRequestW(hConn_, L"GET", uc.lpszUrlPath, 
        NULL, // Version
        referrer.empty() ? NULL : referrer.c_str(), // Referrer
        szAcceptType,
        INTERNET_FLAG_NO_UI,
        (DWORD_PTR)this);
    ASSERT(hRequest_);

    content_length_ = 0;

    return hRequest_ != NULL;
}

bool AsyncHttp::PreparePost(const std::string & url, const std::string & content_type
                            , std::istream* request_doc)
{
    // TODO:
    return true;
}

bool AsyncHttp::SendRequest()
{
    ASSERT(hRequest_);
    BOOL f = HttpSendRequest(hRequest_, NULL, 0, NULL, 0);
    ASSERT(f || ERROR_IO_PENDING == GetLastError() );
    return true;
}

void AsyncHttp::HandleCreated(HINTERNET hInet)
{
    LOG(LS_VERBOSE) << "HandleCreated: ";
}

bool AsyncHttp::NameResolved(LPCWSTR pszIpAddress)
{
    LOG(LS_VERBOSE) << "NameResolved: ";
    // ip_ = inet_addr(pszIpAddress);
    // ASSERT(ip_ != INADDR_NONE)
    ip_ = pszIpAddress;
    return true;
}

bool AsyncHttp::Redirect(LPCWSTR szUrl, DWORD dwUrlLength)
{
    LOG(LS_VERBOSE) << "Redirect: ";

    redirect_url_.assign(szUrl, dwUrlLength);
    return true;
}

bool AsyncHttp::ResponseReceived(DWORD dwBytes)
{
    LOG(LS_VERBOSE) << "ResponseReceived: " << dwBytes;
    return true;
}

bool AsyncHttp::RequestComplete(DWORD dwResult, DWORD dwError)
{
#if 1
    LOG(LS_VERBOSE) << "RequestComplete: " << dwResult 
        << " Error: " << dwError 
       ;
#endif

    // 在回调中再调用 WinInet 函数不太好

    INTERNET_BUFFERSA ib = {0};
    ib.dwStructSize = sizeof(INTERNET_BUFFERSA);

    const int grow = 0x1000;

    do {
        buf_.resize(content_length_ + grow);
        ib.lpvBuffer = &buf_[0] + content_length_;
        ib.dwBufferLength = grow;
        ib.dwBufferTotal = buf_.size();

        BOOL bOk = InternetReadFileExA(hRequest_, &ib, IRF_NO_WAIT, (LPARAM)this);
        if (bOk)
        {
            content_length_ += ib.dwBufferLength;

            if (ib.dwBufferLength == 0)
            {
                // 补0安全
                buf_.resize(content_length_ + 1);
                buf_[content_length_] = 0;
                RequestDone();
            }
        }
        else
        {
            ASSERT(GetLastError() == ERROR_IO_PENDING);
            break;
        }
    } while(ib.dwBufferLength);


    return true;
}


