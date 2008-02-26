
#ifndef __YELLOW_PAGE_H__
#define __YELLOW_PAGE_H__

#include "childv.h"
#include "sitetree.h"
#include "data/studiodata.h"

class SubYellowPage : public SiteTreeImpl<SubYellowPage>
    , public ChildViewBase
{
public:
    DECLARE_WND_SUPERCLASS(_T("SiteTree"), CTreeViewCtrl::GetWndClassName())
    // DECLARE_WND_CLASS_EX(_T("SiteTree"), 0, -1)

    SubYellowPage(StudioData * bd) : bd_(bd) {}

    BEGIN_MSG_MAP(SubYellowPage)
        CHAIN_MSG_MAP(SiteTreeImpl<SubYellowPage>)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_CREATE, OnDestroy)
        REFLECTED_NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnTreeSelChanged)
        REFLECTED_NOTIFY_CODE_HANDLER(NM_CLICK, OnTreeClick)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
    LRESULT OnTreeSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnTreeClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

    // ChildViewBase
    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }

    // return Site::sid
    void GetSelectedSite(std::vector<Site*> &, HTREEITEM hti = 0);

    // template function for SiteTreeImpl
    void DrawItem(HDC hdc, RECT * lprc, TreeData* td);

    StudioData * bd_;
};


#endif // __YELLOW_PAGE_H__
