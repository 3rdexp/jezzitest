#ifndef __CHILDVIEW_BASE_H__
#define __CHILDVIEW_BASE_H__

class ChildViewBase
{
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
    virtual HWND GetHWND() = 0;
};


#endif // __CHILDVIEW_BASE_H__

