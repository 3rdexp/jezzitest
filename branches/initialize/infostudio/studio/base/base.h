
#ifndef __BASE_H__
#define __BASE_H__


//////////////////////////////////////////////////////////////////////////
//
#ifdef ENGINE_NAMESPACE

#define BEGIN_ENGINE_NAMESPACE  namespace ENGINE_NAMESPACE {
#define END_ENGINE_NAMESPACE    }
#define ENGINE_ ENGINE_NAMESPACE

#else // ENGINE_NAMESPACE

#define BEGIN_ENGINE_NAMESPACE
#define END_ENGINE_NAMESPACE
#define ENGINE_ 

#endif // ENGINE_NAMESPACE



//////////////////////////////////////////////////////////////////////////
//

#ifndef ARRAYSIZE
    #define ARRAYSIZE(a)		(sizeof(a)/sizeof((a)[0]))
#endif

#ifndef ASSERT
    #ifndef NDEBUG
        #define ASSERT(x) if (!(x)) __asm int 3;
    #else
        #define ASSERT(x)
    #endif
#endif


//////////////////////////////////////////////////////////////////////////
//
// TODO(Ken): split to time.h

BEGIN_ENGINE_NAMESPACE

typedef int int32;
typedef unsigned int uint32;

typedef __int64 int64;
typedef unsigned __int64 uint64;

uint32 Time();
uint32 StartTime();
int32 TimeDiff(uint32 later, uint32 earlier);

END_ENGINE_NAMESPACE

#endif // __BASE_H__

