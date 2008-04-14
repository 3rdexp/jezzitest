// ds.cpp : 定义控制台应用程序的入口点。
//

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

using namespace std;

void Wait() {
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

#if 0

#include "base/synchttp.h"

void GlobalResponse(int status_code, const char * buf, int len) {
    PostQuitMessage(0);
}

void OnResponse(int status_code, const char * buf, int len) {
    PostQuitMessage(0);
}



struct foo {
    void f(int status_code, const char * buf, int len) {
        PostQuitMessage(0);
    }

    void OnResponse(int status_code, const char * buf, int len) {
        PostQuitMessage(0);
    }
};

int main(int argc, char* argv[])
{
    xnbase::AsyncInet::Init();

    {
        SyncHttp h;
        h.SetCallback(&OnResponse);
        h.PrepareGet(L"http://www.google.com/");

        Wait();
        h.Close();

        foo bar;
        h.SetCallback(boost::bind(&foo::OnResponse, &bar, _1, _2, _3));
        h.PrepareGet(L"http://www.google.com/en");

        Wait();
    }

    xnbase::AsyncInet::Release();
    
	return 0;
}
#else

#include "engine/infoengine.h"
#include "engine/wndrunner.h"

int main() {

    xnbase::AsyncInet::Init();

    Site site;
    Dictionary dict;
    {
        VariableMap vm;
        vm.insert(VariableMap::value_type(L"male", L"1"));
        vm.insert(VariableMap::value_type(L"female", L"2"));
        dict.Insert(L"sex", vm);
    }
    site.SetDict(dict);
    site.homepage = L"http://baidu.com";

    {
        Action act;
        act.aid = 0;
        act.type = AT_REGISTER;
        act.url = L"http://www.baidu.com/";
        act.method = HV_GET;
        act.restype = ART_NONE;
        site.AddAction(&act);
    }

    {
        Action act;
        act.aid = 1;
        act.url = L"http://www.baidu.com/s";
        act.method = HV_GET;
        act.vars = L"wd={key}&aa={sex}";
        act.type = AT_UTILITY;
        act.restype = ART_NONE;
        site.AddAction(&act);
    }

    // sites_.push_back(site);

    UserInfo userinfo_;
    userinfo_.insert(L"sex", L"male");
    userinfo_.insert(L"pasw", L"strongpsw");
    userinfo_.insert(L"mail", L"boostguy@gmail.com");
    
    WindowRunner wr;
    HWND h = wr.Create(0);
    ATLASSERT(h);

    GeneralTask * nrt = new NormalRequestTask(&wr, site
        , site.actions()[1], userinfo_);
    nrt->Start();

    Wait();

    xnbase::AsyncInet::Release();

    return 0;
}

#endif
