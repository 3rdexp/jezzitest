
#include "base.h"

#include <fstream>

#include "urlcomp.h"
#include "httpcomm.h"
#include "logging.h"

#include "asynchttp.h"
#pragma comment(lib, "wininet.lib")

// namespace xnbase {

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

    LOG(LS_VERBOSE) << "Callback:" << hInternet
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

//////////////////////////////////////////////////////////////////////////
//

AsyncHttp::~AsyncHttp()
{
    if (hRequest_ || hConn_)
    {
        Close();

        if (event_)
        {
            DWORD wait = WaitForSingleObject(event_, INFINITE);
            Assert(wait == WAIT_OBJECT_0);
            CloseHandle(event_);
            event_ = 0;
        }
    }
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

	DWORD flags = INTERNET_FLAG_NO_UI | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_RESYNCHRONIZE;
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

#ifndef NDEBUG
    url_ = url;
    LOG(LS_INFO) << "OpenRequest:" << hRequest_ << " " << url_;
#endif

    content_length_ = 0;
    return hRequest_ != NULL;
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

    LOG(LS_INFO) << "SendRequest: " << hRequest_ << " " << (void*)this;

	char * buffer = (buf_.size() <= 0) ? NULL : &buf_[0];
    ret = HttpSendRequest(hRequest_, NULL, 0, buffer, buf_.size());
    Assert(ret || ERROR_IO_PENDING == GetLastError());
    return ret || ERROR_IO_PENDING == GetLastError();
}

void AsyncHttp::Close()
{
    Assert(!event_);
    Assert(hRequest_ || hConn_);

    event_ = CreateEvent(NULL, FALSE, FALSE, NULL);

    if (hRequest_)
    {
        LOG(LS_VERBOSE) << "call Close: " << hRequest_;
        InternetCloseHandle(hRequest_);
    }

    if (hConn_)
    {
        LOG(LS_VERBOSE) << "call Close: " << hConn_;
        InternetCloseHandle(hConn_);
    }

    
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
    LOG(LS_VERBOSE) << "HandleCreated: " << hInet;
}

bool AsyncHttp::NameResolved(LPCSTR pszIpAddress)
{
    LOG(LS_VERBOSE) << "NameResolved: " << pszIpAddress;
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
        SetEvent(event_);
// 	if (hConn_ != NULL)
// 	{
// 		Assert(hConn_ != NULL);
// 		::InternetCloseHandle(hConn_);
// 		hConn_ = NULL;
// 	}
// 
// 	if (hRequest_ != NULL)
// 	{
// 		::InternetCloseHandle(hRequest_);
// 		hRequest_ = NULL;
// 	}

	return true;
}

bool AsyncHttp::Redirect(LPCSTR szUrl, DWORD dwUrlLength)
{
    LOG(LS_VERBOSE) << "Redirect: " << szUrl;

    redirect_url_.assign(szUrl, dwUrlLength);
    redirect_ = true;
    return true;
}

bool AsyncHttp::ResponseReceived(DWORD dwBytes)
{
    LOG(LS_VERBOSE) << "ResponseReceived: " << dwBytes;
    return true;
}
//////////////////////////////////////////////////////////////////////////
//update by wangliwei start 2007-12-18 19:16
/*
bool AsyncHttp::RequestComplete(DWORD dwResult, DWORD dwError)
{
#if 1
    LOG(LS_VERBOSE) << "RequestComplete: " << dwResult 
        << " Error: " << dwError
       ;
#endif

    if (status_ == 0)
    {
        DWORD dwCode = 0;
        DWORD dwSize = sizeof(DWORD);
        BOOL bRet = ::HttpQueryInfoA(hRequest_, HTTP_QUERY_STATUS_CODE
            | HTTP_QUERY_FLAG_NUMBER , &dwCode, &dwSize, NULL);
        Assert(bRet);
        if (bRet)
            status_ = dwCode;

        LOG(LS_VERBOSE) << "RequestComplete, status code:" << status_;
    }

    // 可能需?Redirect �后的数据否？
    if (redirect_)
    {
        OnResponse(0, 0);
        return true;
    }
    

    // 在回调中再调?WinInet �函数不太?

    INTERNET_BUFFERSA ib = {0};
    ib.dwStructSize = sizeof(INTERNET_BUFFERSA);

    const int grow = 0x2000;

	std::ofstream out;
	if (!saveto_.empty())
	{
        out.open(saveto_.c_str(), std::ios::binary);
        Assert(out.is_open());
	}

#if 1
    // TODO: �如果写文件了，就不要?buf_
    do {
        buf_.resize(content_length_ + grow);
        ib.lpvBuffer = &buf_[0] + content_length_;
        ib.dwBufferLength = grow;
        ib.dwBufferTotal = buf_.size();

        LOG(LS_VERBOSE) << "call InternetReadFileExA once";
        // (LPARAM)this => (LPARAM)0 �也解决不了问?
        BOOL bOk = InternetReadFileExA(hRequest_, &ib, IRF_NO_WAIT, (LPARAM)0);
        if (bOk)
		{
            if(out.is_open())
                out.write(&buf_[0], buf_.size());

			content_length_ += ib.dwBufferLength;
			if (ib.dwBufferLength == 0)
			{
				buf_.resize(content_length_);
			}
        }
        else
        {
            Assert(GetLastError() == ERROR_IO_PENDING);
            break;
        }
    } while(ib.dwBufferLength);
#else
    DWORD dwReaded = 0;
    do {
        buf_.resize(content_length_ + grow);
        ib.lpvBuffer = &buf_[0] + content_length_;
        ib.dwBufferLength = grow;
        ib.dwBufferTotal = buf_.size();

        BOOL ret = InternetReadFile(hRequest_, ib.lpvBuffer
            , ib.dwBufferLength, &dwReaded);
        Assert(ret);
        if (ret)
        {
            if(out.is_open())
                out.write(&buf_[0], buf_.size());

            content_length_ += dwReaded;
            if (dwReaded == 0 || dwReaded < ib.dwBufferLength)
            {
                buf_.resize(content_length_);
            }
        }
    } while (dwReaded == 0 || dwReaded < ib.dwBufferLength);
    
#endif

    LOG(LS_VERBOSE) << "RequestComplete readed: " << buf_.size();

    // �通知前必须关闭文件！
    if (out.is_open())
        out.close();

	OnResponse(&buf_[0], buf_.size());
    // 之后释放缓冲区，TODO: 可能造成问题
    std::vector<char>().swap(buf_);
    return true;
}*/

bool AsyncHttp::RequestComplete(DWORD dwResult, DWORD dwError)
{
	LOG(LS_VERBOSE) << "RequestComplete: " << dwResult 
		<< " Error: " << dwError;

	Assert(hRequest_);

    if (readed_ == 0)
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
            Assert(bRet);
            if (bRet)
                content_length_ = dwCode;
        }
    }

    bool finished = false;

    // TODO: 区别是否是 SendRequest 回调和 ReadFile回调
    if (dwResult == TRUE)
    {
        const int grow = 8192;

        BOOL loop = TRUE;

        while (loop)
        {
            // 
            buf_.resize(readed_ + grow);
            INTERNET_BUFFERSA ib = {sizeof(INTERNET_BUFFERSA)};
            ib.lpvBuffer = &buf_[0] + readed_;
            ib.dwBufferLength = grow;
            ib.dwBufferTotal = buf_.size();

            loop = InternetReadFileExA(hRequest_, &ib, IRF_NO_WAIT, (LPARAM)this);

            LOG(LS_VERBOSE) << "Read once " << hRequest_ << " return:" << loop
                << " readed:" << ib.dwBufferLength;

            if (loop)
            {
                readed_ += ib.dwBufferLength;
                loop = (0 != ib.dwBufferLength);
                finished = (0 == ib.dwBufferLength); // TODO: 正确否？
            }
            else
            {
                // TODO: use InternetGetLastResponseInfo
                DWORD err = GetLastError();
                Assert(err == ERROR_IO_PENDING);
            }
        }
    }

    if (finished)
    {
#ifndef NDEBUG
        LOG(LS_INFO) << "OnResponse:" << hRequest_
            << " status:" << status_
            << " readed:" << readed_;
#endif
        if (readed_)
            OnResponse(&buf_[0], readed_);    
        else
            OnResponse(0, 0);
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

AutoHttp::~AutoHttp()
{
    LOG(LS_VERBOSE) << "~AutoHttp:" << (void *)this;
}

// }
