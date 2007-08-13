// Verify Image Dialog
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__VERIFYIMG_WND_H__)
#define __VERIFYIMG_WND_H__

#include <atlimage.h>

#define WM_ADDIMAGE     WM_APP + 1

class VerifyImgDlg : public CDialogImpl<VerifyImgDlg>
    , public CMessageFilter
{
public:
	enum { IDD = IDD_VERIYIMAGE };

    virtual BOOL PreTranslateMessage(MSG* pMsg)
    {
        return ::IsDialogMessage(m_hWnd, pMsg);
    }

	BEGIN_MSG_MAP(VerifyImgDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
        MESSAGE_HANDLER(WM_ADDIMAGE, OnAddImage)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
        CImage img;
		CenterWindow(GetParent());
		return TRUE;
	}

    LRESULT OnAddImage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        return 0;
    }

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
        // image path,
        // SiteTaskTask *
		CloseDialog(wID);
		return 0;
	}

    void CloseDialog(int nVal)
    {
        DestroyWindow();
        ::PostQuitMessage(nVal);
    }
};

#endif // !defined(__VERIFYIMG_WND_H__)
