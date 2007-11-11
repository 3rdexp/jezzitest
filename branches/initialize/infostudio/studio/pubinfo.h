#ifndef __PUBINFO_DIALOG_H__
#define __PUBINFO_DIALOG_H__

#include <atlframe.h>
#include "engine/coreinfo.h"

inline std::wstring GetText(CWindow & wnd)
{
    int n = wnd.GetWindowTextLength();
    wchar_t * w = new wchar_t[n + 1];
    wnd.GetWindowText(w, n + 1);

    std::wstring s(w, n);
    delete [] w;
    return s;
}

template<int theID = IDD_PUBLISH_INFO>
class PublishInfoDialog : public CDialogImpl<PublishInfoDialog<theID> >
    , public CDialogResize<PublishInfoDialog<theID> >
{
public:
    enum { IDD = theID};

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        return CWindow::IsDialogMessage(pMsg);
    }

    typedef CDialogResize<PublishInfoDialog<theID> > ResizeBase;

    BEGIN_MSG_MAP(PublishInfoDialog)
        CHAIN_MSG_MAP(ResizeBase)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_ID_HANDLER(IDOK, OnOK)
        COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(PublishInfoDialog)
    END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        CenterWindow(GetParent());
        DlgResize_Init(false);

        if (pub_.pubid) Set();
        return 0;
    }

    LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        EndDialog(wID);
        return 0;
    }

    LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        pub_.content = GetText(GetDlgItem(IDC_CONTENT));
        pub_.title = GetText(GetDlgItem(IDC_TITLE));
        pub_.keywords = GetText(GetDlgItem(IDC_KEYWORDS));
        // pub_.title = GetText(GetDlgItem(IDC_TITLE));

        EndDialog(wID);
        return 0;
    }

    void Set()
    {
        GetDlgItem(IDC_CONTENT).SetWindowText(pub_.content.c_str());
    }

    Publish pub_;
};




#endif // __PUBINFO_DIALOG_H__
