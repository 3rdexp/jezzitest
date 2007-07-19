

#include "task.h"
#include "taskrunner.h"

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
	if (parent_)
		parent_->Start();
	else
	{
		busy_ = true;
		int new_state = Process(state_);
		busy_ = false;
	}
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

#include <wininet.h>