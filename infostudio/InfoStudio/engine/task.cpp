
#include <ctime>
#include <algorithm>

#include "task.h"

#define ASSERT

namespace {
__int64 CurrentTime()
{
	return static_cast<__int64>(time(0));
}
}

int Task::unique_id_seed_ = 0;

Task::Task(Task *parent)
	: state_(STATE_INIT),
	parent_(parent),
	blocked_(false),
	done_(false),
	aborted_(false),
	busy_(false),
	error_(false),
	child_error_(false),
	start_time_(0)	
{
	children_.reset(new ChildSet());
	
	if (parent_ != NULL) {
		parent_->AddChild(this);
	}

	unique_id_ = unique_id_seed_++;

	// sanity check that we didn't roll-over our id seed
	ASSERT(unique_id_ < unique_id_seed_);
}

Task::~Task()
{
}

void Task::Abort() {
	if (aborted_ || done_)
		return;
	aborted_ = true;
	if (!busy_) {
		done_ = true;
		blocked_ = true;
		error_ = true;
		Stop();
	}
}

__int64 Task::ElapsedTime() {
	return CurrentTime() - start_time_;
}

void Task::Start() {
	if (state_ != STATE_INIT)
		return;

    // 如果有parent，应该从parent执行起
    // 先执行子任务
	
	if (parent_) {
        parent_->Start();
        return;
    }

    // Set the start time before starting the task.  Otherwise if the task
    // finishes quickly and deletes the Task object, setting start_time_
    // will crash
    start_time_ = CurrentTime();

    // 只要不是全部 Blocked，就继续执行
    int did_run = true;
    while (did_run) 
    {
        did_run = false;
        // use indexing instead of iterators because children_ may grow
        for (size_t i = 0; i < children_->size(); ++i)
        {
            Task * p = children_->at(i);
            while (!p->Blocked())
            {
                p->Step();
                did_run = true;
            }
        }
    }

    // Tasks are deleted when running has paused
    bool need_timeout_recalc = false;
    for (size_t i = 0; i < children_->size(); ++i) {
        Task* task = children_->at(i);
        if (task->IsDone()) {
//            if (next_timeout_task_ &&
//                task->get_unique_id() == next_timeout_task_->get_unique_id()) {
//                    next_timeout_task_ = NULL;
//                    need_timeout_recalc = true;
//                }

                delete task;
                children_->at(i) = NULL;
        }
    }

    // Finally, remove nulls
    std::vector<Task *>::iterator it;
    it = std::remove(children_->begin(),
        children_->end(),
        reinterpret_cast<Task *>(NULL));

    children_->erase(it, children_->end());

    /*
    while (!AllChildrenDone())
	{
        if (children_->size() > 0) {
            ChildSet copy = *children_;
            for (ChildSet::iterator it = copy.begin(); it != copy.end(); ++it) {
                (*it)->Process();
            }
        }
    }

    while (!done_)
    {
        busy_ = true;
		int new_state = Process(state_);
		busy_ = false;
	

		if (new_state == STATE_BLOCKED) {
			blocked_ = true;
			// Let the timeout continue
		} else {
			state_ = new_state;
			blocked_ = false;
			// ResetTimeout();
		}

		if (new_state == STATE_DONE) {
			done_ = true;
		} else if (new_state == STATE_ERROR) {
			done_ = true;
			error_ = true;
		}

		if (done_) {
			Stop();
			blocked_ = true;
		}
	}
    */
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
        Stop();
        return;
    }

    busy_ = true;
    int new_state = Process(state_);
    busy_ = false;

    if (aborted_) {
        Abort();  // no need to wake because we're awake
        return;
    }

    if (new_state == STATE_BLOCKED) {
        blocked_ = true;
        // Let the timeout continue
    } else {
        state_ = new_state;
        blocked_ = false;
        // ResetTimeout();
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

int Task::Process(int state) {
	int newstate = STATE_ERROR;

//	if (TimedOut()) {
//		ClearTimeout();
//		newstate = OnTimeout();
//		SignalTimeout();
//	} else {
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
//	}

	return newstate;
}


void Task::AddChild(Task *child) {
	children_->push_back(child);
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
			(*it)->Abort();
		}
	}
}

void Task::OnChildStopped(Task *child) {
	if (child->HasError())
		child_error_ = true;

    ChildSet::iterator i = std::remove(children_->begin(),
        children_->end(),child);

    children_->erase(i, children_->end());
}

void Task::Stop() {
	// No need to wake because we're either awake or in abort
	AbortAllChildren();
	parent_->OnChildStopped(this);
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

// #include <wininet.h>