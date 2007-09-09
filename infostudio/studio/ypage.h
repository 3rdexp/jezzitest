
#ifndef __YELLOW_PAGE_H__
#define __YELLOW_PAGE_H__

#include "childv.h"
#include "sitetree.h"
#include "data/basedata.h"

// #define ONE_TREE
#ifdef ONE_TREE

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

#else

#include <atlsplit.h>

class SubYellowPage : public CSplitterWindow
    , public ChildViewBase
{
public:
    SubYellowPage(BaseData * bd) : bd_(bd)
    {
    }

    BEGIN_MSG_MAP(SubYellowPage)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        NOTIFY_CODE_HANDLER(TVN_SELCHANGED, OnTreeSelChanged)

        CHAIN_MSG_MAP(CSplitterWindow)
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

    // 
    void SelectIndustry(int id);
private:   

    // 
    LRESULT OnTreeSelChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

    CTreeViewCtrl tv_;
    CListViewCtrl lv_;

    BaseData * bd_;
    std::vector<const SiteInfo*> curcol_;
};

#endif

#endif // __YELLOW_PAGE_H__
