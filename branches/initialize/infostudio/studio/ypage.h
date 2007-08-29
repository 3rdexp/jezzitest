
#ifndef __YELLOW_PAGE_H__
#define __YELLOW_PAGE_H__

#include "childv.h"

class SubYellowPage : public CSplitterWindow
    , public ChildViewBase
{
public:
    BEGIN_MSG_MAP(SubYellowPage)
        CHAIN_MSG_MAP(CSplitterWindow)
    END_MSG_MAP()

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }
};


#endif // __YELLOW_PAGE_H__
