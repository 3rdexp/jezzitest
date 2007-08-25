#ifndef __CRITICAL_SECTION__H__
#define __CRITICAL_SECTION__H__


#include <windows.h>

#define TRACK_OWNER 

class CriticalSection {
public:
  CriticalSection() {
    InitializeCriticalSection(&crit_);
    // Windows docs say 0 is not a valid thread id
    thread_ = 0;
  }
  ~CriticalSection() {
    DeleteCriticalSection(&crit_);
  }
  void Enter() {
    EnterCriticalSection(&crit_);
    TRACK_OWNER(thread_ = GetCurrentThreadId());
  }
  void Leave() {
    TRACK_OWNER(thread_ = GetCurrentThreadId());
    LeaveCriticalSection(&crit_);
  }

private:
  CRITICAL_SECTION crit_;
  DWORD thread_;
};

// CritScope, for serializing exection through a scope

class CritScope {
public:
  CritScope(CriticalSection *pcrit) {
    pcrit_ = pcrit;
    pcrit_->Enter();
  }
  ~CritScope() {
    pcrit_->Leave();
  }
private:
  CriticalSection *pcrit_;
};

#endif // __CRITICAL_SECTION__H__
