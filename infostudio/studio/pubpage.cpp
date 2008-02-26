
#include "stdafx.h"

#include <vector>

#include <atlctrls.h>
#include <atlframe.h>
#include <atlctrlw.h>

#include "base/sigslot.h"
#include "engine/coreinfo.h"
#include "data/studiodata.h"

#include "resource.h"
#include "mainfrm.h"

#include "pubtree.h"
#include "publist.h"
#include "pubinfo.h"

#include "pubpage.h"

LRESULT SubPublishPage::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    CRect rcc;
    GetClientRect(&rcc);

    // Left
    tree_.Create(m_hWnd, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN 
        | WS_CLIPSIBLINGS | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS 
        | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS | TVS_TRACKSELECT, WS_EX_CLIENTEDGE);
    ASSERT(tree_);

    CRect rc(rcc.left + 240, rcc.top, rcc.right, rcc.bottom);
    rsplit_.Create(m_hWnd, &rc);
    ASSERT(rsplit_);

    SetSplitterPanes(tree_, rsplit_);
    SetSplitterRect(rcc, false);
    SetSplitterPos(240);

    // Right
    list_.Create(rsplit_, rcDefault, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN 
        | WS_CLIPSIBLINGS | LVS_REPORT, WS_EX_CLIENTEDGE);
    ASSERT(list_);

    info_.Create(rsplit_);
    ASSERT(info_);

    rsplit_.SetSplitterPanes(list_, info_);
    //        rsplit_.SetSplitterRect(rc, false);
    rsplit_.SetSplitterPos(-1);

    // TODO: Insert data
    list_.InsertColumn(0, L"heeeeeee");
    list_.InsertColumn(1, L"geeeeeee");

    list_.InsertItem(0, L"first");
    list_.InsertItem(1, L"second");

    HTREEITEM h = tree_.InsertItem(L"Root", TVI_ROOT, TVI_FIRST);
    int i=10;
    while (--i)
        tree_.InsertItem(L"child", h, TVI_FIRST);

    tree_.Expand(h, TVE_EXPAND);

    // insert publish to tree, item not expand
    // publish item
    //  |--- execute once
    //  |--- execute once
    //   --- execute once

    // list contains all sites execute
    // 

//     std::vector<Publish> & pubs = data_->GetPublish();
//     for (std::vector<Publish>::const_iterator i=pubs.begin();
//         i!=pubs.end(); ++i)
//     {
//         //
//     }

    return 0;
}


LRESULT SubPublishPage::OnPublishNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& bHandled)
{
    bHandled = FALSE;
    PublishInfoDialog<IDD_PUBLISH_INFO1> dlg;
    if (IDOK == dlg.DoModal())
    {
        Publish & pub = dlg.pub_;
        // TODO: Save 
    }
    return 0;
}

LRESULT SubPublishPage::OnTreeContextMenu(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 0;
}
