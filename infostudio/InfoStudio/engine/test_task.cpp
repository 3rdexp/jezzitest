// test_asyncinet.cpp : 定义控制台应用程序的入口点。
//

#include <fstream>
#include <iostream>

#include "logging.h"
#include "task.h"
#include "asynchttp.h"


void InitLog()
{
    static std::ofstream errfs("cerr.log");
    
    std::cerr.rdbuf(errfs.rdbuf());
}

static int finished = 0;

class MyTask : public Task
{
public:
	MyTask(Task * parent) : Task(parent) {}
	virtual int ProcessStart()
	{
		// 启动一个 http 请求
		// Get("http://xxx.com/");
        std::clog << (void *) this << " ProcessStart\n";
		return STATE_RESPONSE;
	}
	virtual int ProcessResponse()
	{
        finished ++;
        std::clog << (void *) this << " ProcessResponse " << this->get_unique_id() << "\n";
		// const char * body = ResponseBody();
		return STATE_DONE;
	}
};

int main(int argc, char* argv[])
{
#if 0
	MyTask root(0);
    int all = 1;
    int c = rand() % 200;
    for (int i=0; i<c; ++i)
    {
        MyTask * p = new MyTask(&root);
        all ++;
        int r = rand() % 100;
        for (int j=0; j<r; ++j)
        {
            MyTask * p2 = new MyTask(p);
            all ++;
        }
    }

	root.Start();
    std::cout << "all: " << all << std::endl;
    std::cout << "finished: " << finished << std::endl;
#endif



    AsyncInet::Init();

    LPCWSTR urls[] = 
    {
        L"http://musicdata.dudu.com/search.php?id=mf7w0fxzIVfTFFa9",
            L"http://www.codeproject.com",
            L"http://news.sohu.com",
            L"http://news.163.com",
            L"http://www.google.com/intl/zh-CN/about.html",
            L"http://www.google.com", // redirect
            L"http://www.google.com/images/google_80wht.gif",
            L"http://pack.google.com",
            L"http://163.com",
            L"http://ken.com",
            L"http://bug/bugzilla",
            L"http://news.sina.com.cn",
            L"http://download.kuho.com/kuho/kuho.exe",
            L"http://labs.google.com/",
            L"http://reader.google.com/",
            L"http://sourceforge.net/projects/libtorrent/",
            L"http://sf.net/",
            L"http://www.boost.org/",
            L"http://bizsolutions.google.com/services/",
    };
#ifndef ARRAYSIZE
    #define ARRAYSIZE(a)		(sizeof(a)/sizeof((a)[0]))
#endif

    LOG(LS_INFO) << "first log";

    // LogMessage::LogToStream(&std::cerr, LS_INFO);
    LogMessage::LogThreads(true);
    LogMessage::LogTimestamps(true);

    LOG(LS_INFO) << "second log";



#if 0
    const int c = ARRAYSIZE(urls);


    AsyncHttp client[c];
    for (int i=0; i<c; ++i)
    {
        if (client[i].PrepareGet(urls[i]))
        {
            client[i].SendRequest();
        }
    }

    // http://www.google.com/intl/en_ALL/images/logo.gif

    char ch;
    std::cin >> ch;

    AsyncInet::Release();
#endif

	return 0;
}

