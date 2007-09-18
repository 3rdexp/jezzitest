// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "childv.h"
#include "proplist/PropertyItem.h"

class ChildViewBase;
class BaseData;
class MutableData;

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

    // UI stuff
    CCommandBarCtrl m_CmdBar;

	CStudioView m_studio;
    CSplitterWindow m_wndSplitter;

    ChildViewBase * current_;

    enum CV_TYPE { CV_USERINFO, CV_YELLOWPAGE };
    enum { MAX_CHILD = 2 };

    ChildViewBase * children_[MAX_CHILD];

    CMainFrame() : current_(0) 
        , m_studio(this)
        , bd_(0), md_(0)
    {
        std::fill_n(children_, size_t(MAX_CHILD), (ChildViewBase*)0);
    }

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
			return TRUE;

        if (current_)
	        return current_->PreTranslateMessage(pMsg);

        return FALSE;
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)

        NOTIFY_CODE_HANDLER(PIN_ITEMCHANGED, OnUserInfoItemChanged)
        REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

    ChildViewBase * CreateChildView(CV_TYPE);
    ChildViewBase * ActiveChildView(CV_TYPE);

    void InitData(BaseData * bd, MutableData * md)
    {
        bd_ = bd;
        md_ = md;
    }

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);

    // sub UserInfo
    LRESULT OnUserInfoItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// TODO: add code to initialize document
		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// CAboutDlg dlg;
		// dlg.DoModal();
		return 0;
	}

    void InitViews();

    //////////////////////////////////////////////////////////////////////////
    // database stuff
    BaseData * bd_;
    MutableData * md_;

#if 0
    LRESULT ReflectNotifications(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
    {
        HWND hWndChild = NULL;

        switch(uMsg)
        {
        case WM_COMMAND:
            if(lParam != NULL)	// not from a menu
                hWndChild = (HWND)lParam;
            break;
        case WM_NOTIFY:
            hWndChild = ((LPNMHDR)lParam)->hwndFrom;
            if (current_ && hWndChild == current_->GetHWND())
            {
                // ATLTRACE("= %d\n", ((LPNMHDR)lParam)->code);
            }
            break;
        case WM_PARENTNOTIFY:
            switch(LOWORD(wParam))
            {
            case WM_CREATE:
            case WM_DESTROY:
                hWndChild = (HWND)lParam;
                break;
            default:
                hWndChild = GetDlgItem(HIWORD(wParam));
                break;
            }
            break;
        case WM_DRAWITEM:
            if(wParam)	// not from a menu
                hWndChild = ((LPDRAWITEMSTRUCT)lParam)->hwndItem;
            break;
        case WM_MEASUREITEM:
            if(wParam)	// not from a menu
                hWndChild = GetDlgItem(((LPMEASUREITEMSTRUCT)lParam)->CtlID);
            break;
        case WM_COMPAREITEM:
            if(wParam)	// not from a menu
                hWndChild = GetDlgItem(((LPCOMPAREITEMSTRUCT)lParam)->CtlID);
            break;
        case WM_DELETEITEM:
            if(wParam)	// not from a menu
                hWndChild = GetDlgItem(((LPDELETEITEMSTRUCT)lParam)->CtlID);
            break;
        case WM_VKEYTOITEM:
        case WM_CHARTOITEM:
        case WM_HSCROLL:
        case WM_VSCROLL:
            hWndChild = (HWND)lParam;
            break;
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORMSGBOX:
        case WM_CTLCOLORSCROLLBAR:
        case WM_CTLCOLORSTATIC:
            hWndChild = (HWND)lParam;
            break;
        default:
            break;
        }

        if(hWndChild == NULL)
        {
            bHandled = FALSE;
            return 1;
        }

        ATLASSERT(::IsWindow(hWndChild));
        return ::SendMessage(hWndChild, OCM__BASE + uMsg, wParam, lParam);
    }
#endif
};
