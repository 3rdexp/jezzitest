// test_asyncinet.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include "task.h"

class MyTask : public Task
{
public:
	MyTask(Task * parent) : Task(parent) {}
	virtual int ProcessStart()
	{
		// ����һ�� http ����
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

