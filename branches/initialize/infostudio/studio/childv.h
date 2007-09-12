#ifndef __CHILDVIEW_BASE_H__
#define __CHILDVIEW_BASE_H__

class ChildViewBase
{
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
    virtual HWND GetHWND() = 0;
};

template<class T>
class ChildViewT : public T
    , public ChildViewBase
{
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg) 
    {
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }
};

#endif // __CHILDVIEW_BASE_H__

