#ifndef __SITETREE_H__
#define __SITETREE_H__

#include <atlctrls.h>

struct TreeData
{
    virtual ~TreeData() {};
    virtual bool mcols() const { return false; }
};


typedef CWinTraitsOR<WS_CLIPCHILDREN | WS_CLIPSIBLINGS
    | TVS_LINESATROOT | TVS_HASLINES | TVS_HASBUTTONS | TVS_DISABLEDRAGDROP 
    | TVS_SHOWSELALWAYS | TVS_TRACKSELECT>
    SiteTreeTraits;

template<class T>
class SiteTreeImpl : public CWindowImpl<T, CTreeViewCtrl, SiteTreeTraits>
    , public CCustomDraw<SiteTreeImpl>
{
public:
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
        {
            TreeData * pd = (TreeData*)tvcd->nmcd.lItemlParam;
            if (pd->mcols())
                return CDRF_NOTIFYPOSTPAINT;
        }
        return CDRF_DODEFAULT;
    }

    DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
    {
        NMTVCUSTOMDRAW * tvcd = (NMTVCUSTOMDRAW *)lpNMCustomDraw;
        if (tvcd->nmcd.lItemlParam)
        {
            TreeData * pd = (TreeData*)tvcd->nmcd.lItemlParam;
            if (pd->mcols())
                return CDRF_NOTIFYPOSTPAINT;
        }//    return CDRF_NOTIFYPOSTPAINT;
        return CDRF_DODEFAULT;
    }

    DWORD OnItemPostPaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
    {
        NMTVCUSTOMDRAW * tvcd = (NMTVCUSTOMDRAW *)lpNMCustomDraw;
        T * pT = static_cast<T*>(this);
        pT->DrawItem(tvcd->nmcd.hdc, &tvcd->nmcd.rc, (TreeData*)tvcd->nmcd.lItemlParam);
        return CDRF_DODEFAULT;
    }

    void DrawItem(HDC hdc, RECT * lprc, TreeData* td)
    {
        CDCHandle dc(hdc);
        CRect rc(*lprc);

        rc.left = 200;
        dc.DrawText(L"Sub item 1", -1, rc, DT_LEFT | DT_VCENTER);
        rc.left += 200;
        dc.DrawText(L"Sub item 2", -1, rc, DT_LEFT | DT_VCENTER);
    }
};


#endif // __SITETREE_H__
