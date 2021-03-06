
#ifndef __ASYNCHTTP_H__
#define __ASYNCHTTP_H__

#include <windows.h>
#include <wininet.h>
#include <map>
#include <vector>
#include <string>
#include <fstream>

// #include "base/criticalsection.h"
#include "httpcomm.h"

#define LOG_URL

namespace xnbase {

    class AsyncInet;

    class AsyncHttp 
    {
    public:
        AsyncHttp() 
            : hConn_(0)
            , hRequest_(0)
            , content_length_(0)
            , status_(0), auto_redirect_(true), redirect_(false) 
            , event_(0)
            , delete_self_(false)
        {
        }
        virtual ~AsyncHttp();

        bool PrepareGet(const std::string & url, const std::string & referrer = "");
        bool PrepareGet(const std::wstring & url, const std::wstring & referrer = L"");
        bool PreparePost(const std::string & url, const std::string & content_type
            , std::istream* request_doc = 0, const std::string & referrer = "");
        bool PreparePost(const std::wstring & url, const std::wstring & content_type
            , std::istream* request_doc = 0, const std::wstring & referrer = L"");
        bool SendRequest();

        void Close();
        void Close2(); // TODO: ugly
        bool delete_self_;

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
        const std::vector<char> & getContent() const { return buf_; }

        void saveTo(const std::string & filename) { saveto_ = filename; }
        std::string savedFile() const { return saveto_; }

        void setAutoRedirect(bool r) { auto_redirect_ = r; }
        bool hasRedirect() const { return redirect_; }

        int getStatusCode() const { return status_; }
        int getContentLength() const {
            return content_length_; 
        }
        int getReaded() const { return readed_; }

    protected:
        friend class AsyncInet;
        friend void CALLBACK InternetStatusCallback(HINTERNET hInternet, DWORD_PTR dwContext
            , DWORD dwInternetStatus, LPVOID lpvStatusInformation, DWORD dwStatusInformationLength);

        // 极端特殊情形才需要重？
        virtual void HandleCreated(HINTERNET hInet);
        virtual bool NameResolved(LPCSTR pszIpAddress);
        virtual bool Redirect(LPCSTR szUrl, DWORD dwUrlLength);
        virtual bool ResponseReceived(DWORD dwBytes);
        virtual bool RequestComplete(DWORD dwResult, DWORD dwError);
        virtual bool HandleClosing(HINTERNET h);

        // TODO: impl
        virtual HINTERNET CachedConnection(LPCSTR szHost);

        // 返回结果通知
        virtual void OnResponse(const char* buffer, int length) {}

    protected:
        HINTERNET hConn_;
        HINTERNET hRequest_;
        std::string redirect_url_;
        std::string ip_;
        std::vector<char> buf_;
        unsigned long readed_;
        unsigned long content_length_;
        std::string saveto_;
        bool auto_redirect_;    // 是否自动Redirect,类似浏览器
        bool redirect_;         // 请求返回后，标明是否曾经发生过Redirect
        int status_;
        std::ofstream save_;
        HANDLE event_;
#ifdef LOG_URL
        std::string url_;
#endif

    private:
        typedef std::multimap<std::string, std::string, iless> HeaderMap;
        HeaderMap m_headers;

        AsyncHttp(const AsyncHttp &);
        AsyncHttp operator=(const AsyncHttp &);
    };

    class AsyncInet
    {
    public:
        static AsyncInet & GetInstance();

        static bool Init();
        static void Release(); // Release 以后就不应该继续使用

        static const std::string GetCookie(const std::string &url) ;
        static bool SetCookie(const std::string & url, const std::string & name
            , const std::string & data);
        

        bool Open(const std::string & agent, DWORD dwAccessType = INTERNET_OPEN_TYPE_PRECONFIG, 
            LPCSTR lpszProxyName = NULL, LPCSTR lpszProxyBypass = NULL,
            DWORD dwFlag = INTERNET_FLAG_ASYNC);

        void Close();
        bool SetMaxConnection(int max = 2);

        HINTERNET get()
        {
            Assert(hInet_);
            return hInet_; 
        }
    private:
        HINTERNET hInet_;
        // TODO:
        // typedef std::map<std::string, HINTERNET> ConnectionMap;
        // ConnectionMap allConn_;

        AsyncInet();
        ~AsyncInet();

        AsyncInet(const AsyncInet&);
        AsyncInet operator=(const AsyncInet&);
    };

}

#endif // __ASYNCHTTP_H__
