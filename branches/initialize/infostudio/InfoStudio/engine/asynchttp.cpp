
#include <windows.h>

#include "urlcomp.h"
#include "httpcomm.h"
#include "logging.h"
#include "asynchttp.h"

#pragma comment(lib, "wininet.lib")

BEGIN_ENGINE_NAMESPACE

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

std::string GetStatusText(DWORD dwStatus)
{
    std::map<DWORD, std::string> status_map;
#define Assign(s, v) status_map.insert( std::make_pair(v, std::string(#s)) );
    Assign( INTERNET_STATUS_RESOLVING_NAME,			 10)
        Assign( INTERNET_STATUS_NAME_RESOLVED,           11)
        Assign( INTERNET_STATUS_CONNECTING_TO_SERVER,    20)
        Assign( INTERNET_STATUS_CONNECTED_TO_SERVER,     21)
        Assign( INTERNET_STATUS_SENDING_REQUEST,         30)
        Assign( INTERNET_STATUS_REQUEST_SENT,            31)
        Assign( INTERNET_STATUS_RECEIVING_RESPONSE,      40)
        Assign( INTERNET_STATUS_RESPONSE_RECEIVED,       41)
        Assign( INTERNET_STATUS_CTL_RESPONSE_RECEIVED,   42)
        Assign( INTERNET_STATUS_PREFETCH,                43)
        Assign( INTERNET_STATUS_CLOSING_CONNECTION,      50)
        Assign( INTERNET_STATUS_CONNECTION_CLOSED,       51)
        Assign( INTERNET_STATUS_HANDLE_CREATED,          60)
        Assign( INTERNET_STATUS_HANDLE_CLOSING,          70)
        Assign( INTERNET_STATUS_DETECTING_PROXY,         80)
        Assign( INTERNET_STATUS_REQUEST_COMPLETE,        100)
        Assign( INTERNET_STATUS_REDIRECT,                110)
        Assign( INTERNET_STATUS_INTERMEDIATE_RESPONSE,   120)
        Assign( INTERNET_STATUS_USER_INPUT_REQUIRED,     140)
        Assign( INTERNET_STATUS_STATE_CHANGE,            200)
        Assign( INTERNET_STATUS_COOKIE_SENT,             320)
        Assign( INTERNET_STATUS_COOKIE_RECEIVED,         321)
        Assign( INTERNET_STATUS_PRIVACY_IMPACTED,        324)
        Assign( INTERNET_STATUS_P3P_HEADER,              325)
        Assign( INTERNET_STATUS_P3P_POLICYREF,           326)
        Assign( INTERNET_STATUS_COOKIE_HISTORY,          327)
#undef Assign
        return status_map[dwStatus];
}

//////////////////////////////////////////////////////////////////////////
// 
static void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext
    , DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength)
{
    AsyncHttp * p = (AsyncHttp*)dwContext;

    LOG(LS_VERBOSE) << "AsyncHttp:" << (void*)p 
        << " status:" << GetStatusText(dwInternetStatus)
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
        p->Redirect((LPCSTR)lpvStatusInformation, dwStatusInformationLength);
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
        p->NameResolved((LPCSTR)lpvStatusInformation);
    }
    //		else if(INTERNET_STATUS_HANDLE_CLOSING == dwInternetStatus)
    //		{
    //			p->HandleClosing();
    //		}
}

//////////////////////////////////////////////////////////////////////////
//
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

const std::string DefaultAgent("Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;"
                                " .NET CLR 1.1.4322)");

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

bool AsyncInet::Open(const std::string & agent, DWORD dwAccessType, LPCSTR lpszProxyName
        , LPCSTR lpszProxyBypass, DWORD dwFlag)
{
    ASSERT(!hInet_);
    hInet_ = InternetOpen(agent.c_str(), dwAccessType, lpszProxyName, lpszProxyBypass, dwFlag);
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

bool AsyncInet::SetCookie(const std::string & url, const std::string & name
                        , const std::string & data)
{
    return ::InternetSetCookie(url.c_str()
        , name.empty() ? NULL : name.c_str(), data.c_str());
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

bool AsyncHttp::PrepareGet(const std::string & url, const std::string & referrer)
{
    UrlComponet uc;
    if (!uc.Crack(url.c_str()))
        return false;

    ASSERT(!hConn_);

    AsyncInet & ai = AsyncInet::GetInstance();

    hConn_ = InternetConnect(ai.get(), uc.lpszHostName, uc.nPort, 
        uc.lpszUserName, uc.lpszPassword, uc.nScheme, // INTERNET_SERVICE_HTTP )
        INTERNET_FLAG_PASSIVE, (DWORD_PTR)this);
    ASSERT(hConn_);
    if (!hConn_)
        return false;

    LPCSTR szAcceptType[] = { "*.*", 0 };

    hRequest_ = HttpOpenRequest(hConn_, "GET", uc.lpszUrlPath, 
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
        , std::istream* request_doc, const std::string & referrer)
{
    UrlComponet uc;
    if (!uc.Crack(url.c_str()))
        return false;

    ASSERT(!hConn_);

    AsyncInet & ai = AsyncInet::GetInstance();

    hConn_ = InternetConnect(ai.get(), uc.lpszHostName, uc.nPort, 
        uc.lpszUserName, uc.lpszPassword, uc.nScheme, // INTERNET_SERVICE_HTTP )
        INTERNET_FLAG_PASSIVE, (DWORD_PTR)this);
    ASSERT(hConn_);
    if (!hConn_)
        return false;

    LPCSTR szAcceptType[] = { "*.*", 0 };

    hRequest_ = HttpOpenRequest(hConn_, "POST", uc.lpszUrlPath, 
        NULL, // Version
        referrer.empty() ? NULL : referrer.c_str(), // Referrer
        szAcceptType,
        INTERNET_FLAG_NO_UI,
        (DWORD_PTR)this);
    ASSERT(hRequest_);

    if (hRequest_)
    {
        setContent(content_type, request_doc);
    }

    return hRequest_ != NULL;
}

bool AsyncHttp::SendRequest()
{
    ASSERT(hRequest_);

    BOOL ret = FALSE;

    // ---------------------------------------------------
    // build header
    if (!m_headers.empty())
    {
        std::ostringstream hs;
        for (HeaderMap::const_iterator i = m_headers.begin();
            i != m_headers.end(); ++i)
        {
            hs << i->first << ":" << i->second << "\r\n";
        }

        ret = HttpAddRequestHeaders(hRequest_
            , hs.str().data()
            , hs.str().size(), HTTP_ADDREQ_FLAG_ADD);
        ASSERT(ret);
    }

    ret = HttpSendRequest(hRequest_, NULL, 0, &buf_[0], buf_.size());
    ASSERT(ret || ERROR_IO_PENDING == GetLastError());
    return ret;
}

//////////////////////////////////////////////////////////////////////////
// 
void AsyncHttp::HandleCreated(HINTERNET hInet)
{
    LOG(LS_VERBOSE) << "HandleCreated: ";
}

bool AsyncHttp::NameResolved(LPCSTR pszIpAddress)
{
    LOG(LS_VERBOSE) << "NameResolved: ";
    // ip_ = inet_addr(pszIpAddress);
    // ASSERT(ip_ != INADDR_NONE)
    ip_ = pszIpAddress;
    return true;
}

bool AsyncHttp::Redirect(LPCSTR szUrl, DWORD dwUrlLength)
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

    const int grow = 0x2000;

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

//////////////////////////////////////////////////////////////////////////
// 
void AsyncHttp::changeHeader(const std::string& name, const std::string& value, HeaderCombine combine)
{
    if (combine == HC_AUTO) {
        HttpHeader header;
        // Unrecognized headers are collapsible
        combine = !FromString(header, name) || HttpHeaderIsCollapsible(header)
            ? HC_YES : HC_NO;
    } else if (combine == HC_REPLACE) {
        m_headers.erase(name);
        combine = HC_NO;
    }
    // At this point, combine is one of (YES, NO, NEW)
    if (combine != HC_NO) {
        HeaderMap::iterator it = m_headers.find(name);
        if (it != m_headers.end()) {
            if (combine == HC_YES) {
                it->second.append(",");
                it->second.append(value);
            }
            return;
        }
    }
    m_headers.insert(HeaderMap::value_type(name, value));
}

void AsyncHttp::clearHeader(const std::string& name)
{
    m_headers.erase(name);
}

bool AsyncHttp::hasHeader(const std::string& name, std::string* value) const 
{
    HeaderMap::const_iterator it = m_headers.find(name);
    if (it == m_headers.end()) {
        return false;
    } else if (value) {
        *value = it->second;
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
// 
void
AsyncHttp::setContent(const std::string& content_type, std::istream * document) 
{
    ASSERT(!content_type.empty());
    
    setHeader(HH_CONTENT_TYPE, content_type);

    if (document)
    {
        buf_.clear();
        std::copy(std::istream_iterator<char>(*document)
            , std::istream_iterator<char>()
            , std::back_inserter(buf_));
    }
   
    size_t content_length = buf_.size(); 
    if (content_length) {
        char buffer[32];
        _snprintf(buffer, sizeof(buffer), "%d", content_length);
        setHeader(HH_CONTENT_LENGTH, buffer);
    } else {
        setHeader(HH_TRANSFER_ENCODING, "chunked");
    }
}

END_ENGINE_NAMESPACE
