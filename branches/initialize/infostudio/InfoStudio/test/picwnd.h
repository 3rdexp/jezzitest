

#pragma once

#include <atlmisc.h>

#include "picture.h"


class PictureWnd : public CWindowImpl<PictureWnd, CWindow>
{
public:
    void SetPicture(CPicture & pic)
    {
        pic_ = pic;
        Invalidate();
    }

    BEGIN_MSG_MAP(PictureWnd)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
    END_MSG_MAP()

    LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CPaintDC dc(m_hWnd);

        if (pic_)
        {
            CRect rcClient;
            GetClientRect(&rcClient);
            CRect rcImage(CPoint(0,0),pic_.GetImageSize());
            CRect rc;
            rc.IntersectRect(&rcImage, &rcClient);
            pic_.Render(dc, rc);
        }

        return 0;
    }

    LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        RECT rect;
        GetClientRect(&rect);

        CDCHandle dc = (HDC)wParam;
        dc.FillRect(&rect, COLOR_WINDOW);
        return 0;
    }

    CPicture pic_;
};