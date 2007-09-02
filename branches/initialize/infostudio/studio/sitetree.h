#ifndef __SITETREE_H__
#define __SITETREE_H__

#include <atlctrls.h>

typedef CWinTraitsOR<WS_CLIPCHILDREN | WS_CLIPSIBLINGS
    | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP 
    | TVS_SHOWSELALWAYS | TVS_TRACKSELECT>
    SiteTreeTraits;

class SiteTreeImpl : public CWindowImpl<SiteTreeImpl, CTreeViewCtrl, SiteTreeTraits>
    , public CCustomDraw<SiteTreeImpl>
{
public:
    DECLARE_WND_SUPERCLASS(0, CTreeViewCtrl::GetWndClassName())
    BEGIN_MSG_MAP(SiteTreeImpl)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        CHAIN_MSG_MAP_ALT(CCustomDraw<SiteTreeImpl>, 1)
    END_MSG_MAP()

    LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
    {
        bHandled = FALSE;
        DefWindowProc();
        ModifyStyle(0, TVS_CHECKBOXES);
        return 0;
    }

    DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
    {
        return CDRF_NOTIFYITEMDRAW;
        NMTVCUSTOMDRAW * tvcd = (NMTVCUSTOMDRAW *)lpNMCustomDraw;
        if (tvcd->nmcd.lItemlParam)
            return CDRF_NOTIFYPOSTPAINT;
        return CDRF_DODEFAULT;
    }

    DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
    {
        NMTVCUSTOMDRAW * tvcd = (NMTVCUSTOMDRAW *)lpNMCustomDraw;
        if (tvcd->nmcd.lItemlParam)
            return CDRF_NOTIFYPOSTPAINT;
        return CDRF_DODEFAULT;
    }

    DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
    {
        NMTVCUSTOMDRAW * tvcd = (NMTVCUSTOMDRAW *)lpNMCustomDraw;
        CDCHandle dc(tvcd->nmcd.hdc);
        CRect rc = tvcd->nmcd.rc;
        rc.left = 200;
        dc.DrawText(L"Sub item 1", -1, rc, DT_LEFT | DT_VCENTER);
        rc.left += 200;
        dc.DrawText(L"Sub item 2", -1, rc, DT_LEFT | DT_VCENTER);
        return CDRF_DODEFAULT;
    }
};


#endif // __SITETREE_H__