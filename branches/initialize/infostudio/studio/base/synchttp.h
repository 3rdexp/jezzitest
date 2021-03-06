#ifndef __SYNCHTTP_H__
#define __SYNCHTTP_H__

#include <boost/function.hpp>
#include "asynchttp.h"
#include <atlctrls.h>

struct HttpResponse {
    HttpResponse(int status, const char * buf, int len) 
        : status_(status), buf_(buf), len_(len) {}

    int status() const { return status_; }
    const char * buffer() const { return buf_; }
    int length() const { return len_; }
private:
    const int status_;
    const char * buf_;
    const int len_;
};

class SyncHttp {
public:
    SyncHttp() : handler_(0)
        , responsed_(false)
    {
        if (!proxy_.m_hWnd)
            proxy_.Create(NULL, CRect(100, 100, 800, 600));
        proxy_.AddRef();
    }

    virtual ~SyncHttp() {
        delete handler_;
        int c = proxy_.Release();
        if (0 == c)
            proxy_.DestroyWindow();
    }

    typedef boost::function<void (int, const char *, int)> call_type;

    void SetCallback(const call_type & resp) { call_ = resp; }

    bool PrepareGet(const std::wstring & url, const std::wstring & referrer = L"") {
        Assert(!handler_);
        handler_ = new Handler(this, proxy_);
        handler_->PrepareGet(url, referrer);
        return handler_->SendRequest();
    }

    bool PreparePost(const std::wstring & url, const std::wstring & content_type
        , std::istream * request_doc = 0, const std::wstring & referrer = L"") {
             Assert(!handler_);
             handler_ = new Handler(this, proxy_);
             handler_->PreparePost(url, content_type, request_doc, referrer);
            return handler_->SendRequest();
            return false;
    }    

    void Close() {
        if (handler_) {
            handler_->Close2();
            handler_ = 0;
        }
    }

private:
    friend class ThreadProxy;
    class ThreadProxy : public CWindowImpl<ThreadProxy, CEdit, CFrameWinTraits>
    {
    public:
        ThreadProxy() : ref_(0) {}

        BEGIN_MSG_MAP(ThreadProxy)
            MESSAGE_HANDLER(WM_USER, OnHttpResponse)
        END_MSG_MAP()

        LRESULT OnHttpResponse(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/) {
            SyncHttp * h = (SyncHttp *)wParam;
            h->Response();
            return 0;
        }

        void AddRef() { ref_++; }
        int Release() { return --ref_; }

    private:
        int ref_;
    };

    void Response() {
        // get Handler's content, status code
        Assert(handler_);
        const std::vector<char> & c = handler_->getContent();
        call_(handler_->getStatusCode(), &c[0], handler_->getContentLength());
    }

    static ThreadProxy proxy_;

    class Handler : public xnbase::AsyncHttp {
    public:
        Handler(SyncHttp * parent, ThreadProxy & proxy) : parent_(parent) 
            , proxy_(proxy)
        {}
        virtual void OnResponse(const char* buffer, int length) {
            proxy_.SendNotifyMessage(WM_USER, (WPARAM)(void *)parent_, 0);
        }

    private:
        SyncHttp * parent_;
        ThreadProxy & proxy_;
    };

    Handler * handler_;
    bool responsed_;
    call_type call_;
};

#endif // __SYNCHTTP_H__
