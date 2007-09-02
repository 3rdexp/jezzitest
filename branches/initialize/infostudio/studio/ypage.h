
#ifndef __YELLOW_PAGE_H__
#define __YELLOW_PAGE_H__

#include "childv.h"
#include "sitetree.h"
#include "data/basedata.h"

class SubYellowPage : public SiteTreeImpl
    , public ChildViewBase
{
public:
    SubYellowPage(BaseData * bd) : bd_(bd)
    {
    }

    BEGIN_MSG_MAP(SubYellowPage)
        CHAIN_MSG_MAP(SiteTreeImpl)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    // ChildViewBase
    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }

    BaseData * bd_;
};


#endif // __YELLOW_PAGE_H__
