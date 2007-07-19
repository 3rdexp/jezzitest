// test_asyncinet.cpp : 定义控制台应用程序的入口点。
//


#include "task.h"

class MyTask : public Task
{
public:
	MyTask(Task * parent) : Task(parent) {}
	virtual int ProcessStart()
	{
		// 启动一个 http 请求
		// Get("http://xxx.com/");
		return STATE_DONE;
	}
	virtual int ProcessResponse()
	{
		// const char * body = ResponseBody();
		return STATE_DONE;
	}
};

int main(int argc, char* argv[])
{
	MyTask root(0);
	MyTask * p = new MyTask(&root);

	root.Start();

	return 0;
}

