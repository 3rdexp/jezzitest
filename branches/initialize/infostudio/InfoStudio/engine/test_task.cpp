// test_asyncinet.cpp : 定义控制台应用程序的入口点。
//

#include <fstream>
#include <iostream>
#include "task.h"



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

	return 0;
}

