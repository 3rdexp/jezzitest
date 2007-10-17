
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
        CRect rcc;
        GetClientRect(&rcc);

        // Left
        tree_.Create(m_hWnd, rcDefault);
        ASSERT(tree_);

        CRect rc(rcc.left + 240, rcc.top, rcc.right, rcc.bottom);
        rsplit_.Create(m_hWnd, &rc);
        ASSERT(rsplit_);

        SetSplitterPanes(tree_, rsplit_);
        SetSplitterRect(rcc, false);
        SetSplitterPos(240);

        // Right
        list_.Create(rsplit_, rcDefault);
        ASSERT(list_);

        info_.Create(rsplit_);
        ASSERT(info_);

        rsplit_.SetSplitterPanes(list_, info_);
//        rsplit_.SetSplitterRect(rc, false);
        rsplit_.SetSplitterPos(-1);

        return 0;
    }

    PublishTree tree_;
    PublishResultList list_;
    PublishInfoDialog info_;

    CHorSplitterWindow rsplit_;
};

#endif // __PUBPAGE_H__
