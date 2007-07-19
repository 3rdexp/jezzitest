
#include "sigslot.h"

// 支持任务级联的基类
class Task
{
public:
	Task(Task * parent);
	virtual ~Task();

	int32 get_unique_id() { return unique_id_; }

	int GetState() const { return state_; }
	bool HasError() const { return (GetState() == STATE_ERROR); }
	bool Blocked() const { return blocked_; }
	bool IsDone() const { return done_; }
	int64 ElapsedTime();

	Task *GetParent() { return parent_; }

	// Called from outside to stop task without any more callbacks
	void Abort();

	// For managing children
	bool AllChildrenDone();
	bool AnyChildError();

protected:

	enum {
		STATE_BLOCKED = -1,
		STATE_INIT = 0,
		STATE_START = 1,
		STATE_DONE = 2,
		STATE_ERROR = 3,
		STATE_RESPONSE = 4,
		STATE_NEXT = 5,  // Subclasses which need more states start here and higher
	};

	virtual std::string GetStateName(int state) const;
	virtual int Process(int state);
	virtual void Stop();
	virtual int ProcessStart() = 0;
	virtual int ProcessResponse() { return STATE_DONE; }

	void AddChild(Task *child);
	void AbortAllChildren();

private:
	void OnChildStopped(Task *child);

private:
	int state_;
	Task *parent_;
	bool blocked_;
	bool done_;
	bool aborted_;
	bool busy_;
	bool error_;
	bool child_error_;
	int64 start_time_;
	int32 unique_id_;

	static int32 unique_id_seed_;

	typedef std::set<Task *> ChildSet;
	boost::scoped_ptr<ChildSet> children_;
};

class AsyncTask : public sigslot::has_slots<sigslot::multi_threaded_local>
{
public:
	AsyncTask();
	virtual ~AsyncTask();

	virtual void ProcessStart();
	virtual void ProcessResponse();

	bool PrepareGet();
	bool PreparePost();

protected:
	// HINTERNET hConnection;
};


class MyTask : public Task
{
public:
	virtual void ProcessStart()
	{
		// 启动一个 http 请求
		Get("http://xxx.com/");
	}
	virtual void ProcessResponse()
	{
		const char * body = ResponseBody();
	}
};