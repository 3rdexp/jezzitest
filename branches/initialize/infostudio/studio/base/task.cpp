
#include <ctime>
#include <algorithm>

#include "base.h"
#include "task.h"

BEGIN_ENGINE_NAMESPACE

int32 Task::unique_id_seed_ = 0;

Task::Task(Task *parent)
    : state_(STATE_INIT),
    parent_(parent),
    blocked_(false),
    done_(false),
    aborted_(false),
    busy_(false),
    error_(false),
    child_error_(false),
    start_time_(0),
    timeout_seconds_(0),
    timeout_time_(0),
    timeout_suspended_(false)
{
    children_.reset(new ChildSet());
    runner_ = ((parent == NULL) ?
        reinterpret_cast<TaskRunner *>(this) :
    parent->GetRunner());
    if (parent_ != NULL) {
        parent_->AddChild(this);
    }

    unique_id_ = unique_id_seed_++;

    // sanity check that we didn't roll-over our id seed
    ASSERT(unique_id_ < unique_id_seed_);
}

int64 Task::CurrentTime() {
    return runner_->CurrentTime();
}

int64 Task::ElapsedTime() {
    return CurrentTime() - start_time_;
}

void Task::Start() {
    if (state_ != STATE_INIT)
        return;
    // Set the start time before starting the task.  Otherwise if the task
    // finishes quickly and deletes the Task object, setting start_time_
    // will crash.
    start_time_ = CurrentTime();
    GetRunner()->StartTask(this);
}

void Task::Step() {
    if (done_) {
#ifdef DEBUG
        // we do not know how !blocked_ happens when done_ - should be impossible.
        // But it causes problems, so in retail build, we force blocked_, and
        // under debug we assert.
        assert(blocked_);
#else
        blocked_ = true;
#endif
        return;
    }

    // Async Error() was called
    if (error_) {
        done_ = true;
        state_ = STATE_ERROR;
        blocked_ = true;
        //   obsolete - an errored task is not considered done now
        //   SignalDone();
        Stop();
        return;
    }

    busy_ = true;
    int new_state = Process(state_);
    busy_ = false;

    if (aborted_) {
        Abort(true);  // no need to wake because we're awake
        return;
    }

    if (new_state == STATE_BLOCKED) {
        blocked_ = true;
        // Let the timeout continue
    } else {
        state_ = new_state;
        blocked_ = false;
        ResetTimeout();
    }

    if (new_state == STATE_DONE) {
        done_ = true;
    } else if (new_state == STATE_ERROR) {
        done_ = true;
        error_ = true;
    }

    if (done_) {
        //  obsolete - call this yourself
        //    SignalDone();
        Stop();
        blocked_ = true;
    }
}

void Task::Abort(bool nowake) {
    if (aborted_ || done_)
        return;
    aborted_ = true;
    if (!busy_) {
        done_ = true;
        blocked_ = true;
        error_ = true;
        Stop();
        if (!nowake)
            Wake();  // to self-delete
    }
}

void Task::Wake() {
    if (done_)
        return;
    if (blocked_) {
        blocked_ = false;
        GetRunner()->WakeTasks();
    }
}

void Task::Error() {
    if (error_ || done_)
        return;
    error_ = true;
    Wake();
}

std::string Task::GetStateName(int state) const {
    static const std::string STR_BLOCKED("BLOCKED");
    static const std::string STR_INIT("INIT");
    static const std::string STR_START("START");
    static const std::string STR_DONE("DONE");
    static const std::string STR_ERROR("ERROR");
    static const std::string STR_RESPONSE("RESPONSE");
    static const std::string STR_HUH("??");
    switch (state) {
    case STATE_BLOCKED: return STR_BLOCKED;
    case STATE_INIT: return STR_INIT;
    case STATE_START: return STR_START;
    case STATE_DONE: return STR_DONE;
    case STATE_ERROR: return STR_ERROR;
    case STATE_RESPONSE: return STR_RESPONSE;
    }
    return STR_HUH;
}

int Task::Process(int state) {
    int newstate = STATE_ERROR;

    if (TimedOut()) {
        ClearTimeout();
        newstate = OnTimeout();
        SignalTimeout();
    } else {
        switch (state) {
      case STATE_INIT:
          newstate = STATE_START;
          break;
      case STATE_START:
          newstate = ProcessStart();
          break;
      case STATE_RESPONSE:
          newstate = ProcessResponse();
          break;
      case STATE_DONE:
      case STATE_ERROR:
          newstate = STATE_BLOCKED;
          break;
        }
    }

    return newstate;
}

void Task::AddChild(Task *child) {
    children_->insert(child);
}

bool Task::AllChildrenDone() {
    for (ChildSet::iterator it = children_->begin();
        it != children_->end();
        ++it) {
            if (!(*it)->IsDone())
                return false;
        }
        return true;
}

bool Task::AnyChildError() {
    return child_error_;
}

void Task::AbortAllChildren() {
    if (children_->size() > 0) {
        ChildSet copy = *children_;
        for (ChildSet::iterator it = copy.begin(); it != copy.end(); ++it) {
            (*it)->Abort(true);  // Note we do not wake
        }
    }
}

void Task::Stop() {
    // No need to wake because we're either awake or in abort
    AbortAllChildren();
    parent_->OnChildStopped(this);
}

void Task::OnChildStopped(Task *child) {
    if (child->HasError())
        child_error_ = true;
    children_->erase(child);
}

void Task::set_timeout_seconds(const int timeout_seconds) {
    timeout_seconds_ = timeout_seconds;
    ResetTimeout();
}

bool Task::TimedOut() {
    return timeout_seconds_ &&
        timeout_time_ &&
        CurrentTime() > timeout_time_;
}

void Task::ResetTimeout() {
    bool timeout_allowed = (state_ != STATE_INIT)
        && (state_ != STATE_DONE)
        && (state_ != STATE_ERROR);
    if (timeout_seconds_ && timeout_allowed && !timeout_suspended_)
        timeout_time_ = CurrentTime() +
        (timeout_seconds_ * kSecToMsec * kMsecTo100ns);
    else
        timeout_time_ = 0;

    GetRunner()->UpdateTaskTimeout(this);
}

void Task::ClearTimeout() {
    timeout_time_ = 0;
    GetRunner()->UpdateTaskTimeout(this);
}

void Task::SuspendTimeout() {
    if (!timeout_suspended_) {
        timeout_suspended_ = true;
        ResetTimeout();
    }
}

void Task::ResumeTimeout() {
    if (timeout_suspended_) {
        timeout_suspended_ = false;
        ResetTimeout();
    }
}

//////////////////////////////////////////////////////////////////////////
//
TaskRunner::TaskRunner()
: Task(NULL),
tasks_running_(false),
next_timeout_task_(NULL) {
}

TaskRunner::~TaskRunner() {
    // this kills and deletes children silently!
    AbortAllChildren();
    RunTasks();
}

void TaskRunner::StartTask(Task * task) {
    tasks_.push_back(task);

    // the task we just started could be about to timeout --
    // make sure our "next timeout task" is correct
    UpdateTaskTimeout(task);

    WakeTasks();
}

void TaskRunner::RunTasks() {
    // Running continues until all tasks are Blocked (ok for a small # of tasks)
    if (tasks_running_) {
        return;  // don't reenter
    }

    tasks_running_ = true;

    int did_run = true;
    while (did_run) {
        did_run = false;
        // use indexing instead of iterators because tasks_ may grow
        for (size_t i = 0; i < tasks_.size(); ++i) {
            while (!tasks_[i]->Blocked()) {
                tasks_[i]->Step();
                did_run = true;
            }
        }
    }
    // Tasks are deleted when running has paused
    bool need_timeout_recalc = false;
    for (size_t i = 0; i < tasks_.size(); ++i) {
        if (tasks_[i]->IsDone()) {
            Task* task = tasks_[i];
            if (next_timeout_task_ &&
                task->get_unique_id() == next_timeout_task_->get_unique_id()) {
                    next_timeout_task_ = NULL;
                    need_timeout_recalc = true;
                }

                delete task;
                tasks_[i] = NULL;
        }
    }
    // Finally, remove nulls
    std::vector<Task *>::iterator it;
    it = std::remove(tasks_.begin(),
        tasks_.end(),
        reinterpret_cast<Task *>(NULL));

    tasks_.erase(it, tasks_.end());

    if (need_timeout_recalc)
        RecalcNextTimeout(NULL);

    tasks_running_ = false;
}

void TaskRunner::PollTasks() {
    // see if our "next potentially timed-out task" has indeed timed out.
    // If it has, wake it up, then queue up the next task in line
    if (next_timeout_task_ &&
        next_timeout_task_->TimedOut()) {
            next_timeout_task_->Wake();
            WakeTasks();
        }
}

// this function gets called frequently -- when each task changes
// state to something other than DONE, ERROR or BLOCKED, it calls
// ResetTimeout(), which will call this function to make sure that
// the next timeout-able task hasn't changed.  The logic in this function
// prevents RecalcNextTimeout() from getting called in most cases,
// effectively making the task scheduler O-1 instead of O-N

void TaskRunner::UpdateTaskTimeout(Task *task) {
    ASSERT(task != NULL);

    // if the relevant task has a timeout, then
    // check to see if it's closer than the current
    // "about to timeout" task
    if (task->get_timeout_time()) {
        if (next_timeout_task_ == NULL ||
            (task->get_timeout_time() <=
            next_timeout_task_->get_timeout_time())) {
                next_timeout_task_ = task;
            }
    } else if (next_timeout_task_ != NULL &&
        task->get_unique_id() == next_timeout_task_->get_unique_id()) {
            // otherwise, if the task doesn't have a timeout,
            // and it used to be our "about to timeout" task,
            // walk through all the tasks looking for the real
            // "about to timeout" task
            RecalcNextTimeout(task);
        }
}

void TaskRunner::RecalcNextTimeout(Task *exclude_task) {
    // walk through all the tasks looking for the one
    // which satisfies the following:
    //   it's not finished already
    //   we're not excluding it
    //   it has the closest timeout time

    int64 next_timeout_time = 0;
    next_timeout_task_ = NULL;

    for (size_t i = 0; i < tasks_.size(); ++i) {
        Task *task = tasks_[i];
        // if the task isn't complete, and it actually has a timeout time
        if (!task->IsDone() &&
            (task->get_timeout_time() > 0))
            // if it doesn't match our "exclude" task
            if (exclude_task == NULL ||
                exclude_task->get_unique_id() != task->get_unique_id())
                // if its timeout time is sooner than our current timeout time
                if (next_timeout_time == 0 ||
                    task->get_timeout_time() <= next_timeout_time) {
                        // set this task as our next-to-timeout
                        next_timeout_time = task->get_timeout_time();
                        next_timeout_task_ = task;
                    }
    }
}

END_ENGINE_NAMESPACE
