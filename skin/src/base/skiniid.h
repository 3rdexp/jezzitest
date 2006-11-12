#include <guiddef.h>

#define DEFINE_SKINGUID(name, l, w1, w2) DEFINE_GUID(name, l, w1, w2, 0xB0,0,0,0,0,0,0,0xEE)

DEFINE_SKINGUID(IID_ISkinConfig,	0x00CAE001, 0, 0);
DEFINE_SKINGUID(IID_ISkinDraw,		0x00CAE002, 1, 0);
DEFINE_SKINGUID(IID_ISkinScheme,	0x00CAE003, 2, 0);

DEFINE_SKINGUID(IID_ISkinMgr,		0x00CAE004, 3, 0);
DEFINE_SKINGUID(IID_IDCCache,		0x00CAE004, 4, 0);