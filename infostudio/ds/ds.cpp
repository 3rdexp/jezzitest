// ds.cpp : 定义控制台应用程序的入口点。
//

#include <boost/bind.hpp>
#include <boost/function.hpp>

#if 1

#include <atlbase.h>
#include <atlapp.h>
#include <atlmisc.h>

#include "../studio/base/synchttp.h"


using namespace std;

void GlobalResponse(int status_code, const char * buf, int len) {
    PostQuitMessage(0);
}

void OnResponse(int status_code, const char * buf, int len) {
    PostQuitMessage(0);
}

void Wait() {
    MSG msg;
    while (GetMessage(&msg, 0, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
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
        h.PrepareGet("http://www.google.com/");

        Wait();
        h.Close();

        foo bar;
        h.SetCallback(boost::bind(&foo::OnResponse, &bar, _1, _2, _3));
        h.PrepareGet("http://www.google.com/en");

        Wait();
    }

    xnbase::AsyncInet::Release();
    
	return 0;
}
#else

long f_3(long a, long b, long c)
{
    return a + 10 * b + 100 * c;
}

struct foo {
    void bar(long a, long b, long c) {}
};

int main() {
    boost::function3<long, long, long , long > ff;

    boost::bind(&f_3, _1, _2, _3)(1, 0, 0);
    foo f;
    boost::bind(&foo::bar, &f, _1, _2, _3)(1, 0, 0);
}

#endif
