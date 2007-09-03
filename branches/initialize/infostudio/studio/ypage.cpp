

#include "stdafx.h"

#include "ypage.h"
#include "data/basedata.h"

HTREEITEM RescurInsert(CTreeViewCtrl & tree, HTREEITEM parent, HTREEITEM after, const Industry & ind)
{
    TVINSERTSTRUCT tvs;
    ZeroMemory(&tvs, sizeof(tvs));

    tvs.hParent = parent;
    tvs.hInsertAfter = after;
    tvs.item.pszText = const_cast<wchar_t*>(ind.name.c_str());
    tvs.item.cchTextMax = ind.name.size();
    tvs.item.mask = TVIF_TEXT | TVIF_PARAM;
    tvs.item.lParam = ind.id;

    HTREEITEM ret = tree.InsertItem(&tvs);
    // ret = InsertItem(i->name.c_str(), parent, after);
    ASSERT(ret);
    HTREEITEM inner_after = TVI_FIRST;
    for (Industry::list_type::const_iterator i=ind.children.begin(); 
        i != ind.children.end(); ++i)
    {
        inner_after = RescurInsert(tree, ret, inner_after, *i);
    }

#ifdef ONE_TREE // test code
    if (ind.children.empty())
    {
        HTREEITEM hti = tree.InsertItem(L"site a", ret, inner_after);
        tree.SetItemData(hti, 1);
    }
#endif

    return ret;
}


#ifdef ONE_TREE

LRESULT SubYellowPage::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/
                                , BOOL& bHandled)
{
    // bHandled = FALSE;
    if (bd_) {
        HTREEITEM parent = TVI_ROOT, after = TVI_FIRST;

        Industry & ind = bd_->GetIndustry();
        for (Industry::list_type::const_iterator i=ind.children.begin(); 
            i != ind.children.end(); ++i)
        {
            after = RescurInsert(*this, parent, after, *i);
        }
    }
    return 0;
}

#else


LRESULT SubYellowPage::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/
                                , BOOL& bHandled)
{
    // left tree
    tv_.Create(m_hWnd, rcDefault, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD
        | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP 
        | TVS_SHOWSELALWAYS);

    // right list
    lv_.Create(m_hWnd, rcDefault, 0, WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD
        | LVS_REPORT);

    lv_.SetExtendedListViewStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

    m_cxyMin = 195;
    SetSplitterPanes(tv_, lv_);

    SetSplitterPos(195);

    if (bd_) {
        HTREEITEM parent = TVI_ROOT, after = TVI_FIRST;

        Industry & ind = bd_->GetIndustry();
        for (Industry::list_type::const_iterator i=ind.children.begin(); 
            i != ind.children.end(); ++i)
        {
            after = RescurInsert(tv_, parent, after, *i);
        }
    }
    return 0;
}

LRESULT SubYellowPage::OnTreeSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    NMTREEVIEW * ntv = (NMTREEVIEW *)pnmh;
    int id = ntv->itemNew.lParam;
    if (bd_)
    {
        // bd_->GetIndustry().find(id);
    }
    return 0;
}

#endif


