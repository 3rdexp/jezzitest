
#include <ctime>

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
			(*it)->Abort();
		}
	}
}

void Task::OnChildStopped(Task *child) {
	if (child->HasError())
		child_error_ = true;
	children_->erase(child);
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