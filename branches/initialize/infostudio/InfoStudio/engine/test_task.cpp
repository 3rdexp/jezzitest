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

    LPCSTR urls[] = 
    {
        "http://musicdata.dudu.com/search.php?id=mf7w0fxzIVfTFFa9",
            "http://www.codeproject.com",
            "http://news.sohu.com",
            "http://news.163.com",
            "http://www.google.com/intl/zh-CN/about.html",
            "http://www.google.com", // redirect
            "http://www.google.com/images/google_80wht.gif",
            "http://pack.google.com",
            "http://163.com",
            "http://ken.com",
            "http://bug/bugzilla",
            "http://news.sina.com.cn",
            "http://download.kuho.com/kuho/kuho.exe",
            "http://labs.google.com/",
            "http://reader.google.com/",
            "http://sourceforge.net/projects/libtorrent/",
            "http://sf.net/",
            "http://www.boost.org/",
            "http://bizsolutions.google.com/services/",
    };


    // LOG(LS_INFO) << "first log";

    // LogMessage::LogToStream(&std::cerr, LS_INFO);
    LogMessage::LogThreads(true);
    LogMessage::LogTimestamps(true);

    LogMessage::LogToDebug(LS_VERBOSE);


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

    

#elif 0
    AsyncHttp cp;
    std::stringstream ss;
    ss << "f1=v1&hell=god";
//    AsyncInet::GetInstance().SetCookie("http://localhost/", ""
//        , "Test=test_value; expires=Sat, 01-Jan-2008 00:00:00 GMT; path=/;");
    cp.PreparePost("http://localhost:8080/info.php", "application/x-www-form-urlencoded", &ss);
    cp.setHeader(HH_COOKIE, "fuck=you");
    cp.changeHeader(HH_COOKIE, "you=wrong");
    cp.SendRequest();    


#elif 1
    struct StringPair
    {
        const char * key;
        const char * value;
    };

    StringPair form[] = {
        {"ftext", "Your Name"},
        {"诡异", "中 文"},
        {"fpassword", "eeee"},
        {"fradio", "0"},
        {"ftext2", "long value 中文"},
        {"fcheckbox", "on"},
        {"fselect", "1"},
        {"fselect", "3"},
        {"fsubmit", "OK"},
    };

    RequestMap request;
    const int c = ARRAYSIZE(form);
    for (int i=0; i<c; ++i)
        request.addParam(form[i].key, form[i].value, false);

    std::string rm_ansi = request.build();
    std::string rm_utf8 = request.build(RequestMap::CHARSET_UTF8);

#endif

    // get
    // utf8
    // ftext=Your+Name&%E8%AF%A1%E5%BC%82=%E4%B8%AD+%E6%96%87&fpassword=eeee&fradio=0&ftext2=long+value+%E4%B8%AD%E6%96%87&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK
    // gb2312
    // ftext=Your+Name&%B9%EE%D2%EC=%D6%D0+%CE%C4&fpassword=eeee&fradio=0&ftext2=long+value+%D6%D0%CE%C4&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK

    // post
    // utf8
    // ftext=Your+Name&%E8%AF%A1%E5%BC%82=%E4%B8%AD+%E6%96%87&fpassword=eeee&fradio=0&ftext2=long+value+%E4%B8%AD%E6%96%87&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK
    // gb2312
    // ftext=Your+Name&%B9%EE%D2%EC=%D6%D0+%CE%C4&fpassword=eeee&fradio=0&ftext2=long+value+%D6%D0%CE%C4&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK


    

    char ch;
    std::cin >> ch;
    AsyncInet::Release();

	return 0;
}
