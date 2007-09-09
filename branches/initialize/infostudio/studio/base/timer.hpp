#ifndef __WIN_PROGRESS_H__
#define __WIN_PROGRESS_H__

// 实现windows平台上更精确的时钟

#include <windows.h>
#include <sstream>
#include <string>

class ptimer
{
public:
    ptimer()
    {
        QueryPerformanceCounter(&_start);
    }

    void restart()
    {
        QueryPerformanceCounter(&_start);
    }

    double elapsed() const
    {
        LARGE_INTEGER end, freq;
        QueryPerformanceCounter(&end);

        QueryPerformanceFrequency(&freq);

        return (double)(end.QuadPart - _start.QuadPart) * 1000 / freq.LowPart;
    }

private:
    LARGE_INTEGER _start;
};

void Dump(ptimer & pt, const std::string & name)
{
    std::ostringstream oss;
    oss << name << " : " << pt.elapsed() << " ms";
    OutputDebugStringA(oss.str().c_str());

    pt.restart();
}

#endif // __WIN_PROGRESS_H__
