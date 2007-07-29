// test_asyncinet.cpp : 定义控制台应用程序的入口点。
//

#include <fstream>
#include <iostream>

#include "logging.h"
#include "task.h"
#include "asynchttp.h"
#include "asynctask.h"


void InitLog()
{
    static std::ofstream errfs("cerr.log");
    
    std::cerr.rdbuf(errfs.rdbuf());
}

static int finished = 0;

// 同步
class SyncTask : public Task
{
public:
	SyncTask(Task * parent) : Task(parent) {}
	virtual int ProcessStart()
	{
		// 启动一个 http 请求
		// Get("http://xxx.com/");
        std::clog << (void *) this << " ProcessStart\n";
		return STATE_RESPONSE;
	}
	virtual int ProcessResponse()
	{
        std::clog << (void *) this << " ProcessResponse " << this->get_unique_id() << "\n";
		return STATE_DONE;
	}

    virtual void Stop()
    {
        finished ++;
    }
};

bool run = false;
class SyncRunner : public TaskRunner
{
public:
    virtual void WakeTasks()
    {
        if (run)
            RunTasks();
    }

    virtual int64 CurrentTime()
    {
        return Time();
    }
};

class SimplyGetTask : public AsyncTask
{
public:
    SimplyGetTask(Task * parent, const std::string & url, const std::string & saveto)
        : AsyncTask(parent), url_(url), outf_(saveto.c_str(), std::ios::binary)
    {
    }

    int ProcessStart()
    {
        PrepareGet(url_);
        SendRequest();
        return STATE_BLOCKED;
    }

    void Stop()
    {
        LOG(INFO) << url_;
        if (!buf_.empty())
            std::copy(buf_.begin(), buf_.end(), std::ostream_iterator<char>(outf_));
        outf_.close();
    }

    std::string url_;
    std::ofstream outf_;
};

#include "unescape.h"

int main(int argc, char* argv[])
{
#if 0
    {
        UrlQueryUnEscape une;
        std::string r = une("long+value+%D6%D0%CE%C4");

        r = une("ftext=Your+Name&%B9%EE%D2%EC=%D6%D0+%CE%C4&fpassword=eeee&fradio=0&ftext2=long+value+%D6%D0%CE%C4&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK");

        r = une("ftext=Your+Name&%E8%AF%A1%E5%BC%82=%E4%B8%AD+%E6%96%87&fpassword=eeee&fradio=0&ftext2=long+value+%E4%B8%AD%E6%96%87&fcheckbox=on&fselect=1&fselect=3&fsubmit=OK");
    }
#endif

#if 0
	SyncRunner root;
    int all = 0;
    int c = rand() % 200;
    for (int i=0; i<c; ++i)
    {
        SyncTask * p = new SyncTask(&root);
        all ++;
        int r = rand() % 100;
        for (int j=0; j<r; ++j)
        {
            SyncTask * p2 = new SyncTask(p);
            all ++;
            root.StartTask(p2);
        }

        root.StartTask(p);
    }

    std::cout << "all: " << all << std::endl;
    std::cout << "finished: " << finished << std::endl;

    run = true;
    root.RunTasks();
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
        // "http://www.google.com/images/google_80wht.gif",
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

    std::string dest = request.build();

    request.clear("");
    request.parse(dest);

    std::string dest2 = request.build();
    ASSERT(dest == dest2);

    std::string rm_ansi = request.build();
    std::string rm_utf8 = request.build(RequestMap::CHARSET_UTF8);

    std::stringstream ss(rm_utf8);
    AsyncHttp cp;
    cp.PreparePost("http://localhost:8080/info.php", "application/x-www-form-urlencoded", &ss);
    cp.SendRequest();

#elif 1
    SyncRunner sr;
    
    for (int i=0; i<ARRAYSIZE(urls); ++i)
    {
        std::string fn = urls[i];
        std::string::size_type p = std::string::npos;
        do {
            p = fn.find_first_of(":/?=&");
            if (p != std::string::npos)
                fn.replace(p, 1, "_");
        } while(p != std::string::npos);

        fn.insert(0, "outdata\\");
        
        SimplyGetTask * pt = new SimplyGetTask(&sr, urls[i], fn);
        sr.StartTask(pt);
    }
    run = true;
    sr.RunTasks();
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
