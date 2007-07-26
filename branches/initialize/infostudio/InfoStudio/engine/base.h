
#ifndef __BASE_H__
#define __BASE_H__

#ifdef ENGINE_NAMESPACE

#define BEGIN_ENGINE_NAMESPACE  namespace ENGINE_NAMESPACE {
#define END_ENGINE_NAMESPACE    }
#define ENGINE_ ENGINE_NAMESPACE

#else // ENGINE_NAMESPACE

#define BEGIN_ENGINE_NAMESPACE
#define END_ENGINE_NAMESPACE
#define ENGINE_ 

#endif // ENGINE_NAMESPACE


// TODO(Ken): split to time.h

BEGIN_ENGINE_NAMESPACE

typedef int int32;
typedef unsigned int uint32;

uint32 Time();
uint32 StartTime();
int32 TimeDiff(uint32 later, uint32 earlier);

END_ENGINE_NAMESPACE

#endif // __BASE_H__

