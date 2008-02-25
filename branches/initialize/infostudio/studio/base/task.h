
#ifndef __TASK_H__
#define __TASK_H__

#include <boost/scoped_ptr.hpp>
#include <string>
#include <vector>

#include "sigslot.h"
#include "base.h"


class Task;
class TaskRunner;

const int64 kSecToMsec = 1000;
const int64 kMsecTo100ns = 10000;
const int64 kSecTo100ns = kSecToMsec * kMsecTo100ns;

class Task {
public:
    Task(Task *parent);
    virtual ~Task() {}

    int32 get_unique_id() { return unique_id_; }

    void Start();
    void Step();
    int GetState() const { return state_; }
    bool HasError() const { return (GetState() == STATE_ERROR); }
    bool Blocked() const { return blocked_; }
    bool IsDone() const { return done_; }
    int64 ElapsedTime();

    Task *GetParent() { return parent_; }
    TaskRunner *GetRunner() { return runner_; }
    virtual Task *GetParent(int code) { return parent_->GetParent(code); }

    // Called from outside to stop task without any more callbacks
    void Abort(bool nowake = false);

    // For managing children
    bool AllChildrenDone();
    bool AnyChildError();

    bool TimedOut();

    int64 get_timeout_time() { return timeout_time_; }
    void set_timeout_seconds(int timeout_seconds);

    sigslot::signal0<> SignalTimeout;

    // Called inside the task to signal that the task may be unblocked
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

    // Called inside to advise that the task should wake and signal an error
    void Error();

    int64 CurrentTime();

    virtual std::string GetStateName(int state) const;
    virtual int Process(int state);
    virtual void Stop();
    virtual int ProcessStart() = 0;
    virtual int ProcessResponse() { return STATE_DONE; }

    virtual void SetState(int state) { state_ = state; }

    // for managing children (if any)
    void AddChild(Task *child);
    void AbortAllChildren();

    void ResetTimeout();
    void ClearTimeout();

    void SuspendTimeout();
    void ResumeTimeout();

protected:
    virtual int OnTimeout() {
        // by default, we are finished after timing out
        return STATE_DONE;
    }

private:
    void OnChildStopped(Task *child);

    int state_;
    Task *parent_;
    TaskRunner *runner_;
    bool blocked_;
    bool done_;
    bool aborted_;
    bool busy_;
    bool error_;
    bool child_error_;
    int64 start_time_;
    int64 timeout_time_;
    int timeout_seconds_;
    bool timeout_suspended_;
    int32 unique_id_;

    static int32 unique_id_seed_;

    // for managing children
    typedef std::set<Task *> ChildSet;
    boost::scoped_ptr<ChildSet> children_;
};

//////////////////////////////////////////////////////////////////////////
//
class TaskRunner : public Task, public sigslot::has_slots<> {
public:
    TaskRunner();
    virtual ~TaskRunner();

    virtual void WakeTasks() = 0;

    // This method returns the current time in 100ns units since 1/1/1601.  This
    // Is the GetSystemTimeAsFileTime method on windows.
    virtual int64 CurrentTime() = 0 ;

    void StartTask(Task *task);
    void RunTasks();
    void PollTasks();

    void UpdateTaskTimeout(Task *task);

    // dummy state machine - never run.
    virtual int ProcessStart() { return STATE_DONE; }

private:
    std::vector<Task *> tasks_;
    Task *next_timeout_task_;
    bool tasks_running_;

    void RecalcNextTimeout(Task *exclude_task);
};


#endif // __TASK_H__
