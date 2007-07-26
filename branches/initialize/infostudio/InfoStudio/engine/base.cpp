
#include <windows.h>
#include "base.h"

BEGIN_ENGINE_NAMESPACE


uint32 Time() {
    return GetTickCount();
}

uint32 StartTime() {
    // Close to program execution time
    static const uint32 g_start = Time();
    return g_start;
}

bool TimeIsBetween(uint32 later, uint32 middle, uint32 earlier) {
    if (earlier <= later) {
        return ((earlier <= middle) && (middle <= later));
    } else {
        return !((later < middle) && (middle < earlier));
    }
}

int32 TimeDiff(uint32 later, uint32 earlier) {
    uint32 LAST = 0xFFFFFFFF;
    uint32 HALF = 0x80000000;
    if (TimeIsBetween(earlier + HALF, later, earlier)) {
        if (earlier <= later) {
            return static_cast<long>(later - earlier);
        } else {
            return static_cast<long>(later + (LAST - earlier) + 1);
        }
    } else {
        if (later <= earlier) {
            return -static_cast<long>(earlier - later);
        } else {
            return -static_cast<long>(earlier + (LAST - later) + 1);
        }
    }
}



END_ENGINE_NAMESPACE


