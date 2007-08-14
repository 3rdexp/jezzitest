
#include "stdafx.h"

#include "resource.h"

#include "../infoengine.h"
#include "verifyimgdlg.h"


LRESULT VerifyImgDlg::OnInput(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    int i = wID - IDC_INPUT1;
    ASSERT(i < items_.size());
    VerifyItem & vi = items_[i];
    CWindow wnd(GetDlgItem(IDC_CODE1 + i));
    CString str;
    wnd.GetWindowText(str);
    vi.code = (LPCWSTR)str;
    vi.task->EnterVerifyCode(vi.code);
    return 0;
}