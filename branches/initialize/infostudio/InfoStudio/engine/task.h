
#ifndef __TASK_H__
#define __TASK_H__

#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

BEGIN_ENGINE_NAMESPACE

// 支持任务级联的基类
class Task
{
public:
	Task(Task * parent);
	virtual ~Task();

	int get_unique_id() { return unique_id_; }

	void Start();
    void Step();
	int GetState() const { return state_; }
	bool HasError() const { return (GetState() == STATE_ERROR); }
	bool Blocked() const { return blocked_; }
	bool IsDone() const { return done_; }
	__int64 ElapsedTime();

	Task *GetParent() { return parent_; }

	// Called from outside to stop task without any more callbacks
	void Abort();

	// For managing children
	bool AllChildrenDone();
	bool AnyChildError();

    void Wake();

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

    // 必须是可重入的锁
    virtual void RecLock() {}
    virtual void Unlock() {}

	void AddChild(Task *child);
	void AbortAllChildren();

private:
	void OnChildStopped(Task *child);

    struct RecAutoLock {
        RecAutoLock(Task* task) : task_(task) {
            task_->RecLock();
        }
        ~RecAutoLock() {
            task_->Unlock();
        }
        Task * task_;
    };

private:
	int state_;
	Task *parent_;
	bool blocked_;
	bool done_;
	bool aborted_;
	bool busy_;
	bool error_;
	bool child_error_;
	__int64 start_time_;
	int unique_id_;

	static int unique_id_seed_;

	typedef std::vector<Task *> ChildSet;
	boost::scoped_ptr<ChildSet> children_;
};

END_ENGINE_NAMESPACE

#endif // __TASK_H__
