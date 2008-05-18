

#include "stdafx.h"

#include "ypage.h"
#include "data/studiodata.h"

struct IndustryData : public TreeData
{
    IndustryData(int cid) : cid_(cid), site_count_(0) {}
    int cid_;
    int site_count_;
};

struct SiteInfoData : public TreeData
{
    SiteInfoData(Site * site) : site_(site) {}
    Site * site_;

    virtual bool mcols() const { return true; }
};


//////////////////////////////////////////////////////////////////////////
//
HTREEITEM RescurInsert(CTreeViewCtrl & tree, StudioData * data, HTREEITEM parent, HTREEITEM after, const Industry & ind)
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
        inner_after = RescurInsert(tree, data, ret, inner_after, i->second);
    }

    std::vector<Site*> sites = data->FindSite(ind.id);
    
    for (std::vector<Site*>::const_iterator j=sites.begin();
        j != sites.end(); ++j)
    {
        ZeroMemory(&tvs, sizeof(tvs));
        tvs.hParent = ret;
        tvs.hInsertAfter = inner_after;
        tvs.item.pszText = const_cast<wchar_t*>((*j)->name.c_str());
        tvs.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_STATE;
        tvs.item.state = INDEXTOSTATEIMAGEMASK(2);
        tvs.item.stateMask = TVIS_STATEIMAGEMASK;
        tvs.item.lParam = (LPARAM)new SiteInfoData(*j);

        inner_after = tree.InsertItem(&tvs);
        ASSERT(inner_after);
    }

    return ret;
}

LRESULT SubYellowPage::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/
                                , BOOL& bHandled)
{
    header_.Create(m_hWnd, 0, 0, WS_CHILD | WS_BORDER | HDS_BUTTONS | HDS_HORZ);

    CRect rcc; GetClientRect(&rcc);
    HDLAYOUT hdl; 
    WINDOWPOS wp;
    hdl.prc = &rcc; 
    hdl.pwpos = &wp;

    header_.Layout(&hdl);
    header_.SetWindowPos(wp.hwndInsertAfter, wp.x, wp.y, wp.cx, wp.cy, wp.flags | SWP_SHOWWINDOW); 

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
    {
        // TODO:
    }
    return 0;
}

void SubYellowPage::DrawItem(HDC hdc, RECT * lprc, TreeData* td)
{
    CDCHandle dc(hdc);
    CRect rc(*lprc);

    SiteInfoData * sd = dynamic_cast<SiteInfoData*>(td);

    rc.left = 200;
    dc.DrawText(sd->site_->homepage.c_str(), sd->site_->homepage.size()
        , rc, DT_LEFT | DT_VCENTER);

    rc.left += 200;
    dc.DrawText(L"TODO", -1, rc, DT_LEFT | DT_VCENTER);
}

void SubYellowPage::GetSelectedSite(std::vector<Site*> & vec, HTREEITEM hti)
{
    // enum all tree items ?
    if (!hti)
        hti = GetRootItem();

    if (hti && GetCheckState(hti))
    {
        TreeData * pd = (TreeData *)GetItemData(hti);
        if (pd->mcols())
            vec.push_back(dynamic_cast<SiteInfoData*>(pd)->site_);
    }

    HTREEITEM htc = GetNextItem(hti, TVGN_CHILD);
    while (htc)
    {
        GetSelectedSite(vec, htc);

        htc = GetNextItem(htc, TVGN_NEXT);
    }
}



