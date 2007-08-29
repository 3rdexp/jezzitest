// studioView.h : interface of the CStudioView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CMainFrame;

class CStudioView : public CDialogImpl<CStudioView>
    , public CDialogResize<CStudioView>
{
public:
	enum { IDD = IDD_STUDIO_FORM };

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CWindow::IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CStudioView)
        CHAIN_MSG_MAP(ResizeBase)
        MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        COMMAND_RANGE_HANDLER(IDC_BUTTON1, IDC_BUTTON5, OnButton)
	END_MSG_MAP()

    typedef CDialogResize<CStudioView> ResizeBase;

    BEGIN_DLGRESIZE_MAP(CStudioView)
    END_DLGRESIZE_MAP()

    LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
    {
        DlgResize_Init(false);
        return 0;
    }

    LRESULT OnButton(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);    

    CMainFrame* frame_;

    CStudioView(CMainFrame * frame) : frame_(frame) 
    {}

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};
