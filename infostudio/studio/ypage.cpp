

#include "stdafx.h"

#include "ypage.h"
#include "data/basedata.h"

#ifdef ONE_TREE

struct IndustryData : public TreeData
{
    IndustryData(int cid) : cid_(cid) {}
    int cid_;
};

struct SiteData : public TreeData
{
    SiteData(const SiteInfo * site) : site_(site) {}
    const SiteInfo * site_;

    virtual bool mcols() const { return true; }
};


//////////////////////////////////////////////////////////////////////////
//
HTREEITEM RescurInsert(CTreeViewCtrl & tree, BaseData * bd, HTREEITEM parent, HTREEITEM after, const Industry & ind)
{
    TVINSERTSTRUCT tvs;
    ZeroMemory(&tvs, sizeof(tvs));

    tvs.hParent = parent;
    tvs.hInsertAfter = after;
    tvs.item.pszText = const_cast<wchar_t*>(ind.name.c_str());
    tvs.item.cchTextMax = ind.name.size();
    tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    tvs.item.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_EXPANDED;
    tvs.item.stateMask = TVIS_STATEIMAGEMASK;
    tvs.item.lParam = (LPARAM)new IndustryData(ind.id);

    HTREEITEM ret = tree.InsertItem(&tvs);
    ASSERT(ret);

//    tree.SetCheckState(ret, TRUE);

    HTREEITEM inner_after = TVI_FIRST;
    for (Industry::children_type::const_iterator i=ind.children.begin(); 
        i != ind.children.end(); ++i)
    {
        inner_after = RescurInsert(tree, bd, ret, inner_after, i->second);
    }

    std::vector<const SiteInfo*> sites = bd->FindSite(ind.id);
    
    for (std::vector<const SiteInfo*>::const_iterator j=sites.begin();
        j != sites.end(); ++j)
    {
        ZeroMemory(&tvs, sizeof(tvs));
        tvs.hParent = ret;
        tvs.hInsertAfter = inner_after;
        tvs.item.pszText = const_cast<wchar_t*>((*j)->name.c_str());
        tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
        tvs.item.state = INDEXTOSTATEIMAGEMASK(2);
        tvs.item.stateMask = TVIS_STATEIMAGEMASK;
        tvs.item.lParam = (LPARAM)new SiteData(*j);

        inner_after = tree.InsertItem(&tvs);
        ASSERT(inner_after);
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
        for (Industry::children_type::const_iterator i=ind.children.begin(); 
            i != ind.children.end(); ++i)
        {
            after = RescurInsert(*this, bd_, parent, after, i->second);
        }
    }
    return 0;
}

static void TreeItemClean(CTreeViewCtrl & tree, HTREEITEM htip)
{
    TreeData * ptd = (TreeData *)tree.GetItemData(htip);
    delete ptd;

    HTREEITEM hti = tree.GetNextItem(htip, TVGN_CHILD);
    while (hti)
    {
        TreeItemClean(tree, hti);

        hti = tree.GetNextItem(hti, TVGN_NEXT);
    }
}

LRESULT SubYellowPage::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    HTREEITEM root = GetNextItem(NULL, TVGN_ROOT);
    TreeItemClean(*this, root);
    return 0;
}

LRESULT SubYellowPage::OnTreeSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    NMTREEVIEW * ntv = (NMTREEVIEW *)pnmh;
    int id = ntv->itemNew.lParam;
    if (bd_) {
    }
    return 0;
}

LRESULT SubYellowPage::OnTreeClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    CPoint pt;
    GetCursorPos(&pt);
    ScreenToClient(&pt);

    UINT flags = 0;
    HTREEITEM hti = HitTest(pt, &flags);
    if (flags & TVHT_ONITEMSTATEICON)
        __asm int 3;
    return 0;
}

void SubYellowPage::DrawItem(HDC hdc, RECT * lprc, TreeData* td)
{
    CDCHandle dc(hdc);
    CRect rc(*lprc);

    SiteData * sd = dynamic_cast<SiteData*>(td);

    rc.left = 200;
    dc.DrawText(sd->site_->homepage.c_str(), sd->site_->homepage.size()
        , rc, DT_LEFT | DT_VCENTER);

    rc.left += 200;
    dc.DrawText(L"TODO", -1, rc, DT_LEFT | DT_VCENTER);
}




#else


HTREEITEM RescurInsert(CTreeViewCtrl & tree, HTREEITEM parent, HTREEITEM after, const Industry & ind)
{
    TVINSERTSTRUCT tvs;
    ZeroMemory(&tvs, sizeof(tvs));

    tvs.hParent = parent;
    tvs.hInsertAfter = after;
    tvs.item.pszText = const_cast<wchar_t*>(ind.name.c_str());
    tvs.item.cchTextMax = ind.name.size();
    tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
    tvs.item.state = INDEXTOSTATEIMAGEMASK(2) | TVIS_EXPANDED;
    tvs.item.stateMask = TVIS_STATEIMAGEMASK;
    tvs.item.lParam = ind.id;

    HTREEITEM ret = tree.InsertItem(&tvs);
    ASSERT(ret);

    //    tree.SetCheckState(ret, TRUE);

    HTREEITEM inner_after = TVI_FIRST;
    for (Industry::children_type::const_iterator i=ind.children.begin(); 
        i != ind.children.end(); ++i)
    {
        inner_after = RescurInsert(tree, ret, inner_after, i->second);
    }

    return ret;
}


//////////////////////////////////////////////////////////////////////////
//
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

    lv_.InsertColumn(0, L"Ãû³Æ", LVCFMT_LEFT, 100);
    lv_.InsertColumn(1, L"ÍøÕ¾", LVCFMT_LEFT, 100);
    lv_.InsertColumn(3, L"×´Ì¬", LVCFMT_LEFT, 50);

    if (bd_) {
        HTREEITEM parent = TVI_ROOT, after = TVI_FIRST;

        Industry & ind = bd_->GetIndustry();
        for (Industry::children_type::const_iterator i=ind.children.begin(); 
            i != ind.children.end(); ++i)
        {
            after = RescurInsert(tv_, parent, after, i->second);
        }

        SelectIndustry(0);
    }
    return 0;
}

LRESULT SubYellowPage::OnTreeSelChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
    NMTREEVIEW * ntv = (NMTREEVIEW *)pnmh;
    int id = ntv->itemNew.lParam;
    if (bd_) {
        SelectIndustry(id);
    }
    return 0;
}

static void InsertSite(CListViewCtrl & lv, const SiteInfo* site)
{
    int index = lv.InsertItem(0, site->name.c_str());
    ASSERT(-1 != index);
    BOOL f = lv.SetItemText(index, 1, site->homepage.c_str());
    ASSERT(f);
}

void SubYellowPage::SelectIndustry(int id)
{
    lv_.SetRedraw(FALSE);
    lv_.DeleteAllItems();

    if (bd_) 
    {
        if (id == 0)
        {
            std::vector<const SiteInfo*> sites = bd_->AllSite();
            curcol_.swap(sites);
        }
        else
        {
            std::vector<const SiteInfo*> sites = bd_->FindSite(id);
            curcol_.swap(sites);
        }

        for(std::vector<const SiteInfo*>::const_iterator i = curcol_.begin();
            i != curcol_.end(); ++i)
        {
            InsertSite(lv_, *i);
        }
    }

    lv_.SetRedraw(TRUE);
}

#endif


