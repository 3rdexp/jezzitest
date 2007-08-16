
#include "stdafx.h"

#include "resource.h"

#include "../infoengine.h"
#include "verifyimgdlg.h"

VerifyImgDlg * gvidlg = 0;

LRESULT VerifyImgDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    gvidlg = this;
    CenterWindow(GetParent());
    picwnd_.SubclassWindow(GetDlgItem(IDC_IMAGE1));

    CPicture pic;
    pic.Load(_T("C:\\w\\infostudio\\InfoStudio\\test\\msdn.bmp"));
    picwnd_.SetPicture(pic);
    return TRUE;
}

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

void VerifyImgDlg::CloseDialog(int nVal)
{
    gvidlg = 0;
    DestroyWindow();
    ::PostQuitMessage(nVal);
}