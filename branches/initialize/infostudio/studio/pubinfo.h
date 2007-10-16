
#ifndef __PUBINFO_H__
#define __PUBINFO_H__

class PublishInfo : public CDialogImpl<PublishInfo>
    , public CDialogResize<PublishInfo>
{
public:
    enum { IDD = IDD_PUBLISH_INFO};

    BOOL PreTranslateMessage(MSG* pMsg)
    {
        return CWindow::IsDialogMessage(pMsg);
    }

    typedef CDialogResize<PublishInfo> ResizeBase;

    BEGIN_MSG_MAP(PublishInfo)
        CHAIN_MSG_MAP(ResizeBase)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_RANGE_HANDLER(IDC_BUTTON1, IDC_BUTTON5, OnButton)
    END_MSG_MAP()

    BEGIN_DLGRESIZE_MAP(PublishInfo)
    END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        DlgResize_Init(false);
        return 0;
    }

    LRESULT OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
    {
        return 0;
    }
};


#endif // __PUBINFO_H__
