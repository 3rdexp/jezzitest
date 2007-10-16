
#ifndef __PUBPAGE_H__
#define __PUBPAGE_H__

#include <atlsplit.h>

#include "childv.h"

#include "pubtree.h"
#include "publist.h"
#include "pubinfo.h"

// 
class SubPublishPage : public CSplitterWindowImpl<SubPublishPage, true>
    , public ChildViewBase
{
public:
    DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, COLOR_WINDOW)

    virtual BOOL PreTranslateMessage(MSG* pMsg) 
    {
        // TODO:
        return FALSE;
    }
    virtual HWND GetHWND()
    {
        return m_hWnd;
    }

    SubPublishPage() {}

    typedef CSplitterWindowImpl<SubPublishPage, true> _baseClass;

    BEGIN_MSG_MAP(SubPublishPage)
        CHAIN_MSG_MAP(_baseClass)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
    END_MSG_MAP()

private:
    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        // create
        tree_.Create(m_hWnd, rcDefault);
        list_.Create(m_hWnd, rcDefault);

        SetSplitterPanes(tree_, list_);
        return 0;
    }

    PublishTree tree_;
    PublishList list_;
//    PublishInfo info_;
};

#endif // __PUBPAGE_H__
