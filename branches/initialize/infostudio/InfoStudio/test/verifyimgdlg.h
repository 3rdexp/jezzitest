// Verify Image Dialog
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(__VERIFYIMG_WND_H__)
#define __VERIFYIMG_WND_H__

#include <vector>
#include <string>
#include "picwnd.h"

#define WM_ADDIMAGE     WM_APP + 1

class SiteTask;

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
        COMMAND_RANGE_HANDLER(IDC_INPUT1, IDC_INPUT1 + 10, OnInput)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

    struct VerifyItem {
        VerifyItem(const CPicture & pic, SiteTask * task) 
            : img(pic), task(task)
        {
        }

        CPicture img;
        SiteTask * task;
        std::wstring code;
    };

    std::vector<VerifyItem> items_;

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
        picwnd_.SubclassWindow(GetDlgItem(IDC_IMAGE1));

        CPicture pic;
        pic.Load(_T("C:\\w\\infostudio\\InfoStudio\\test\\msdn.bmp"));
        picwnd_.SetPicture(pic);
		return TRUE;
	}

    LRESULT OnAddImage(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
    {
        LPCTSTR fn = (LPCTSTR)wParam;
        SiteTask * task = (SiteTask*)lParam;
        CPicture img;
        if (img.Load(fn))
        {
            VerifyItem vi(img, task);
            items_.push_back(vi);
        }
        return 0;
    }

    LRESULT OnInput(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    

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

    PictureWnd picwnd_;
};

#endif // !defined(__VERIFYIMG_WND_H__)
