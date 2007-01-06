// non client area draw window

#pragma once

#include "frame.h"

namespace Skin {

class SkinDialog : public SkinControlImpl<SkinDialog, SkinFrameImpl<SkinDialog>, ClassPolicy>
{
public:
    static LPCTSTR GetWndClassName()
    {
        return _T("#32770");
    }
    enum { class_id = DIALOG };

    typedef SkinDialog this_type;
    typedef SkinFrameImpl<SkinDialog> framebase_type;

	void OnFirstMessage()
	{
		return ;
	}


    BEGIN_MSG_MAP(this_type)
        //if ((uMsg < WM_MOUSEFIRST || uMsg > WM_MOUSELAST)
          //  && uMsg != WM_NCHITTEST && uMsg != WM_SETCURSOR)
          //  ATLTRACE("%04x Dialog\n", uMsg);
//        MESSAGE_HANDLER(WM_PAINT, OnPaint)
//        MESSAGE_HANDLER(WM_KILLFOCUS, OnFocus)
//        MESSAGE_HANDLER(WM_SETFOCUS, OnFocus)
//        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
//        MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDblClk)
//        MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
//        MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
//        MESSAGE_HANDLER(WM_ENABLE, OnEnable)
//        MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
//        MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
//        MESSAGE_HANDLER(WM_SETTEXT, OnSetText)
//        MESSAGE_HANDLER(BM_SETCHECK, OnStateChange)
//        MESSAGE_HANDLER(BM_SETSTATE, OnStateChange)
//        MSG_WM_CREATE(OnCreate)
        CHAIN_MSG_MAP(framebase_type)
        // REFLECT_NOTIFICATIONS()
    END_MSG_MAP()
};

} // namespace Skin
