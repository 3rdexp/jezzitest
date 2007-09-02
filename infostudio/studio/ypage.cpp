

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
    tvs.item.mask = TVIF_TEXT;

    HTREEITEM ret = tree.InsertItem(&tvs);
    // ret = InsertItem(i->name.c_str(), parent, after);
    ASSERT(ret);
    HTREEITEM inner_after = TVI_FIRST;
    for (Industry::list_type::const_iterator i=ind.children.begin(); 
        i != ind.children.end(); ++i)
    {
        inner_after = RescurInsert(tree, ret, inner_after, *i);
    }

    if (ind.children.empty())
    {
        HTREEITEM hti = tree.InsertItem(L"site a", ret, inner_after);
        tree.SetItemData(hti, 1);
    }

    return ret;
}

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