
#ifndef __ASYNCHTTP_H__
#define __ASYNCHTTP_H__

#include <windows.h>
#include <wininet.h>
#include <map>
#include <vector>
#include <iosfwd>

#include "criticalsection.h"
#include "httpcomm.h"

#ifndef ASSERT
    #include <cassert>
    #define ASSERT assert
#endif

class AsyncInet;

class AsyncHttp 
{
public:
    AsyncHttp();
    virtual ~AsyncHttp();

    bool PrepareGet(const std::string & url, const std::string & referrer = "");
    bool PreparePost(const std::string & url, const std::string & content_type
        , std::istream* request_doc = 0, const std::string & referrer = "");
    bool SendRequest();

    // --------------------------------------------
    // 有些 Header Key 可以重复
    enum HeaderCombine { HC_YES, HC_NO, HC_AUTO, HC_REPLACE, HC_NEW };

    // string 
    void changeHeader(const std::string& name, const std::string& value, HeaderCombine combine = HC_AUTO);
    void addHeader(const std::string& name, const std::string& value, bool append = true)
    {
        changeHeader(name, value, append ? HC_AUTO : HC_NO);
    }
    void setHeader(const std::string& name, const std::string& value, bool overwrite = true) 
    {
        changeHeader(name, value, overwrite ? HC_REPLACE : HC_NEW);
    }
    void clearHeader(const std::string& name);
    bool hasHeader(const std::string& name, std::string* value) const;

    // Convenience methods using HttpHeader
    // 
    inline void changeHeader(HttpHeader header, const std::string& value,
                            HeaderCombine combine = HC_AUTO) {
        changeHeader(ToString(header), value, combine);
    }
    inline void addHeader(HttpHeader header, const std::string& value,
                        bool append = true) {
        addHeader(ToString(header), value, append);
    }
    inline void setHeader(HttpHeader header, const std::string& value,
                        bool overwrite = true) {
        setHeader(ToString(header), value, overwrite);
    }
    inline void clearHeader(HttpHeader header) {
        clearHeader(ToString(header));
    }
    inline bool hasHeader(HttpHeader header, std::string* value) const {
        return hasHeader(ToString(header), value);
    }

    // -------------------------------------
    void setContent(const std::string& content_type, std::istream* document);

protected:
    friend class AsyncInet;
    friend void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext
        , DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);
    // 
    void HandleCreated(HINTERNET hInet);
    bool NameResolved(LPCSTR pszIpAddress);
    bool Redirect(LPCSTR szUrl, DWORD dwUrlLength);
    bool ResponseReceived(DWORD dwBytes);
    bool RequestComplete(DWORD dwResult, DWORD dwError);

    virtual void RequestDone() {}

    // 1 对于简单任务，直接返回一个新的Connection
    // 2 对于复杂任务，可以一个站点公用一个Connection
    virtual HINTERNET GetConnection(LPCSTR szHost)
    {
        return hConn_;
    }

protected:
    HINTERNET hConn_;
    HINTERNET hRequest_;
    std::string redirect_url_;
    std::string ip_;
    std::vector<char> buf_;
    unsigned long content_length_;

private:
    typedef std::multimap<std::string, std::string, iless> HeaderMap;
    HeaderMap m_headers;
};


class AsyncInet
{
public:
    AsyncInet();
    ~AsyncInet();

    bool Open(const std::string & agent, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG, 
        LPCSTR lpszProxyName = NULL, LPCSTR lpszProxyBypass = NULL,
        DWORD dwFlag = INTERNET_FLAG_ASYNC);

    void Close();

    HINTERNET get()
    {
        ASSERT(hInet_);
        return hInet_; 
    }


    bool SetCookie(const std::string & url, const std::string & name, const std::string & data);


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
