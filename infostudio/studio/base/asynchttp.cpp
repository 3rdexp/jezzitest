
#include "urlcomp.h"
#include "base.h"
#include "unicode.h"
#include <fstream>

#include "httpcomm.h"
#include "logging.h"

#include "asynchttp.h"
#pragma comment(lib, "wininet.lib")

namespace xnbase {

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

    LOG(LS_SENSITIVE) << "Callback:" << hInternet
        << " AsyncHttp:" << (void*)p
        << " status:" << GetStatusText(dwInternetStatus);
    Assert(p);
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
	else if(INTERNET_STATUS_HANDLE_CLOSING == dwInternetStatus)
	{
		p->HandleClosing(hInternet);
	}
}

//////////////////////////////////////////////////////////////////////////
//

AsyncHttp::~AsyncHttp()
{
#ifdef LOG_URL
    LOG(LS_SENSITIVE) << "~AsyncHttp " << (void *)this << " " << url_;
#endif

    if ((hRequest_ || hConn_) && !event_)
    {
        Close();

        if (delete_self_)
        {
            CloseHandle(event_);
            event_ = 0;
        }

        if (event_)
        {
            DWORD wait = WaitForSingleObject(event_, INFINITE);
            Assert(wait == WAIT_OBJECT_0);
            CloseHandle(event_);
            event_ = 0;
        }
    }
#ifndef LOG_URL
    LOG(LS_SENSITIVE) << "~AsyncHttp end " << (void *)this << " " << url_;
#endif
}

bool AsyncHttp::PrepareGet(const std::string & url, const std::string & referrer)
{
    UrlComponet uc;
    if (!uc.Crack(url.c_str()))
        return false;

    Assert(!hConn_);

    status_ = 0;
    content_length_ = 0;
    readed_ = 0;
    redirect_ = false;

    hConn_ = CachedConnection(uc.lpszHostName);
    if (!hConn_)
    {
        AsyncInet & ai = AsyncInet::GetInstance();
        hConn_ = InternetConnectA(ai.get(), uc.lpszHostName, uc.nPort,
            uc.lpszUserName, uc.lpszPassword, INTERNET_SERVICE_HTTP,
            0, (DWORD_PTR)this);
    }
    Assert(hConn_);
    if (!hConn_)
        return false;

    LPCSTR szAcceptType[] = { "*.*", 0 };

	DWORD flags = INTERNET_FLAG_NO_UI | INTERNET_FLAG_KEEP_CONNECTION 
        | INTERNET_FLAG_NEED_FILE // 保存为文件!!
        | INTERNET_FLAG_RESYNCHRONIZE;
    if (uc.nScheme == INTERNET_SCHEME_HTTPS)
        flags |= INTERNET_FLAG_SECURE;

    if (!auto_redirect_)
        flags |= INTERNET_FLAG_NO_AUTO_REDIRECT;

    std::string p(uc.lpszUrlPath);
    p += uc.lpszExtraInfo;

    hRequest_ = HttpOpenRequestA(hConn_, "GET", p.c_str(),
        NULL, // Version
        referrer.empty() ? NULL : referrer.c_str(), // Referrer
        szAcceptType,
        flags,
        (DWORD_PTR)this);
    Assert(hRequest_);

#ifdef LOG_URL
    url_ = url;
    LOG(LS_SENSITIVE) << "OpenRequest:" << hRequest_ << " " << url_;
#endif

    content_length_ = 0;
    return hRequest_ != NULL;
}

bool AsyncHttp::PrepareGet(const std::wstring & url, const std::wstring & referrer) {
    return PrepareGet(w2string(url), w2string(referrer));
}

bool AsyncHttp::PreparePost(const std::string & url, const std::string &
   content_type, std::istream* request_doc, const std::string & referrer)
{
    UrlComponet uc;
    if (!uc.Crack(url.c_str()))
        return false;

    Assert(!hConn_);

    status_ = 0;
    readed_ = 0;
    content_length_ = 0;

    hConn_ = CachedConnection(uc.lpszHostName);
    if (!hConn_)
    {
        AsyncInet & ai = AsyncInet::GetInstance();
        hConn_ = InternetConnectA(ai.get(), uc.lpszHostName, uc.nPort,
            uc.lpszUserName, uc.lpszPassword, INTERNET_SERVICE_HTTP,
            0, (DWORD_PTR)this);
    }
    Assert(hConn_);
    if (!hConn_)
        return false;

    LPCSTR szAcceptType[] = { "*.*", 0 };

    DWORD flags = INTERNET_FLAG_NO_UI;
    if (uc.nScheme == INTERNET_SCHEME_HTTPS)
        flags |= INTERNET_FLAG_SECURE;

    std::string p(uc.lpszUrlPath);
    p += uc.lpszExtraInfo;

    hRequest_ = HttpOpenRequestA(hConn_, "POST", p.c_str(),
        NULL, // Version
        referrer.empty() ? NULL : referrer.c_str(), // Referrer
        szAcceptType,
        flags,
        (DWORD_PTR)this);
    Assert(hRequest_);

    if (hRequest_)
    {
        setContent(content_type, request_doc);
    }

    return hRequest_ != NULL;
}

bool AsyncHttp::PreparePost(const std::wstring & url, const std::wstring &
                            content_type, std::istream* request_doc, const std::wstring & referrer) {
    return PreparePost(w2string(url), w2string(content_type), request_doc, w2string(referrer));
}

bool AsyncHttp::SendRequest()
{
    Assert(hRequest_);

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

        ret = HttpAddRequestHeadersA(hRequest_
            , hs.str().data()
            , hs.str().size(), HTTP_ADDREQ_FLAG_ADD);
        Assert(ret);
    }

    LOG(LS_SENSITIVE) << "SendRequest: " << hRequest_ << " " << (void*)this;

	char * buffer = (buf_.size() <= 0) ? NULL : &buf_[0];
    ret = HttpSendRequest(hRequest_, NULL, 0, buffer, buf_.size());
    Assert(ret || ERROR_IO_PENDING == GetLastError());
    return ret || ERROR_IO_PENDING == GetLastError();
}

void AsyncHttp::Close()
{
    Assert(!event_);
    Assert(hRequest_ || hConn_);

    if (!event_) // 让 Close 可以重复调用
    {
        event_ = CreateEvent(NULL, FALSE, FALSE, NULL);
        Assert(event_);
    }

    if (hRequest_)
    {
        // BOOL f = ::HttpEndRequest(hRequest_, NULL, 0, 0);
        // if (!f)
        //    Assert(GetLastError() == ERROR_IO_PENDING);
        // return false, error code ==997
        LOG(LS_SENSITIVE) << "call Close: " << hRequest_ << " " << (void*)this;;
        InternetCloseHandle(hRequest_);
    }

    if (hConn_)
    {
        LOG(LS_SENSITIVE) << "call Close: " << hConn_;
        InternetCloseHandle(hConn_);
    }
}

void AsyncHttp::Close2()
{
    // 尽可能的让回调线程删除 this
    if (!hRequest_ && !hConn_)
    {
        delete this;
        return;
    }

    delete_self_ = true;
    Close();
}

//////////////////////////////////////////////////////////////////////////
//
// 1 对于简单任务，直接返回一个新的Connection
// 2 对于复杂任务，可以一个站点公用一个Connection
HINTERNET AsyncHttp::CachedConnection(LPCSTR szHost)
{
    return 0;
}

void AsyncHttp::HandleCreated(HINTERNET hInet)
{
    LOG(LS_SENSITIVE) << "HandleCreated: " << hInet;
}

bool AsyncHttp::NameResolved(LPCSTR pszIpAddress)
{
    LOG(LS_SENSITIVE) << "NameResolved: " << pszIpAddress;
    // ip_ = inet_addr(pszIpAddress);
    // Assert(ip_ != INADDR_NONE)
    ip_ = pszIpAddress;
    return true;
}

bool AsyncHttp::HandleClosing(HINTERNET h)
{
    LOG(LS_SENSITIVE) << "HandleClosing cn:" << hConn_
        << " req:" << hRequest_;
    if (hConn_ == h)
        hConn_ = 0;
    else if(hRequest_ == h)
        hRequest_ = 0;

    if (hConn_ == 0 && hRequest_ == 0)
    {
        SetEvent(event_);

        if (delete_self_)
            delete this;
    }

	return true;
}

bool AsyncHttp::Redirect(LPCSTR szUrl, DWORD dwUrlLength)
{
    LOG(LS_SENSITIVE) << "Redirect: " << szUrl;

    redirect_url_.assign(szUrl, dwUrlLength);
    redirect_ = true;
    return true;
}

bool AsyncHttp::ResponseReceived(DWORD dwBytes)
{
    LOG(LS_SENSITIVE) << "ResponseReceived: " << dwBytes;
    return true;
}

bool AsyncHttp::RequestComplete(DWORD dwResult, DWORD dwError)
{
	LOG(LS_SENSITIVE) << "RequestComplete: " << dwResult
		<< " Error: " << dwError;

	Assert(hRequest_);
    if (event_)
        return true;

    if (readed_ == 0 && dwError == 0)
    {
        // TODO: 找到一个合适的地方得到 status code
        DWORD dwCode = 0;
        DWORD dwSize = sizeof(DWORD);
        BOOL bRet = ::HttpQueryInfoA(hRequest_, HTTP_QUERY_STATUS_CODE
            | HTTP_QUERY_FLAG_NUMBER , &dwCode, &dwSize, NULL);
        Assert(bRet);
        if (bRet)
            status_ = dwCode;

        // TODO: get Content-Length
        if (status_ == HC_OK)
        {
            bRet = ::HttpQueryInfoA(hRequest_, HTTP_QUERY_CONTENT_LENGTH
                | HTTP_QUERY_FLAG_NUMBER , &dwCode, &dwSize, NULL);
            if (!bRet)
                Assert(GetLastError() == ERROR_HTTP_HEADER_NOT_FOUND);
            if (bRet)
                content_length_ = dwCode;
        }
    }

    bool finished = false;

    // TODO: 区别是否是 SendRequest 回调和 ReadFile回调
    if (dwResult == TRUE)
    {
        const int grow = 4096;
        BOOL loop = TRUE;

        // 尝试打开文件
        if (!save_.is_open() && !saveto_.empty())
        {
            save_.open(saveto_.c_str(), std::ios::binary | std::ios::trunc);
            Assert(save_.is_open());
        }

        while (loop)
        {
            INTERNET_BUFFERSA ib = {sizeof(INTERNET_BUFFERSA)};
            
            // 如果文件能打开，写入文件，否则还是内存
            if (save_.is_open())
            {
                if (buf_.empty())
                    buf_.resize(grow);

                ib.lpvBuffer = &buf_[0];
                ib.dwBufferLength = grow;
                ib.dwBufferTotal = buf_.size();
            }
            else
            {
                buf_.resize(readed_ + grow);
                ib.lpvBuffer = &buf_[0] + readed_;
                ib.dwBufferLength = grow;
                ib.dwBufferTotal = buf_.size();
            }

            loop = InternetReadFileExA(hRequest_, &ib, IRF_NO_WAIT, (LPARAM)this);

            LOG(LS_SENSITIVE) << "Read once " << hRequest_ << " return:" << loop
                << " readed:" << ib.dwBufferLength;

            if (loop)
            {
                if (ib.dwBufferLength && save_.is_open())
                {
                    save_.write(&buf_[0], ib.dwBufferLength);
                }

                readed_ += ib.dwBufferLength;
                loop = (0 != ib.dwBufferLength);

                finished = (0 == ib.dwBufferLength); // TODO: 正确否？
            }
            else
            {
                // TODO: use InternetGetLastResponseInfo
                DWORD err = GetLastError();
                Assert(err == ERROR_IO_PENDING);

                // TODO: err = 6, why???
                // CAUTION: 导致死锁
                if (err != ERROR_IO_PENDING)
                    finished = true;
            }
        }
    }

    if (finished)
    {
#ifdef LOG_URL
        LOG(LS_SENSITIVE) << "OnResponse:" << hRequest_
            << " status:" << status_
            << " readed:" << readed_
            << " event:" << event_;
#endif
        if (!event_) // race condition ?
        {
            if (readed_)
            {
                if (!save_.is_open())
                {
                    buf_.resize(readed_);

                    Assert(readed_ == buf_.size());
                    OnResponse(&buf_[0], readed_);
                }
                else
                {
                    save_.close();
                    OnResponse(0, readed_);
                }
            }
            else
            {
                LOG(LS_SENSITIVE) << "OnResponse:" << hRequest_
                    << "OnResponse(0, 0);";
                OnResponse(0, 0);
            }
        }
    }

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
// TODO: 不适合大数据量的 post
void AsyncHttp::setContent(const std::string& content_type, std::istream * document)
{
    Assert(!content_type.empty());

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
    Assert(!hInet_);
    hInet_ = InternetOpenA(agent.c_str(), dwAccessType, lpszProxyName, lpszProxyBypass, dwFlag);
    Assert(hInet_);
    if(hInet_ && dwFlag & INTERNET_FLAG_ASYNC)
    {
        InternetSetStatusCallbackA(hInet_, InternetStatusCallback);
    }
    return hInet_ != NULL;
}

void AsyncInet::Close()
{
    if (hInet_ != NULL)
    {
        Assert(hInet_);
        ::InternetSetStatusCallback(hInet_, NULL);
        BOOL f = InternetCloseHandle(hInet_);
        Assert(f);
    }

    hInet_ = 0;
}

bool AsyncInet::SetCookie(const std::string & url, const std::string & name
                          , const std::string & data)
{
    return ::InternetSetCookieA(url.c_str()
        , name.empty() ? NULL : name.c_str(), data.c_str());
}

const std::string AsyncInet::GetCookie(const std::string &url)
{
    std::string ret;

    DWORD dwSize = 0;
    BOOL bRet = ::InternetGetCookieA(url.c_str(), NULL, 0, &dwSize);
    Assert(bRet);
    if (bRet)
    {
        dwSize += 1;
        char * p = new char[dwSize];
        bRet = ::InternetGetCookieA(url.c_str(), NULL, p, &dwSize);
        Assert(bRet);

        ret.assign(p, dwSize);
    }

    return ret;
}

bool AsyncInet::SetMaxConnection(int max /* = 2 */)
{
    BOOL bRet = ::InternetSetOptionA(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &max, sizeof(int));
    int nError = GetLastError();
    Assert(bRet && nError == 0);
    return bRet;
}

}
